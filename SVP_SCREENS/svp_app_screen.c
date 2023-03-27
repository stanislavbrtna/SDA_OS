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

#define MAX_APP_COUNT 25
#define MAX_FOLDER_STACK 5

/* APP screen
feature request:
de-init
sd-unmounted lock state
*/

static uint16_t scrollbar;
static uint8_t * selectedObject;
static uint8_t * selectedObjectStr;
static uint8_t folderStack[MAX_FOLDER_STACK][APP_NAME_LEN+1];
static uint8_t folderStackStr[MAX_FOLDER_STACK][APP_NAME_LEN+1];
static uint16_t folder_stack_max;
static uint8_t appNum;

// loads or handles inner screen
uint16_t inner_handler(uint8_t init, uint8_t * fileName) {
  static uint16_t retScreen;
  static uint16_t appCount;
  static uint8_t appFName[MAX_APP_COUNT][APP_NAME_LEN + 1];
  static uint8_t appHumanName[MAX_APP_COUNT][APP_NAME_LEN + 1];
  static uint8_t appIcoName[MAX_APP_COUNT][APP_NAME_LEN + 1];
  static uint16_t appFNameBtn[MAX_APP_COUNT];
  uint16_t x = 0;
  uint8_t retval = 0;
  svp_csvf appsCSV;

  if (init) {
    retScreen = gr2_add_screen(&sda_sys_con);
    gr2_set_x_cell(retScreen, 16, &sda_sys_con);
    gr2_set_y_cell(retScreen, 16, &sda_sys_con);
    svp_switch_main_dir();
    svp_chdir((uint8_t *)"APPS");

    // file error detection
    if (svp_csv_open(&appsCSV, fileName) == 0) {
      gr2_add_text(1, 1, 20, 4, ASCR_CARD_ERROR, retScreen, &sda_sys_con);
      return retScreen;
    }
    // 0 filename.svs, 1 icon.ppm, 2 human name

    // test if file actualy works
    if (svp_csv_get_cell(&appsCSV, 1, (uint8_t *)"", appFName[x], APP_NAME_LEN) == 0) {
      gr2_add_text(1, 1, 20, 3, ASCR_FILE_ERROR, retScreen, &sda_sys_con);
      return retScreen;
    }

    do{
      // load data
      svp_csv_get_cell(&appsCSV, 0, (uint8_t *)"", appFName[x], APP_NAME_LEN);
      svp_csv_get_cell(&appsCSV, 1, (uint8_t *)"none.p16", appIcoName[x], APP_NAME_LEN);
      svp_csv_get_cell(&appsCSV, 2, ASCR_NONAME, appHumanName[x], APP_NAME_LEN);
#ifdef APP_SCREEN_DEBUG
      printf("%u:app found: %s ico: %s name: %s id:", x, appFName[x], appIcoName[x], appHumanName[x]);
#endif
      // add icon
      appFNameBtn[x] = gr2_add_icon(
        1 + 6*(x%3), 1 + 6*(x/3), 7 + 6*(x%3), 7 + 6*(x/3),
        appHumanName[x],
        appIcoName[x],
        retScreen,
        &sda_sys_con
      );

      if (svp_strlen(appHumanName[x]) <= 8) {
        gr2_text_set_align(appFNameBtn[x], GR2_ALIGN_CENTER, &sda_sys_con);
      }
      gr2_set_param(appFNameBtn[x], 1, &sda_sys_con);

#ifdef APP_SCREEN_DEBUG
      printf("%u\n", appFNameBtn[x]);
#endif
      x++;

      if (x == MAX_APP_COUNT) {
        printf("Loading semi-FAILED, too mavy apps!\n");
        break;
      }

    }while(svp_csv_next_line(&appsCSV));

    svp_csv_close(&appsCSV);

    appCount = x;
    appNum = x;

    gr2_set_param(scrollbar, appCount / 3, &sda_sys_con);

#ifdef APP_SCREEN_DEBUG
    printf("Loading Done\n");
#endif
    return retScreen;
  } else {
    // normal event handling
    for(x = 0; x < appCount; x++) {
      if (gr2_clicked(appFNameBtn[x], &sda_sys_con)) {
        selectedObject = appFName[x];
        selectedObjectStr = appHumanName[x];
        retval = 1;

#ifdef APP_SCREEN_DEBUG
        printf("inner_handler: clicked\n");
#endif
        break;
      }

      if (gr2_get_event(appFNameBtn[x], &sda_sys_con) != EV_NONE) {
        gr2_set_event(appFNameBtn[x], EV_NONE, &sda_sys_con);
      }

    }

    return retval;
  }
}


