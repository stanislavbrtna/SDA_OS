  Automatically generated documentation for GR2 SVS wrapper, follows markdown syntax.

#### Constants
| Constant | Value | Meaning |
|---|---|---|
|EV_PRESSED|1| Event: pressed|
|EV_RELEASED|3| Event: released|
|EV_HOLD|2| Event: hold|
|EV_NONE|0| Event: none|
|COL_BORDER|1| Color: Border|
|COL_TEXT|2| Color: Text|
|COL_BACKGROUND|3| Color: Background|
|COL_FILL|4| Color: Fill|
|COL_ACTIVE|5| Color: active|
|ALIGN_LEFT|5| Text align: Left|
|ALIGN_RIGHT|5| Text align: Right|
|ALIGN_CENTER|5| Text align: Center|

#### Element constructors

##### New screen
    sys.gui.addScreen();
Creates new screen.
Return: [num]scrId
##### New frame
    sys.gui.addFrame([num]x1, [num]y1, [num]x2, [num]y2, [num]value, [num]scrId);
Creates new pscg frame. Value contains id of screen inside frame.
Return: [num]elementId
##### New text field
    sys.gui.addText([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Adds a new text field.
Return: [num]id
##### New button
    sys.gui.addButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Creates new button.
Return: [num]id
##### New color button
    sys.gui.addCButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Adds color button, color is stored in its value.
Return: [num]id
##### New check box
    sys.gui.addCheckBox([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Creates new checkbox. Checkbox state is stored in its value.
Return: [num]id
##### New icon
    sys.gui.addIcon([num]x1, [num]y1, [num]x2, [num]y2, [str]description, [str]image, [num]scrId);
Adds icon. Image must be a file in current working directory, with resolution 64x64px.
When parameter of icon element is not zero, color of value param - 1 (16bit RGB565) is drawn as transparent.
Return: [num]id
##### New image
    sys.gui.addImage([num]x1, [num]y1, [num]x2, [num]y2, [str]fname, [num]scrId);
Creates new .ppm image container. Name of image is stored in str_value
Size attribute is stored in value, (one by default)
Return: [num]id
##### New vertical slider
    sys.gui.addSliderV([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new vertical slider. (pAddSlider is also accepted)
Return: [num]id
##### New horizontal slider
    sys.gui.addSliderH([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new horizontal slider.
Return: [num]id
##### New progress bar
    sys.gui.addBar([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds progress bar. Orientation depends on bar dimensions.
Return: [num]id
#### Destructors
    sys.gui.destroy([num]id);
Destroys element with given id.
Return: none
#### Getters & setters
##### Value
    sys.gui.getValue([num]Id);
Gets value of pscg item.
Return: [num]value

    sys.gui.setValue([num]Id, [num]value);
Sets value of pscg item.
Return: None
##### Modified flag
    sys.gui.setModif([num]Id);
Sets modified flag of an element.
Return: None
##### Parameter
    sys.gui.getParam([num]Id);
Gets elements parameter value.
Return: [num]param

    sys.gui.setParam([num]Id, [num]value);
Sets elements parameter value.
Return: None
##### Events
    sys.gui.getEvent([num]Id);
Gets event from an element.
Return: [num] event value, one of EV_ defines.

    sys.gui.getEventC([num]Id);
Gets event from an element and clears the event.
Return: [num] event value, one of EV_ defines.

    sys.gui.setEvent([num]Id, [num] event);
Sets event of an element.
Return: None

    sys.gui.clrScrEv([num]Id);
Clears event for whole screen and its sub-screens.
Return: None
##### Screen
    sys.gui.setScreen([num]Id, [num]screenId);
Sets element screen parameter.
Return: None
##### Grayout
    sys.gui.getGrayout([num]Id);
Gets element grayout.
Return: [num] grayout, 1 if element is grey, 0 if it is normal

    sys.gui.setGrayout([num]Id, [num]grayout);
Sets element grayout.
Return: None
##### Visibility
    sys.gui.getVisible([num]Id);
Gets element visibility.
Return: [num] visibility

    sys.gui.setVisible([num]Id, [num]visibility);
Sets element visibility.
Return: None
##### Ghost buttons
    sys.gui.getGhost([num]Id);
Gets element ghost parameter.
Return: [num] isGhost

    sys.gui.setGhost([num]Id, [num]ghost);
Sets element ghost parameter.
Return: None
##### Selected buttons
    sys.gui.getSelect([num]Id);
Gets element selected parameter.
Return: [num] isSelected

    sys.gui.setSelect([num]Id, [num]select);
Sets element select parameter.
Return: None
##### String parameter
    sys.gui.getStr([num]Id);
Gets element value_str parameter.
Return: [str]str

    sys.gui.setStr([num]Id, [str]str);
Sets element value_str parameter.
Return: None

    sys.gui.setTxtSize([num]Id, [num]val);
Sets size of text inside buttons or text fields.
Possible values are those used by LCD_Set_Sys_Font
By default they are: 12, 18, 32, 70, 87
Return: None
##### Size and placement

    sys.gui.setRelInit([num]val);
Sets aplication gr2 context to relative init mode.
Return: None

    sys.gui.setXYXY([num]Id, [num] x1, [num] y1, [num] x2, [num] y2);
Sets position of element inside screen.
Return: None

    sys.gui.setSpacing([num]Id, [num] left, [num] right, [num] top, [num] bottom);
Sets element spacing atributes of given screen.
Return: None

    sys.gui.getXcell([num]screenId);
Gets screen Xcell parameter.
Return: [num] Xcell

    sys.gui.setXcell([num]screenId, [num] Xcell);
Sets screen Xcell parameter. (32 by default)
Return: None

    sys.gui.getYcell([num]screenId);
Gets screen Ycell parameter.
Return: [num] Ycell

    sys.gui.setYcell([num]screenId, [num] Ycell);
Sets screen Ycell parameter. (32 by default)
Return: None

    sys.gui.getXscroll([num]screenId);
Gets screen Xscroll.
Return: [num] Xscroll

    sys.gui.setXscroll([num]screenId, [num]Xscroll);
Sets Xscroll parameter.
Return: None

    sys.gui.getYscroll([num]screenId);
Gets Yscroll parameter.
Return: [num] Yscroll

    sys.gui.setYscroll([num]screenId, [num]Yscroll);
Sets Yscroll parameter.
Return: None
##### Fonts & texts
    sys.gui.setDefFont([num]screenId, [num] val);
Sets defalt screen font.
Return: None

    sys.gui.getTexAct([num]Id);
Gets if given editable text field is currently active.
Return: [num]isActive

    sys.gui.setTexAct([num]Id);
Sets given editable text field as currently active.
Return: None

    sys.gui.setTexFit([num]Id, [num]val);
Sets automatic line-breaking on.
Return: None

    sys.gui.setTexEd([num]Id, [num]val);
Sets text field as editable.
Return: None

    sys.gui.setTexPwd([num]Id, [num]val);
Sets text field as password field.
Return: None

    sys.gui.getTexPwd([num]Id);
Gets if text field is a password field.
Return: [num]isPassword

    sys.gui.setTexAlign([num]Id, [num]val);
Sets text alignment. (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
Return: None

    sys.gui.getTexAlign([num]Id);
Gets text alignment.
Return: [num]alignment (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
##### Colours

    sys.gui.setColor([num]Col, [num]val);
Sets given color to given value.
Return: None

    sys.gui.getColor([num]Col);
Gets value of given color define.
Return: [num]val
