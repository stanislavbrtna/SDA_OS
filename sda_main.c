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

#include "SDA_OS.h"

uint16_t slotScreen[APP_SLOT_MAX];
gr2context * slotScreenContext[APP_SLOT_MAX];

uint16_t slotOnTop[APP_SLOT_MAX];
uint16_t slotValid[APP_SLOT_MAX];

// GR2 system data structure
pscgElement sda_system_gr2_elements[SDA_SYS_ELEM_MAX];
pscgScreen sda_system_gr2_screens[SDA_SYS_SCREEN_MAX];
gr2context sda_sys_con;

// GR2 app data structure
pscgElement sda_app_gr2_elements[SDA_APP_ELEM_MAX];
pscgScreen sda_app_gr2_screens[SDA_APP_SCREEN_MAX];
gr2context sda_app_con;

gr2context * sda_current_con;

// system overlay
uint16_t overlayScr;
gr2context * overlayCont;
uint16_t overlayX1;
uint16_t overlayX2;
uint16_t overlayY1;
uint16_t overlayY2;
uint16_t ov_id; // current overlay id

uint8_t soft_error_flag; // 1 if error overlay is displayed
uint8_t timeUpdateFlag; // 1 if time is updated
uint8_t sleepLock; // flag to disable automatic sleep

uint8_t mainDir[258]; // name of main directory

uint8_t prev_top_slot;

// error overlays
uint16_t error_overlay_scr;
uint16_t error_overlay_ok;
uint16_t error_overlay = 0xFFFF;

// power options overlay
uint16_t batt_overlay;
uint8_t batt_overlay_flag;
volatile uint8_t systemBattClick;

extern volatile uint8_t tickLock;
/*****************************************************************************/
/*                       headers of svs wrappers                             */
/*****************************************************************************/
void pcBasicWrapInit();
void svsGr2WrapInit();
void svsSVPWrapInit();
void svsDirectSWrapInit();

/*****************************************************************************/
/*                            misc SDA functions                             */
/*****************************************************************************/

void testCode() {
	// Testcode section is used for testing new things, it is called once after init
	return;
}

void sdaSlotSetValid(uint16_t slot) {
	slotValid[slot] = 1;
}

void sdaSlotSetInValid(uint16_t slot) {
	slotValid[slot] = 1;
}

uint16_t sdaSlotGetValid(uint16_t slot) {
	return slotValid[slot];
}

void sdaSetSleepLock(uint8_t val) {
	sleepLock = val;
}

uint16_t sdaGetSlotScreen(uint8_t slot) {
	return slotScreen[slot];
}

void sda_batt_overlay_destructor() {
	pscg_destroy(batt_overlay, &sda_sys_con);
	setRedrawFlag();
	batt_overlay_flag = 0;
	batt_overlay = 0xFFFF;
}

