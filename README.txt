MENWIZ: YET ANOTHER CHARACTER LCD MENU GENERATOR LIBRARY


DESCRIPTION
- (near) asynchronous: the library does not take mu control while working; 
  this is usefull if your sketch need to work while you are using menus
- simple to use (compact and simple code, ortogonal interface)
- user defined splash screen (optional): if you need to show something at
  startup; time elapsed is user defined.
- user defined callback screen (optional) activated after a user defined 
  elapsed time since last button push. The menu gently leave the screen to
  the user sketch code, untill any button is pushed.  
- user-defined actions (callbacks) from inside the menu
- user defined variables binding: associate (bind) a sketch variable to any 
  menu object, whenever a value is changed inside the menus, the program 
  has direct access to that value via its own (binded) variable  
- built in 6-buttons and 4-buttons modes; the user can overload the  built-in
  mode, implementing its own navigation callback (e.g. analog buttons, 
  rotary encoders etc) 
- EEPROM support to save and load variables to/from EEPROM non volatile mamory; 
  it is possible to disable EEPROM support to spare some memory
- use of F() operator to store menu strings in progmem to save the heap 
  (starting from V 1.0).
- menu objects behaviours
- Quick tour user manual 

WHAT IS NEW IN THE LATEST VERSION
Latest version: V 1.2.0 (December 26 2012):

- Solved few bug occourring when a root menu is declared as collapsed menu in ver 1.1.0 .
- It is now  possible to disable button.h support in order to save space.
- New behaviours added:  MW_MENU_INDEX (applies to menwiz objects).
- The getVer() function is now declared as #define pseudofunction, outside menwiz class.


KNOWN LIMITATIONS AND BUGS

t.b.d.

BACKWARD COMPATIBILITY

Please read the manual (chhaper "MENWIZ changes history")
 
Warnings:

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
https://github.com/brunialti/MENWIZ
You can download all the stuff as ZIP file.
Due to github policy it is not avalilable anymore the /downloads directory

Please visit also the following forum topic:
http://arduino.cc/forum/index.php/topic,99693.msg866553.html#new

Comments, advices and user experiences are welcome


VERSION NUMBER CONVENTION

<version>.<subversion>.<build>
  <version>: 0=not officially released
             (check the latest available sources, the lib could work  not properly
  <subversion>: new functions, modified function interfaces
  <build>: bug fixing, minor internal changes


The TEST_LIB zip file contains all the library *required* to use MENWIZ lib.

(*) New LiquidCrystal is a replacement of the standard LiquidCrystal and 
    LiquidCrystal_I2C libraries. it is more efficient and very well supported
    The nice thing about the "New LiquidCrystal" library, is that it already 
    supports a wide range of LCD controllers:
    - I2C base on the PCF8574 or compatible
    - 4bit and 8bit parallel
    - ShiftRegister: 3wire, 2wire, 1wire (beta)
    You can load the latest version at:
    https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home

