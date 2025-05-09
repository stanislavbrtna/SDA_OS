  Automatically generated documentation of SDA OS system functions

 - [OS Gui](sda_os_gui.md)
 - [Sound](sda_os_sound.md)
 - [Callendar widget](sda_os_widgets.md)
 - [Text obfuscation](sda_os_crypto.md)

### Basic Application structure

    function init {
      screen = sys.gui.addScreen();
      sys.gui.addText(1, 1, 2, 6, "Hello, world!", screen);
      sys.os.gui.setMainScr(screen);
    }
    
    function update {
      # empty in this example
    }

#### Required functions
Each application must implement all required functions.
##### Init function
    function init {}
Init is called once when the app is loaded.
arg0 - arg2: call arguments passed from sys.os.subProcess

##### Update function
    function update
Update function is called each update cycle, when the app
is active and in the foreground.

#### Optional functions
User might also implement optional functions. OS will call these functions under circumstances defined below.

##### Exit function
    function exit {}
Exit function is called upon exiting the app.  

##### Suspend function
    function suspend {}
Suspend function is called when the app is suspended.

##### Wakeup function
    function wakeup {}
Wakeup is called when the app is woken from suspend state.
If the application process was set as a singular, and the app was woken from sys.os.subProcess call,
init arguments are passed to the wakeup function.

#### SDA API Level history

API level given by *sys.os.getVer* etc. works like this: 
10510 - SDA version 1.5.1 with all its features

Note: before 1.4 sub-version used only one digit
1120 - SDA version 1.1.2 with all its features

#### Constants
| Constant     | Description      |
|   ---        |    ---           |
| SVP_LANG_CZ  | Czech language   |
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
If called from timer or uart callback, the app is promoted to the foreground.

Return: None
##### Disable app close
    sys.os.noClose([num] enabled);
If enabled, running application is only suspended
when close button is pressed.
Application can still be closed from task manager.

Return: None
##### Get if running in simulator
    sys.os.inSim();
Gets if app is running in simulator. 1 - Simulator, 0 - Real hardware.

Return: [num] result
##### Show Error
    sys.os.error([str]errorText);
Throws error message

Return: None
##### Gets app path
    sys.os.getAppPath();
Gets diretory path of the currently running svs app
includung the name of the app. 

Return: [str] Path
#### Keyboard
##### Show keyboard
    sys.os.showKbd();
Shows system keyboard

Return: None
##### Hide keyboard
    sys.os.hideKbd();
Hides system keyboard.

Return: None
##### Get Keyboard state
    sys.os.kbdGetState();
Gets if keyboard is deployed 1 - keyboard shown, 0 - keyboard hidden

Return: [num] state

#### Misc

##### Get random number
    sys.os.rnd();
Returns random number

Return: [num]RandomValue
##### Suspend app
    sys.os.suspend();
Suspends (minimizes) currently running app.

Return: none
##### Exit app
    sys.os.exit();
    sys.os.exit([undef] arg0, [undef] arg1, [undef] arg2); # optional return values
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
Returns language of the running SDA_OS build.

Return: defines SVP_LANG_CZ (0) or SVP_LANG_ENG(1)
#### Subprocess
##### Set process as singular
    sys.os.setSingular();
Sets current process as singular.

Return: None
##### Launch subprocess
    sys.os.subProcess([str]fileName, [str/ref] callback, [undef] arg0, [undef] arg1, [undef] arg2);
Runs child process with given arguments. 
*fileName* must contain valid path to .svs file located in APPS directory.
*callback* stores name of a function that will be called after child process exits.
Subprocess will be launched after the current application returns from its *update* function.
When strings are passed as arguments,
their total size must not exceed APP_ARG_STR_LEN define (2048 by default).
Also be carefull when passing string expressions, they will be passed
to the child process after the update function returns and could be garbage collected
in the meantime. So prefferably pass string constants or global variables. 

Return: None
##### Enable launching subprocess from cwd
    sys.os.subProcCWD([num] val);
Sets if subprocesses are launched from cwd or from APPS folder.

val:
 | Value: | Description            |
 | ---    | ---                    |
 |    0   | APPS folder (default)  |
 |    1   | CWD                    |

Return: None
##### Disable caching for launched subprocess
    sys.os.subProcNC();
Disables caching for next call of sys.os.subProcess.
Usefull when running modified content and precaching on launch is enabled
in settings.

Return: None
##### Return data to parent process
    sys.os.subRetval([undef] arg0, [undef] arg1, [undef] arg2);
Sets values that will be returned to parent process
When strings are passed as arguments,
their total size must not exceed APP_ARG_STR_LEN define (2048 by default).

Return: None
##### Sets the clipboard string
    sys.os.setClipboard([str] string);
Sets the OS clipboard 256 chars by default

Return: [num] 1 - ok, 0 - string too long
##### Gets the clipboard string
    sys.os.getClipboard();
Gets the OS clipboard 256 chars max by default.

Return: [str] clipboard_string
#### OS settings functions
##### Reload homescreen settings
    sys.os.settings.homeRld();
Reloads homescreen settings stored in homescreen.cfg

Return: none
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
