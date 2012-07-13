//MENWIZ ESAMPLE
#include <Wire.h>
//INSERT ALL THE FOLLOWING 4 INCLUDES AFTER INCLUDING WIRE LIB 
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <buttons.h>
#include <MENWIZ.h>

// DEFINE ARDUINO PINS FOR THE NAVIGATION BUTTONS
#define UP_BUTTON_PIN       9
#define DOWN_BUTTON_PIN     10
#define LEFT_BUTTON_PIN     7 
#define RIGHT_BUTTON_PIN    8
#define CONFIRM_BUTTON_PIN  12
#define ESCAPE_BUTTON_PIN   11

//Create global object menu and lcd
menwiz menu;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Addr, En, Rw, Rs, d4, d5, d6, d7, backlighpin, polarity

//instantiate global variables to bind to menu
int tp=0;
float f=26.0;
boolean bb=0;
byte b=50;

void setup(){
  _menu *r,*s1,*s2;
  _var *v; 
  int  mem;

  Serial.begin(19200);  
  
  // have a look on memory before menu creation
  Serial.println(sizeof(menwiz));
  mem=menu.freeRam();
  
  // inizialize the menu object (20 colums x 4 rows)
  menu.begin(&lcd,20,4);

  //create the menu tree
  r=menu.addMenu(MW_ROOT,NULL,"MAIN MENU");           //create a root menu at first (required)
    s1=menu.addMenu(MW_SUBMENU,r,"MEASURE SUBMENU");  //add a child (submenu) node to the root menu
      s2=menu.addMenu(MW_VAR,s1,"Test list");         //add a terminal node (that is "variable"); 
          s2->addVar(MW_LIST,&tp);                       //create a variable of type "option list".. 
                                                         //...associated to the terminal node and bind it to the app variable "tp" of type int
          s2->addItem(MW_LIST,"option 1");            //add option to the OPTION LIST
          s2->addItem(MW_LIST,"option 2");            //add option to the OPTION LIST
          s2->addItem(MW_LIST,"option 3");            //add option to the OPTION LIST
      s2=menu.addMenu(MW_VAR,s1,"Test float var");    //add a terminal node (that is "variable"); 
          s2->addVar(MW_AUTO_FLOAT,&f,11.00,100.00,0.5); //create a variable of type "float number"... 
                                                         //...associated to the terminal node and bind it to the app variable "f" of type float
      s2=menu.addMenu(MW_VAR,s1,"Test byte var");     //add a terminal node (that is "variable"); 
          s2->addVar(MW_AUTO_BYTE,&b,25,254,10);         //create a variable of type "byte"...
                                                         //...associated to the terminal node and bind it to the app variable "b" of typr byte
      s2=menu.addMenu(MW_VAR,s1,"Test boolean var");  //add a terminal node (that is "variable"); 
          s2->addVar(MW_BOOLEAN,&bb);                    //create a variable of type "boolean" 
                                                         //...associated to the terminal node and bind it to the app variable "bb" of type boolean
    s1=menu.addMenu(MW_VAR,r,"TEST ACTION")              //add a terminal node (that is "variable"); 
      s1->addVar(MW_ACTION,act);                         //create an "action" associated to the terminal node... 
                                                         //...the act function will be called when enter button is pushed
     
  //(optional) create a splash screen with some usefull infos 
  //the character \n marks end of line 
  //(tip): use preallocated internal menu.sbuf buffer to save memory space!
  sprintf(menu.sbuf,"MENWIZ TEST V %s\n.Free mem. :%d\n.Used mem  :%d\n.Lap secs  :%d",menu.getVer(),menu.freeRam(),mem-menu.freeRam(),5);
  menu.addSplash((char *) menu.sbuf, 5000);
 
  //declare navigation buttons (required)
  menu.navButtons(UP_BUTTON_PIN,DOWN_BUTTON_PIN,LEFT_BUTTON_PIN,RIGHT_BUTTON_PIN,ESCAPE_BUTTON_PIN,CONFIRM_BUTTON_PIN);
  //create a user define screen callback to activate after 7 secs since last button push 
  menu.addUsrScreen(msc,10000);
  }

void loop(){
  // NAVIGATION & DRAWING MANAGEMENT
  menu.draw(); 
  //PUT APPLICATION CODE HERE
  }

// user defined default screen
void msc(){
  sprintf(menu.sbuf,"User screen\nUptime (s): %ld\nFree mem  : %d\n\n",millis()/1000,(int)menu.freeRam());
  menu.drawUsrScreen(menu.sbuf);
  }

void act(){
  Serial.println("FIRED ACTION!");
  }

