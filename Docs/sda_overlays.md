#### Overlay API
##### Set overlay screen
    pSetOvrScr([num]screen_id);
Sets overlay screen, returns overlay id.
Return: [num] Overlay id
##### Get overlay id
    pGetOvrId();
Gets id of current overlay.
Return: [num] OverlayId
##### Destroy overlay
    pOvrDestroy();
Destroys current overlay, also destroys its screen.
Return: None
##### Set position and size of overlay
    pOvrSetXYXY([num]x1, [num]y1, [num]x2, [num]y2);
Sets position and size of current overlay.
Return: None
##### Set overlay height
    pSetOvrY( val); set Overlay Y2
Sets lower coordinates of current overlay.
Return: None

#### Date overlay

##### Create date overlay
    oDateAdd([num]year, [num]month, [num]day); #return overlay id
Creates date overlay id, returns id
Return: [num]Date overlay id
##### Update date overlay
    oDateUpd([num]id);
Updates date overlay
Return: None
##### Get ok from overlay
    oDateGetOk([num]id); #return if ok
Gets if overlay ok button was pressed.
Return: 1 if overlay ok button was pressed
##### Clear ok from overlay
    oDateClrOk([num]id);
Clears ok flag from overlay
Return: None
##### Get year form overlay
    oDateGetYr([num]id); #return year
Gets year from overlay with given id.
Return: [num]Year
##### Get day from overlay
    oDateGetDay([num]id);
Gets day from overlay with given id.
Return: [num]Day
##### Get month from overlay
    oDateGetMon([num]id);
Gets month from overlay with given id.
Return: [num]Month

#### Time overlay

##### Create time overlay
    oTimAdd();
Adds a time overlay, returns it's id
Return: [num]overlay id
##### Set time overlay time
    oTimSet([num]overlay_id, [num]hr, [num]min);
Sets time in overlay with given id.
Return: None
##### Update time overlay
    oTimUpd([num]overlay_id);
Updates time overlay.
Return: None
##### Get time overlay ok
    oTimGetOk([num]overlay_id);
Gets ok from time overlay.
Return: 1 if ok was pressed
##### Get time overlay minutes
    oTimGetMin([num]overlay_id);
Returns minutes from overlay.
Return: [num]Minutes
##### Get time overlay hours
    oTimGetHr([num]overlay_id);
Returns hours from overlay.
Return: [num]Hours
##### Clear ok
    oTimClrOk([num]overlay_id);
Clears ok from time overlay
Return: None
