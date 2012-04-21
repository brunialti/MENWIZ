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

#include <Wire.h>
#include <LCD.h>
#include <buttons.h>

#if (ARDUINO >= 100)
  #include <Arduino.h> 
#else
  #include <WProgram.h>
#endif 

// DIMENSIONS (DIMENSIONAL LIMITS OF STATICALLY ALLOCATED STRUCTURES)
// ---------------------------------------------------------------------------
#define MAX_MENU       15   //maximum number of nodes (absolute supported max number of addMenu calls)
#define MAX_OPTXMENU   5    //maximum number of options/submenus for each node (max number of addItem call for each menu item) 
#define MAX_BUFFER     84   //dimension=columns x rows + rows. CHANGE IT IF LCD IS BIGGER THAN 4X20 CHARS

// NODE TYPES 
// ---------------------------------------------------------------------------
#define MW_ROOT        10  //root menu
#define MW_SUBMENU     11  //submenu
#define MW_VAR         12  //terminal node

// CUR MODE
// ---------------------------------------------------------------------------
#define MW_MODE_SPLASH     20  
#define MW_MODE_USRSCREEN  21  
#define MW_MODE_MENU       22  

// BUTTON CODES
// ---------------------------------------------------------------------------
#define MW_BTNULL      30   //NOBUTTON
#define MW_BTU         31   //UP
#define MW_BTD         32   //DOWN
#define MW_BTL         33   //RIGTH
#define MW_BTR         34   //LEFT
#define MW_BTE         35   //ESCAPE
#define MW_BTC         36   //CONFIRM

// VALUE TYPES
// ---------------------------------------------------------------------------
#define MW_LIST        41  //OPTION LIST
#define MW_BOOLEAN     42  //ON/OFF TOGGLE
#define MW_AUTO_INT    43  //INTEGER VALUE WITH INCREMENT STEP
#define MW_AUTO_FLOAT  44  //FLOATING POINT VALUE WITH INCREMENT STEP
#define MW_AUTO_BYTE   45  //byte VALUE WITH INCREMENT STEP
#define MW_TEXT        46  //not implemented yet
#define MW_ACTION      47  //FIRE AN ACTION WHEN CONFIRM BUTTON IS PUSHED
#define MW_EDIT_INT    48  //not implemented yet
#define MW_EDIT_FLOAT  49  //not implemented yet

// DEREFERENCING OPERATORS
// ---------------------------------------------------------------------------
#define VBOOL(a)    *(boolean*)a
#define VINT(a)     *(int*)a
#define VBYTE(a)    *(byte*)a
#define VFLOAT(a)   *(float*)a
#define VFUNC(a)    (* a)

// FLAGS
// ---------------------------------------------------------------------------
#define FL_SPLASH 		1   		
#define FL_SPLASH_DRAW		2 	
#define FL_USRSCREEN_DRAW 	3  	

// OTHERS
// ---------------------------------------------------------------------------
#define MW_EOL_CHAR    0x0A
#define MW_TYPE byte

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

class _var{
public:
           _var();

  MW_TYPE  type;
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

  MW_TYPE  type;
  char*    label;
  byte     sbm;  //submemu id if type=SUBMENU
private:
protected:
};

class _menu{
public:
           _menu();
  void     addVar(int, int*);
  void     addVar(int, int*, int, int, int);
  void     addVar(int, float*, float, float, float);
  void     addVar(int, byte *,byte ,byte ,byte);
  void     addVar(int, boolean *);
  void     addVar(int, void (*f)());
  _option* addItem(int, char*);

  MW_TYPE  type;
  _var     var;
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
  void     addSplash(char *,int);
  void     addUsrScreen(void (*f)(), unsigned long);
  void     addUsrNav(int (*f)());
  _menu*   addMenu(int, _menu *, char *);
  void     draw();
  void     navButtons(int,int,int,int,int,int);
  void     drawUsrScreen(char *);       //draw user screen(s)
  int      getErrorMessage(boolean); 	//if arg=true, err message is printed to the default Serial terminal, otherwise the function returns error code only
  int      freeRam();
  char*    getVer();
  byte     flags;
  LCD *    lcd;
  char     sbuf[MAX_BUFFER];            //lcd screen buffer (+ 1 for each line) 
  unsigned long tm_start;       	//start time (set when begin method is invocated)
  unsigned long tm_splash;      	//splash screen duration  
  unsigned long tm_usrScreen;   	//lap time before usrscreen  
  _cback   usrScreen;	        	//callback
  _cback   usrNav;    
  byte     idx_m;
  _menu    m[MAX_MENU];
  _menu*   cur_menu;
  _menu*   root;
  _nav     btx;
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

#endif