static void add_to_stack(uint8_t * fname) {
  uint16_t x;
  folder_stack_max++;
  for (x = 0; x < APP_NAME_LEN; x++) {
    if (fname[x] != 0){
      folderStack[folder_stack_max][x] = fname[x];
    } else {
      folderStack[folder_stack_max][x] = fname[x];
      break;
    }
  }
#ifdef APP_SCREEN_DEBUG
  printf("folder stack added: %s\n", folderStack[folder_stack_max]);
#endif
}


static void get_from_stack(uint8_t * fname) {
  uint16_t x;

  folder_stack_max--;
#ifdef APP_SCREEN_DEBUG
  printf("folder stack got: %s\n", folderStack[folder_stack_max]);
#endif
  for (x = 0; x < APP_NAME_LEN; x++) {
    if (folderStack[folder_stack_max][x] != 0){
      fname[x] = folderStack[folder_stack_max][x];
    } else {
      fname[x] = folderStack[folder_stack_max][x];
      break;
    }
  }
}

/*
 * Detects type according to file extension
 *
 * fname - string filename
 *
 * return - 0:error 1:menu 2:svs
 */
static uint8_t detect_type(uint8_t * fname) {
  uint16_t x;

  for(x = 0; x < APP_NAME_LEN-3; x++) {
    if (fname[x] == '.'){
      if ((fname[x + 1] == 'm') && (fname[x + 2] == 'n') && (fname[x + 3] == 'u')) {
        return 1;
      }

      if ((fname[x + 1] == 's') && (fname[x + 2] == 'v') && (fname[x + 3] == 's')) {
        return 2;
      }
    }
  }
  return 0;
}


/*
 * Resizes screen according to scrollbar and app runing
 *
 * id - pscg id of the screen to resize
 *
 * return void
 */
void inScreenResizer(uint16_t id) {
#ifdef APP_SCREEN_DEBUG
  printf("id: %u\n", id);
#endif
  if (id == 0) {
    printf("Error: trying to resize screen zero!\n");
    return;
  }
  gr2_set_x1y1x2y2(id, 0, 1, 10, 13, &sda_sys_con);
  if (svmGetRunning()) {
    gr2_set_y2(id, 12, &sda_sys_con);
    if (appNum > 9) {
      gr2_set_visible(scrollbar, 1, &sda_sys_con);
      gr2_set_x1y1x2y2(id, 0, 1, 9, 12, &sda_sys_con);
      gr2_set_y2(scrollbar, 12, &sda_sys_con);
      gr2_set_xscroll(id, 0, &sda_sys_con);
    } else {
      gr2_set_xscroll(id, -16, &sda_sys_con);
      gr2_set_visible(scrollbar, 0, &sda_sys_con);
    }
  } else {
    gr2_set_y2(id, 14, &sda_sys_con);
    if (appNum > 12) {
      gr2_set_visible(scrollbar, 1, &sda_sys_con);
      gr2_set_x1y1x2y2(id, 0, 1, 9, 14, &sda_sys_con);
      gr2_set_y2(scrollbar, 12, &sda_sys_con);
      gr2_set_xscroll(id, 0, &sda_sys_con);
    } else {
      gr2_set_xscroll(id, -16, &sda_sys_con);
      gr2_set_visible(scrollbar, 0, &sda_sys_con);
    }
  }
}


