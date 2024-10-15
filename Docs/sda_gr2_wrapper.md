  Automatically generated documentation for GR2 SVS wrapper, follows markdown syntax.

### GR2 Graphics library


#### Constants

##### Element events
|  Constant     |  Value |  Meaning          |
|  ---          |  ---   |  ---              |
|  EV_PRESSED   |  1     |  Event: pressed   |
|  EV_RELEASED  |  3     |  Event: released  |
|  EV_HOLD      |  2     |  Event: hold      |
|  EV_NONE      |  0     |  Event: none      |

##### UI Colors
|  Constant       |  Value |  Meaning            |
|  ---            |  ---   |  ---                |
|  COL_BORDER     |  1     |  Color: Border      |
|  COL_TEXT       |  2     |  Color: Text        |
|  COL_BACKGROUND |  3     |  Color: Background  |
|  COL_FILL       |  4     |  Color: Fill        |
|  COL_ACTIVE     |  5     |  Color: active      |

##### Text alignment
|  Constant     |  Value |  Meaning            |
|  ---          |  ---   |  ---                |
|  ALIGN_LEFT   |  5     |  Text align: Left   |
|  ALIGN_RIGHT  |  5     |  Text align: Right  |
|  ALIGN_CENTER |  5     |  Text align: Center |


#### Element constructors

##### New screen
    sys.gui.addScreen();
or also
    sys.gui.addScreen([num]x1, [num]y1, [num]x2, [num]y2, [num]scrId);
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
Adds icon. Image must be a file in current working directory.
Optimal resolution is 64x64px. Icon will try to accomodate given space by scaling itself.
use sys.gui.setTxtAlign to change position of the description box.
When parameter of icon element is not zero, color of value param - 1 (16bit RGB565) is drawn as transparent.

Return: [num]id
##### Set icon
    sys.gui.setIcon([num]id, [str]image);
Sets image for given icon. Image must be a file in current working directory, with resolution 64x64px.
When parameter of icon element is not zero, color of value param - 1 (16bit RGB565) is drawn as transparent.

Return: none
##### New image
    sys.gui.addImage([num]x1, [num]y1, [num]x2, [num]y2, [str]fname, [num]scrId);
Creates new image (ppm or p16) container. Name of image is stored in str_value
Size attribute is stored in value, (one by default)
Color of value param - 1 (16bit RGB565) is drawn as transparent.

