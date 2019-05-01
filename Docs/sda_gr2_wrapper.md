	Automatically generated documentation for GR2 SVS wrapper, follows markdown syntax.

#### Constants
| Constant | Value | Meaning |
|---|---|---|
|EV_PRESSED|1| Event: pressed|
|EV_RELEASED|3| Event: released|
|EV_HOLD|2| Event: hold|
|EV_NONE|0| Event: none|
|COL_BORDER|1| Event: none|
|COL_TEXT|2| Event: none|
|COL_BACKGROUND|3| Event: none|
|COL_FILL|4| Event: none|
|COL_ACTIVE|5| Event: none|

#### Element constructors

##### New screen
    sys.pAddScreen();
Creates new screen.
Return: [num]scrId
##### New frame
    sys.pAddFrame([num]x1, [num]y1, [num]x2, [num]y2, [num]value, [num]scrId);
Creates new pscg frame. Value contains id of screen inside frame.
Return: [num]elementId
##### New text field
    sys.pAddText([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Adds a new text field.
Return: [num]id
##### New button
    sys.pAddButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Creates new button.
Return: [num]id
##### New color button
    sys.pAddCButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Adds color button, color is stored in its value.
Return: [num]id
##### New check box
    sys.pAddCheckBox([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Creates new checkbox. Checkbox state is stored in its value.
Return: [num]id
##### New image
    sys.pAddImage([num]x1, [num]y1, [num]x2, [num]y2, [str]fname, [num]scrId);
Creates new .ppm image container. Name of image is stored in str_value
Size attribute is stored in value, (one by default)
Return: [num]id
##### New vertical slider
    sys.pAddSliderV([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new vertical slider. (pAddSlider is also accepted)
Return: [num]id
##### New horizontal slider
    sys.pAddSliderH([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new horizontal slider.
Return: [num]id
##### New progress bar
    sys.pAddBar([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds progress bar. Orientation depends on bar dimensions.
Return: [num]id
#### Destructors
    sys.pDestroy([num]id);
Destroys element with given id.
Return: none
#### Getters & setters
##### Value
    sys.pGetValue([num]Id);
Gets value of pscg item.
Return: [num]value

    sys.pSetValue([num]Id, [num]value);
Sets value of pscg item.
Return: None
##### Modified flag
    sys.pSetModif([num]Id);
Sets modified flag of an element.
Return: None
##### Parameter
    sys.pGetParam([num]Id);
Gets elements parameter value.
Return: [num]param

    sys.pSetParam([num]Id, [num]value);
Sets elements parameter value.
Return: None
##### Events
    sys.pGetEvent([num]Id);
Gets event from an element.
Return: [num] event value, one of EV_ defines.

    sys.pGetEventC([num]Id);
Gets event from an element and clears the event.
Return: [num] event value, one of EV_ defines.

    sys.pSetEvent([num]Id, [num] event);
Sets event of an element.
Return: None

    sys.pClrScrEv([num]Id);
Clears event for whole screen and its sub-screens.
Return: None
##### Screen
    sys.pSetScreen([num]Id, [num]screenId);
Sets element screen parameter.
Return: None
##### Grayout
    sys.pGetGrayout([num]Id);
Gets element grayout.
Return: [num] grayout, 1 if element is grey, 0 if it is normal

    sys.pSetGrayout([num]Id, [num]grayout);
Sets element grayout.
Return: None
##### Visibility
    sys.pGetVisible([num]Id);
Gets element visibility.
Return: [num] visibility

    sys.pSetVisible([num]Id, [num]visibility);
Sets element visibility.
Return: None
##### Ghost buttons
    sys.pGetGhost([num]Id);
Gets element ghost parameter.
Return: [num] isGhost

    sys.pSetGhost([num]Id, [num]ghost);
Sets element ghost parameter.
Return: None
##### Selected buttons
    sys.pGetSelect([num]Id);
Gets element selected parameter.
Return: [num] isSelected

    sys.pSetSelect([num]Id, [num]select);
Sets element select parameter.
Return: None
##### String parameter
    sys.pGetStr([num]Id);
Gets element value_str parameter.
Return: [str]str

    sys.pSetStr([num]Id, [str]str);
Sets element value_str parameter.
Return: None

    sys.pSetTxtSize([num]Id, [num]val);
Sets size of text inside buttons or text fields.
Possible values are those used by LCD_Set_Sys_Font
By default they are: 18, 32, 70, 87
Return: None
##### Size and placement

    sys.pSetRelInit([num]val);
Sets aplication gr2 context to relative init mode.
Return: None

    sys.pSetXYXY([num]Id, [num] x1, [num] y1, [num] x2, [num] y2);
Sets position of element inside screen.
Return: None

    sys.pSetSpacing([num]Id, [num] left, [num] right, [num] top, [num] bottom);
Sets element spacing atributes of given screen.
Return: None

    sys.pGetXcell([num]screenId);
Gets screen Xcell parameter.
Return: [num] Xcell

    sys.pSetXcell([num]screenId, [num] Xcell);
Sets screen Xcell parameter. (32 by default)
Return: None

    sys.pGetYcell([num]screenId);
Gets screen Ycell parameter.
Return: [num] Ycell

    sys.pSetYcell([num]screenId, [num] Ycell);
Sets screen Ycell parameter. (32 by default)
Return: None

    sys.pGetXscroll([num]screenId);
Gets screen Xscroll.
Return: [num] Xscroll

    sys.pSetXscroll([num]screenId, [num]Xscroll);
Sets Xscroll parameter.
Return: None

    sys.pGetYscroll([num]screenId);
Gets Yscroll parameter.
Return: [num] Yscroll

    sys.pSetYscroll([num]screenId, [num]Yscroll);
Sets Yscroll parameter.
Return: None
##### Fonts & texts
    sys.pSetDFont([num]screenId, [num] val);
Sets defalt screen font.
Return: None

    sys.pGetTexAct([num]Id);
Gets if given editable text field is currently active.
Return: [num]isActive

    sys.pSetTexAct([num]Id);
Sets given editable text field as currently active.
Return: None

    sys.pSetTexFit([num]Id, [num]val);
Sets automatic line-breaking on.
Return: None

    sys.pSetTexEd([num]Id, [num]val);
Sets text fiels as editable.
Return: None
##### Colours

    sys.pSetColor([num]Col, [num]val);
Sets given color to given value.
Return: None

    sys.pGetColor([num]Col);
Gets value of given color.
Return: [num]val
