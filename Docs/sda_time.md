
#### Time API

##### Get time
    sys.time.get();
Returns system time in the timestamp form.
Count of seconds from 00:00 1. 1. 2007

Return: [num]Timestamp
##### Get time update flag
    sys.time.getUpd()
Returns 1 when time update flag occurs.

Return: 1 on time update (roughly 1s)
##### Get seconds
    sys.time.getSec();
Returns system time.

Return: [num]Seconds
##### Get minutes
    sys.time.getMin();
Returns system time.

Return: [num]Minutes
##### Get hours
    sys.time.getHr();
Returns system time

Return: [num]Hours
##### Get day
    sys.time.getDay();
Returns system time.

Return: [num]Day
##### Get month
    sys.time.getMonth();
Returns system time.

Return: [num]Month
##### Get year
    sys.time.getYear();
Returns system time.

Return: [num]Year
##### Get uptime
    sys.time.getUptime();
Returns system uptime.

Return: [num]Uptime(secs)
##### Get uptime in miliseconds
    sys.time.getAppUptime();
Returns milisecond uptime of the app.

Return: [num]Uptime(miliseconds)
##### Get new timestamp
    sys.time.setTs([num]Year, [num]Month, [num]Day, [num]Hour, [num]Min, [num]Sec);
Returns timestamp of given time, works only for years 2007 and above.

Return: [num]Timestamp
##### Get seconds from timestamp
    sys.time.getTsSec([num]timestamp);
Returns seconds from given timestamp.

Return: [num]Seconds
##### Get minutes from timestamp
    sys.time.getTsMin([num]timestamp);
Returns seconds from given timestamp.

Return: [num]Minutes
##### Get hours from timestamp
    sys.time.getTsHr([num]timestamp);
Returns seconds from given timestamp.

Return: [num]Seconds
##### Get days from timestamp
    sys.time.getTsDay([num]timestamp);
Returns seconds from given timestamp.

Return: [num]Days
##### Get weekday from timestamp
    sys.time.getTsWkDay([num]timestamp);
Returns weekday from given timestamp.

Return: [num]Weekday (starting with monday)
##### Get month from timestamp
    sys.time.getTsMonth([num]timestamp);
Returns seconds from given timestamp.

Return: [num]Months
##### Get year from timestamp
    sys.time.getTsYear([num]timestamp);
Returns seconds from given timestamp.

Return: [num]Years
#### Timer API

System timer will call the callback after given time, it will wake up the app if it is in the background. You can push app to foreground by calling sys.os.arise(); from the callback.

##### Set timer
    sys.time.setTimer([num]time_ms, [str]callaback);
Sets the timer.

Return: none
##### Clear timer
    sys.time.clearTimer();
Clears the timer if it is running.

Return: none
#### Alarm API
Alarm API creates system handled, repeatable alarms, that will be stored during reboots and will automatically launch the app.
When alarm is created, alarm id is returned, this id should be stored for future operations with the alarm.
Every alarm can have an user-defined parameter.
##### Register alarm
    sys.alarm.setFixed([num]timestamp, [num]param);
Creates new one-time alarm.
Returns id of the new alarm.

Return: [num]id
##### Register repeating alarm
    sys.alarm.setRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
Creates new repeatable alarm. Zero value in wkday/day/month means repeat every wkday/day/month.
Returns id of the new alarm.

Return: [num]id
##### Remove alarm
    sys.alarm.destroy([num]id);
Returns if alarm was deleted.

Return: [num] 0 - Ok, 1 - Fail
##### Get alarm flag
    sys.alarm.getFlag();
Returns 1 when alarm has occured.

Return: [num]flag
##### Clear alarm flag
    sys.alarm.clrFlag();
Clears alarm flag.

Return: none
##### Get alarm id
    sys.alarm.getId();
Returns id of the current alarm.

Return: [num]id
##### Get alarm parameter
    sys.alarm.getPar();
Returns parameter of the current alarm.

Return: [num]param
