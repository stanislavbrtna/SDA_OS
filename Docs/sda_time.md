
#### Time API

##### Get time
    sys.time.get();
Returns system time in the timestamp form.
Count of seconds from 0:0 1.1. 2007
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
#### Alarm API
##### Register alarm
    sys.alarm.setFixed([num]timestamp, [num]param);
Returns id of the new alarm.
Return: [num]id
##### Register repeating alarm
    sys.alarm.setRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
Returns id of the new alarm.
Return: [num]id
##### Remove alarm
    sys.alarm.destroy([num]id);
Returns if alarm was deleted.
Return: [num] 0 - Ok, 1 - Fail
##### Get alarm flag
    sys.alarm.getFlag();
Returns id of the new alarm.
Return: [num]flag
##### Clear alarm flag
    sys.alarm.clrFlag();
Clears alarm flag.
Return: none
##### Get alarm id
    sys.alarm.getId();
Returns id of the curent alarm.
Return: [num]id
##### Get alarm parameter
    sys.alarm.getPar();
Returns parameter of the current alarm.
Return: [num]param