void batt_overlay_handle(uint8_t init) {
	static uint16_t backlightSlider;
	static uint16_t backlightButton;
	static uint16_t soundEnable;
	static uint8_t backlightOld;

	if (init == 1) {
		batt_overlay = pscg_add_screen(&sda_sys_con);
		pscg_set_x_cell(batt_overlay, 16, &sda_sys_con);
		pscg_set_y_cell(batt_overlay, 16, &sda_sys_con);
		backlightSlider
			= pscg_add_slider_h(
					1, 1, 15,	3,
					255 - MIN_BACKLIGHT_VALUE,
					svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
					batt_overlay,
					&sda_sys_con
			);
		backlightButton
			= pscg_add_button(
					11, 4, 15, 6,
					OVRL_OK,
					batt_overlay,
					&sda_sys_con
			);
		soundEnable
			= pscg_add_checkbox(
					1, 4, 10, 6,
					OVRL_SILENT,
					batt_overlay,
					&sda_sys_con
			);
		pscg_set_value(soundEnable, svpSGlobal.mute, &sda_sys_con);
		return;
	}

	if (pscg_get_event(backlightSlider, &sda_sys_con)) {
		if ((pscg_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE) > 255){
			svpSGlobal.lcdBacklight = 255;
		} else {
			svpSGlobal.lcdBacklight
				= (uint8_t) (pscg_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE);
		}
		svp_set_backlight(svpSGlobal.lcdBacklight);
	}
	pscg_set_event(backlightSlider, EV_NONE, &sda_sys_con);

	if (sda_wrap_get_button(BUTTON_RIGHT) || sda_wrap_get_button(BUTTON_UP)) {
	  if (svpSGlobal.lcdBacklight < 255) {
	    svpSGlobal.lcdBacklight++;
	    svp_set_backlight(svpSGlobal.lcdBacklight);
	    pscg_set_value(
				backlightSlider,
				svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
			  &sda_sys_con
		  );
	  }
	  sda_wrap_clear_button(BUTTON_RIGHT);
	  sda_wrap_clear_button(BUTTON_UP);
	}

	if (sda_wrap_get_button(BUTTON_LEFT) || sda_wrap_get_button(BUTTON_DOWN)) {
	  if (svpSGlobal.lcdBacklight > MIN_BACKLIGHT_VALUE) {
	    svpSGlobal.lcdBacklight--;
	    svp_set_backlight(svpSGlobal.lcdBacklight);
	    pscg_set_value(
				backlightSlider,
				svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
			  &sda_sys_con
		  );
	  }
	  sda_wrap_clear_button(BUTTON_LEFT);
	  sda_wrap_clear_button(BUTTON_DOWN);
	}

	if (pscg_get_event(backlightButton, &sda_sys_con) == EV_RELEASED || svpSGlobal.lcdState == LCD_OFF) {
		batt_overlay_flag = 0;
		setRedrawFlag();
		destroyOverlay();
		return;
	}
	pscg_set_event(backlightButton, EV_NONE, &sda_sys_con);

	if (pscg_get_event(soundEnable, &sda_sys_con) == EV_RELEASED) {
		svpSGlobal.mute = pscg_get_value(soundEnable, &sda_sys_con);
		sda_store_mute_config();
	}
	pscg_set_event(soundEnable, EV_NONE, &sda_sys_con);

	if (svpSGlobal.mute != pscg_get_value(soundEnable, &sda_sys_con))
	pscg_set_value(soundEnable, svpSGlobal.mute, &sda_sys_con);

	if (svpSGlobal.lcdBacklight != backlightOld) {
		pscg_set_value(
				backlightSlider,
				svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
				&sda_sys_con
		);
	}

	backlightOld = svpSGlobal.lcdBacklight;
}

void svp_switch_main_dir() {
	svp_chdir(mainDir);
}

void setRedrawFlag() {
	svp_set_irq_redraw();
	svpSGlobal.systemRedraw = 1;
}

void sda_error_overlay_destructor() {
	setRedrawFlag();
	soft_error_flag = 0;
	error_overlay = 0xFFFF;
}

void svp_errSoftPrint(svsVM *s) {
	if (errCheck(s)) {
		sda_show_error_message((uint8_t *)s->errString);
	}
	svp_chdir(mainDir);
	svp_chdir((uint8_t *)"APPS");
	errSoftPrint(s);
	printf("\n");
}

void sda_show_error_message(uint8_t * text) {
	soft_error_flag = 1;
	error_overlay_scr = pscg_add_screen(&sda_sys_con);
	pscg_set_x_cell(error_overlay_scr, 16, &sda_sys_con);
	pscg_add_text(
			2, 1, 14, 2,
			(uint8_t *)"Error occured:",
			error_overlay_scr,
			&sda_sys_con
	);
	pscg_text_set_fit(
			pscg_add_text(1, 2, 15, 7, text, error_overlay_scr, &sda_sys_con),
			1,
			&sda_sys_con
			);
	error_overlay_ok
		= pscg_add_button(6, 8, 10, 9, OVRL_OK, error_overlay_scr, &sda_sys_con);
	error_overlay = setOverlayScreen(error_overlay_scr, &sda_sys_con);
	setOverlayDestructor(sda_error_overlay_destructor);
}

void sda_error_overlay_handle() {
	if (error_overlay != getOverlayId()) {
		return;
	}

	if (pscg_get_event(error_overlay_ok, &sda_sys_con) == EV_RELEASED) {
		destroyOverlay();
		return;
	}
	pscg_set_event(error_overlay_ok, EV_NONE, &sda_sys_con);
}

void sdaSlotOnTop(uint8_t slot) {
	uint8_t x;
	if (!(slotValid[slot]) && (slot <= APP_SLOT_MAX)) {
		return;
	}

	for (x = 0; x < APP_SLOT_MAX; x++) {
		slotOnTop[x] = 0;
	}
	slotOnTop[slot] = 1;
	if ((mainScr != slotScreen[slot]) || (sda_current_con != slotScreenContext[slot])) {
		mainScr = slotScreen[slot];
		sda_current_con = slotScreenContext[slot];
		setRedrawFlag();
	}

	prev_top_slot = slot;
}

