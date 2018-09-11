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
    pAddScreen();
Creates new screen.
Return: [num]scrId
##### New frame
    pAddFrame([num]x1, [num]y1, [num]x2, [num]y2, [num]value, [num]scrId);
Creates new pscg frame. Value contains id of screen inside frame.
Return: [num]elementId
##### New text field
    pAddText([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Adds a new text field.
Return: [num]id
##### New button
    pAddButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Creates new button.
Return: [num]id
##### New color button
    pAddCButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Adds color button, color is stored in its value.
Return: [num]id
##### New check box
    pAddCheckBox([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
Creates new checkbox. Checkbox state is stored in its value.
Return: [num]id
##### New image
    pAddImage([num]x1, [num]y1, [num]x2, [num]y2, [str]fname, [num]scrId);
Creates new .ppm image container. Name of image is stored in str_value
Size attribute is stored in value, (one by default)
Return: [num]id
##### New vertical slider
    pAddSliderV([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new vertical slider. (pAddSlider is also accepted)
Return: [num]id
##### New horizontal slider
    pAddSliderH([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new horizontal slider.
Return: [num]id
##### New progress bar
    pAddBar([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds progress bar. Orientation depends on bar dimensions.
Return: [num]id
#### Destructors
    pDestroy([num]id);
Destroys element with given id.
Return: none

    pDestroyScr([num]scrId);
Destroy screen with given id (with all its sub-screens). To be removed.
Return: None
#### Getters & setters
##### Value
    pGetValue([num]Id);
Gets value of pscg item.
Return: [num]value

    pSetValue([num]Id, [num]value);
Sets value of pscg item.
Return: None
##### Modified flag
    pSetModif([num]Id);
Sets modified flag of an element.
Return: None
##### Parameter
    pGetParam([num]Id);
Gets elements parameter value.
Return: [num]param

    pSetParam([num]Id, [num]value);
Sets elements parameter value.
Return: None
##### Events
    pGetEvent([num]Id);
Gets event from an element.
Return: [num] event value, one of EV_ defines.

    pGetEventC([num]Id);
Gets event from an element and clears the event.
Return: [num] event value, one of EV_ defines.

    pSetEvent([num]Id, [num] event);
Sets event of an element.
Return: None

    pClrScrEv([num]Id);
Clears event for whole screen and its sub-screens.
Return: None
##### Screen
    pSetScreen([num]Id, [num]screenId);
Sets element screen parameter.
Return: None
##### Grayout
    pGetGrayout([num]Id);
Gets element grayout.
Return: [num] grayout, 1 if element is grey, 0 if it is normal

    pSetGrayout([num]Id, [num]grayout);
Sets element grayout.
Return: None
##### Visibility
    pGetVisible([num]Id);
Gets element visibility.
Return: [num] visibility

    pSetVisible([num]Id, [num]visibility);
Sets element visibility.
Return: None
##### Ghost buttons
    pGetGhost([num]Id);
Gets element ghost parameter.
Return: [num] isGhost

    pSetGhost([num]Id, [num]ghost);
Sets element ghost parameter.
Return: None
##### Selected buttons
    pGetSelect([num]Id);
Gets element selected parameter.
Return: [num] isSelected

    pSetSelect([num]Id, [num]select);
Sets element select parameter.
Return: None
##### String parameter
    pGetStr([num]Id);
Gets element value_str parameter.
Return: [str]str

    pSetStr([num]Id, [str]str);
Sets element value_str parameter.
Return: None

    pSetTxtSize([num]Id, [num]val);
Sets size of text inside buttons or text fields.
Possible values are those used by LCD_Set_Sys_Font
By default they are: 18, 32, 70, 87
Return: None
##### Size and placement

    pSetRelInit([num]val);
Sets aplication gr2 context to relative init mode.
Return: None

    pSetXYXY([num]Id, [num] x1, [num] y1, [num] x2, [num] y2);
Sets position of element inside screen.
Return: None

    pGetXcell([num]screenId);
Gets screen Xcell parameter.
Return: [num] Xcell

    pSetXcell([num]screenId, [num] Xcell);
Sets screen Xcell parameter. (32 by default)
Return: None

    pGetYcell([num]screenId);
Gets screen Ycell parameter.
Return: [num] Ycell

    pSetYcell([num]screenId, [num] Ycell);
Sets screen Ycell parameter. (32 by default)
Return: None

    pGetXscroll([num]screenId);
Gets screen Xscroll.
Return: [num] Xscroll

    pSetXscroll([num]screenId, [num]Xscroll);
Sets Xscroll parameter.
Return: None

    pGetYscroll([num]screenId);
Gets Yscroll parameter.
Return: [num] Yscroll

    pSetYscroll([num]screenId, [num]Yscroll);
Sets Yscroll parameter.
Return: None
##### Fonts & texts
    pSetDFont([num]screenId, [num] val);
Sets defalt screen font.
Return: None

    pGetTexAct([num]Id);
Gets if given editable text field is currently active.
Return: [num]isActive

    pSetTexAct([num]Id);
Sets given editable text field as currently active.
Return: None

    pSetTexFit([num]Id, [num]val);
Sets automatic line-breaking on.
Return: None

    pSetTexEd([num]Id, [num]val);
Sets text fiels as editable.
Return: None
##### Colours

    pSetColor([num]Col, [num]val);
Sets given color to given value.
Return: None

    pGetColor([num]Col);
Gets value of given color.
Return: [num]val