Return: [num]id
##### New vertical slider
    sys.gui.addSliderV([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new vertical slider.

Return: [num]id
##### New horizontal slider
    sys.gui.addSliderH([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds a new horizontal slider.

Return: [num]id
##### New progress bar
    sys.gui.addBar([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
Adds progress bar. Orientation depends on its dimensions.

Return: [num]id
#### Destructors
    sys.gui.destroy([num]id);
Destroys element with given id.

Return: none
### Getters & setters
#### Modified flag
    sys.gui.setModif([num]Id);
Sets modified flag of an element.
Return: None
#### Element property: Value
##### Get value
    sys.gui.getValue([num]Id);
Gets value of gr2 element.

Return: [num]value
##### Set value
    sys.gui.setValue([num]Id, [num]value);
Sets value of pscg item.

Return: None
#### Element property: Parameter
##### Get parameter
    sys.gui.getParam([num]Id);
Gets elements parameter value.

Return: [num]param
##### Set parameter
    sys.gui.setParam([num]Id, [num]value);
Sets elements parameter value.
Return: None
#### Events Handling

##### Get event
    sys.gui.getEvent([num]Id);
Gets event from an element.

Return: [num] event value, one of EV_ defines.
##### Get event and clear
    sys.gui.getEventC([num]Id);
Gets event from an element and clears the event.

Return: [num] event value, one of EV_ defines.
##### Set event
    sys.gui.setEvent([num]Id, [num] event);
Sets event of an element.

Return: None
##### Clear screen events
    sys.gui.clrScrEv([num]Id);
Clears event for whole screen and its sub-screens.

Return: None
##### Set screen
    sys.gui.setScreen([num]Id, [num]screenId);
Sets element screen parameter.

Return: None
#### Element property: Grayout
##### Get grayout
    sys.gui.getGrayout([num]Id);
Gets element grayout.

Return: [num] grayout, 1 if element is grey, 0 if it is normal
##### Get grayout
    sys.gui.setGrayout([num]Id, [num]grayout);
Sets element grayout.

Return: None
##### Element property: Visibility

##### Get visibility
    sys.gui.getVisible([num]Id);
Gets element visibility.

Return: [num] visibility
##### Set visibility
    sys.gui.setVisible([num]Id, [num]visibility);
Sets element visibility.

Return: None
#### Element property: Ghost
When ghost i s enabled, only button text and its outline is drawn.
##### Get ghost
    sys.gui.getGhost([num]Id);
Gets element ghost parameter.

Return: [num] isGhost
##### Set ghost
    sys.gui.setGhost([num]Id, [num]ghost);
Sets element ghost parameter.

Return: None
#### Element property: Select
##### Get select
    sys.gui.getSelect([num]Id);
Gets element selected parameter.

Return: [num] isSelected
##### Set select
    sys.gui.setSelect([num]Id, [num]select);
Sets element select parameter.

Return: None
#### Slider size
##### Set slider size
    sys.gui.setSliderSize([num]Id, [num]val);
Sets size of slider in pixels.

Return: None
##### Set rounded
    sys.gui.setRounded([num]Id, [num]rounded);
Sets element rounded parameter.

Return: None
#### Element property: String parameter
##### Set String
    sys.gui.getStr([num]Id);
Gets element value_str parameter.

Return: [str]str
##### Get String
    sys.gui.setStr([num]Id, [str]str);
Sets element value_str parameter.

Return: None
##### Keypad control
    sys.gui.getBtnSel([num]screenId);
Gets element selected by the keypad input method
from the given screen.

Return: [num] Id if something is selected, otherwise 0
#### Size and placement
##### Set relative init
    sys.gui.setRelInit([num]val);
Sets aplication gr2 context to relative init mode.
In relative init mode the x2 parameter is used as element width
and the y2 as element height.

Return: None
##### Set rounded init
    sys.gui.setRndInit([num]val);
Sets aplication gr2 context rounded init mode.
In rounded init mode all elements are initialized as rounded type.

Return: None
##### Set screen background redraw
    sys.gui.setBgRedraw([num]screen_id, [num]val);
Enables or disables redrawing background on a screen update.
Usefull for iproving redraw speed in scrolling texts.

Return: None
##### Set elemnent size and position
    sys.gui.setXYXY([num]Id, [num] x1, [num] y1, [num] x2, [num] y2);
Sets position of element inside screen.

Return: None

    sys.gui.setX1([num]Id, [num] x1);
Sets position of element inside screen.

Return: None

    sys.gui.setX2([num]Id, [num] x2);
Sets position of element inside screen.

Return: None

    sys.gui.setY1([num]Id, [num] y1);
Sets position of element inside screen.

Return: None

    sys.gui.setY2([num]Id, [num] y2);
Sets position of element inside screen.

Return: None
##### Get element size and position
    sys.gui.getX1([num]Id);
Gets element position.

Return: [num] x1

    sys.gui.getX2([num]Id);
Gets element position.

Return: [num] x2

    sys.gui.getY1([num]Id);
Gets element position.

Return: [num] y1

    sys.gui.getY2([num]Id);
Gets element position.

Return: [num] y2
#### Screen spacing & cell size
##### Set spacing
    sys.gui.setSpacing([num]Id, [num] left, [num] right, [num] top, [num] bottom);
Sets element spacing atributes of given screen.

Return: None
##### Set cell width
    sys.gui.setXcell([num]screenId, [num] Xcell);
Sets screen Xcell parameter. (32 by default)

Return: None
##### Get cell width
    sys.gui.getXcell([num]screenId);
Gets screen Xcell parameter.

Return: [num] Xcell
##### Set cell height
    sys.gui.setYcell([num]screenId, [num] Ycell);
Sets screen Ycell parameter. (32 by default)

Return: None
##### Get cell height
    sys.gui.getYcell([num]screenId);
Gets screen Ycell parameter.

Return: [num] Ycell
##### Set screen xscroll
    sys.gui.setXscroll([num]screenId, [num]Xscroll);
Sets Xscroll parameter. Determines screen horizontal offset.

Return: None
##### Get screen xscroll
    sys.gui.getXscroll([num]screenId);
Gets screen Xscroll.

Return: [num] Xscroll
##### Set screen yscroll
    sys.gui.setYscroll([num]screenId, [num]Yscroll);
Sets Yscroll parameter. Determines screen vertical offset.

Return: None
##### Get screen yscroll
    sys.gui.getYscroll([num]screenId);
Gets Yscroll parameter.

Return: [num] Yscroll
#### Colors
#### Sets gr2 context color
    sys.gui.setColor([num]Col, [num]val);
Sets given color to given value (16bit RGB565).
Available system colors: COL_BORDER, COL_TEXT, COL_BACKGROUND, COL_FILL, COL_ACTIVE

Return: None
#### Gets gr2 context color
    sys.gui.getColor([num]Col);
Gets value of given color define.
Available system colors: COL_BORDER, COL_TEXT, COL_BACKGROUND, COL_FILL, COL_ACTIVE

Return: [num]val
#### Global text modificators
 Functions that works on all elements that display text.
##### Set text size
    sys.gui.setTxtSize([num]Id, [num]val);
Sets size of text inside buttons or text fields.
Possible values are those used by LCD_Set_Sys_Font
By default they are: 12, 18 (default), 32, 70, 87

Return: None
##### Get text size
    sys.gui.getTxtSize([num]Id);
Gets size of text inside buttons or text fields.

Return: [num] font_size
##### Set text alignment
    sys.gui.setTxtAlign([num]Id, [num]val);
    sys.gui.setTexAlign([num]Id, [num]val); # TBR
Sets text alignment. (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)

Return: None
##### Set text alignment
    sys.gui.getTxtAlign([num]Id);
    sys.gui.getTexAlign([num]Id); # TBR
Gets text alignment.

Return: [num]alignment (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
#### Misc
##### Set default text size for a screen
    sys.gui.defTxtSize([num]screenId, [num] val);
    sys.gui.setDefFont([num]screenId, [num] val); # TBR
Sets defalt screen text size.

Return: None
#### Text element modificators
 Functions that work only on text element.
#####  Set text field as password
    sys.gui.setTxtPwd([num]Id, [num]val);
    sys.gui.setTexPwd([num]Id, [num]val); # TBR
Sets text field as password field. Draws stars instead of characters.

Return: None
##### Get password value
    sys.gui.getTxtPwd([num]Id);
    sys.gui.getTexPwd([num]Id); # TBR
Gets if text field is a password field.

Return: [num]isPassword
##### Set text fit
    sys.gui.setTxtFit([num]Id, [num]val);
    sys.gui.setTexFit([num]Id, [num]val); # TBR
Sets automatic line-breaking. val: 1 - enabled, 0 - disabled

Return: None
##### Set text fit
    sys.gui.setTxtEd([num]Id, [num]val);
    sys.gui.setTexEd([num]Id, [num]val); # TBR
Sets text field as editable.

Return: None
#### Text element activation/deactivation
##### Set text active
    sys.gui.setTxtAct([num]Id);
    sys.gui.setTexAct([num]Id); # TBR
Sets given editable text field as currently active.

Return: None
##### Get text active
    sys.gui.getTxtAct([num]Id);
    sys.gui.getTexAct([num]Id);
Gets if given editable text field is currently active.

Return: [num]isActive
##### Deactivate active text
    sys.gui.txtDeact();
    sys.gui.texDeact(); # TBR
Deactivates currently active text field.

Return: None
#### Text block functions
##### Enable block select mode
    sys.gui.setTxtBlk([num]Id, [num]val);
    sys.gui.setTexBlk([num]Id, [num]val); # TBR
Enables block selection in a text field.
This is enabled by default, so it's mainly for disabling block mode.

Return: None
##### Set text block
    sys.gui.setBlk([num] id, [num] start, [num] stop);
Sets start and stop of a block in active text field

Return: None
##### Get block start
    sys.gui.getBlkStart([num] id);
Gets text field block start.

Return: [num] block_start
##### Get block end
    sys.gui.getBlkEnd([num] id);
Gets text field block end.

Return: [num] block_end
