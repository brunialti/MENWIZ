// ---------------------------------------------------------------------------
// Created by Roberto Brunialti on 20/04/12.
// Copyright 2012 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
// 
// @file MENWIZ.h
// This file implements a basic menu management library in the Arduino SDK
// 
// @brief
// This is a menu management library. The library allows user to create
// an intire menu tree with relatively few lines of code.
// The library allows the users to define callbacks able to overload internal
// functions (i.e. navigation device management) or to be executed inside a menu.
// It is possible to define also splash screens and a default user screen to be
// activated after a time interval since the last interactio,
//
// @author R. Brunialti - roberto_brunialti@tiscali.it
// ---------------------------------------------------------------------------

#ifndef MENWIZ_h
#define MENWIZ_h

#define EEPROM_SUPPORT     //uncomment if you want to use the readEeprom and writeEeprom methods!
#define BUTTON_SUPPORT     //uncomment if you want to use the readEeprom and writeEeprom methods!

#include <Wire.h>
#include <LCD.h>

#ifdef BUTTON_SUPPORT 
  #include <buttons.h>
#endif

#if (ARDUINO >= 100)
  #include <Arduino.h> 
#else
  #include <WProgram.h>
#endif 

#ifdef EEPROM_SUPPORT
  #include <EEPROM.h>
  extern EEPROMClass EEPROM;
#endif

#define getVer()         (char*) MW_ver
extern const char MW_ver[];

// DIMENSIONS (DIMENSIONAL LIMITS OF STATICALLY ALLOCATED STRUCTURES)
// ---------------------------------------------------------------------------
#define MAX_MENU       	15   //maximum number of nodes (absolute supported max number of addMenu calls)
#define MAX_OPTXMENU   	9    //maximum number of options/submenus for each node (max number of addItem call for each menu item) 

// VALUE TYPES
// ---------------------------------------------------------------------------
#define MW_LIST         11  //OPTION LIST
#define MW_BOOLEAN     	12  //ON/OFF TOGGLE
#define MW_AUTO_INT    	13  //INTEGER VALUE WITH INCREMENT STEP
#define MW_AUTO_FLOAT  	14  //FLOATING POINT VALUE WITH INCREMENT STEP
#define MW_AUTO_BYTE   	15  //byte VALUE WITH INCREMENT STEP
#define MW_TEXT        	16  //not implemented yet
#define MW_ACTION      	17  //FIRE AN ACTION WHEN CONFIRM BUTTON IS PUSHED
#define MW_EDIT_INT    	18  //not implemented yet
#define MW_EDIT_FLOAT  	19  //not implemented yet

// NODE TYPES 
// ---------------------------------------------------------------------------
#define MW_ROOT        	20  //root menu
#define MW_SUBMENU     	21  //submenu
#define MW_VAR         	22  //terminal node

// CUR MODE
// ---------------------------------------------------------------------------
#define MW_MODE_SPLASH     30  
#define MW_MODE_USRSCREEN  31  
#define MW_MODE_MENU       32  

// BUTTON CODES
// ---------------------------------------------------------------------------
#define MW_BTNULL      0   //NOBUTTON
#define MW_BTU         2   //UP
#define MW_BTD         3   //DOWN
#define MW_BTL         4   //RIGTH
#define MW_BTR         5   //LEFT
#define MW_BTE         6   //ESCAPE
#define MW_BTC         7   //CONFIRM

// DEREFERENCING OPERATORS
// ---------------------------------------------------------------------------
#define VBOOL(a)    *(boolean*)a
#define VINT(a)     *(int*)a
#define VBYTE(a)    *(byte*)a
#define VFLOAT(a)   *(float*)a
#define VFUNC(a)    (* a)

// FLAGS CLASS MENWIZ
// ---------------------------------------------------------------------------
#define FL_SPLASH 		0   		
#define FL_SPLASH_DRAW		1 	
#define FL_USRSCREEN_DRAW 	2  	
#define MW_MENU_INDEX 		3  	

