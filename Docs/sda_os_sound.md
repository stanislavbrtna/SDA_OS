#### Sound
##### Beep the speaker
    sys.snd.beep();
Initiates system beep.

Return: None
##### Beep the speaker with callback
    sys.snd.beepC([num] frequency_hz, [num] duration_ms, [str] callback);
Makes sound of given frequency and duration, calls given callback afterwards.
Internally calls sys.snd.beepTime and sys.snd.beepFreq, so calling sys.snd.beep();
will produce tone with frequency nad duration of last sys.snd.beepC call.
If 0 is given as frequency_hz, only callback will be performed afrer given duration.
If empty string is passed as callback, no callback will be performed. 

Return: None
##### Set beep param to default
    sys.snd.beepDef();
Sets beep to its default values.

Return: None
##### Set the duration
    sys.snd.beepTime([num]time (~ms));
Sets lenght of beep.

Return: None
##### Set the frequency
    sys.snd.beepFreq([num]frequency (Hz));
Sets frequency of the beep in Hz in range from 27 to 20000.

Return: None
##### Get if system sound is disabled
    sys.snd.getMute();
Returns system mute

Return: [num]1 if system is on mute.