void setInitStructDefaults() {
	svpSGlobal.uptime = 0;
	svpSGlobal.lcdOnTime = 0;
	svpSGlobal.dateUpdated = 0;
	svpSGlobal.systemRedraw = 0;
	svpSGlobal.systemXBtnClick = 0;
	svpSGlobal.systemXBtnVisible = 1;
	svpSGlobal.kbdVisible = 0;
	svpSGlobal.mute = 0;
	// 101 is non-valid init value, displays questionmark
	svpSGlobal.battPercentage = 101;
	svpSGlobal.battString[0] = ' ';
	svpSGlobal.battString[1] = ' ';
	svpSGlobal.battString[2] = ' ';
	svpSGlobal.battString[3] = '?';
	svpSGlobal.battString[4] = 'V';
	svpSGlobal.battString[5] = 0;
	svpSGlobal.powerState = PWR_MID;
	svpSGlobal.pwrType = POWER_BATT;
	svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
	svpSGlobal.lcdBacklight = 255;
}

// just simple check for now
void sdaCheckFs() {
	if (svp_fexists((uint8_t *)"svp.cfg") == 0) {
		printf("Config file not found!\n");

		// halt for now
		// TODO: check and rebuild directory structure and configs

		LCD_Fill(LCD_MixColor(255, 0, 0));
		LCD_DrawText_ext(32, 100, 0xFFFF, (uint8_t *)"SDA Error:\nConfig file not found!\nFix SD card and press Reset.");

		LCD_DrawText_ext(32, 320, 0xFFFF, (uint8_t *)"SDA-OS v."SDA_OS_VERSION);
#ifdef PC
		getchar();
#else
		while(1);
#endif
	}
}

void sda_power_sleep() {
	if (svpSGlobal.lcdState == LCD_ON) {
		svp_set_lcd_state(LCD_OFF);
	}
	svpSGlobal.powerMode = SDA_PWR_MODE_SLEEP;
	system_clock_set_low();
	svpSGlobal.powerState = PWR_LOW;
}

void sda_lcd_on_handle() {
	system_clock_set_normal();
	svp_set_irq_redraw();
	svpSGlobal.powerState = PWR_MAX;
	svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
}

void sda_lcd_off_handle() {
	svpSGlobal.powerState = PWR_LOW;
	svpSGlobal.powerMode = SDA_PWR_MODE_SLEEP;
}

void sda_power_main_handler() {
	static lcdStateType lcdStateOld;
	static uint32_t lastInputTime;
	static uint32_t lcdOffBlinkTimer;
	static uint32_t pwrDelay;

	if (svpSGlobal.touchValid) {
		lastInputTime = svpSGlobal.uptime;
		svpSGlobal.powerState = PWR_MAX;
	}

	// lcd auto shut down
	if (((svpSGlobal.lcdShutdownTime * 60) < (svpSGlobal.uptime - lastInputTime))
				&& (sleepLock == 0) && (lcdStateOld == LCD_ON)) {
		sda_power_sleep();
	}

	// when lcd is turned ON
	if ((svpSGlobal.lcdState == LCD_ON) && (lcdStateOld == LCD_OFF)) {
		lastInputTime = svpSGlobal.uptime;
		sda_lcd_on_handle();
		lcdOffBlinkTimer = 0;
		led_set_pattern(LED_OFF);
	}

	// when lcd is turned OFF
	if ((svpSGlobal.lcdState == LCD_OFF) && (lcdStateOld == LCD_ON)) {
		led_set_pattern(LED_ON);
		lcdOffBlinkTimer = svpSGlobal.uptime + 1;
		sda_lcd_off_handle();
	}

	if ((lcdOffBlinkTimer != 0) && (lcdOffBlinkTimer < svpSGlobal.uptime)) {
		led_set_pattern(LED_OFF);
		lcdOffBlinkTimer = 0;
		// after we blink the led, system will underclock itself
		// to gave time for apps or system to do stuff after lcd shutdown
		system_clock_set_low();
	}

	lcdStateOld = svpSGlobal.lcdState;

	// mid power after 30s
	if ((15 < (svpSGlobal.uptime - lastInputTime))
				&& (svpSGlobal.powerState == PWR_MAX)) {
		svpSGlobal.powerState = PWR_MID;
	}

	if (svpSGlobal.powerMode == SDA_PWR_MODE_SLEEP) {
		return;
	}

	if (svpSGlobal.powerState == PWR_MAX) {
		pwrDelay = 10000;
	}

	if (svpSGlobal.powerState == PWR_MID) {
		pwrDelay = 22000;
	}

	if (svpSGlobal.powerState == PWR_LOW) {
		pwrDelay = 50000;
	}

	for (uint32_t x = 0; x < pwrDelay; x++) { // waiting for next touch event
	#ifdef PC
			break;
	#endif
		if (svpSGlobal.touchValid == 1) {
			lastInputTime = svpSGlobal.uptime;
			break;
		}

		if (svpSGlobal.btnFlag == 1) {
			svpSGlobal.btnFlag = 0;
			lastInputTime = svpSGlobal.uptime;
			svpSGlobal.powerState = PWR_MAX;
			break;
		}
	}
}