uint16_t svp_appScreen(uint8_t init, uint8_t top) {
  static uint16_t appScreen;
  static uint16_t inScreen;
  static uint16_t textLabel;
  static uint16_t btnBack;
  static uint16_t btnSwitch;
  static uint8_t labelbuff[APP_NAME_LEN+1];
  uint8_t appActive;
  static uint8_t appActivePrev;
  static uint8_t inScrReloaded;

  if (init == 1) {
    folder_stack_max = 0;

    appScreen = gr2_add_screen(&sda_sys_con);

    scrollbar = gr2_add_slider_v(9, 1, 10, 12, 100, 0, appScreen, &sda_sys_con);

    if (inScreen != 0) {
      gr2_destroy_screen(inScreen, &sda_sys_con);
    }

    inScreen = inner_handler(1, (uint8_t *)"main.mnu");
    inScrReloaded = 1;

    add_to_stack((uint8_t *)"main.mnu");

    inScreenResizer(inScreen);

    gr2_set_screen(inScreen, appScreen, &sda_sys_con);

    textLabel = gr2_add_text(2, 0, 10, 1, ASCR_APPLICATIONS, appScreen, &sda_sys_con);

    btnBack = gr2_add_button(0, 0, 2, 1, (uint8_t *)"<-", appScreen, &sda_sys_con);

    btnSwitch = gr2_add_button(2, 13, 8, 14, ASCR_RUNNING_APP, appScreen, &sda_sys_con);

    gr2_text_set_align(btnSwitch, GR2_ALIGN_CENTER, &sda_sys_con);

    gr2_set_visible(btnBack, 0, &sda_sys_con);

    //svmCloseRunning(); - ?? perhaps was here for mount/umount before sda went multi-process

    return appScreen;
  }

  //loop top
  if (top == 1) {

    svpSGlobal.systemXBtnVisible = 0;

    if (svpSGlobal.sdaDeviceLock == DEVICE_LOCKED) {
      sda_slot_on_top(0);
    }

    gr2_set_yscroll(inScreen, gr2_get_value(scrollbar, &sda_sys_con) * 96, &sda_sys_con);

    appActive = svmGetRunning();

    gr2_set_visible(btnSwitch, appActive, &sda_sys_con);

    if (inner_handler(0, (uint8_t *)"")) {
      uint8_t type = 0;
      gr2_ki_unselect(inScreen, &sda_sys_con);
      type = detect_type(selectedObject);
      if (type == 2) {
        if(svmLaunch(selectedObject, 0) == 0) {
          sda_show_error_message((uint8_t *)"Error occured while launching file!");
        }
      }else if (type == 1) {
        add_to_stack(selectedObject);
        sda_strcp(selectedObjectStr, labelbuff, sizeof(labelbuff));
        gr2_set_str(textLabel, labelbuff, &sda_sys_con);
        sda_strcp(selectedObjectStr, folderStackStr[folder_stack_max], sizeof(folderStackStr));

        gr2_set_visible(btnBack, 1, &sda_sys_con);
        gr2_destroy_screen(inScreen, &sda_sys_con);
        inScreen = inner_handler(1, selectedObject);
        inScrReloaded = 1;
#ifdef APP_SCREEN_DEBUG
        printf("screen id %u\n", inScreen);
#endif
        inScreenResizer(inScreen);
        gr2_set_screen(inScreen, appScreen, &sda_sys_con);
      } else {
        sda_show_error_message((uint8_t *)"Menu file error: Unknown file type!");
      }
    }

    sda_screen_button_handler(appScreen, btnBack, &sda_sys_con);

    if (gr2_get_event(btnBack, &sda_sys_con) == EV_RELEASED) {
      get_from_stack(labelbuff); // got prev
      #ifdef APP_SCREEN_DEBUG
      printf("reloading: %s\n", labelbuff);
      #endif
      gr2_destroy_screen(inScreen, &sda_sys_con);
#ifdef APP_SCREEN_DEBUG
      printf("screen id %u\n", inScreen);
#endif
      inScreen = inner_handler(1, labelbuff);
      inScrReloaded = 1;
      inScreenResizer(inScreen);
      gr2_set_screen(inScreen, appScreen, &sda_sys_con);

      sda_strcp(folderStackStr[folder_stack_max], labelbuff, sizeof(labelbuff));

      gr2_set_str(textLabel, labelbuff, &sda_sys_con);

      if (folder_stack_max == 1) {
        gr2_set_visible(btnBack, 0, &sda_sys_con);
        gr2_set_str(textLabel, ASCR_APPLICATIONS, &sda_sys_con);
      }
    }
    gr2_set_event(btnBack, EV_NONE, &sda_sys_con);

    if (appActive){
      if (gr2_get_event(btnSwitch, &sda_sys_con) == EV_RELEASED) {
        taskSwitcherOpen();
      }
      gr2_set_event(btnSwitch, EV_NONE, &sda_sys_con);
    }

    if (appNum > 9){ //scrollable
      if (svpSGlobal.keyEv[BUTTON_UP] == EV_PRESSED){
        if (gr2_get_value(scrollbar, &sda_sys_con)< 1){
          gr2_set_value(scrollbar, 0, &sda_sys_con);
        }else{
          gr2_set_value(scrollbar, gr2_get_value(scrollbar, &sda_sys_con) - 1, &sda_sys_con);
        }
      }
      svpSGlobal.keyEv[1] = EV_NONE;

      if (svpSGlobal.keyEv[BUTTON_DOWN] == EV_PRESSED) {
        if (gr2_get_value(scrollbar, &sda_sys_con) > gr2_get_param(scrollbar, &sda_sys_con)){
          gr2_set_value(scrollbar, gr2_get_param(scrollbar, &sda_sys_con), &sda_sys_con);
        }else{
          gr2_set_value(scrollbar, gr2_get_value(scrollbar, &sda_sys_con) + 1, &sda_sys_con);
        }
      }

      svpSGlobal.keyEv[2] = EV_NONE;
    }

    if ((appActivePrev != appActive) || (inScrReloaded)) {
      inScreenResizer(inScreen);

      if (inScrReloaded) {
        gr2_set_value(scrollbar, 0, &sda_sys_con);
      }

      inScrReloaded = 0;
    }

    appActivePrev = appActive;

  }
  //else: background jobs
  return 0;
}

