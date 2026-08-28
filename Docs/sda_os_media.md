#### PCM audio playback
This is an optional feature based on feature flag *SDA_FEATURE_PCM_SOUND*.
Only one file can be played at a time, there is no sound mixer available.
##### Play media
    sys.pcm.play([str] string);
Plays given wav file (PCM sound must be supported by the device).
Supported file formats: mp3, wav

Return: playback id
##### Stop playback
    sys.pcm.stop([num] id);
Stops media playback

Return: none
##### Seek
    sys.pcm.seek([num]secs, [num] id);
Seeks in current media to given timestamp (in seconds).

Return: [num] 0 - ok, 1 - error
##### Get media Id
    sys.pcm.getId();
Get id of the currently playing media.

Return: [num] id
##### Get media status
    sys.pcm.getState([num] id);
Get media playback status.
 0 - unavailable
 1 - Playback
 2 - Paused
 3 - Stopped
 4 - Finished

Return: [num] status
##### Get playback position
    sys.pcm.getPos([num] id);
Get media playback position in seconds.

Return: [num] position in seconds
##### Get media duration
    sys.pcm.getDuration([str] fileName);
Returns media duration in seconds.

Return: none
##### Get media bitrate
    sys.pcm.getBitRate([str] fileName);
Returns media bitrate.

Return: none
##### Get media sample rate
    sys.pcm.getSmplRate([str] fileName);
Returns media sample rate.

Return: none
##### Get media output type
    sys.pcm.getDevice();
Returns if current playback device is speaker or headphones.

Return: [num] 1 - speaker, 2 - headphones
##### Get media playback volume
    sys.pcm.getVolume();
Returns volume of the current output device in range 0 - 1000.

Return: [num] volume
##### Set volume
    sys.pcm.setVolume([num]value);
Sets playback volume for current output device.

Return: none
##### Set pause
    sys.pcm.setPause([num]pauseOn, [num]id);
Pauses and resumes media playback.

Return: none