/*****************************************************************************/
/*                             SDA main loop                                 */
/*****************************************************************************/

uint8_t sda_main_loop() {
	static uint8_t init;
	static uint8_t kbdVisibleOld;
	static uint8_t oldsec;

	uint8_t scr_touch_retval = 0;
	static uint8_t kbdRedraw;

	static psvcKbdLayout kbdLayout;
	static uint8_t kbdLayoutId;


	if (init == 0) {
		printf(
				"SDA OS Init: %d:%d:%d %d.%d. %d\n",
				svpSGlobal.hour,
				svpSGlobal.min,
				svpSGlobal.sec,
				svpSGlobal.day,
				svpSGlobal.month,
				svpSGlobal.year
			);
		led_set_pattern(LED_ALARM);
		LCD_Set_Sys_Font(18);
		svp_setMounted(1); // because SD or FS is already mounted

		sdaCheckFs(); // but better to be sure

		// init wrappers
		svsDirectSWrapInit();
		pcBasicWrapInit();
		svsGr2WrapInit();
		svsSVPWrapInit();

		gr2_InitContext(
				&sda_sys_con,
				sda_system_gr2_elements,
				SDA_SYS_ELEM_MAX - 1,
				sda_system_gr2_screens,
				SDA_SYS_SCREEN_MAX - 1
		);

		gr2_InitContext(
				&sda_app_con,
				sda_app_gr2_elements,
				SDA_APP_ELEM_MAX - 1,
				sda_app_gr2_screens,
				SDA_APP_SCREEN_MAX - 1
		);

		// get default
		svp_getcwd(mainDir, 256);

		// Init
		svp_crypto_init();

		setInitStructDefaults();

		oldsec = 99;
		kbdVisibleOld = 0;
		kbdRedraw = 0;

		kbdLayoutId = 0;
		init_kblayout_standard(&kbdLayout);

		// loading config from SD
		sda_load_config();

		// initialize screens: home, apps and options
		slotScreen[0] = svp_homeScreen(1, 0);
		slotScreenContext[0] = &sda_sys_con;
		slotValid[0] = 1;
		slotOnTop[0] = 1;

		slotScreen[1] = svp_appScreen(1, 0);
		slotScreenContext[1] = &sda_sys_con;
		slotValid[1] = 1;
		slotOnTop[1] = 0;

		slotScreen[2] = svp_optScreen(1, 0);
		slotScreenContext[2] = &sda_sys_con;
		slotValid[2] = 1;
		slotOnTop[2] = 0;

		sdaSvmRun(1, 0);

		slotScreenContext[4] = &sda_app_con;
		mainScr = slotScreen[0];
		sda_current_con = &sda_sys_con;
		led_set_pattern(LED_ON); // after init, we set led to on

		sdaReloadAlarms();
		sdaPrintActiveAlarm();

		// screen redraw for the first time
		touch_lock = 1;
		tickLock = 0;
		LCD_setDrawArea(0, 0, LCD_W, LCD_H);
		pscg_draw_screen(0, 32, 319, 479, mainScr, 1, &sda_sys_con);
		touch_lock = 0;
		tickLock = 1;
		init = 1;
		led_set_pattern(LED_OFF);

		testCode();
	}

	if (svpSGlobal.sec != oldsec) {
		timeUpdateFlag = 1;
		oldsec = svpSGlobal.sec;
	}

	if (svpSGlobal.touchType != EV_NONE) {
#ifdef TOUCHTEST
		if (svpSGlobal.touchType == EV_PRESSED){
			printf("touchValid! PRESSED x: %d y:%d\n", svpSGlobal.touchX,svpSGlobal.touchY );
		}else if(svpSGlobal.touchType == EV_RELEASED){
			printf("touchValid! RELEASED x: %d y:%d\n", svpSGlobal.touchX,svpSGlobal.touchY );
		}else if(svpSGlobal.touchType == EV_HOLD){
			printf("touchValid! HOLD x: %d y:%d\n", svpSGlobal.touchX,svpSGlobal.touchY );
		}
#endif
		if (svpSGlobal.kbdVisible == 1) { // if there is a keyboard
			uint8_t retVal = 0;
			touch_lock = 1;

			retVal
				= svp_touch_keyboard(
							0,
							319,
							&kbdLayout,
							svpSGlobal.touchX,
							svpSGlobal.touchY,
							svpSGlobal.kbdKeyStr,
							svpSGlobal.touchType
						);
			touch_lock = 0;

			if (retVal != 0) {
				if (retVal == 2) { // esc
					pscg_text_deactivate(&sda_sys_con);
					pscg_text_deactivate(&sda_app_con);
					svpSGlobal.kbdKeyStr[0] = 0;
					svpSGlobal.kbdVisible = 0;
					kbdLayoutId = 0;
				} else {
					if (svpSGlobal.kbdKeyStr[0] == 1) { // special button command
						sda_keyboard_set_layout(svpSGlobal.kbdKeyStr[1], &kbdLayout);
						svpSGlobal.kbdKeyStr[0] = 0;
						kbdRedraw = 1;
					} else {
						svpSGlobal.kbdFlag = 1;
					}
				}
			}
		}

		if (overlayScr == 0) { // if there is no overlay
			// touch is in main screen
			if ((svpSGlobal.touchType != EV_NONE)) {
				scr_touch_retval
					= pscg_touch_input(
								0,
								32,
								319,
								479 - 160 * svpSGlobal.kbdVisible,
								svpSGlobal.touchX,
								svpSGlobal.touchY,
								svpSGlobal.touchType,
								mainScr,
								sda_current_con
							);

				if (scr_touch_retval == 2) { // retval 2 means open the keyboard
					showKeyboard();
				}
			}
		} else {
			// touch in overlay
			if ((svpSGlobal.touchType != EV_NONE)) {
				scr_touch_retval = pscg_touch_input(
						overlayX1,
						overlayY1,
						overlayX2,
						overlayY2,
						svpSGlobal.touchX,
						svpSGlobal.touchY,
						svpSGlobal.touchType,
						overlayScr,
						overlayCont
					);

				if (scr_touch_retval == 2) { // retval 2 means open the keyboard
					showKeyboard();
				}
			}

			if (((svpSGlobal.touchX < overlayX1 - 10)
			    || (svpSGlobal.touchX > overlayX2 + 10)
			    || (svpSGlobal.touchY < overlayY1 - 10 && svpSGlobal.touchY > 32)
			    || (svpSGlobal.touchY > overlayY2 + 10))
			    && svpSGlobal.kbdVisible == 0 && kbdVisibleOld == 0
			    && svpSGlobal.touchType == EV_RELEASED) {
				destroyOverlay();
				setRedrawFlag();
			}
		}
		sx_set_touch_ev(svpSGlobal.touchType, svpSGlobal.touchX, svpSGlobal.touchY);
	}
	sda_store_buttons();
	svp_clr_button_ev();
	svpSGlobal.touchType = EV_NONE;
/*****************************************************************************/
/*                             screen redraw                                 */
/*****************************************************************************/

	touch_lock = 1;
	tickLock = 0;
	LCD_setDrawArea(0, 0, LCD_W, LCD_H);
	if (svpSGlobal.systemRedraw == 1 || kbdRedraw) {
		sdaSetRedrawDetect(1);
	}

	if ( (svpSGlobal.kbdVisible == 1 && kbdVisibleOld == 0)
				|| (svpSGlobal.systemRedraw && svpSGlobal.kbdVisible == 1)
				|| kbdRedraw
				) {
		svp_draw_keyboard(0, 319, &kbdLayout);
		kbdRedraw = 0;
		if (kbdRedraw == 0) {
			svpSGlobal.systemRedraw = 1;
		}
	}

	if ((svpSGlobal.kbdVisible == 1) && (kbdVisibleOld == 0)) {
		//
	  init_kblayout_standard(&kbdLayout);
	  kbdRedraw = 1;
	}

	if ((svpSGlobal.kbdVisible == 0) && (kbdVisibleOld == 1)) {
		svpSGlobal.systemRedraw = 1;
	}

	kbdVisibleOld = svpSGlobal.kbdVisible;

	if (overlayScr == 0) {
		LCD_setDrawArea(0, 0, LCD_W - 1, LCD_H - 160 * svpSGlobal.kbdVisible);
		if (svpSGlobal.systemRedraw == 1) {
			pscg_draw_screen(0, 32, 319, 479 - 160 * svpSGlobal.kbdVisible, mainScr, 1, sda_current_con);
			svpSGlobal.systemRedraw = 0;
		} else {
			pscg_draw_screen(0, 32, 319, 479 - 160 * svpSGlobal.kbdVisible, mainScr, 0, sda_current_con);
		}
	} else {
		if (svpSGlobal.systemRedraw == 1) {
			pscg_draw_screen(0, 32, 319, 479 - 160 * svpSGlobal.kbdVisible, mainScr, 1, sda_current_con);
			pscg_draw_screen(
						overlayX1,
						overlayY1,
						overlayX2,
						overlayY2,
						overlayScr,
						1,
						overlayCont
			);
			svpSGlobal.systemRedraw = 0;
		}
		pscg_draw_screen(overlayX1, overlayY1, overlayX2, overlayY2, overlayScr, 0, overlayCont);
	}
	pscg_draw_end(sda_current_con);

	touch_lock = 0;
	tickLock = 1;

/*****************************************************************************/
/*                            update of screens                              */
/*****************************************************************************/

	// System overlays are updated before screens
	if (batt_overlay_flag == 1) {
		batt_overlay_handle(0);
	}

	if (soft_error_flag == 1) {
		sda_error_overlay_handle();
	}

	taskSwitcherUpdate();

	// updating screens
	if (slotValid[0]) {
		svp_homeScreen(0, slotOnTop[0]);
	}

	if (slotValid[1]) {
		svp_appScreen(0, slotOnTop[1]);
	}

	if (slotValid[2]) {
		svp_optScreen(0, slotOnTop[2]);
	}

	if (slotValid[4]) {
		sdaSvmRun(0, slotOnTop[4]);
	}


	// top bar button handlers
	// handler for that big S! button
	if ((svpSGlobal.systemOptClick == 1)) {
		if(prev_top_slot != 0) {

			svpSGlobal.systemXBtnClick = 0;
			svpSGlobal.systemXBtnVisible = 0;

			// destroy overlay if there is one
			if (getOverlayId() != 0) {
				destroyOverlay();
			}

			hideKeyboard();
			sdaSlotOnTop(0);
			svp_chdir(mainDir);
			svp_chdir((uint8_t *)"APPS");
			sleepLock = 0;
		}
		svpSGlobal.systemOptClick = 0;
	}

	if ((svpSGlobal.systemOptClick == 2)) {
		taskSwitcherOpen();
		svpSGlobal.systemOptClick = 0;
	}

	// batt button handler
	static uint8_t batt_prev;
	if ((systemBattClick == 1 || svpSGlobal.systemPwrLongPress == 1) && (batt_prev == 0)) {
		systemBattClick = 0;
		svpSGlobal.systemPwrLongPress = 0;
		if (batt_overlay_flag == 0) {
			destroyOverlay();
			batt_overlay_handle(1);
			setOverlayScreen(batt_overlay, &sda_sys_con);
			setOverlayDestructor(sda_batt_overlay_destructor);
			batt_overlay_flag = 1;
			setOverlayY2(172);
		}
	}
	batt_prev = systemBattClick;

/*****************************************************************************/
/*                          end of main loop                                 */
/*****************************************************************************/

	// cleaning input flags
	svpSGlobal.touchValid = 0; //if the touch event was not handled, we discard it
	svpSGlobal.btnFlag = 0;
	timeUpdateFlag = 0;
	sdaSetRedrawDetect(0);

	// check for notification
	uint8_t notifAppName[APP_NAME_LEN];
	int32_t id;
	int32_t param;
	if (sdaGetCurentAlarm(&id, &param, notifAppName, sizeof(notifAppName))) {
		svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
		setNotificationFlag(id, param);
		sdaSvmLaunch(notifAppName, 0);
	}

	// power management
	sda_power_main_handler();

	return 0;
}
