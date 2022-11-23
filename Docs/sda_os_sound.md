#### Sound
##### Beep the speaker
    sys.snd.beep();
Initiates system beep.
Return: None
##### Set beep param to default
    sys.snd.beepDef();
Sets beep to its default values.
Return: None
##### Set time of beep
    sys.snd.beepTime([num]time (~ms));
Sets lenght of beep.
Return: None
##### Set period of beep
    sys.snd.beepFreq([num]frequency (Hz));
Sets frequency of the beep in Hz in range from 27 to 20000.
Return: None
##### Get if system sound is disabled
    sys.snd.getMute();
Returns system mute
Return: [num]1 if system is on mute.
