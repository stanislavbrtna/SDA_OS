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
| LED_ON | Nonification led on |
| LED_OFF | Nonification led off |
| LED_BLINK | Nonification led pattern |
| LED_SHORTBLINK | Nonification led pattern |
| LED_ALARM | Nonification led pattern |
| BTN_A | Button define |
| BTN_LEFT | Button define |
| BTN_UP | Button define |
| BTN_DOWN | Button define |
| BTN_RIGHT | Button define |
| BTN_B | Button define |
| PIN_IN | Expansion pin setup define |
| PIN_OUT | Expansion pin setup define |
| PIN_ALT | Expansion pin setup define |
| PIN_NOPULL | Expansion pin setup define |
| PIN_PULLUP | Expansion pin setup define |
| PIN_PULLDOWN | Expansion pin setup define |

#### Gui
##### Set main application screen
    sys.pSetMainScr([num]id);
Sets main screen to id
Return: None
##### Get main application screen
    sys.pGetMainScr();
Gets main screen id
Return: [num]id
##### Get redraw flag
    sys.pGetRedraw();
Gets redraw flag. *getRedraw* also works.
Return: [num] 1 if redraw flag is set, otherwise 0
##### Set redraw
    sys.pSetRedraw();
Sets redraw flag
Return: None
##### Show Error
    sys.error([str]errorText);
Throws error message
Return: None
#### Counters
##### Set counter
    sys.cntSet([num] ms);
Sets system timer, it counts down and stops at zero.
Return: None
##### Gets counter
    sys.cntGet();
Gets system timer value
Return: value of system timer
#### Buttons
##### Get button event
    sys.btnGetEv([num] btn)
Return last button event
Return: 0 - none, 1-pressed, 2 - hold, 3 - released
##### Clears button events
    sys.btnClrEv([num]btn)
Sets button event to EV_NONE
Return: None
##### Enable button events with LCD off
    sys.btnStdbyEn([num]val)
Enables button readout with LCD off
Return: None
#### Text field handling
##### Handle text input
    sys.pHandleText([num]id, [str]text);
Handles text input fields. Id is field id. Text is default text value.
Return: [str] New modified text value
#### Keyboard
##### Hide keyboard
    sys.keybHide();
Hides system keyboard.
Return: None
##### Show keyboard
    sys.keybShow();
Shows system keyboard
Return: None
#### Misc
##### Get random number
    rnd();
Returns random number
Return: [num]RandomValue
##### Get if system sound is disabled
    sys.getMute();
Returns system mute
Return: [num]1 if system is on mute.

#### Date selector widget

##### Init calendar widget
    sys.wCalInit([num]year, [num]month, [num]day) #return: cal screen ID
Creates callendar widget screen.
With given year, month and day.
Return: [num]Callendar widget screen id.
##### Select date
    sys.wCalSelect([num]year, [num]month, [num]day);
Sets year, month and day to callendar widget.
Return: None
##### Update
    sys.wCalUpdate();
Updates callendar widget.
Return: [num]1 when callendar is clicked.
##### Mark day
    sys.wCalMark([num]day);
Marks day in callendar widget.
Return: None
##### Get selected day
    sys.wCalGetDay();
Returns selected day.
Return: [num]day

#### Misc

##### Lock LCD sleep
    sys.sleepLock([num]val);
Sets sleep lock value. On 1 system wont go to sleep.
Return: None
##### Turn on the LCD
    sys.lcdWake();
Wakes LCD screen.
Return: None
##### Get LCD state
    sys.getLcdState();
Gets state of lcd.
Return: 1 if lcd is on, otherwise 0
##### Wake the SDA from sleep
    sys.sdaWake();
Wakes SDA without turning the screen on.
SDA will wake in the low power mode and will sleep again after the lcd shutdown time.
Return: None
##### Quit program
    sys.exit();
Stops program execution after exiting *update* function and performing *exit* function.
Return: None
##### Check API level
    sys.checkSVSVer([num] API_Level);
Checks for API Lvl support.
If host level is below given API_Level, error is thrown and app is terminated.
Return: None
##### Get API level
    sys.getSVSVer();
Checks for API Lvl support.
Return: None
##### Get system language
    sys.sdaGetLang();
Returns SDA_OS language.
Return: 0 if czech, 1 if english, also defines SVP_LANG_CZ SVP_LANG_ENG
#### Text obfuscation
##### Unlock
    sys.crUnLock([str]password);
