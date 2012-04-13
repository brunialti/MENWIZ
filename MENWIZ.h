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
// This is an of a manu management library. The library allows user to create
// an intire menu tree with relatively few lines of code.
// The library allows the users to define callbacks able to overload internal
// functions (as navigation device management) or to be executed inside a menu.
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

// NODE TYPES 
// ---------------------------------------------------------------------------
#define MW_ROOT      100  //root menu
#define MW_SUBMENU   101  //submenu
#define MW_VAR       102  //terminal node

// CUR MODE
// ---------------------------------------------------------------------------
#define MW_MODE_SPLASH     10  
#define MW_MODE_USRSCREEN  11  
#define MW_MODE_MENU       12  

// BUTTON CODES
// ---------------------------------------------------------------------------
#define MW_BTNULL    400   //NOBUTTON
#define MW_BTU       401   //UP
#define MW_BTD       402   //DOWN
#define MW_BTL       403   //RIGTH
#define MW_BTR       404   //LEFT
#define MW_BTE       405   //ESCAPE
#define MW_BTC       406   //CONFIRM

// VALUE TYPES
// ---------------------------------------------------------------------------
#define MW_LIST        501  //OPTION LIST
#define MW_BOOLEAN     502  //ON/OFF TOGGLE
#define MW_AUTO_INT    503  //INTEGER VALUE WITH INCREMENT STEP
#define MW_AUTO_FLOAT  504  //not implemented yet
#define MW_TEXT        505  //not implemented yet
#define MW_ACTION      506  //FIRE AN ACTION WHEN CONFIRM BUTTON IS PUSHED
#define MW_EDIT_INT    507  //not implemented yet
#define MW_EDIT_FLOAT  508  //not implemented yet

// DEREFERENCING OPERATORS
// ---------------------------------------------------------------------------
#define VBOOL(a)    *(boolean*)a
#define VINT(a)     *(int*)a
#define VBYTE(a)    *(byte*)a
#define VFLOAT(a)   *(float*)a
#define VFUNC(a)    (* a)

// OTHERS
// ---------------------------------------------------------------------------
#define MAX_MENU       15   //maximum number of nodes
#define MAX_OPTXMENU   5    //maximum number of oprions for each node
#define LAP_MENU       4000
#define MW_EOL_CHAR    0x0A

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
  int      type;
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
  void     addVar(int, boolean *);
  void     addVar(int, void (*f)());
  _option* addItem(int, char*);
  int      type;
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
  char*    getVer();
  void     addSplash(char *,int);
  void     addUsrScreen(void (*f)(), unsigned long);
  void     addUsrNav(int (*f)());
  _menu*   addMenu(int, _menu *, char *);
  void     draw();
  void     navButtons(int,int,int,int,int,int);
  int      getErrorMessage(boolean); 	//if arg=true, err message is printed to the default Serial terminal, otherwise the function returns error code only
  int      freeRam();
  LCD *    lcd;
  char*    sbuf;             		//lcd screen buffer (+ 1 for each line) 
  byte     row;
  byte     col;
  byte     cur_mode;
  unsigned long tm_start;       	//start time (set when begin method is invocated)
  //splash screen mngt
  boolean  fl_splash;                   //splash screen required
  boolean  fl_splash_draw;    		//yet drawn flag
  unsigned long tm_splash;      	//splash screen duration  
  //user default screen mngt
  _cback   usrScreen;	        	//callback
  boolean  fl_usrScreen_draw;   	//yet drawn flag 
  unsigned long tm_usrScreen;   	//lap time before usrscreen  
  void     drawUsrScreen(char *);       //draw user screen(s)
  //user defined navigation device callback
  _cback   usrNav;    

  byte     idx_m;
  _menu    m[MAX_MENU];
  _menu*   cur_menu;
  _menu*   root;
  _nav     btx;
private:
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


