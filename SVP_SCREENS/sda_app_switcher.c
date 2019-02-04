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

#include "svp_screens.h"

static uint16_t task_switcher;
static uint16_t task_switcher_inner;
static uint16_t task_overlay;
static uint8_t valid;

static uint16_t appButtons[MAX_OF_SAVED_PROC];
static uint16_t appButtonsClose[MAX_OF_SAVED_PROC];
static uint16_t appId[MAX_OF_SAVED_PROC];
static uint16_t ok;
static uint16_t close_all;
static uint16_t scrollbar;
static uint16_t numberOfApps;

static uint8_t niceSuspendName[MAX_OF_SAVED_PROC][35];


static void reloadNiceNames() {
  uint8_t *buff;
  uint32_t len;
  uint16_t slash;

  for(uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    slash = 0;
    buff = svmGetSuspendedName(x);

    if (buff == 0) {
      continue;
    }

    len = sda_strlen(buff);

    for(uint16_t i = 0; i < len; i++) {
      if (buff[i] == '/') {
        slash = i;
      }
    }

    if (slash == 0) {
      sda_strcp(buff, niceSuspendName[x], 35);
    } else {
      sda_strcp(buff + slash + 1, niceSuspendName[x], 35);
    }

  }
}

void taskSwitcherDestructor() {
	pscg_destroy(task_switcher, &sda_sys_con);
	valid = 0;
}

void taskSwitcherOpen() {
	if (valid == 1) {
		return;
	}

  hideKeyboard();

	uint16_t n = 0;
	task_switcher = pscg_add_screen(&sda_sys_con);
	task_switcher_inner = pscg_add_screen(&sda_sys_con);
	pscg_set_screen(task_switcher_inner, task_switcher, &sda_sys_con);
	pscg_set_x_cell(task_switcher, 16, &sda_sys_con);
	pscg_set_x1y1x2y2(task_switcher_inner, 1, 1, 15, 8, &sda_sys_con);

	pscg_add_text(
								1, 0, 15, 1,
								SWICH_RUNNING_APPS,
								task_switcher,
								&sda_sys_con
	);
	scrollbar
		= pscg_add_slider_v(
												15, 1, 17, 8,
												MAX_OF_SAVED_PROC * 10 + 32,
												0,
												task_switcher,
												&sda_sys_con
			);

	for(uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		appId[n] = svmGetSuspendedId(x);

		if(appId[n] == 0){
			continue;
		}

    reloadNiceNames();

		appButtons[n]
			= pscg_add_button(
												0, 1 + n, 6, 2 + n,
												niceSuspendName[x],
												task_switcher_inner,
												&sda_sys_con
				);
		appButtonsClose[n]
			= pscg_add_button(
												6, 1 + n, 7, 2 + n,
												(uint8_t *)"-",
												task_switcher_inner,
												&sda_sys_con
				);

		n++;
	}
	numberOfApps = n;

	ok = pscg_add_button(12, 9, 15, 10, OVRL_OK, task_switcher, &sda_sys_con);

	close_all
		= pscg_add_button(
											1, 9, 9, 10,
											SWITCH_CLOSE_ALL,
											task_switcher,
											&sda_sys_con
			);

	task_overlay = setOverlayScreen(task_switcher, &sda_sys_con);
	setOverlayX1(16);
	setOverlayY1(32 + 16);
	setOverlayX2(320 - 16);
	setOverlayY2(480 - 96);

	setOverlayDestructor(taskSwitcherDestructor);
	valid = 1;

	if (numberOfApps == 0) {
		pscg_set_grayout(close_all, 1, &sda_sys_con);
		pscg_set_grayout(scrollbar, 1, &sda_sys_con);
	}
}

void taskSwitcherUpdate() {
	if (valid == 0) {
		return;
	}

	for(uint16_t x = 0; x < numberOfApps; x++) {
		if (pscg_get_event(appButtons[x], &sda_sys_con) == EV_RELEASED) {
			hideKeyboard();
			if (svmWake(appId[x])) {
			  sda_show_error_message((uint8_t *)"Error occured while waking app.");
			}
			setRedrawFlag();
			destroyOverlay();
			return;
		}

		if (pscg_get_event(appButtonsClose[x], &sda_sys_con) == EV_RELEASED) {
			svmClose(appId[x]);
			taskSwitcherOpen();
			return;
		}

		pscg_set_event(appButtons[x], EV_NONE, &sda_sys_con);
		pscg_set_event(appButtonsClose[x], EV_NONE, &sda_sys_con);
	}

	if (pscg_get_event(ok, &sda_sys_con) == EV_RELEASED) {
		setRedrawFlag();
		destroyOverlay();
		return;
	}
	pscg_set_event(ok, EV_NONE, &sda_sys_con);

	if (pscg_get_event(close_all, &sda_sys_con) == EV_RELEASED) {
		svmCloseAll();
		destroyOverlay();
		return;
	}
	pscg_set_event(close_all, EV_NONE, &sda_sys_con);

	pscg_set_yscroll(
										task_switcher_inner,
										pscg_get_value(scrollbar, &sda_sys_con) * 32 / 10,
										&sda_sys_con
	);
}
