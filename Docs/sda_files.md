### SDA Files

Since SDA_OS 1.0.2, more than one general purpose file is supported.
Functions for basic file i/o operations now accept optional index parameter
that specifies what file is used.
Number of files currently openned is defined in SDA_FILES_OPEN_MAX define.
Default value is 10.

##### Open file
    sys.fs.open([str]fname);
    sys.fs.open([num]index, [str]fname);
Opens text file for read or write. If no file index is given, index 0 is used.
Return: 1 on success, 0 on failure
##### Read given number of chars
    sys.fs.readChars([num] bytes);
    sys.fs.readChars([num]index, [num] bytes);
Reads given number of chars from file.
If no file index is given, index 0 is used.
Return: [str] result
##### Writes given string to file
    sys.fs.writeChars([str]string);
    sys.fs.writeChars([num]index, [str]string);
Writes given string to file.
Return: 1 - ok, 0 - fail
##### Read byte from file
    sys.fs.readByte();
    sys.fs.readByte([num]index);
Reads byte from file.
Return: [num] result: 0 to 255 - ok, -1 - error, -2 - EOF
##### Write byte to file
    sys.fs.writeByte([num] byte (0 - 255));
    sys.fs.writeByte([num]index, [num] byte (0 - 255));
Writes byte to file.
Return: [num] 0 - fail, 1 - ok
##### Seek position in file
    sys.fs.seek([num] pos_from_start);
    sys.fs.seek([num]index, [num] pos_from_start);
Writes byte to file.
Return: [num] 0 - fail, 1 - ok
##### Truncate file
    sys.fs.truncate();
Truncate currently opened file at the position of write pointer.
Return: [num] 0 - fail, 1 - ok
##### Tels position in file
    sys.fs.tell();
    sys.fs.tell([num]index);
Returns current write pointer position in the file.
Return: [num] pos
##### Get size of file
    sys.fs.size();
    sys.fs.size([num] index);
Returns size of openned file.
Return: [num] size in bytes
##### Close file
    sys.fs.close();
    sys.fs.close([num] index);
Closes open file.
Return: [num] 1 - ok, 0 - error

#### Directory functions

##### Get if path is dir
    sys.fs.isDir([str] path);
Gets if path is a directory or not.
Return: [num] 0 - file, 1 - dir
##### Create directory
    sys.fs.mkDir([str] name);
Creates new directory
Return: [num] 1 - ok, 0 - fail
##### Change working directory
    sys.fs.chDir([str] pathInData);
Changes working directory.
call sys.fs.chDir(0); or sys.fs.chDir(); to get to the DATA directory
call sys.fs.chDir(1); to get to the APPS directory
Return: None

#### File copy

##### File copy select source
    sys.fs.copySource([str]source);
Selects source file for copy operation.
Return: [num] 1 - ok, 0 - failed
##### File copy start
    sys.fs.copyStart([str]dest, [num]ChunkSize);
Starts copy operation, chunksize of bytes will be copyed each cycle.
Return: [num] 1 - ok, 0 - failed
##### File copy status
    sys.fs.copyStat([num]opt);
 opt: 0 - status ret: [num]0 - nothing, 1 - source selected, 2 - copy in progress
 opt: 1 - size of source [num]bytes
 opt: 2 - remaining bytes [num]bytes
##### Stop current copy operation
    sys.fs.copyStop();
Stops current copy operation.
Return: None

#### Check, remove, rename

##### Check if file exist
    sys.fs.exists([str]fname);
Checks if file exists.
Return: 1 if file exists, otherwise 0
##### Remove file
    sys.fs.delete([str]fname);
Deletes file with fiven fname. Can also delete empty directories.
Return: None
##### Move/rename file
    sys.fs.rename([str]oldPath, [str]newPath);
Moves/renames given file.
Return: None

#### Find files

##### Find begin
    sys.fs.find([str]extension, [str]directory);
Inits file find operation, returns first result.
Return: [str]filename or "" if none
##### Find next
    sys.fs.findNext();
Next iteration of file find operation.
Return: [str]filename or "" if none
##### Example
    
    for(findfil = sys.fs.find("txt", "."); findfil != ""; findfil = sys.fs.findNext();) {
      print("found: " + findfil);
    }
    

Find is not stateless, sys.fs.find must be re-inicialized after recursive call.
Example of recursive function:
    
    function ls {
      local findfil;
      local n = 0;
      print(arg0);
      for(findfil = sys.fs.find("", arg0); findfil != ""; findfil = sys.fs.findNext();) {
        if (sys.fs.isDir(arg0 + "/" + findfil)) {
          ls(arg0 + "/" + findfil);
          findfil = sys.fs.find("", arg0);
          local x;
          for (x = 0; x<n; x++;) {
            findfil = sys.fs.findNext();
          }
        } else {
          print(arg0 + "/" + findfil);
        }
        n++;
      }  
    }
    

#### Files as strings

##### Reads file as string
    sys.fs.readStr([str]fname);
Reads text file to svs string buffer.
Return: [str]FileContents
##### Write string as file
    sys.fs.writeStr([str]str, [str]fname);
Writes svs string to file.
Return: None
#### SDA CSV files API
##### Open csv file
    sys.fs.csv.open([str]fname);
Opens csv file.
Return: [num]1 on succes.
##### Close csv file
    sys.fs.csv.close();
Closes csv file.
Return: None.
##### New csv line
    sys.fs.csv.newLine([num]numberOfCells);
Adds new line to csv with given number of cells.
Return: None.
##### Get csv cell
    sys.fs.csv.getCell([num]cellNumber, [str]default);
Gets data from specified cell on current line.
Return: [str]cellContents
##### Set csv cell
    sys.fs.csv.setCell([num]cellNumber, [str]value);
Sets data of specified cell on current line.
Return: [str]cellContents
##### Feed line
    sys.fs.csv.lineFeed();
Moves to the next lone of csv file
Return: [num] 1 - ok, 0 - end of file
##### Remove line
    sys.fs.csv.removeLine();
Removes current line from csv
Return: None
##### Rewind file
    sys.fs.csv.rewind();
Rewinds file back on the start.
Return: None
#### Config files API
##### Open config file
    sys.fs.conf.open([str]fname);
Opens config file.
Return: [num]1 on succes.
##### Close config file
    sys.fs.conf.close();
Close conf file.
Return: [num]1 on succes.
##### Check if key exists
    sys.fs.conf.exists([str]key);
Checks if key exists in conf file
Return: [num] 1 if key exists.
##### Read key
    sys.fs.conf.read([str]key);
Reads key from config file as a string, 128 chars max.
Return: [str]Value
##### Read Key as int
    sys.fs.conf.readInt([str]key, [num]default);
Reads key from config file as num (integrer). To be removed.
Return: [num]Value
##### Write key
    sys.fs.conf.write([str]key, [str]val);
Writes value in specified key.
Return: None
##### Remove key
    sys.fs.conf.remove([str]key);
Removes given key.
Return: None
