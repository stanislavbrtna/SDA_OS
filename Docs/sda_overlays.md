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
    sys.o.setY( val); set Overlay Y2
Sets lower coordinates of current overlay (in screen pixels).
Return: None

#### Date overlay

##### Create date overlay
    sys.o.date.add([num]year, [num]month, [num]day); #return overlay id
Creates date overlay id, returns id
Return: [num]Date overlay id
##### Update date overlay
    sys.o.date.update([num]id);
Updates date overlay
Return: None
##### Get ok from overlay
    sys.o.date.getOk([num]id); #return if ok
Gets if overlay ok button was pressed.
Return: 1 if overlay ok button was pressed
##### Clear ok from overlay
    sys.o.date.clrOk([num]id);
Clears ok flag from overlay
Return: None
##### Get year form overlay
    sys.o.date.getYr([num]id); #return year
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
Adds a time overlay, returns it's id
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
    sys.oTimClrOk([num]overlay_id);
Clears ok from time overlay
Return: None
