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
##### Set root for redraw
    sys.os.gui.setRoot([num]in_apps, [str]dir);
Sets custom root directory for the redraw function.
Return: None
##### Handle keypad input of a screen
    sys.os.gui.btnCtrl([num]screen_id, [num]back_btn_id);
Allows control of a given screen via buttons.
Element given as back_btn_id will be linked with back button,
otherwise back button will bring the user on the SDA_OS main screen.
Return: None
#### Text field handling
##### Handle text input
    sys.os.gui.handleText([num]id, [str]text);
Handles text input fields. Id is field id. Text is default text value.
Return: [str] New modified text value
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
