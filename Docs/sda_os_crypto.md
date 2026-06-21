#### Text obfuscation
##### Unlock overlay init
    sys.cr.unLockInit();
Creates Pin unlock overlay

Return: [num] overlay ID, 0 when error
##### Unlock overlay update
    sys.cr.update([num] ovId);
Updates unlock overlay

Return: None
##### Unlock overlay get ok
    sys.cr.getOk([num] ovId);
Gets if unlock was successfull

Return: [num] 1 - unlock success, 2 - unlock canceled
##### Unlock overlay clear ok
    sys.cr.clrOk([num] ovId);
Creates unlock overlay

Return: None
##### Get if is locked
    sys.cr.getLock();
Gets if current app can use PIN key

Return: [num] 1 - crypto unlocked, 0 - crypto locked
##### Get if crypto is set up
    sys.cr.enabled();
Gets if crypto is enabled. (Device is has keys loaded.)

Return: [num] 1 - crypto enabled, 0 - crypto disabled
##### Load custom key string
    sys.cr.loadKey([str]key, [str]password);
Loads custom string as a crypto key

Return: 0 if success, 1 if error
##### Derive user key from password
    sys.cr.deriveKey([str]password);
Loads derived key as a user key.

##### Generate keystring
    sys.cr.genKey([str]password);
Generates custom keystring.

Return: 0 if success, 1 if error
##### Clear User key
    sys.cr.clearKey();
Clears usr key from memory.

Return: None
##### Set key type
    sys.cr.setKey([num]keytype);
Sets given key for crypto operations.
0 - Device encryption key, 1 - user encryption key

Return: none
##### Lock
    sys.cr.lock();
Locks sda encryption

Return: None
##### Encrypt file
    sys.cr.encrypt([str]fname);
Encrypts file.

Return: 0 if success, 1 if error
##### Decrypt file
    sys.cr.decrypt([str]fname);
Encrypts file.

Return: 0 if success, 1 if error
##### Get TOTP
    sys.cr.getTotp([str]secret, [num]time_offset_s);
Returns OTP auth code for given secret.

Return: 0 if success, 1 if error
##### Get TOTP Remaining time
    sys.cr.getTRem([num]time_offset_s);
Returns for how long the OTP code is valid, in seconds.

Return: [num] time_in_s
##### Encrypt string
    sys.cr.encryptStr([str]source);
Encrypts given string.

Return: [str] encryptedString
##### Decrypt string
    sys.cr.decryptStr([str]source);
Decrypts given string.

Return: [str] decryptedString
