
# SDA_OS Native API 
Since the native interfaces of SDA_OS are still unstable, only a few that matured is documented here. Go for the sources for more info.

## Alarms
Applications running in the SVS runtime can register alarms, these alarms will launch the app at given time. They are not intended to use them in native C part of SDA_OS.

    uint8_t removeAlarm(int32_t id, uint8_t * appName);
Removes alarm with given ID and application name. Returns zero if succeeded.

    uint8_t sdaGetCurentAlarm(int32_t * id, int32_t * param, uint8_t * appNameBuffer, uint16_t size);
If an alarm occurred, it will return 1 and set the id, param and appNameBuffer to current alarm values. Size parameter should be sizeof(appNameBuffer). Alarm is then cleared and next alarm is reloaded. If no alarm occurs, function returns zero.

    int32_t sdaRegisterAlarm(
                                uint8_t * appname,
                                int32_t timestamp,
                                uint8_t hour,
                                uint8_t min,
                                uint8_t wkday,
                                uint8_t day,
                                uint8_t month,
                                int32_t param);
Registers system alarm. For once occurring alarms, use SDA timestamp as timestamp parameter. For repeated alarms use hour, min, wkday, day and month fields and timestamp set to zero. If month, day, or wkday is zero, value is ignored (if day =  0 alarm occurs every day).

    void sdaPrintActiveAlarm();

Prints currently active alarm on standard output.

## Overlays

    uint16_t setOverlayScreen(uint16_t val, gr2context * c);
Sets overlay screen to screen with id *val* of gr2context *c*. Returns id of new overlay.

    void setOverlayX1(uint16_t val);
    void setOverlayY1(uint16_t val);
    void setOverlayX2(uint16_t val);
    void setOverlayY2(uint16_t val);
Sets overlay size and position. In native display pixels.

    uint16_t getOverlayId();
Returns current overlay id, if the id is different from id given by *setOverlayScreen* your overlay was destroyed.

    void setOverlayDestructor(void (*destructor) ());
   Sets the destructor function for current overlay.

`void destroyOverlay();` 
Destroys overlay, if destructor is set, it calls the destructor, otherwise it destroys the overlay screen.

    void overlayDestructorDone();
Call this before returning from your destructor.

## Configuration

    void sda_load_config();

Loads the system-wide configs.

    void sda_store_config_gui(uint8_t set_def);

Stores GUI config values or restores gui defaults.

    void sda_store_config();
Stores the system-wide configs. 

    void sda_store_calibration();

Stores screen calibration data.

    void sda_store_mute_config();

Stores mute config.

## SVS Machine
Main feature of SDA_OS is running its applications. These applications are stored as script files in the APPS folder. The SVS machine functions are stored in *sda_system/sda_svm.c* and *sda_system/sda_svm.h*.

    uint8_t sdaSvmLaunch(uint8_t * fname, uint16_t parentId);

Launches .svs file specified in *fname* from APPS directory. Parent id is 0 for apps launched form launcher. When the parent id is not zero, parent application is woken upon closing its child. 

    void sdaSvmOnTop();

If instance of some app is running, this switches to it.

    uint8_t sdaSvmGetRunning();

returns non-zero if some app is currently loaded

    void sdaSvmCloseApp();

Gently closes the currently running app, performs exit() call.

    void sdaSvmKillApp();

Kills currently running app.

    uint16_t sdaSvmRun(uint8_t init, uint8_t top);

Performs update of currently running app. Runs its *update* function.

    void sdaSvmSetMainScreen(uint16_t val);

uint16_t sdaSvmGetMainScreen();
Sets or gets main screen from SVS.

    void sdaSetRedrawDetect(uint8_t val);
    uint8_t sdaGetRedrawDetect();

Screen redraw detection for SVS apps.

    void sdaSvmSetError(uint8_t * str);

Shows error.


    void sdaSvmCall(
    		uint8_t *name,
    		uint8_t *callback,
    		varType arg0, uint8_t type0,
    		varType arg1, uint8_t type1,
    		varType arg2, uint8_t type2
    	);
Launches instance of svs file given in *name*, passes given arguments to that instance. If callback is specified, it will be called upon returning to the parent process.

    void sdaSvmRetval(
    		varType arg0, uint8_t type0,
    		varType arg1, uint8_t type1,
    		varType arg2, uint8_t type2
    	);
Sets return values that will be passed to the parent instance.

    uint8_t svmSuspend();

Suspends currently running app, it performs suspend call ant then uses *sdaSvmSave* to save the currently running app in the cache folder.

    uint8_t svmWake(uint16_t id);

Loads the app with given id and calls the wakeup function.

    void sdaSvmSave();
Saves currently running app.

    uint8_t sdaSvmLoad(uint16_t id);
Loads previously stored running app with given id.

    void svmClose(uint16_t id);

Closes app with given id. Loads it if it is suspended.

    void svmCloseAll();

Closes all running and suspended apps.

    void svmSetSingular(uint16_t id);

Sets app as singular. It will be launched as only one instance, if another launch is attempted, system will switch to the currently running instance.

    uint8_t sdaSvmGetRunning();
Returns 1 if there is an app currently running.

    uint16_t sdaSvmGetId();

Gets id of currently running app.

    uint16_t svmGetSuspendedId(uint16_t id);

Gets if app with given id is suspended.

    uint8_t *svmGetSuspendedName(uint16_t id);

Gets (file)name of currently suspended app.


    void setNotificationFlag(int32_t id, int32_t param);
    int32_t getNotificationId();
    int32_t getNotificationParam();
    int8_t getNotificationFlag();
    void clearNotificationFlag();

Notification flags are used primarily for passing alarms to the running app.

## Time
Time functions are stored in *sda_util/sda_time.c* and *sda_util/sda_time.h*. 
SDA_OS uses its own timestamp format for its alarms. Timestamp only works for years 2007 and above.

    int32_t sdaTimeGetTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

Returns timestamp

    uint8_t sdaTimeGetSeconds(int32_t sdaTime);
    uint8_t sdaTimeGetMinutes(int32_t sdaTime);
    uint8_t sdaTimeGetHours(int32_t sdaTime);
    uint8_t sdaTimeGetWeekDay(int32_t sdaTime);
    uint8_t sdaTimeGetDays(int32_t sdaTime);
    uint8_t sdaTimeGetMonths(int32_t sdaTime);
    uint16_t sdaTimeGetYears(int32_t sdaTime);

Returns various data from a given timestamp.

    void sdaTimePrint(int32_t sdaTime);

Prints given timestamp in human readable form to standard output.

> Written with [StackEdit](https://stackedit.io/).

