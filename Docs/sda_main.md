  Automatically generated documentation on wrap_umc_svp.c
#### SVP API Level history

Api level changes:
101 - Fexists, svp lang, led API, BTN defines
102 - pscg_image added to GR2 wrap
103 - pGetEventC added to gr2 wrapper
104 - Timestamp API
105 - cKeyRemove
106 - pscg: pGetTexAct
107 - error
108 - pGetVisible
109 - pSetTexAct,  setSingular(), notification api, pGetMainScr

- API level overhaul -
660 - SDA version 0.6.6 with all its features
 ...
720 - SDA version 0.7.2 with all its features
#### Constants
| Constant | Description |
|   ---    |    ---      |
| SVP_LANG_CZ | Czech language |
| SVP_LANG_ENG | English language |

#### Main OS functions
##### Get redraw flag
    sys.os.getRedraw();
Gets redraw flag. *getRedraw* also works.
Return: [num] 1 if redraw flag is set, otherwise 0
##### Set redraw
    sys.os.setRedraw();
Sets redraw flag
Return: None
##### Wake the SDA from sleep
    sys.os.wake();
Wakes SDA without turning the screen on.
SDA will wake in the low power mode and will sleep again after the lcd shutdown time.
Return: None
##### Pushes app to foreground
    sys.os.arise();
If called from timer callback, the app is promoted to the foreground.
Return: None
##### Show Error
    sys.os.error([str]errorText);
Throws error message
Return: None
##### Get path to executable
    sys.os.getAppPath();
Get path to the executable
Return: [str] Path
#### Keyboard
##### Hide keyboard
    sys.os.hideKbd();
Hides system keyboard.
Return: None
##### Show keyboard
    sys.os.showKbd();
Shows system keyboard
Return: None

#### Misc

##### Get random number
    sys.os.rnd();
Returns random number
Return: [num]RandomValue
##### Quit program
    sys.os.exit();
Stops program execution after exiting *update* function and performing *exit* function.
Return: None
##### Check API level
    sys.os.checkVer([num] API_Level);
Checks for API Lvl support.
If host level is below given API_Level, error is thrown and app is terminated.
Return: None
##### Get API level
    sys.os.getVer();
Checks for API Lvl support.
Return: [num] SDA_OS version number
##### Get system language
    sys.os.getLang();
Returns SDA_OS language.
Return: 0 if czech, 1 if english, also defines SVP_LANG_CZ SVP_LANG_ENG
#### Subprocess
##### Set process as singular
    sys.os.setSingular();
Sets current process as singular.
Return: None
##### Launch subprocess
    sys.os.subProcess([str]fileName, [str/ref] callback, [str] arg0, [str] arg1, [str] arg2);
Runs child process
Return: None
##### Enable launching subprocess from cwd
    sys.os.subProcCWD([num] val);
Sets if subprocesses are launched from cwd or from APPS folder.
val: 0 - APPS folder,1 - cwd
Return: None
##### Return data to parent process
    sys.os.subRetval([str] arg0, [str] arg1, [str] arg2);
Sets values that will be returned to parent process
Return: None
#### Sound
##### Beep the speaker
    sys.snd.beep();
Initiates system beep.
Return: None
##### Set beep param to default
    sys.snd.beepDef();
Sets beep to its default values.
Return: None
##### Set time of beep
    sys.snd.beepTime([num]time (~ms));
Sets lenght of beep.
Return: None
##### Set period of beep
    sys.snd.beepFreq([num]frequency (Hz));
Sets frequency of the beep in Hz in range from 27 to 20000.
Return: None
##### Get if system sound is disabled
    sys.snd.getMute();
Returns system mute
Return: [num]1 if system is on mute.
#### Gui
##### Set main application screen
    sys.os.gui.setMainScr([num]id);
Sets main screen to screen with given id
When you wish to display overlay only, set this to 0.
Return: None
##### Get main application screen
    sys.os.gui.getMainScr();
Gets main screen id
Return: [num]id
#### Text field handling
##### Handle text input
    sys.os.gui.handleText([num]id, [str]text);
Handles text input fields. Id is field id. Text is default text value.
Return: [str] New modified text value
#### Text obfuscation
##### Unlock overlay init
    sys.cr.unLockInit();
Creates unlock overlay
Return: [num] overlay ID, 0 when error
##### Unlock overlay update
    sys.cr.update([num] ovId);
Updates unlock overlay
Return: None
##### Unlock overlay get ok
    sys.cr.getOk([num] ovId);
Gets if unlock was successfull
Return: [num] 1 - unlock success, 2 - unlock canceled
##### Unlock overlay clear ok
    sys.cr.clrOk([num] ovId);
Creates unlock overlay
Return: None
##### Get if is locked
    sys.cr.getLock();
Gets if crypto is unlocked
Return: [num] 1 - crypto unlocked, 0 - crypto locked
##### Loads password as a key
    sys.cr.loadPass();
Loads OS password as a key
Return: 0 if success, 1 if error
##### Load custom key string
    sys.cr.loadStr([str]key);
Loads custom string as a crypto key
Return: 0 if success, 1 if error
##### Load custom keyfile
    sys.cr.loadKey([str]keyfile);
Loads custom keyfile as a crypto key
Return: 0 if success, 1 if error
##### Load OS keyfile
    sys.cr.loadOSKey();
Loads OS keyfile as a crypto key
Return: 0 if success, 1 if error
##### Generate keyfile
    sys.cr.genKey([str]keyfile);
Generates custom keyfile.
Return: 0 if success, 1 if error
##### Lock
    sys.cr.lock();
Locks sda encryption
Return: None
##### Encrypt file
    sys.cr.encrypt([str]fname);
Encrypts file.
Return: 0 if success, 1 if error
##### Decrypt file
    sys.cr.decrypt([str]fname);
Encrypts file.
Return: 0 if success, 1 if error

#### Date selector widget

##### Init calendar widget
    sys.w.cal.init([num]year, [num]month, [num]day) #return: cal screen ID
Creates callendar widget screen.
With given year, month and day.
Return: [num]Callendar widget screen id.
##### Select date
    sys.w.cal.select([num]year, [num]month, [num]day);
Sets year, month and day to callendar widget.
Return: None
##### Update
    sys.w.cal.update();
Updates callendar widget.
Return: [num]1 when callendar is clicked.
##### Mark day
    sys.w.cal.mark([num]day);
Marks day in callendar widget.
Return: None
##### Set highlighting
    sys.w.cal.highlight([num]val);
Enable that all buttons except marked are rendered as ghost buttons.
Return: None
##### Get selected day
    sys.w.cal.getDay();
Returns selected day.
Return: [num]day
#### Counters
##### Set counter
    sys.cnt.set([num] ms);
Sets system timer, it counts down and stops at zero.
Return: None
##### Gets counter
    sys.cnt.get();
Gets system timer value
Return: value of system timer
