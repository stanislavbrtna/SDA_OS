
#### Time API

##### Get time
    sys getTime();
Returns system time in the timestamp form.
Count of seconds from 0:0 1.1. 2007
Return: [num]Timestamp
##### Get time update flag
    sys getTimeUpd()
Returns 1 when time update flag occurs.
Return: 1 on time update (roughly 1s)
##### Get seconds
    sys getTimeSec();
Returns system time.
Return: [num]Seconds
##### Get minutes
    sys getTimeMin();
Returns system time.
Return: [num]Minutes
##### Get hours
    sys getTimeHr();
Returns system time
Return: [num]Hours
##### Get day
    sys getTimeDay();
Returns system time.
Return: [num]Day
##### Get month
    sys getTimeMonth();
Returns system time.
Return: [num]Month
##### Get year
    sys getTimeYear();
Returns system time.
Return: [num]Year
##### Get uptime
    sys getUptime();
Returns system uptime.
Return: [num]Uptime(secs)
##### Get new timestamp
    sys setTs([num]Year, [num]Month, [num]Day, [num]Hour, [num]Min, [num]Sec);
Returns timestamp of given time, works only for years 2007 and above.
Return: [num]Timestamp
##### Get seconds from timestamp
    sys getTsSec([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Seconds
##### Get minutes from timestamp
    sys getTsMin([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Minutes
##### Get hours from timestamp
    sys getTsHr([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Seconds
##### Get days from timestamp
    sys getTsDay([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Days
##### Get weekday from timestamp
    sys getTsWkDay([num]timestamp);
Returns weekday from given timestamp.
Return: [num]Weekday (starting with monday)
##### Get month from timestamp
    sys getTsMonth([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Months
##### Get year from timestamp
    sys getTsYear([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Years
##### Register alarm
    sys alarmSetFixed([num]timestamp, [num]param);
Returns id of the new alarm.
Return: [num]id
##### Register repeating alarm
    sys alarmSetRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
Returns id of the new alarm.
Return: [num]id
##### Remove alarm
    sys alarmDestroy([num]id);
Returns if alarm was deleted.
Return: [num] 0 - Ok, 1 - Fail
##### Get alarm flag
    sys alarmGetFlag();
Returns id of the new alarm.
Return: [num]flag
##### Clear alarm flag
    sys alarmClrFlag();
Clears alarm flag.
Return: none
##### Get alarm id
    sys alarmGetId();
Returns id of the curent alarm.
Return: [num]id
##### Get alarm parameter
    sys alarmGetPar();
Returns parameter of the current alarm.
Return: [num]param
