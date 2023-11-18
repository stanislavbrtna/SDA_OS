#### Text obfuscation
##### Unlock overlay init
    sys.cr.unLockInit();
Creates unlock overlay
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
Gets if crypto is unlocked
Return: [num] 1 - crypto unlocked, 0 - crypto locked
##### Loads password as a key
    sys.cr.loadPass();
Loads OS password as a key
Return: 0 if success, 1 if error
##### Load custom key string
    sys.cr.loadStr([str]key);
Loads custom string as a crypto key
Return: 0 if success, 1 if error
##### Load custom keyfile
    sys.cr.loadKey([str]keyfile);
Loads custom keyfile as a crypto key
Return: 0 if success, 1 if error
##### Load OS keyfile
    sys.cr.loadOSKey();
Loads OS keyfile as a crypto key
Return: 0 if success, 1 if error
##### Generate keyfile
    sys.cr.genKey([str]keyfile);
Generates custom keyfile.
Return: 0 if success, 1 if error
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
##### Encrypt string
    sys.cr.encryptStr([str]source);
Encrypts given string.
Return: [str] encryptedString
##### Decrypt string
    sys.cr.decryptStr([str]source);
Decrypts given string.
Return: [str] decryptedString
