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
##### Get text cursor position
    sys.os.gui.getCPos([num] id);
Gets the cursor position of a text field
Return: [num]id
##### Set text cursor position
    sys.os.gui.setCPos([num] id, [num]val);
Sets the cursor position of a text field
Return: [num]id
