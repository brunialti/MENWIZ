MENWIZ YET ANOTHER CHARACTER LCD MENU WIZARD LIBRARY

I implemented a sketch to manage sensor parameters with LCDs without stopping the mesure in real time.
I abstracted the menu management and I wrote a new library called MENWIZ (menu wizard). 

The pros are:
- (near) asynchronous: that is the library does not take mu control while working, usefull if your sketch need to work while you are changing the settings
- simple to use (few primitives and compact code)
- user defined splash screen (optional): if you need to show something at startup. The time is user defined.
- user defined callback default screen (optional) activated after a user defined elapsed time since last button push. The menu gently leave the screen to the user, untill any button is pushed.  
- you can run a user-defined action (callback routine) from inside the menu
- automatic user defined variables binding: that is all parameters changes inside the menus affects also the user defined binded variables 

The cons are:
- do not look for clean code and full featured library, MENUWIZ is still at the level of proof of concept (may be a little bit further ...)
- few data types available for parameters (option lists, integer, boolean), more are to be implemented
- at this stage only a simple "6 buttons" navigation mode is implemented
- the library footprint is not very small. The provided example sketch dimension exceed 12k  

Warnings
- MENWIZ requires LiquidCrystal_I2C (*) and Buttons libraries, enclosed with the package
- I tested MENWIZ mainly with 4x20 I2C lcd screens. It *should* work also with other lcds (e.g 16x2)), but I did'nt tested it extensively
- Menu labels require memory. Be carefull not to run out of  memory (the library provides a simple memory-check function: use it!)

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
I used what i felt to be "standard" libraries. Should you have some problem using yours actual libs, try using them

(*)NOT TESTED YET: in MENWIZ.h if you comment the #define I2C statement, the lib *should* work with LiquidCrystal library thanks to conditional compiler directives