Unlocks svp encryption
Return: [num] 0 if success, 2 if error, 3 if locked
##### Lock
    sys.crLock();
Locks svp encryption
Return: None
##### Encrypt file
    sys.crEncrypt([str]fname);
Encrypts file.
Return: 0 if success, 1 if error
##### Decrypt file
    sys.crDecrypt([str]fname);
Encrypts file.
Return: 0 if success, 1 if error
#### Subprocess
##### Set process as singular
    sys.setSingular();
Sets current process as singular.
Return: None
##### Launch subprocess
    sys.subProcess([str]fileName, [str/ref] callback, [str] arg0, [str] arg1, [str] arg2);
Runs child process
Return: None
##### Enable launching subprocess from cwd
    sys.subProcCWD([num] val);
Sets if subprocesses are launched from cwd or from APPS folder.
val: 0 - APPS folder,1 - cwd
Return: None
##### Return data to parent process
    sys.subRetval([str] arg0, [str] arg1, [str] arg2);
Sets values that will be returned to parent process
Return: None
#### Sound and notification LED
##### Beep the speaker
    sys.beep();
Initiates system beep.
Return: None
##### Set beep param to default
    sys.beepDef();
Sets beep to its default values.
Return: None
##### Set time of beep
    sys.beepT([num]time (~ms));
Sets lenght of beep.
Return: None
##### Set period of beep
    sys.beepF([num]period of ~20kHz pulses to one pulse);
Sets period of ~20kHz pulses to one pulse.
Return: None
#### Expansion Ports
##### Serial expansion transmit
    sys.serialTrs([str]data);
Sends given string to serial port on internal or external expansion connector.
Depends on what is initialized.
Return: None
##### Serial expansion transmit queue
    sys.srlTrsQAdd([num]data);
Queues given hex value to transmit buffer.
Max 32 bytes.
Return: [num] 1-ok, 0-error
##### Serial expansion transmit queue clear
    sys.srlTrsQClr();
Clears transmit buffer.
Return: None
##### Serial expansion transmit queue
    sys.srlTrsQ();
Sends previously stored queue to the initialized serial port.
Max 32 bytes.
Return: None
##### Serial expansion recieve
    sys.serialRcv([num]timeout);
Gets string (max 512 bytes) from currently initialized serial port.
If nothing is sent during timeout (in ms), empty string is returned.
Return: [str] data
##### Set notification led pattern
    sys.sdaSetLed([num]led_type);
Sets notification led to a given pattern, uses:
LED_ON, LED_OFF ,LED_BLINK, LED_SHORTBLINK, LED_ALARM
Return: None
##### Define direction of pins on the internal expansion
    sys.iPinDef([num]Pin, [num]type, [num]pullUp);
Sets direction of internal expansion pins.
Uses defines: PIN_IN, PIN_OUT, PIN_ALT, PIN_NOPULL, PIN_PULLUP, PIN_PULDOWN
Pin number is number of pin on the connector, can be read from schematics.
Return: None
##### Set state of pins on the internal expansion
    sys.iPinSet([num]Pin, [num]val);
Sets state of internal expansion pin.
Value 1 sets the pin high, value 0 sets it low.
Pin number is number of pin on the connector, can be read from schematics.
Return: None
##### Get state of pins on the internal expansion
    sys.iPinGet([num]Pin, [num]val);
Gets state of internal expansion pin.
Pin number is number of pin on the connector, can be read from schematics.
Return: 1 if the pin is high, 0 if it is low.
##### Define direction of pins on the expansion
    sys.ePinDef([num]Pin, [num]type, [num]pullUp);
Sets direction of external expansion pins.
Uses defines: PIN_IN, PIN_OUT, PIN_ALT, PIN_NOPULL, PIN_PULLUP, PIN_PULDOWN
Pin number is number of pin on the connector, can be read from schematics.
Return: None
##### Set state of pins on the expansion
    sys.ePinSet([num]Pin, [num]val);
Sets state of external expansion pin.
Value 1 sets the pin high, value 0 sets it low.
Pin number is number of pin on the connector, can be read from schematics.
Return: None
##### Get state of pins on the expansion
    sys.ePinGet([num]Pin, [num]val);
Gets state of external expansion pin.
Pin number is number of pin on the connector, can be read from schematics.
Return: 1 if the pin is high, 0 if it is low.
##### Get ADC readout
    sys.eADCRead();
Gets state of external expansion pin.
Pin number is number of pin on the connector, can be read from schematics.
Return: [float] measured voltage in volts.
