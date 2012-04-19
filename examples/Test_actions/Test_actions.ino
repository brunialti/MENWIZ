//MENWIZ GOLF ROBOT EXAMPLE
// choose mode, set distance to fire (yards for all modee except feets for putt) and fire! 
//REMEMBER TO OPEN THE SERIAL TERMINAL ...

#include <Wire.h>
//INSERT ALL THE FOLLOWING 4 INCLUDES AFTER INCLUDING WIRE LIB 
#include <LCD.h>
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

//Create global object menu and lcd
menwiz menu;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Addr, En, Rw, Rs, d4, d5, d6, d7, backlighpin, polarity

//instantiate global variables to bind to menu
int m=2;
int  fd=0,yd=0;

void setup(){
  _menu *r,*s1;

  Serial.begin(19200);  
 
  // inizialize the menu object (20 colums x 4 rows LCD)
  menu.begin(&lcd,20,4);

  //create the menu tree
  r=menu.addMenu(MW_ROOT,NULL,"GOLF ROBOT");           //create a root menu at first (required)
      s1=menu.addMenu(MW_VAR,r,"Modes menu");          //add a terminal node in the menu tree (that is "variable"); 
          s1->addVar(MW_LIST,&m);                      //create the terminal node variable of type OPTION LIST and bind it to the app variable "m"
          s1->addItem(MW_LIST,"Drive");                //add an option to the OPTION LIST
          s1->addItem(MW_LIST,"Punch");                //add an other option to the OPTION LIST
          s1->addItem(MW_LIST,"Chip");                 //add the third option to the OPTION LIST
          s1->addItem(MW_LIST,"Putt");                 //add the last option to the OPTION LIST
      s1=menu.addMenu(MW_VAR,r,"Putt Dist.(feets)");   //create an other "variable" menu terminal mode
          s1->addVar(MW_AUTO_INT,&fd,0,100,1);         //int type, fd binded variable, rage 0-100, step 1
      s1=menu.addMenu(MW_VAR,r,"Other Dist. (yrds)");
          s1->addVar(MW_AUTO_INT,&yd,0,300,5);         //int type, yd binded variable, rage 0-300, step 5
      s1=menu.addMenu(MW_VAR,r,"Fire action");         //latest menu entry
          s1->addVar(MW_ACTION,fireAction);            // associate an action (variable of type function) to the menu entry

  //declare navigation buttons (required)
  // equivalent shorter call: menu.navButtons(9,10,7,8,11,12);
  menu.navButtons(UP_BOTTON_PIN,DOWN_BOTTON_PIN,LEFT_BOTTON_PIN,RIGHT_BOTTON_PIN,ESCAPE_BOTTON_PIN,CONFIRM_BOTTON_PIN);
  }

void loop(){
  // NAVIGATION MANAGEMENT & DRAWING ON LCD. NOT BLOCKING
  menu.draw(); 

  //PUT APPLICATION CODE HERE
  // if any .... :-)
  }

// user defined action for fire action 
void fireAction(){
  Serial.print("FIRED ");
  switch (m){
    case 0:
      Serial.print("Drive to ");Serial.print(yd); Serial.println(" yrds");
      break;
    case 1:
      Serial.print("Punch to ");Serial.print(yd); Serial.println(" yrds");
      break;
    case 2:
      Serial.print("Chip to ");Serial.print(yd); Serial.println(" yrds");
      break;
    case 3:
      Serial.print("Put  to "); Serial.print(fd); Serial.println(" feets");
      break;
    default:
      break;
    }
  }

