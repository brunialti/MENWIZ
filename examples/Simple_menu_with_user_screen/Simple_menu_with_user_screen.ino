//MENWIZ ESAMPLE
//
//Needs 6 buttons.
//Navigation tips:
//- to confirm chanhes allways push confirm button

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <buttons.h>
#include <MENWIZ.h>

// DEFINE ARDUINO PINS FOR THE NAVIGATION BUTTONS
#define UP_BOTTON_PIN       9
#define DOWN_BOTTON_PIN     10
#define LEFT_BOTTON_PIN     7 
#define RIGHT_BOTTON_PIN    8
#define CONFIRM_BOTTON_PIN  12
#define ESCAPE_BOTTON_PIN   11

//Create global object LCD and MENU
menwiz menu;
LiquidCrystal_I2C lcd(0x27,20,4);

//instantiate global variables to bind to menu
byte tp=2;
int  gp=26;
boolean wr=0;

void setup(){
  char b[84];
  _menu *r,*s1,*s2;
  _var *v; 
  int  mem;

  Serial.begin(19200);  
  // have a look on memory before menu creation
  mem=menu.freeRam();
  
  // inizialize the menu object (20 colums x 4 rows LCD)
  menu.begin(&lcd,20,4);

  //create the menu tree
  r=menu.addMenu(MW_ROOT,NULL,"MAIN MENU");           //create a root menu at first (required)
    s1=menu.addMenu(MW_SUBMENU,r,"MEASURE SUBMENU");   //add a submenu node to the root menu
      s2=menu.addMenu(MW_VAR,s1,"Choose T scale");//add a terminal node in the menu tree (that is "variable"); 
          s2->addVar(MW_LIST,&tp);                    //create the terminal node variable of type OPTION LIST and bind it to the app variable "tp"
          s2->addItem(MW_LIST,"Celsius");             //add an option to the OPTION LIST
          s2->addItem(MW_LIST,"Farenheit");           //add an other option to the OPTION LIST
          s2->addItem(MW_LIST,"Raw sensor data");     //add the third and last option to the OPTION LIST
      s2=menu.addMenu(MW_VAR,s1,"Set T1 trigger");
          s2->addVar(MW_AUTO_INT,&gp,10,100,2);
      s2=menu.addMenu(MW_VAR,s1,"Enable heating");
          s2->addVar(MW_BOOLEAN,&wr);

  //(optional) create a splash screen (lap 6 seconds) with some usefull infos 
  //the character # marks end of line
  //(tip): use preallocated internal menu.sbuf buffer to save memory space!
  sprintf(menu.sbuf,"MENWIZ TEST V %s#Free memory   : %d#Menu mem alloc: %d#Splash lap sec: %d",menu.getVer(),menu.freeRam(),mem-menu.freeRam(),6);
  menu.addSplash((char *) menu.sbuf, 6000);
 
  //declare navigation buttons (required)
  // equivalent shorter call: menu.navButtons(9,10,7,8,11,12);
  menu.navButtons(UP_BOTTON_PIN,DOWN_BOTTON_PIN,LEFT_BOTTON_PIN,RIGHT_BOTTON_PIN,ESCAPE_BOTTON_PIN,CONFIRM_BOTTON_PIN);
  // create a user define screen callback to activate after 7 secs since last button push 
  menu.addUsrScreen(msc,7000);
  }

void loop(){
  // NAVIGATION MANAGEMENT & DRAWING ON LCD. NOT BLOCKING
  menu.draw(); 
  //PUT APPLICATION CODE HERE
  }

// user defined default screen
void msc(){
  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print("Test user screen");
  lcd.setCursor(0,2);
  lcd.print(millis()/1000);  lcd.print("        ");
  lcd.setCursor(0,3);
  lcd.print("                    ");
  }

