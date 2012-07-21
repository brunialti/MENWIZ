MENWIZ: YET ANOTHER CHARACTER LCD MENU WIZARD LIBRARY

This library implement LCD menu creation:

- (near) asynchronous: the library does not take mu control while working; 
  this is usefull if your sketch need to work while you are using menus
- simple to use (compact and simple code, ortogonal interface)
- user defined splash screen (optional): if you need to show something at
  startup; time elapsed is user defined.
- user defined callback screen (optional) activated after a user defined 
  elapsed time since last button push. The menu gently leave the screen to
  the user sketch code, untill any button is pushed.  
- you can run a user-defined action (callback routine) from inside the menu
- user defined variables binding: when you set a value inside a menu, the 
  value is changed also in the binded variable available to the sketch code
- built in 6-buttons and 4-buttons modes; the user can overload the  built-in
  mode, implementing its own navigation callback (e.g. 3-buttons mode, analog
  buttons, rotary encoders etc) 
- EEPROM support to save and load variables from EEPROM non volatile mamory; 
  it is possible to disable EEPROM support to spare some memory

Warnings
- MENWIZ requires New LiquidCrystal (*) and Buttons libraries, enclosed with
  the package. The New LiquidCrystal allows to use 4 wires, 8 wires, I2C, 
  serial and other LCDs. The use of New LiquidCrystal is beyond this 
  introduction
- the examples are done for 20x4 I2C LCD. other interfaces and screen dimensions
  can be used accordingly with New LiquidCrystal interface (changing 
  mode/interface does not affect library code, but only sketch code) 
- starting from 0.6.0 version, due to the EEPROM support, the sketch must
  include the file EEPROM.h (until you disable the EEPROM support)

The MENWIZ library is avalable at:
https://github.com/brunialti/MENWIZ/downloads

Please visit also the following forum topic:
http://arduino.cc/forum/index.php/topic,99693.msg866553.html#new

Comments, advices and user experiences are welcome


VERSION NUMBER CONVENTION

<version>.<subversion>.<build>
  <version>: 0=not officially released
             (check the latest available sources, the lib could work  not properly
  <subversion>: new functions, modified function interfaces
  <build>: bug fixing, minor internal changes


The TEST_LIB zip file contains the library used to test the MENWIZ lib.
I used what I felt to be "standard" libraries. 
Should you have some problem using yours actual libs, try using them

(*) New LiquidCrystal is a replacement of the standard LiquidCrystal and 
    LiquidCrystal_I2C libraries. it is more efficient and very well supported
    The nice thing about the "New LiquidCrystal" library, is that it already 
    supports a wide range of LCD controllers:
    - I2C base on the PCF8574 or compatible
    - 4bit and 8bit parallel
    - ShiftRegister: 3wire, 2wire, 1wire (beta)
    You can load the latest version at:
    https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

