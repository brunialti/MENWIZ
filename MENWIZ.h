//==========.h
//<- 127
//-> 126
//   165 punto centrale
//   161 quadratino centrale

#define I2C

#ifndef MENWIZ_h
#define MENWIZ_h

#include <Wire.h>

#ifdef I2C
  #include <LiquidCrystal_I2C.h>
#else
  #include <LiquidCrystal.h>
#endif 

#include <buttons.h>

#if (ARDUINO >= 100)
  #include <Arduino.h> 
#else
  #include <WProgram.h>
#endif 

//MENU TYPE 
#define MW_ROOT      100  //root menu
#define MW_SUBMENU   101  //submenu
#define MW_VAR       102  //terminal node

//OPTION TYPE  
#define MW_SELECT    201  //multiple choice
#define MW_INPUT     202  //input value
#define MW_RANGE     203  //value range 

//CUR MODE
#define MW_MODE_SPLASH     10  
#define MW_MODE_USRSCREEN  11  
#define MW_MODE_MENU       12  

//BUTTON CODES
#define MW_BTNULL    400   //NOBUTTON
#define MW_BTU       401   //UP
#define MW_BTD       402   //DOWN
#define MW_BTL       403   //RIGTH
#define MW_BTR       404   //LEFT
#define MW_BTE       405   //ESCAPE
#define MW_BTC       406   //CONFIRM

//VALUE TYPES
#define MW_LIST        501  //OPTION LIST
#define MW_BOOLEAN     502  //ON/OFF TOGGLE
#define MW_AUTO_INT    503  //INTEGER VALUE WITH INCREMENT STEP
#define MW_AUTO_FLOAT  504  //not implemented yet
#define MW_TEXT        505  //not implemented yet
#define MW_ACTION      506  //FIRE AN ACTION WHEN CONFIRM BUTTON IS PUSHED
#define MW_EDIT_INT    507  //not implemented yet
#define MW_EDIT_FLOAT  508  //not implemented yet

#define MAX_MENUS      15
#define MAX_OPTXMENU   5

#define LAP_MENU       4000

//DEREFERENCING OPERATORS
//variables 
#define VBOOL(a)    *(boolean*)a
#define VINT(a)     *(int*)a
#define VBYTE(a)    *(byte*)a
#define VFLOAT(a)   *(float*)a
//functions 
#define VFUNC(a)    (* a)


typedef struct{
  int last_button;
  unsigned long tm_push;
  Button    BTU;   
  Button    BTD;   
  Button    BTL;   
  Button    BTR;   
  Button    BTE;   
  Button    BTC;   
}_nav;

class _var{
public:
           _var();
  int      type;
  void*    val;
  void*    old;
  void     (*action)();
  void*    lower;
  void*    upper;
  void*    incr;   
};

class _option{
public:
           _option();
  byte     type;
  char*    label;
  byte     sbm;  //submemu id if type=SUBMENU
private:
protected:
};

class _menu{
public:
           _menu();
  void     addVar(int, void*);
  void     addVar(int, void*, int, int, int);
  void     addVar(int, boolean *);
  void     addVar(int, void (*f)());
  _option* addItem(int, char*);
  _var     var;
  byte     type;
  boolean  fl_option;
  char*    label;
  byte     cod;
  byte     parent;
  byte     cur_item;
  byte     idx_o;    //option index
  void*    o[MAX_OPTXMENU];
private:
protected:
};

class menwiz{
public:
           menwiz();
  void     begin(void *,int, int);
  char*    getVer();
  void     addSplash(char *,int);
  void     addUsrScreen(void (*f)(), unsigned long);
  _menu*   addMenu(int, _menu *, char *);
  void     draw();
  void     navButtons(int,int,int,int,int,int);
  int      scanButtons();
  const char *getErrorMessage();
  int      freeRam();
  //VARIABLES
#ifdef I2C
  LiquidCrystal_I2C *lcd;
#else
  LiquidCrystal *lcd;
#endif 
  char*    sbuf;             //lcd screen buffer (+ 1 for each line) 
  byte     row;
  byte     col;
  byte     cur_mode;
  boolean  fl_splash;    
  boolean  fl_splash_draw;    
  unsigned long tm_splash;      //splash screen duration  
  boolean  fl_usrscreen;    
  boolean  fl_usrscreen_draw;    
  unsigned long tm_usrscreen;   //lap time before usrscreen  
  void     (*UsrScreen)();
  boolean  fl_menu_draw;    
  unsigned long tm_start;       //start time (set when begin method is invocated)
  byte     idx_m;
  _menu    m[MAX_MENUS];
  _menu*   cur_menu;
  _menu*   root;
  _nav     btx;
private:
  void     draw_menu(_menu *);
  void     draw_val(_menu *);
  void     draw_splash();
protected:
};

#endif


