//MENWIZ ESAMPLE
#include <Wire.h>
//INSERT ALL THE FOLLOWING 5 INCLUDES AFTER INCLUDING WIRE LIB 
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <buttons.h>
#include <MENWIZ.h>
#include <EEPROM.h>    // to be included only if defined EEPROM_SUPPORT

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
int      tp=0;
float    f=26.0;
boolean  bb=0;
byte     b=50;

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
  //menu.setBehaviour(MW_MENU_INDEX,false);    

  //create the menu tree
  r=menu.addMenu(MW_ROOT,NULL,F("MAIN MENU"));              //create a root menu at first (required)
    s1=menu.addMenu(MW_SUBMENU,r,F("MEASURE SUBMENU"));     //add a child (submenu) node to the root menu
    //s1->setBehaviour(MW_MENU_COLLAPSED,true);          
    
      s2=menu.addMenu(MW_VAR,s1,F("list"));            //add a terminal node (that is "variable"); 
          s2->addVar(MW_LIST,&tp);                          //create a variable of type "option list".. 
          s2->addItem(MW_LIST,F("option 1"));               //add option to the OPTION LIST
          s2->addItem(MW_LIST,F("option 2"));               //add option to the OPTION LIST
          s2->addItem(MW_LIST,F("option 3"));               //add option to the OPTION LIST
          s2->addItem(MW_LIST,F("option 4"));               //add option to the OPTION LIST
          s2->addItem(MW_LIST,F("option 5"));               //add option to the OPTION LIST
//          s2->setBehaviour(MW_SCROLL_HORIZONTAL,true);    
//          s2->setBehaviour(MW_LIST_2COLUMNS,true);          
//          s2->setBehaviour(MW_LIST_3COLUMNS,true);          

      s2=menu.addMenu(MW_VAR,s1,F("float var"));       //add a terminal node (that is "variable"); 
          s2->addVar(MW_AUTO_FLOAT,&f,11.00,100.00,0.5); //create a variable of type "float number"... 
                                                         //...associated to the terminal node and bind it to the app variable "f" of type float
      s2=menu.addMenu(MW_VAR,s1,F("byte var"));        //add a terminal node (that is "variable"); 
          s2->addVar(MW_AUTO_BYTE,&b,25,254,10);         //create a variable of type "byte"...
                                                         //...associated to the terminal node and bind it to the app variable "b" of typr byte
      s2=menu.addMenu(MW_VAR,s1,F("boolean var"));     //add a terminal node (that is "variable"); 
          s2->addVar(MW_BOOLEAN,&bb);                    //create a variable of type "boolean" 
                                                         //...associated to the terminal node and bind it to the app variable "bb" of type boolean
    s1=menu.addMenu(MW_VAR,r,F("WRITE TO SERIAL"));             //add a terminal node (that is "variable") create an "action" associated to the terminal node... 
      s1->addVar(MW_ACTION,act);                         //the act function as default will be called when enter button is pushed
//      s1->setBehaviour(MW_ACTION_CONFIRM,false);         //...if you don't need action confirmation

    s1=menu.addMenu(MW_VAR,r,F("SAVE TO EPROM"));           //add a terminal node (that is "variable") create an "action" associated to the terminal node... 
      s1->addVar(MW_ACTION,savevar);                     //the act function as default will be called when enter button is pushed

    s1=menu.addMenu(MW_VAR,r,F("LOAD FROM EEPROM"));        //add a terminal node (that is "variable") create an "action" associated to the terminal node... 
      s1->addVar(MW_ACTION,loadvar);                     //the act function as default will be called when enter button is pushed

  //declare navigation buttons (required)
  menu.navButtons(UP_BUTTON_PIN,DOWN_BUTTON_PIN,ESCAPE_BUTTON_PIN,CONFIRM_BUTTON_PIN);

  //(optional)create a user define screen callback to activate after 10 secs (10.000 millis) since last button push 
  menu.addUsrScreen(msc,10000);

  //(optional) create a splash screen (duration 5.000 millis)with some usefull infos the character \n marks end of LCD line 
  //(tip): use preallocated internal menu.sbuf buffer to save memory space!
//  sprintf(menu.sbuf,"MENWIZ TEST V %s\n.Free mem. :%d\n.Used mem  :%d\n.Lap secs  :%d",menu.getVer(),menu.freeRam(),mem-menu.freeRam(),5);
//  menu.addSplash((char *) menu.sbuf, 5000);
  }

void loop(){
  menu.draw(); 
  //PUT APPLICATION CODE HERE (if any)
  }

// user defined callbacks
// WARNING avoid sprintf usage: it requires > 1.5 kbytes of memory! 
void msc(){
  static  char buf[7];
  strcpy(menu.sbuf,"User screen"); //1st lcd line
  strcat(menu.sbuf,"\nUptime (s): ");strcat(menu.sbuf,itoa((int)(millis()/1000),buf,10));//2nd lcd line
  strcat(menu.sbuf,"\nFree mem  : ");strcat(menu.sbuf,itoa((int)menu.freeRam(),buf,10));//3rd lcd line
  strcat(menu.sbuf,"\n"); //4th lcd line (empty)
  menu.drawUsrScreen(menu.sbuf);
  }
  
void act(){
  Serial.println("FIRED ACTION!");
 }
 
void savevar(){
  menu.writeEeprom();
  }
  
void loadvar(){
  menu.readEeprom();
  }

