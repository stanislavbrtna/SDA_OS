  Automatically generated documentation of SDA OS system functions

 - [OS Gui](sda_os_gui.md)
 - [Sound](sda_os_sound.md)
 - [Callendar widget](sda_os_widgets.md)
 - [Text obfuscation](sda_os_crypto.md)

#### SVP API Level history

API level given by *sys.os.getVer* etc. works like this: 
1000 - SDA version 1.0.0 with all its features
 ...
1120 - SDA version 1.1.2 with all its features

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
#### OS settings functions
##### Requests high privileges
    sys.os.settings.rqAuth();
Requests authorization form user to change system settings.
Result can be retrieved with 'sys.os.settings.getAuth();'
Return: None
##### Gets if privileges are granted
    sys.os.settings.getAuth();
Gets if high privileges are granted.
Return: [num] 1 if authorization is given
##### Sets time and date
    sys.os.settings.setTime([num] year, [num] month, [num] day, [num] hour, [num] min);
Sets values that will be returned to parent process
Return: None