// FLAGS CLASS _MENU 
// ---------------------------------------------------------------------------
#define MW_SCROLL_HORIZONTAL 	0  //Vertical/Horizontal mode
#define MW_ACTION_CONFIRM 	1  //Confirm/no confirm action mode
#define MW_LIST_2COLUMNS        2
#define MW_LIST_3COLUMNS        3
#define MW_MENU_COLLAPSED	4


// OTHERS
// ---------------------------------------------------------------------------
#define MW_EOL_CHAR    0x0A
#define MW_TYPE        uint8_t
#define MW_LCD         LCD   // this could help to change the library: your lcd data type
#define MW_LABEL       const __FlashStringHelper*  
#define MW_FLAGS       uint8_t
#define MW_4BTN        0
#define MW_6BTN        1

typedef struct{
  boolean fl;
  void (*fv)();
  int  (*fi)();
}_cback;

#ifdef BUTTON_SUPPORT
typedef struct{
  Button    BTU;   
  Button    BTD;   
  Button    BTL;   
  Button    BTR;   
  Button    BTE;   
  Button    BTC;   
}_nav;
#endif

typedef struct{
  MW_TYPE  type;
  void*    val;
  void*    old;
  void     (*action)();
  void*    lower;
  void*    upper;
  void*    incr;   
}_var;

class _option{
public:
           _option();

  MW_TYPE  type;
  MW_LABEL label;
  byte     sbm;  //submemu id if type=SUBMENU
};

class _menu{
public:
           _menu();
  void     addVar(MW_TYPE, int*);
  void     addVar(MW_TYPE, int*, int, int, int);
  void     addVar(MW_TYPE, float*, float, float, float);
  void     addVar(MW_TYPE, byte*,byte ,byte ,byte);
  void     addVar(MW_TYPE, boolean*);
  void     addVar(MW_TYPE, void (*f)());
  void     setBehaviour(MW_FLAGS,boolean);
  _option* addItem(int, MW_LABEL);

  MW_TYPE  type;
  MW_FLAGS flags;
  MW_LABEL label;
  _var     *var;
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
  void     addSplash(char *,int);
  void     addUsrScreen(void (*f)(), unsigned long);
  void     addUsrNav(int (*f)(), int);
  void     setBehaviour(MW_FLAGS,boolean);
  _menu*   addMenu(int, _menu *, MW_LABEL);
  void     draw();
  void     drawUsrScreen(char *);       //draw user screen(s)
  int      getErrorMessage(boolean); 	//if arg=true, err message is printed to the default Serial terminal, otherwise the function returns error code only
  int      freeRam();

#ifdef EEPROM_SUPPORT
  void     writeEeprom();
  void     readEeprom();
#endif

#ifdef BUTTON_SUPPORT 
  _nav*    btx;
  void     navButtons(int,int,int,int,int,int);
  void     navButtons(int,int,int,int);
  int      scanNavButtons();
#endif

  MW_FLAGS flags;
  MW_LCD*  lcd;
  char*    sbuf;                        //lcd screen buffer (+ 1 for each line) 
  _cback   usrScreen;	        	//callback
  _cback   usrNav;    
  byte     idx_m;
  _menu    m[MAX_MENU];
  _menu*   cur_menu;
  _menu*   root;
private:
  byte     row;
  byte     col;
  byte     cur_mode;
  int 	   last_button;                 //last pushed button code
  unsigned long tm_push;                //last pushed button time
  unsigned long tm_start;       	//start time (set when begin method is invocated)
  unsigned long tm_splash;      	//splash screen duration  
  unsigned long tm_usrScreen;   	//lap time before usrscreen  
  void     apply2vars(void (*f)(_menu *));
  int      actNavButtons(int);
  void     drawMenu(_menu *);
  void     drawVar(_menu *);
  void     drawList(_menu *, int);
  void     actBTU();
  void     actBTD();
  void     actBTL();
  void     actBTR();
  void     actBTE();
  void     actBTC();
protected:
};

union Etype{
  boolean bl;
  byte b;
  int i;
  float f;
  byte bytes[4];
};

#endif


