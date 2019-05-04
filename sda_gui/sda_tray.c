/*
Copyright (c) 2018 Stanislav Brtna

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "sda_tray.h"

volatile uint8_t irq_redraw;
volatile uint8_t irq_redraw_block;
extern volatile uint8_t systemBattClick;
static uint8_t alarm_string[3];
uint16_t trayBackgroundColor;

void svp_set_irq_redraw() {
	irq_redraw = 1;
}

void svp_tray_time(int16_t x1, int16_t y1, uint16_t w) {
	static uint8_t redraw;
	static uint8_t time_string[6];
	static uint8_t oldmin;
	static uint8_t oldhour;
	uint8_t curr_font;

	if((redraw == 0) || (irq_redraw)) {
		time_string[0] = svpSGlobal.hour / 10 + 48;
		time_string[1] = svpSGlobal.hour % 10 + 48;
		time_string[2] = ':';
		time_string[3] = svpSGlobal.min / 10 + 48;
		time_string[4] = svpSGlobal.min % 10 + 48;
		time_string[5] = 0;

		oldmin = svpSGlobal.min;
		oldhour = svpSGlobal.hour;
		LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
		curr_font = LCD_Get_Font_Size();
		LCD_Set_Sys_Font(18);
		LCD_DrawText_ext(x1, y1 + 8, pscg_get_text_color(&sda_sys_con), time_string);
		LCD_Set_Sys_Font(curr_font);
		redraw = 1;
	}

	if((oldmin != svpSGlobal.min) || (oldhour != svpSGlobal.hour)) {
		redraw = 0;
	}
}

void sda_tray_alarm_enable() {
	alarm_string[0] = 'A';
	alarm_string[1] = 0;
	svp_set_irq_redraw();
}

void sda_tray_alarm_disable() {
	alarm_string[0] = ' ';
	alarm_string[1] = 0;
	svp_set_irq_redraw();
}

static uint8_t tray_clicked(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  if ((svpSGlobal.touchX > x1) &&
       (svpSGlobal.touchX < x2) &&
       (svpSGlobal.touchY > y1) &&
       (svpSGlobal.touchY < y2) &&
       (svpSGlobal.touchValid)) {
    return 1;
  } else {
    return 0;
  }
}

void svp_tray_alarm(int16_t x1, int16_t y1, int16_t w) {
	static uint8_t redraw;

	uint8_t curr_font;

	if((redraw == 0) || (irq_redraw)) {

	  LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
		curr_font = LCD_Get_Font_Size();
		LCD_Set_Sys_Font(18);
		LCD_DrawText_ext(x1, y1 + 8, pscg_get_text_color(&sda_sys_con), alarm_string);
		LCD_Set_Sys_Font(curr_font);

		redraw = 1;
	}
}


void svp_tray_battery(int16_t x1, int16_t y1, int16_t w) {
	static uint8_t redraw;
	static systemPwrType oldBattState;
	static uint8_t batt_string[5];
	static uint8_t oldbatt;
	static uint32_t oldSysPercentageTime;
	static uint8_t battDisplayValue;
	uint8_t curr_font;

	// here is an accumulation of ugly hacks to keep battery percentage from going up

	// if we do not know (etc reset) set it to measured, rounded by 5
	if (battDisplayValue == 0 || battDisplayValue == 101) {
		if (svpSGlobal.battPercentage == 101) {
			battDisplayValue = 101;
		} else {
			battDisplayValue = svpSGlobal.battPercentage - svpSGlobal.battPercentage % 5;
		}
	} else {
		// check every 30s if measured is lower, if yes, then subtract 5% from value
		if ((svpSGlobal.uptime > oldSysPercentageTime + 30) && (svpSGlobal.battPercentage < battDisplayValue)) {
			if (battDisplayValue > 5) {
				battDisplayValue -= 5;
			}
			oldSysPercentageTime = svpSGlobal.uptime;
		}
	}


	if((redraw == 0) || (irq_redraw)) {
	  if (0 != battDisplayValue / 100) {
		  batt_string[0] = battDisplayValue / 100 + 48;
		} else {
		  batt_string[0] = ' ';
		}
		if (0 != battDisplayValue / 10) {
		  batt_string[1] = (battDisplayValue / 10) % 10 + 48;
		} else {
		  batt_string[1] = ' ';
		}
		batt_string[2] = battDisplayValue % 10 + 48;
		batt_string[3] = '%';
		batt_string[4] = 0;

		// reset the measured value when unplugged from charger
		if (oldBattState != svpSGlobal.pwrType && svpSGlobal.pwrType == POWER_BATT) {
			if (svpSGlobal.battPercentage != 101) {
				battDisplayValue = svpSGlobal.battPercentage - svpSGlobal.battPercentage % 5;
			}
		}

		oldbatt = battDisplayValue;
		oldBattState = svpSGlobal.pwrType;

		if(battDisplayValue == 101) {
		  batt_string[0] = ' ';
		  batt_string[1] = ' ';
		  batt_string[2] = '?';
		  batt_string[3] = '%';
		  batt_string[4] = 0;
		}
		LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
		curr_font = LCD_Get_Font_Size();
		LCD_Set_Sys_Font(18);
		if (svpSGlobal.pwrType == POWER_USB) {
			LCD_DrawText_ext(x1, y1 + 8, pscg_get_text_color(&sda_sys_con), IRQ_BATT_CHRG);
			if (svpSGlobal.battPercentage != 101) {
				LCD_FillRect(x1 + 8,
										 y1 + 31 - 6,
										 x1 + (int16_t)(((float)w - 4)*((float)svpSGlobal.battPercentage/(float)100)),
										 y1 + 31 - 2,
										 pscg_get_active_color(&sda_sys_con)
				);
			}
		} else if (svpSGlobal.pwrType == POWER_BATT) {
			LCD_DrawText_ext(x1, y1 + 8, pscg_get_text_color(&sda_sys_con), batt_string);
			if (svpSGlobal.battPercentage != 101) {
				LCD_FillRect(x1 + 8,
										 y1 + 31 - 6,
										 x1 + (int16_t)(((float)w - 4)*((float)battDisplayValue/(float)100)),
										 y1 + 31 - 2,
										 pscg_get_active_color(&sda_sys_con)
				);
			}
		}

		LCD_DrawRectangle(x1 + 8, y1 + 31 - 6, x1 + w - 4, y1 + 31 - 2, pscg_get_border_color(&sda_sys_con));
		LCD_Set_Sys_Font(curr_font);
		redraw = 1;
	}

	if ( tray_clicked(x1 - 1, y1, x1 + w, y1 + 31)) {
		systemBattClick = 1;
	}

	if ((oldbatt != battDisplayValue) || (oldBattState != svpSGlobal.pwrType)) {
		redraw = 0;
	}
}


uint8_t svp_tray_XBtn(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t reset) {
	static uint8_t init;
	static uint8_t xbtnPrev;
	static uint8_t xbtn;

	if((init == 0) || (reset == 1) || (irq_redraw)) {
		LCD_FillRect(x1, y1, x2, y2, pscg_get_fill_color(&sda_sys_con));
		LCD_DrawRectangle(x1, y1, x2, y2, pscg_get_border_color(&sda_sys_con));
		LCD_FillRect(x1 + 6, y1 + 14, x2 - 6, y2 - 14, pscg_get_text_color(&sda_sys_con));
		init = 1;
	}

	if (tray_clicked(x1, y1, x2, y2)) {

		if (xbtnPrev == 0) {
			LCD_FillRect(x1, y1, x2, y2, pscg_get_active_color(&sda_sys_con));
			LCD_DrawRectangle(x1, y1, x2, y2, pscg_get_border_color(&sda_sys_con));
			LCD_FillRect(x1 + 6, y1 + 14, x2 - 6, y2 - 14, pscg_get_text_color(&sda_sys_con));
		}

		xbtn = 1;
	} else {
		xbtn = 0;
	}

	if ((xbtn == 0) && (xbtnPrev == 1) && (svpSGlobal.systemXBtnClick == 0)) {
		svpSGlobal.systemXBtnClick = 1;
		init = 0;
	}

	xbtnPrev = xbtn;
	return 0;
}

uint8_t svp_tray_Opt(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	static uint8_t init;
	static uint8_t clickOld;
	static uint8_t click;
	uint8_t curr_font;
	static uint16_t holdCounter;

  curr_font = LCD_Get_Font_Size();
	LCD_Set_Sys_Font(32);

	if((irq_redraw) || (init == 0) || ((svpSGlobal.systemOptClick == 0) && clickOld == 1 && click == 0)) {
		LCD_FillRect(x1, y1, x2, y2, pscg_get_fill_color(&sda_sys_con));
		LCD_DrawRectangle(x1, y1, x2, y2, pscg_get_border_color(&sda_sys_con));
		LCD_DrawText_ext(x1 + 10, y1 + 2, pscg_get_text_color(&sda_sys_con), (uint8_t *)"S!");
		init = 1;
	}

	if (tray_clicked(x1, y1, x2, y2)) {

		if (clickOld == 0) {
			LCD_FillRect(x1, y1, x2, y2, pscg_get_active_color(&sda_sys_con));
			LCD_DrawRectangle(x1, y1, x2, y2, pscg_get_border_color(&sda_sys_con));
			LCD_DrawText_ext(x1 + 10, y1 + 2, pscg_get_text_color(&sda_sys_con), (uint8_t *)"S!");
		}

		click = 1;
		holdCounter++;
	} else {
	  click = 0;
	}
	LCD_Set_Sys_Font(curr_font);

	if (holdCounter < 15 && click == 0 && clickOld == 1 && svpSGlobal.systemOptClick == 0) {
		svpSGlobal.systemOptClick = 1;
		init = 0;
		holdCounter = 0;
	}

	if (holdCounter > 15 && svpSGlobal.systemOptClick == 0 && click == 1) {
		svpSGlobal.systemOptClick = 2;
		init = 0;
	}

	if (click == 0){
		holdCounter = 0;
	}

	clickOld = click;

	return 0;
}

uint8_t svp_tray() {
  static uint8_t init;
  static uint8_t XbtnVisibleOld;
  LCD_drawArea area;

  if (irq_redraw_block == 1) {
  	return 0;
  }

  LCD_getDrawArea(&area);
  LCD_setDrawArea(PM_TOPBAR_X1, PM_TOPBAR_Y1, PM_TOPBAR_X2, PM_TOPBAR_Y2);

  if ((init == 0) || (irq_redraw)) {
    trayBackgroundColor = LCD_color_darken(pscg_get_background_color(&sda_sys_con), 5);
    if (trayBackgroundColor == 0){
      trayBackgroundColor = LCD_color_lighten(pscg_get_background_color(&sda_sys_con), 5);
    }
		LCD_FillRect(PM_TOPBAR_X1, PM_TOPBAR_Y1, PM_TOPBAR_X2, PM_TOPBAR_Y2, trayBackgroundColor);
		init = 1;
	}

	svp_tray_alarm(145, 0, 32);

	svp_tray_time(180, 0, 46);

	svp_tray_battery(235, 0, 49);

  svp_tray_Opt(PM_TOPBAR_X1, PM_TOPBAR_Y1, 48, 31);

	if(svpSGlobal.systemXBtnVisible == 1) {
	  if((XbtnVisibleOld == 0) && (svpSGlobal.systemXBtnVisible == 1)) {
		  svp_tray_XBtn(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, 1);
	  }else{
	    svp_tray_XBtn(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, 0);
	  }
	}

	if((XbtnVisibleOld == 1) && (svpSGlobal.systemXBtnVisible == 0)) {
	  LCD_FillRect(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, trayBackgroundColor);
	}
	XbtnVisibleOld = svpSGlobal.systemXBtnVisible;

  LCD_setDrawAreaS(&area);
	return 0;
}

uint8_t svp_irq() {

  svp_tray();
  if (irq_redraw) {
    irq_redraw = 0;
  }

  return 0;
}

void irq_redraw_block_enable(){
	irq_redraw_block = 1;
}

void irq_redraw_block_disable(){
	irq_redraw_block = 0;
}

