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

void taskSwitcherDestructor() {
	pscg_destroy(task_switcher, &sda_sys_con);
	valid = 0;
}

void taskSwitcherOpen() {
	if (valid == 1) {
		return;
	}

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

		appButtons[n]
			= pscg_add_button(
												0, 1 + n, 6, 2 + n,
												svmGetSuspendedName(x),
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
			svmWake(appId[x]);
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
