
# SDA_OS code snipplets

### Hello world

    function init {
      scr = sys.gui.addScreen();
      sys.os.gui.setMainScr(scr);
      sys.gui.addText(1, 1, 8, 2, "Hello, world!", scr);
    
      ok = sys.gui.addButton(1, 3, 3, 4, "Ok", scr);
    }
    
    function update {
      if (sys.gui.getEventC(ok) == EV_RELEASED) {
        sys.os.exit();
      }
    }

### Overlay
Overlay init:

    function init_overlay {
      ov_screen = sys.gui.addScreen();    
      sys.gui.addText(1, 1, 8, 5, "Overlay screen", ov_screen);
      b_ok = sys.gui.addButton(5, 6, 2, 1, "Ok", ov_screen);
    }

Overlay handler:   

     function handle_overlay {
          if(sys.o.getId() != ov_id or sys.o.getId() == 0) {
            return;
          }
      
      if (sys.gui.getEventC(b_ok) == EV_RELEASED) {
				# do something
        sys.o.destroy();
        return;
      }
    }

Open overlay:

    if (sys.gui.getEventC(b_open) == EV_RELEASED) {
      init_overlay();
      ov_id = sys.o.setScr(ov_screen);
      sys.o.setY(320);
    }

### File selector overlay

    function init {
      scr = sys.gui.addScreen();
      sys.gui.setRelInit(1);
      btn = sys.gui.addButton(1, 1, 8, 1, "Choose file", scr);
    
      sys.os.gui.setMainScr(scr);
    }
    
    function update {
      if(sys.gui.getEventC(btn) == EV_RELEASED) {
        sys.os.subProcess("lib/fsl.svs", &callRet, 0, 0, 0);
        return;
      }
    }
    
    function callRet {
      print("callback returned");
      print("argument0: " + arg0);
      sys.gui.setStr(btn, "-> " + arg0 + "(" + arg1 + ")");
    }

### System timer

  Systen timers have been introduced in v. 0.9.
  When set, the timer will count to zero and then it will woke the app
  (if the app is in background, more on app states in docs)
  and call the given callback. If sys.os.arise() is called in that callback,
  then after the callback returns app will be put to the foreground.
  
  If the app is closed (or the pda reseted, etc.) during the timer countdown,
  the app state and timer are lost and the callback wont be called.
  If you need that kind of functionality, use alarms.


    function init {
      # setup screen and basic gui
      scr = sys.gui.addScreen();
      sys.os.gui.setMainScr(scr);
      text = sys.gui.addText(1, 1, 8, 2, "Hello, world!", scr);
      set_timer = sys.gui.addButton(1, 3, 8, 4, "set timer (beep)", scr);
      set_timer2 = sys.gui.addButton(1, 5, 8, 6, "set timer (wkup)", scr);
    }
    
    function update {
      #button handling
      if (sys.gui.getEventC(set_timer) == EV_RELEASED) {
        sys.time.setTimer(10000, &callback);
      }
      if (sys.gui.getEventC(set_timer2) == EV_RELEASED) {
        sys.time.setTimer(10000, &callback2); #different callback is used
      }
    }
    
    function callback {
      sys.gui.setStr(text, "called back!");
      print("called");
      sys.snd.beep(); # this will beep on the background
    }
    
    function callback2 {
      sys.gui.setStr(text, "called back2!");
      print("called2");
      sys.os.arise(); # this will wake the app from background
    }
    
### System alarms

System alarms can be added with `sys.alarm.setFixed` or `sys.alarm.setRep`, id of the new alarm is returnded. This id should be stored somewhere, it is needed for removal of the alarm with `sys.alarm.destroy`.  Alarms will persist over system reboot and will wake the application when the time occurs. This wakeup by alarm can be detected with `sys.alarm.getFlag`, then `sys.alarm.getId` can be used to retrive the id of the alarm. 

