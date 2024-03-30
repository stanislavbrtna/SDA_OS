Automatically generated documentation on wrap_directS.c

### Direct Screen draw functions

####  Color & Areas

##### Get color from RGB
    sys.ds.mixColor([num]r, [num]g, [num]b);
Mixes the right color from red, green and blue values (0 - 255)

Return: [num] Color (16bit RGB565)
##### Set draw area
    sys.ds.setArea([num] x1, [num] y1, [num] x2, [num] y2);
Sets the draw area. Uses hardware coordinates.
For example: sys.ds.setArea(0, 32, 319, 479);
This will init all app available space as a draw area.

Return: None
##### Draw rectangle
    sys.ds.drawRect([num] x1, [num] y1, [num] x2, [num] y2, [num]col);
Draws rectangle outline

Return: None
##### Draw filled rectangle
    sys.ds.fillRect([num] x1, [num] y1, [num] x2, [num] y2, [num]col);
Draws filled rectangle

Return: None
##### Draw circle
    sys.ds.drawCircle([num] x1, [num] y1, [num] radius, [num]col);
Draws circle

Return: None
##### Draw filled circle
    sys.ds.fillCircle([num] x1, [num] y1, [num] radius, [num]col);
Fills circle

Return: None
##### Draw portion of a circle
    sys.ds.drawCPart([num] x1, [num] y1, [num] radius, [num] part, [num]col);
Draws part of a circle

Parts:
| 0 | 1 |
| 2 | 3 |
Return: None
##### Fill portion of a circle
    sys.ds.fillCPart([num] x1, [num] y1, [num] radius, [num] part, [num]col);
Fills a part of a circle

Return: None
##### Draw line
    sys.ds.drawLine([num] x1, [num] y1, [num] x2, [num] y2, [num]col);
Draws line

Return: None
##### Draw text
    sys.ds.drawText([num] x1, [num] y1, [str] txt, [num]col);
Draws text

Return: None
##### Set text to fit specified width
    sys.ds.setTextFit([num] enable, [num] width);
Sets max width for next drawn text

Return: None
##### Get text width
    sys.ds.getTextWidth([str] txt);
Gets width of a string, when drawn with current font.

Return: [num] width (px)
##### Get text height
    sys.ds.getTextHeight([str] txt);
Gets height of a string, when drawn with current font.

Return: [num] height (px)
##### Fill area with color
    sys.ds.clearArea([num]col);
Clears draw area with goven color

Return: None
#### P16 image tools
##### Draws P16 image
    sys.ds.drawImage([num]x, [num]y, [num]scale_w, [num]scale_h, [str]name);
Draws p16 image from the working directory. Supports upscaling, and downscaling.

Scale table:

|  Scale value | Image size|
|      ---     |   ---     |
|    -3        |   1/16    |
|    -2        |   1/8     |
|    -1        |   1/4     |
|     0        |   1/2     |
|     1        |    1      |
|     2        |    2x     |
|     n        |    n*x    |

Return: None
##### Get P16 image width
    sys.ds.getImageW([str]name);
Gets width of given p16 file.

Return: [num] width (px)
##### Get P16 image height
    sys.ds.getImageH([str]name);
Gets height of given p16 file.

Return: [num] height (px)
##### Draws PPM
    sys.ds.drawPPM([num]x, [num]y, [num]scale, [str]name);
Draws ppm image (To be removed).

Return: None
#### Touch API
##### Get if screen is touched
    sys.ds.touchEv();
Gets if screen is touched, returns last touch event

Return: [num] Touch event (from event defines)
##### Clears touch event
    sys.ds.touchClr();
Clears touch event

Return: none
##### Get touch y
    sys.ds.touchY();
Gives y coordinate of touch event

Return: [num] Touch y coordinate
##### Get touch x
    sys.ds.touchX();
Gives x coordinate of touch event

Return: [num] Touch y coordinate
