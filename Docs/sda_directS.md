Automatically generated documentation on wrap_directS.c

####	Color & Areas

##### Get color from RGB
    sys.sxMixColor([num] r, [num] g, [num] b)
Mixes the right color
Return: [num] Color
##### Set draw area
    sys.sxSetArea([num] x1, [num] y1, [num] x2, [num] y2)
Sets the draw area
Return: None
##### Draw rectangle
    sys.sxDrawRect([num] x1, [num] y1, [num] x2, [num] y2, [num]col)
Draws rectangle
Return: None
##### Draw filled rectangle
    sys.sxFillRect([num] x1, [num] y1, [num] x2, [num] y2, [num]col)
Draws rectangle
Return: None
##### Draw circle
    sys.sxDrawCircle([num] x1, [num] y1, [num] radius, [num]col)
Draws circle
Return: None
##### Draw filled circle
    sys.sxFillCircle([num] x1, [num] y1, [num] radius, [num]col)
Fills circle
Return: None
##### Draw line
    sys.sxDrawLine([num] x1, [num] y1, [num] x2, [num] y2, [num]col)
Draws line
Return: None
##### Draw text
    sys.sxDrawText([num] x1, [num] y1, [str] txt, [num]col)
Draws text
Return: None
##### Set text to fit specified width
    sys.sxSetTextFit([num] enable, [num] width)
Sets max width for next drawn text
Return: None
##### Fill area with color
    sys.sxClearArea([num]col)
Clears draw area with goven color
Return: None
##### Draws PPM
    sys.pDrawPPM([num]x, [num]y, [num]scale, [str]name);
Draws ppm image.
Return: None
#### Touch API
##### Get if screen is touched
    sys.sxTouchEv();
Gets if screen is touched, returns last touch event
Return: [num] Touch event (from event defines)
##### Clears touch event
    sys.sxTouchClr();
Clears touch event
Return: none
##### Get touch y
    sys.sxTouchY();
Gives y coordinate of touch event
Return: [num] Touch y coordinate
##### Get touch x
    sys.sxTouchX();
Gives x coordinate of touch event
Return: [num] Touch y coordinate