Full example:

    function init {
      scr = sys.gui.addScreen();
      getTime = sys.gui.addButton(1, 3, 5, 4, "Get Time", scr);
      
      setAlarm = sys.gui.addButton(1, 5, 5, 6, "Set once", scr);
      setAlarmR = sys.gui.addButton(5, 5, 10, 6, "Set repeating", scr);
      
      removeAlarm = sys.gui.addButton(1, 7, 7, 8, "Remove alarm", scr);
      
      okB = sys.gui.addButton(1, 9, 5, 10, "Alarm!", scr);
      sys.gui.setVisible(okB, 0);
      sys.gui.setGrayout(removeAlarm, 1);
      
      sys.os.gui.setMainScr(scr);
      
      currentAlarm = 0;
      timeOvr = 0;
    }
    
    function update {
      # get time for the alarm
      if (sys.gui.getEventC(getTime) == EV_RELEASED) {
        timeOvr = sys.o.time.add();
        sys.o.time.set(timeOvr, sys.time.getHr(), (sys.time.getMin() + 2) % 60);
      }
    
      sys.o.time.update(timeOvr);
    
      if (sys.o.time.getOk(timeOvr)) {
        sys.gui.setStr(
          getTime,
          sys.o.time.getHr(timeOvr)
          + ":"
          + fmt(sys.o.time.getMin(timeOvr))
        );
        
        hour = sys.o.time.getHr(timeOvr);
        min = sys.o.time.getMin(timeOvr);
        sys.o.time.clrOk(timeOvr);
      }
      
      # alarm handler
      # this also can be in the init function
      if (sys.alarm.getFlag()) {
        print("Alarm: id:" + sys.alarm.getId() + " param: " + sys.alarm.getPar());
        currentAlarm = sys.alarm.getId();
        
        sys.hw.wakeLcd();
        sys.gui.setVisible(okB, 1);
        sys.gui.setGrayout(removeAlarm, 0);
        
        sys.alarm.clrFlag();
      }
      
      # setting the alarm
      if (sys.gui.getEventC(setAlarm) == EV_RELEASED) {
        ts = sys.time.setTs(sys.time.getYear(), sys.time.getMonth(), sys.time.getDay(), hour, min, 0);
        if (ts > sys.time.get()) {
          currentAlarm = sys.alarm.setFixed(ts, 0);
          sys.gui.setGrayout(removeAlarm, 0);
        }
      }
      
      if (sys.gui.getEventC(setAlarmR) == EV_RELEASED) {
        # sys.alarm.setRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
        # zero value in wkday/day/month means repeat every wkday/day/month
        currentAlarm = sys.alarm.setRep(hour, min, 0, 0, 0, 0);
        # alarm id is better to be stored in a config file
        sys.gui.setGrayout(removeAlarm, 0);
      }
      
      if (sys.gui.getEventC(okB) == EV_RELEASED) {
        sys.gui.setVisible(okB, 0);
      }
      
      if (sys.gui.getEventC(removeAlarm) == EV_RELEASED) {
        if (sys.alarm.destroy(currentAlarm) == 0) {
          sys.gui.setGrayout(removeAlarm, 1);
        }
      }
    }
    
    function fmt { # to get the leading zero if needed
      if (arg0 < 10)
        return "0" + arg0;
      else
        return arg0;
    }
