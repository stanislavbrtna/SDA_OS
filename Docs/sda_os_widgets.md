
#### Date selector widget

##### Init calendar widget
    sys.w.cal.init([num]year, [num]month, [num]day);
Creates callendar widget screen.
With given year, month and day.

Return: [num]Callendar widget screen id.
##### Select date
    sys.w.cal.select([num]year, [num]month, [num]day);
Sets year, month and day to callendar widget.

Return: None
##### Update
    sys.w.cal.update();
Updates callendar widget.

Return: [num] 1 when callendar is clicked.
##### Mark day
    sys.w.cal.mark([num]day);
Marks day in callendar widget.

Return: None
##### Set highlighting
    sys.w.cal.highlight([num]val);
Enable that all buttons except marked are rendered as ghost buttons.

Return: None
##### Get selected day
    sys.w.cal.getDay();
Returns selected day.

Return: [num]day
#### Counters
##### Set counter
    sys.cnt.set([num] ms);
Sets system timer, it counts down and stops at zero.

Return: None
##### Gets counter
    sys.cnt.get();
Gets system timer value

Return: value of system timer
