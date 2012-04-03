MENWIZ YET ANOTHER CHARACTER LCD MENU WIZARD LIBRARY

I had to implement a sketch for sensor management with LCD to set some parameters and to show the mesure in real time.
I abstracted the menu management and i wrote a new library called MENWIZ (menu wizard). 

The pros are:
- non blocking: that is the library does not take mu control while working, usefull if your sketch need to work while you are changing the settings
- simple to use (few primitives and compact code)
- user defined splash screen (optional): if you need to show something at startup. The time is user defined.
- user defined callback default screen (optional) activated after a user defined elapsed time since last button push. The menu gently leave the screen to the user, untill any button is pushed.  
- automatic user defined variables binding: that is all parameters changes inside the menus affects also the user defined binded variables 

The cons are
- do not look for clean code and full featured library, MENUWIZ is still at the level of proof of concept
- few data types available for parameters (option lists, integer, boolean), more are to be implemented
- at this stage only a simple "6 buttons" navigation mode is implemented
- the library footprint is not very small. The provided example sketch dimension exceed 12k  

Warnings
- MENWIZ requires LiquidCrystal_I2C and Buttons libraries, enclosed with the package
- I tested MENWIZ mainly with 4x20 I2C lcd screens. It *should* work also with other lcds (e.g 16x2)), but i did'nt testvit extensively
- Menu labels require memory. Be carefull not to run out of  memory (a simple memory-check function is included inthe lib: use it!)

The library is avalable at:
gethub.com/brunialti/MENWIZ

Any feed back is welcome. Should you be interested I can mantain and expand.
Keep in mind that I want to keep it simple.

Next steps are:
- new data types
- user defined navigation callback routine (so you can use any navigation device you like, inside the framework)
- actions: callback routines to be executed from menu 

The TEST_LIB zip file contains the library used to test the MENWIZ lib.
It should be standard, neverthless should you have some problem, try using it
