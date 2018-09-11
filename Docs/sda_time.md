
#### Time API

##### Get time
    getTime();
Returns system time in the timestamp form.
Count of seconds from 0:0 1.1. 2007
Return: [num]Timestamp
##### Get time update flag
    getTimeUpd()
Returns 1 when time update flag occurs.
Return: 1 on time update (roughly 1s)
##### Get seconds
    getTimeSec();
Returns system time.
Return: [num]Seconds
##### Get minutes
    getTimeMin();
Returns system time.
Return: [num]Minutes
##### Get hours
    getTimeHr();
Returns system time
Return: [num]Hours
##### Get day
    getTimeDay();
Returns system time.
Return: [num]Day
##### Get month
    getTimeMonth();
Returns system time.
Return: [num]Month
##### Get year
    getTimeYear();
Returns system time.
Return: [num]Year
##### Get uptime
    getUptime();
Returns system uptime.
Return: [num]Uptime(secs)
##### Get new timestamp
    setTs([num]Year, [num]Month, [num]Day, [num]Hour, [num]Min, [num]Sec);
Returns timestamp of given time, works only for years 2007 and above.
Return: [num]Timestamp
##### Get seconds from timestamp
    getTsSec([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Seconds
##### Get minutes from timestamp
    getTsMin([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Minutes
##### Get hours from timestamp
    getTsHr([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Seconds
##### Get days from timestamp
    getTsDay([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Days
##### Get weekday from timestamp
    getTsWkDay([num]timestamp);
Returns weekday from given timestamp.
Return: [num]Weekday (starting with monday)
##### Get month from timestamp
    getTsMonth([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Months
##### Get year from timestamp
    getTsYear([num]timestamp);
Returns seconds from given timestamp.
Return: [num]Years
##### Register alarm
    alarmSetFixed([num]timestamp, [num]param);
Returns id of the new alarm.
Return: [num]id
##### Register repeating alarm
    alarmSetRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
Returns id of the new alarm.
Return: [num]id
##### Remove alarm
    sys alarmDestroy([num]id);
Returns if alarm was deleted.
Return: [num] 0 - Ok, 1 - Fail
##### Get alarm flag
    alarmGetFlag();
Returns id of the new alarm.
Return: [num]flag
##### Clear alarm flag
    alarmClrFlag();
Clears alarm flag.
Return: none
##### Get alarm id
    alarmGetId();
Returns id of the curent alarm.
Return: [num]id
##### Get alarm parameter
    alarmGetPar();
Returns parameter of the current alarm.
Return: [num]param
