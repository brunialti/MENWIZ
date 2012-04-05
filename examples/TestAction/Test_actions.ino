//MENWIZ ESAMPLE
//
//Needs 6 buttons.
//Navigation tips:
//- to confirm changes allways push confirm button

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
byte m=0;
int  fd=0,yd=0;

void setup(){
  _menu *r,*s1;

  Serial.begin(19200);  
 
  // inizialize the menu object (20 colums x 4 rows LCD)
  menu.begin(&lcd,20,4);

  //create the menu tree
  r=menu.addMenu(MW_ROOT,NULL,"GOLF ROBOT");           //create a root menu at first (required)
      s1=menu.addMenu(MW_VAR,r,"Modes");    //add a terminal node in the menu tree (that is "variable"); 
          s1->addVar(MW_LIST,&m);                    //create the terminal node variable of type OPTION LIST and bind it to the app variable "tp"
          s1->addItem(MW_LIST,"Drive");             //add an option to the OPTION LIST
          s1->addItem(MW_LIST,"Punch");           //add an other option to the OPTION LIST
          s1->addItem(MW_LIST,"Chip");     //add the third and last option to the OPTION LIST
          s1->addItem(MW_LIST,"Putt");     //add the third and last option to the OPTION LIST
      s1=menu.addMenu(MW_VAR,r,"Putt Dist.(feets)");
          s1->addVar(MW_AUTO_INT,&fd,0,100,1);
      s1=menu.addMenu(MW_VAR,r,"Other Dist. (yrds)");
          s1->addVar(MW_AUTO_INT,&yd,0,300,5);
      s1=menu.addMenu(MW_VAR,r,"Fire action");
          s1->addVar(MW_ACTION,fireAction);

  //declare navigation buttons (required)
  // equivalent shorter call: menu.navButtons(9,10,7,8,11,12);
  menu.navButtons(UP_BOTTON_PIN,DOWN_BOTTON_PIN,LEFT_BOTTON_PIN,RIGHT_BOTTON_PIN,ESCAPE_BOTTON_PIN,CONFIRM_BOTTON_PIN);
  }

void loop(){
  // NAVIGATION MANAGEMENT & DRAWING ON LCD. NOT BLOCKING
  menu.draw(); 
  //PUT APPLICATION CODE HERE
  }

// user defined default screen
void fireAction(){
  Serial.print("FIRED ");
  switch (m){
    case 0:
      Serial.print("Drive to ");
      Serial.print(yd); Serial.println(" yrds");
      break;
    case 1:
      Serial.print("Punch to ");
      Serial.print(yd); Serial.println(" yrds");
      break;
    case 2:
      Serial.print("Chip to ");
      Serial.print(yd); Serial.println(" yrds");
      break;
    case 3:
      Serial.print("Put  to ");
      Serial.print(fd); Serial.println(" feets");
      break;
    default:
      break;
    }
  Serial.print(fd); Serial.println("ACE!");
  }

