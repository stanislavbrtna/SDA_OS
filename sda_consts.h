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

File description:
  This file stores main constants and constraints for SDA-OS.

*/

#ifndef SDA_CONSTS_H
#define SDA_CONSTS_H

// version number string and version number,
#define SDA_OS_VERSION     "1.5.0"
//                  a.k.a  01.05.00
#define SDA_OS_VERSION_NUM  10500

// GR2 elements for system
#define SDA_SYS_ELEM_MAX   190
#define SDA_SYS_SCREEN_MAX  40

// GR2 elements for the apps
#define SDA_APP_ELEM_MAX   250
#define SDA_APP_SCREEN_MAX  20

// LCD resolution
#define SDA_LCD_W 320
#define SDA_LCD_H 480

// size and position of top bar
#define PM_TOPBAR_X1 0
#define PM_TOPBAR_Y1 0
#define PM_TOPBAR_X2 (319 + svpSGlobal.lcdLandscape*160)
#define PM_TOPBAR_Y2 31

// maximum number of system application slots
// slot | use
//  0   | homescreen
//  1   | app screen (launcher)
//  2   | options
//  4   | currently running SVS application
#define APP_SLOT_MAX 8

// SVM Config

// max name lenght of running SVS application, including its path
#define APP_NAME_LEN     128
#ifndef WEBTARGET
#define APP_ARG_STR_LEN 2048 // 1024 magically works for the web sim
#else
#define APP_ARG_STR_LEN 1024 // magically works for the web sim
#endif
// maximum available applications
#define MAX_OF_SAVED_PROC 16

// Max number of openned general-purpose files
#define SDA_FILES_OPEN_MAX 10

// Size of OS clipboard (in bytes)
#define SDA_CLIPBOARD_SIZE 256

// Debug options
// #define APP_SCREEN_DEBUG // App screen (launcher) debug switch
// #define SVM_DBG_ENABLED // SVM debug switch

// shows where the screen is redrawn, works only in simulator, usefull for debug
// #define SIM_SHOW_REDRAW


// SDA files config
// \n is 10 on linux systems, but takes two bytes on windows, so it is hardcoded
// to 10, both for csv and cfg
#define SDA_ENDLINE    10
#define SDA_SEPARATOR '|'

// default minimum backlight value, override this in your SDA_OS base if needed
#ifndef MIN_BACKLIGHT_VALUE
#define MIN_BACKLIGHT_VALUE 170
#endif

// button id defines
#define BUTTON_A     0
#define BUTTON_LEFT  1
#define BUTTON_UP    2
#define BUTTON_DOWN  3
#define BUTTON_RIGHT 4
#define BUTTON_B     5

// Driver options
#ifndef TOUCH_USE_BATTERY_MEASUREMENT
#define TOUCH_USE_BATTERY_MEASUREMENT
#endif

// Screen slot defines
#define SDA_SLOT_HOMESCREEN 0
#define SDA_SLOT_APPLIST    1
#define SDA_SLOT_SETTINGS   2
#define SDA_SLOT_SVM        4

#define SDA_RESOURCES_MAX 5


#endif
