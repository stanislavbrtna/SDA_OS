#### Gui
##### Set main application screen
    sys.os.gui.setMainScr([num]id);
Sets main screen to screen with given id.
When you wish to display overlay only, set this to 0.

Return: None
##### Get main application screen
    sys.os.gui.getMainScr();
Gets main screen id

Return: [num]id
##### Set root for redraw
    sys.os.gui.setRoot([num]in_apps, [str]dir);
Sets custom root directory for the redraw function.
All paths for icons and other images pased to gui functions will
use this folder as a root.

Return: None
##### Handle keypad input of a screen
    sys.os.gui.btnCtrl([num]screen_id, [num]back_btn_id);
Allows control of a given screen via buttons.
Element given as back_btn_id will be linked with back button.
When 0 is passed instead of id, back button will bring the user on the SDA_OS main screen.

Return: None
##### Selects element for keyboard control
    sys.os.gui.btnSelect([num]element_id);
Selects elemenet for keypad control.
If the desired element is on a sub-screen of an button controlled screen,
then the sub-screen must be also selected.

Return: None
##### Gets element selected by keyboard control
    sys.os.gui.btnGetSel([num]screen_id);
Gets selected element in current screen (or its sub-screens)

Return: [num] element_id
##### Clear button control for a screen
    sys.os.gui.btnClear([num]screen_id);
Clears keypad input for entire screen.

Return: None
#### Text field handling
##### Handle text input
    sys.os.gui.handleText([num]id, [str]text);
    sys.os.gui.handleText([num]id);
Handles text input fields. Id is field id. Text is default text value.

Return: [str] New modified text value

Usage:
    string_val = sys.os.gui.handleText([num]id, string_val);
This is usefull if you are using string_val in every loop, handleText
will keep the variable and the string in sync.

Alternative usage:
    sys.os.gui.handleText([num]id);
This will handle the text field and text value can be retrieved
with *sys.gui.getString*.
Note: the text value is still stored in SVS string memory.

##### Set keyboard string
    sys.os.gui.setKbdStr([str] string);
Sets the current keyboard string (max 63 chars)
Backspace code is "\b", delete is "\bd"

Return: [num] 1 - ok, 0 - string too long
##### Paste clipboard
    sys.os.gui.pasteClipboard();
Pastes clipboard into active text field

Return: none
##### Get text cursor position
    sys.os.gui.getCPos([num] id);
Gets the cursor position of a text field

Return: [num]id
##### Set text cursor position
    sys.os.gui.setCPos([num] id, [num]val);
Sets the cursor position of a text field
 
Return: [num]id
##### Switch between landscape and portrait mode
    sys.os.gui.setLandscape([num]val);
Sets the orientation of the display.
1 - Landscape
0 - Portrait
Return: none
##### Get display orientation
    sys.os.gui.getLandscape();
Gets the orientation of the display.
1 - Landscape
0 - Portrait
Return: [num]val 
#### Notification area icons
##### Set notification area icon
    sys.os.gui.setNotif([str] path_to_sic, [str] callback);
Sets the notification area icon. Path is dependent on actual CWD.


Return: [num] id (1 - 3), 0 - Error, probably no empty icon spot
##### Free notification area icon
    sys.os.gui.freeNotif([num] id);
Removes notification icon with given id.

Return: [num] 1 - ok, 0 - Error
#### Default Icons
 List of icons
| Define       | Descrition            |
|   ---        |  ---                  |
| ICON_NONE    | Box with questionmark |
| ICON_BACK    | Back arrow            |
| ICON_FORWARD | Forward arrow         |

Hint: Unknown icons are drawn as ICON_NONE 
##### Add an os icon to a button 
    sys.os.gui.setIcon([num] id, [num] icon_define);
Adds an icon to a given button. Icons are specified with system defines.

Return: none
