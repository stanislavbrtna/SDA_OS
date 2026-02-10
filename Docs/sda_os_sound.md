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
(1000hz, 250ms)

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
##### Get if haptics is enabled
    sys.snd.getHaptics();
Returns system haptics

Return: [num]1 if haptics is enabled.
##### Perform haptic feedbach
    sys.snd.haptics([num]time (~ms));
Performs haptic feedback for a given time. (max 1s)

Return: None
##### Play media
    sys.snd.play([str] string);
Plays given wav file (PCM sound must be supported by the device).

Return: none
##### Stop playback
    sys.snd.stop();
Stops media playback

Return: none
##### Seek
    sys.snd.seek([num]secs);
Seeks in current media to given timestamp (in seconds).

Return: [num] 0 - ok, 1 - error
##### Get playback position
    sys.snd.getPos();
Get media playback position in seconds.

Return: [num] position in seconds
##### Get media duration
    sys.snd.getDuration([str] fileName);
Returns media duration in seconds.

Return: none
##### Get media output type
    sys.snd.getDevice();
Returns if current playback device is speaker or headphones.

Return: [num] 0 - speaker, 1 - headphones
##### Get media playback volume
    sys.snd.getVolume();
Returns volume of the current output device in range 0 - 1000.

Return: [num] volume
##### Set volume
    sys.snd.setVolume([num]value);
Sets playback volume for current output device.

Return: none
