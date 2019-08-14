### SDA Files
##### Open file
    sys.pFrOpen([str]fname);
    sys.fOpen([str]fname);
Opens text file for read or write.
Return: 1 on success, 0 on failure
##### Read given number of chars
    sys.fReadChars([num] bytes);
Reads given number of chars from file.
Return: [str] result
##### Writes given string to file
    sys.fWriteChars([str] string);
Writes given string to file.
Return: 1 - ok, 0 - fail
##### Read byte from file
    sys.fReadByte();
Reads byte from file.
Return: [num] result: 0 to 255 - ok, -1 - error
##### Write byte to file
    sys.fWriteByte([num] byte (0 - 255));
Writes byte to file.
Return: [num] 0 - fail, 1 - ok
##### Seek position in file
    sys.fSeek([num] pos_from_start);
Writes byte to file.
Return: [num] 0 - fail, 1 - ok
##### Truncate file
    sys.fTruncate();
Truncate currently opened file at the position of write pointer.
Return: [num] 0 - fail, 1 - ok
##### Get if path is dir
    sys.fIsDir([str] path);
Gets if path is a directory or not.
Return: [num] 0 - file, 1 - dir
##### Create directory
    sys.fMkDir([str] name);
Creates new directory
Return: [num] 1 - ok, 0 - fail
##### Change working directory
    sys.fChDir([str] pathInData);
Changes working directory.
call sys.fChDir(); to get to the DATA directory
call sys.fChDir(1); to get to the APPS directory
Return: None
##### File copy select source
    sys.fCopySource([str]source);
Selects source file for copy operation.
Return: [num] 1 - ok, 0 - failed
##### File copy start
    sys.fCopyStart([str]dest, [num]ChunkSize);
Starts copy operation, chunksize of bytes will be copyed each cycle.
Return: [num] 1 - ok, 0 - failed
##### File copy status
    sys.fCopyStat([num]opt);
 opt: 0 - status ret: [num]0 - nothing, 1 - source selected, 2 - copy in progress
 opt: 1 - size of source [num]bytes
 opt: 2 - remaining bytes [num]bytes
##### Stop current copy operation
    sys.fCopyStop();
Stops current copy operation.
Return: None
##### Tels position in file
    sys.fTell();
Returns current write pointer position in the file.
Return: [num] pos
##### Get size of file
    sys.pFrSize();
Returns size of openned file.
Return: Size of openned file
##### Check if file exist
    sys.pFrExists([str]fname);
Checks if file exists.
Return: 1 if file exists, otherwise 0
##### Seek in file and fill text field
    sys.pFrSeek([num]text_id, [num]filePos);
Fills pscg element with text from filePos position.
Return: None
##### Close file
    sys.pFrClose();
    sys.fClose();
Closes open file.
Return: None
##### Remove file
    sys.fDelete([str]fname);
Deletes file with fiven fname.
Return: None
##### Move/rename file
    sys.fRename([str]oldPath, [str]newPath);
Moves/renames given file.
Return: None
#### SDA CSV files API
##### Open csv file
    sys.csvOpen([str]fname);
Opens csv file.
Return: [num]1 on succes.
##### Close csv file
    sys.csvClose();
Closes csv file.
Return: None.
##### New csv line
    sys.csvNewLine([num]numberOfCells);
Adds new line to csv with given number of cells.
Return: None.
##### Get csv cell
    sys.csvGetCell([num]cellNumber, [str]default);
Gets data from specified cell on current line.
Return: [str]cellContents
##### Set csv cell
    sys.csvSetCell([num]cellNumber, [str]value);
Sets data of specified cell on current line.
Return: [str]cellContents
##### Feed line
    sys.csvLineFeed();
Moves to the next lone of csv file
Return: [num] 1 - ok, 0 - end of file
##### Remove line
    sys.csvRemoveLine();
Removes current line from csv
Return: None
##### Rewind file
    sys.csvRewind();
Rewinds file back on the start.
Return: None
#### Config files API
##### Open config file
    sys.cOpen([str]fname);
Opens config file.
Return: [num]1 on succes.
##### Close config file
    sys.cClose();
Close conf file.
Return: [num]1 on succes.
##### Check if key exists
    sys.cKeyExists([str]key);
Checks if key exists in conf file
Return: [num] 1 if key exists.
##### Read key
    sys.cKeyRead([str]key);
Reads key from config file as a string, 128 chars max.
Return: [str]Value
##### Read Key as int
    sys.cKeyReadInt([str]key, [num]default);
Reads key from config file as num (integrer). To be removed.
Return: [num]Value
##### Write key
    sys.cKeyWrite([str]key, [str]val);
Writes value in specified key.
Return: None
##### Remove key
    sys.cKeyRemove([str]key);
Removes given key.
Return: None

#### Find files

##### Find begin
    sys.fFind([str]extension, [str]directory);
Inits file find operation, returns first result.
Return: [str]filename or "" if none
##### Find next
    sys.fFindNext();
Next iteration of file find operation.
Return: [str]filename or "" if none
##### Example
    for(findfil = sys.fFind("txt", "."); findfil != ""; findfil = sys.fFindNext();) {
      print("found: " + findfil);
    }

#### Files as strings

##### Reads file as string
    sys.fReadStr([str]fname);
Reads text file to svs string buffer.
Return: [str]FileContents
##### Write string as file
    sys.fWriteStr([str]str, [str]fname);
Writes svs string to file.
Return: None
