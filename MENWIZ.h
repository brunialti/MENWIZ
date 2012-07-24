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

#include <Wire.h>
#include <LCD.h>
#include <buttons.h>

#if (ARDUINO >= 100)
  #include <Arduino.h> 
#else
  #include <WProgram.h>
#endif 

#ifdef EEPROM_SUPPORT
  #include <EEPROM.h>
  extern EEPROMClass EEPROM;
#endif

// DIMENSIONS (DIMENSIONAL LIMITS OF STATICALLY ALLOCATED STRUCTURES)
// ---------------------------------------------------------------------------
#define MAX_MENU       15   //maximum number of nodes (absolute supported max number of addMenu calls)
#define MAX_OPTXMENU   5    //maximum number of options/submenus for each node (max number of addItem call for each menu item) 
#define MAX_BUFFER     84   //dimension=columns x rows + rows. CHANGE IT IF LCD IS BIGGER THAN 4X20 CHARS

// VALUE TYPES
// ---------------------------------------------------------------------------
#define MW_LIST        11  //OPTION LIST
#define MW_BOOLEAN     12  //ON/OFF TOGGLE
#define MW_AUTO_INT    13  //INTEGER VALUE WITH INCREMENT STEP
#define MW_AUTO_FLOAT  14  //FLOATING POINT VALUE WITH INCREMENT STEP
#define MW_AUTO_BYTE   15  //byte VALUE WITH INCREMENT STEP
#define MW_TEXT        16  //not implemented yet
#define MW_ACTION      17  //FIRE AN ACTION WHEN CONFIRM BUTTON IS PUSHED
#define MW_EDIT_INT    18  //not implemented yet
#define MW_EDIT_FLOAT  19  //not implemented yet

// NODE TYPES 
// ---------------------------------------------------------------------------
#define MW_ROOT        20  //root menu
#define MW_SUBMENU     21  //submenu
#define MW_VAR         22  //terminal node

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
#define FL_SPLASH 		1   		
#define FL_SPLASH_DRAW		2 	
#define FL_USRSCREEN_DRAW 	3  	

// BEHAVIOUR MODE FLAG BIT
// ---------------------------------------------------------------------------
#define MW_SCROLL_HORIZONTAL 	1  //Vertical/Horizontal mode
#define MW_ACTION_CONFIRM 	2  //Confirm/no confirm action mode

// OTHERS
// ---------------------------------------------------------------------------
#define MW_EOL_CHAR    0x0A
#define MW_TYPE uint8_t
#define MW_LABEL char*
#define MW_FLAGS uint8_t
#define MW_4BTN 0
#define MW_6BTN 1

typedef struct{
  boolean fl;
  void (*fv)();
  int  (*fi)();
}_cback;

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
  _option* addItem(int, char*);

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
  void     addUsrNav(int (*f)());
  _menu*   addMenu(int, _menu *, char *);
  void     draw();
  void     navButtons(int,int,int,int,int,int);
  void     navButtons(int,int,int,int);
  void     drawUsrScreen(char *);       //draw user screen(s)
  int      getErrorMessage(boolean); 	//if arg=true, err message is printed to the default Serial terminal, otherwise the function returns error code only
  int      freeRam();
  char*    getVer();
  MW_FLAGS flags;
  LCD*     lcd;
  char*    sbuf;                        //lcd screen buffer (+ 1 for each line) 
  unsigned long tm_start;       	//start time (set when begin method is invocated)
  unsigned long tm_splash;      	//splash screen duration  
  unsigned long tm_usrScreen;   	//lap time before usrscreen  
  _cback   usrScreen;	        	//callback
  _cback   usrNav;    
  byte     idx_m;
  _menu    m[MAX_MENU];
  _menu*   cur_menu;
  _menu*   root;
  _nav*    btx;
  void     apply2vars(void (*f)(_menu *));
#ifdef EEPROM_SUPPORT
  void     writeEeprom();
  void     readEeprom();
#endif
private:
  byte     row;
  byte     col;
  byte     cur_mode;
  int      scanNavButtons();
  int      actNavButtons(int);
  void     drawMenu(_menu *);
  void     drawVar(_menu *);
  void     actBTU();
  void     actBTD();
  void     actBTL();
  void     actBTR();
  void     actBTE();
  void     actBTC();
protected:
};

   union EFloat{
      float f;
      byte bytes[4];};
	 
   union EInt{
      int i;
      byte bytes[2];};

   union EBool{
      boolean b;
      byte bytes[1];};
	 
   union EByte{
      byte b;
      byte bytes[1];};

#endif


