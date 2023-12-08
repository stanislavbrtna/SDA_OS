#### Overlay API
##### Set overlay screen
    sys.o.setScr([num]screen_id);
Sets overlay screen, returns overlay id.

Return: [num] Overlay id
##### Get overlay id
    sys.o.getId();
Gets id of current overlay.

Return: [num] OverlayId
##### Destroy overlay
    sys.o.destroy();
Destroys current overlay, also destroys its screen.

Return: None
##### Set position and size of overlay
    sys.o.setXYXY([num]x1, [num]y1, [num]x2, [num]y2);
Sets position and size of current overlay (in screen pixels).

Return: None
##### Set overlay height
    sys.o.setY([num]val);
Sets lower coordinates of current overlay (in screen pixels).

Return: None

#### Date overlay

##### Create date overlay
    sys.o.date.add([num]year, [num]month, [num]day);
Creates date overlay id, returns id

Return: [num]Date overlay id
##### Update date overlay
    sys.o.date.update([num]id);
Updates date overlay

Return: None
##### Get ok from overlay
    sys.o.date.getOk([num]id);
Gets if overlay ok button was pressed.

Return: 1 if overlay ok button was pressed
##### Clear ok from overlay
    sys.o.date.clrOk([num]id);
Clears ok flag from overlay

Return: None
##### Get year form overlay
    sys.o.date.getYr([num]id);
Gets year from overlay with given id.

Return: [num]Year
##### Get day from overlay
    sys.o.date.getDay([num]id);
Gets day from overlay with given id.

Return: [num]Day
##### Get month from overlay
    sys.o.date.getMon([num]id);
Gets month from overlay with given id.

Return: [num]Month

#### Time overlay

##### Create time overlay
    sys.o.time.add();
    sys.o.time.add([num]hr, [num]min);
Adds a time overlay, returns its id

Return: [num]overlay id
##### Set time overlay time
    sys.o.time.set([num]overlay_id, [num]hr, [num]min);
Sets time in overlay with given id.

Return: None
##### Update time overlay
    sys.o.time.update([num]overlay_id);
Updates time overlay.

Return: None
##### Get time overlay ok
    sys.o.time.getOk([num]overlay_id);
Gets ok from time overlay.

Return: 1 if ok was pressed
##### Get time overlay minutes
    sys.o.time.getMin([num]overlay_id);
Returns minutes from overlay.

Return: [num]Minutes
##### Get time overlay hours
    sys.o.time.getHr([num]overlay_id);
Returns hours from overlay.

Return: [num]Hours
##### Clear ok
    sys.o.time.clrOk([num]overlay_id);
Clears ok from time overlay

Return: None

#### Color overlay

##### Create color overlay
    sys.o.color.add([num]color);
Adds a color overlay, returns its id

Return: [num]overlay id
##### Set color overlay color
    sys.o.color.set([num]overlay_id, [num]color);
Sets color in overlay with given id.

Return: None
##### Update color overlay
    sys.o.color.update([num]overlay_id);
Updates color overlay.

Return: None
##### Get color overlay ok
    sys.o.color.getOk([num]overlay_id);
Gets ok from color overlay.

Return: 1 if ok was pressed
##### Get color overlay value
    sys.o.color.getCol([num]overlay_id);
Returns color from overlay.

Return: [num]Color
##### Clear ok
    sys.o.color.clrOk([num]overlay_id);
Clears ok from the overlay

Return: None