### Get time and date
SDA_OS has its own time and date overlay. This can be used to receive time and date from an user.

    function init {
      scr = sys.gui.addScreen();
      getDate = sys.gui.addButton(1, 1, 5, 2, "Get Date", scr);
      getTime = sys.gui.addButton(1, 3, 5, 4, "Get Time", scr);
      sys.os.gui.setMainScr(scr);
    }
    
    function update {
      # Launch overlay
      if (sys.gui.getEventC(getDate) == EV_RELEASED) {
        # set defaults from current date
        dateOvr = sys.o.date.add(
	                  sys.time.getYear(),
	                  sys.time.getMonth(),
	                  sys.time.getDay()
                  );
      }
    
      # Updates given overlay id, does nothing if overlay is inactive
      sys.o.date.update(dateOvr);
    
      # Handles overlay success
      if (sys.o.date.getOk(dateOvr)) {
        sys.gui.setStr(
          getDate,
          sys.o.date.getDay(dateOvr)
          + ". "
          + sys.o.date.getMon(dateOvr)
          + ". "
          + sys.o.date.getYr(dateOvr)
        );
        sys.o.date.clrOk(dateOvr);
      }
    
      # Nearly same for time overlay
      if (sys.gui.getEventC(getTime) == EV_RELEASED) {
        timeOvr = sys.o.time.add();
        sys.o.time.set(timeOvr, sys.time.getHr(), sys.time.getMin());
      }
    
      sys.o.time.update(timeOvr);
    
      if (sys.o.time.getOk(timeOvr)) {
        sys.gui.setStr(
          getTime,
          sys.o.time.getHr(timeOvr)
          + ":"
          + fmt(sys.o.time.getMin(timeOvr))
        );
        sys.o.time.clrOk(timeOvr);
      }
    }
    
    function fmt { # to get the leading zero if needed
      if (arg0 < 10)
        return "0" + arg0;
      else
        return arg0;
    }

### File path functions

#### Get Folder
Get folder from arg0 (path):

    function getFolder {
      arg3 = 0;
      for(arg2 = len(arg0); arg2 > 0; arg2--;) {
        if (getcp(arg0, arg2) == "/") {
          return substr(arg0, 0, arg2);
        }
        if (getcp(arg0, arg2) == ".") {
          arg3 = 1;
        }
      }
      if (arg3) return ""; # 
      
      return arg0;
    }

#### Get Path
Get filename from arg0 (path):

    function get_fname {
      for(arg2 = len(arg0); arg2 > 0; arg2--;) {
        if (getcp(arg0, arg2) == "/") {
          return substr(arg0, arg2 + 1, len(arg0));
        }
      }
      return arg0;
    }

## Hardware

### ADC usage

    function init{
      scr = sys.gui.addScreen();
      btn = sys.gui.addButton(3, 1, 7, 2, "Measure", scr);
      sys.os.gui.setMainScr(scr);
    }
    
    function update{
      if(sys.gui.getEventC(btn) == EV_RELEASED){
        val = sys.hw.eADCRead();
        print(val+" type "+typeof(val));
        sys.gui.setStr(btn, ""+ val +" V");
      }
    }

### External expansion USART

    function init {
      scr = sys.gui.addScreen();
      btn = sys.gui.addButton(1,1,4,2, "Send", scr);
      clr = sys.gui.addButton(4,1,6,2, "Clr", scr);
      btn2 = sys.gui.addButton(6,1,10,2, "Recieve", scr);
      txt = sys.gui.addText(1, 3, 10, 12, "", scr);
      sys.os.gui.setMainScr(scr);
    
      # external pin defines
      sys.hw.ePinDef(5, PIN_ALT, PIN_NOPULL);
      sys.hw.ePinDef(6, PIN_ALT, PIN_NOPULL);
      uart_str = "";
      x = "";
    }
    
    function update {
    
      if (sys.gui.getEventC(btn) == EV_RELEASED) {
        sys.com.uartTrs("test" + a);
        a = a +1;
      }
    
      if (sys.gui.getEventC(btn2) == EV_RELEASED) {
        sys.com.uartRcvIT(); # start the rcv
      }
      
      if (sys.gui.getEventC(clr) == EV_RELEASED) {
        uart_str = "";
        sys.gui.setStr(txt, uart_str);
      }
    
      # waits for endline
      if (sys.com.uartGetRd() == 2) {
        x = sys.com.uartGetStr(); # loads string
        if (x != "") {
          print("" + x); # prints the string
          uart_str += x;
          sys.gui.setStr(txt, uart_str);
        }
      }
    }

