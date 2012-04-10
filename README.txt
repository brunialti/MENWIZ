MENWIZ YET ANOTHER CHARACTER LCD MENU WIZARD LIBRARY
>>>>STILL IN BETA <<<<

This library implement menu simple creation.
The pros are:
- (near) asynchronous: that is the library does not take mu control while working, usefull if your sketch need to work while you are changing the settings
- simple to use (few primitives and compact code)
- user defined splash screen (optional): if you need to show something at startup. The time is user defined.
- user defined callback default screen (optional) activated after a user defined elapsed time since last button push. The menu gently leave the screen to the user, untill any button is pushed.  
- you can run a user-defined action (callback routine) from inside the menu
- user defined variables binding: when you set a value inside a menu, the value is changed also in the binded variable available to the sketch code

The cons are:
- still in beta: do not look for clean code and full featured library
- few data types available for parameters (option lists, integer, boolean, actions), more are to be implemented
- at this stage only a simple "6 buttons" navigation mode is implemented; a generic user defined navigation callback soon available 
- the library footprint is not very small. The provided example sketch dimension exceed 12k  

Warnings
- MENWIZ requires New LiquidCrystal (*) and Buttons libraries, enclosed with the package. The New LiquidCrystal allows to use 4 wires, 8 wires, I2C, serial and other LCDs. The use of New LiquidCrystal is beyond this introduction
- Menu labels require memory. Be carefull not to run out of  memory (the library provides a simple memory-check function: use it!)
- the examples are done for 20x4 I2C LCD. other interfaces and screen dimensions can be used accordingly with New LiquidCrystal interface (changing mode/interface does not affect library code, but only sketch code) 

The library is avalable at:
gethub.com/brunialti/MENWIZ

Any feed back is welcome. Should you be interested I can mantain and expand.
Keep in mind that I want to keep it simple.

Next steps are:
- new data types
. code inspection and cleaning

The version number convention is as following:
<version>.<subversion>.<build>
  <version>: 0=not officially released, you have to check the latest available sources, as the lib could not work properly
  <subversion>: new functions, modified function interfaces
  <build>: bug fixing, minor internal changes


The TEST_LIB zip file contains the library used to test the MENWIZ lib.
I used what I felt to be "standard" libraries. Should you have some problem using yours actual libs, try using them

(*) New LiquidCrystal is a replacement of the standard LiquidCrystal and LiquidCrystal_I2C libraries. it is more efficient and very well supported
That's the nice thing about the "New LiquidCrystal" library, it already supports a wide range of LCD controllers:
- I2C base on the PCF8574 or compatible
- 4bit and 8bit parallel
- ShiftRegister: 3wire, 2wire, 1wire (beta)
please have a look at http://arduino.cc/forum/index.php/topic,77120.0.html
You can load the latest version at  https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

