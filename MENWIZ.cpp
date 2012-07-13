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
// @file MENWIZ.cpp
// This file implements a basic menu management library in the Arduino SDK
// 
// @brief 
// This is a menu management library. The library allows user to create
// an intire menu tree with relatively few lines of code.
// The library allows the users to define callbacks able to overload internal
// functions (i.e. navigation device management) or to be executed inside a menu.
// It is possible to define also splash screens and a default user screen to be
// activated after a time interval since the last interaction
//
// @author R. Brunialti - roberto_brunialti@tiscali.it
// ---------------------------------------------------------------------------
#include "MENWIZ.h"

#define SCREATE(p,s) p=(char *)malloc(strlen((char *)s)+1); strcpy((char *)p,(char *)s)
#define SFORM(b,s,l) memset(b,32,l); memcpy(b,s,strlen(s)); b[l]=NULL; lcd->print(b)
#define ERROR(a)     MW_error=a
#define FILLBLANK(b,l) memset(b,32,l);b[l]=NULL; lcd->print(b)

// GLOBAL VARIABLES
// ---------------------------------------------------------------------------
int MW_FLOAT_DEC=1;  //decimal digits in float screen representation
char buf[81];
const char MW_ver[]={"0.5.3"};
const char MW_FMT_VARINT[]={"%d [%d] %d"};
const char MW_STR_CONFIRM[]={"[Confirm] to run."};
const uint8_t c0[8]={B00000, B00000, B00001, B00010, B10100, B01000, B00000, B00000}; 
byte MW_error;
byte MW_navbtn=0;
boolean MW_invar=false;


menwiz::menwiz(){
  bitWrite(flags,FL_SPLASH,0);
  bitWrite(flags,FL_SPLASH_DRAW,0);
  bitWrite(flags,FL_USRSCREEN_DRAW,0);
  usrScreen.fl=false;
  usrNav.fl=false;
  btx->last_button=MW_BTU;
  idx_m=0;
  row=0;
  col=0;
  tm_start=0;  
  root=NULL;
}

_menu::_menu(){
  idx_o=0;
  cur_item=0;
  label=NULL;
  parent=NULL;
  totopt=0;
}

_option::_option(){
  label=NULL;
}

_menu * menwiz::addMenu(int t,_menu * p, char *lab){
   _option *op;

  ERROR(0);   
  if ((idx_m==0)&&(t!=MW_ROOT)){
    ERROR(200);
    return NULL;
    }
  //INSTANTIATE NEW MENU VARIABLES
  if (idx_m<MAX_MENU){   
    m[idx_m].type=(MW_TYPE)t;         // ROOT| SUBMENU| VAR
    SCREATE(m[idx_m].label,(char *)lab);      // OPT:STORE STRING BY REFERENCE, NOT BY VALUE?
    m[idx_m].cod=idx_m;      // unique menu id
    if (t==MW_ROOT){
      //IF ROOT, PARENT=ITSELF, SET ROOT POINTER, SET ROOT AS START MENU 
      m[idx_m].parent=idx_m;
      root=&m[idx_m];
      cur_menu=&m[idx_m];
      }
    else{
      //IF NOT ROOT, ADD MENU TO THE PARENTS OPLIST 
      m[idx_m].parent=p->cod;
      op=p->addItem(t,(char *)lab);
      op->sbm=idx_m;
      }
    idx_m++;
    return &m[idx_m-1];
    }
  else{
    ERROR(100);
    return NULL;}
// ERROR    
   }


_option *_menu::addItem(int t,char *lab){
  static _option *op=NULL;

  ERROR(0);
  if (idx_o<MAX_OPTXMENU){
    op=(_option *) malloc(sizeof(_option));
    if(op==NULL){
      ERROR(900);
      }
    else {
      op->type=(MW_TYPE)t;
      SCREATE(op->label,(char *)lab);
      o[idx_o]=(_option*)op;
      idx_o++;
      }
    }
  else{ERROR(105);}
// ERROR
   totopt++;
   Serial.print(label);Serial.print(":");Serial.print(lab);Serial.print(".");Serial.println(totopt);
   return op;
   } 

void _menu::addVar(MW_TYPE t, int* v){

  ERROR(0);
  if(type==MW_ROOT){         //patch to be verified
    type=(MW_TYPE)MW_VAR;    //patch to be verified
    }	
  if (t!=MW_LIST)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_LIST;
    var->val=v;
    cur_item=VBYTE(v);
    }
  else{ERROR(110);}
// ERROR    
  }
  
void _menu::addVar(MW_TYPE t, int* v, int low, int up, int incr){

  ERROR(0);
  if (t!=MW_AUTO_INT)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_AUTO_INT;
    var->val=v;
    var->lower=malloc(sizeof(int)); if(var->lower!=NULL) VINT(var->lower)=low; else {ERROR(900); return;}
    var->upper=malloc(sizeof(int)); if(var->upper!=NULL) VINT(var->upper)=up; else {ERROR(900); return;}
    var->incr=malloc(sizeof(int));  if(var->incr!=NULL) VINT(var->incr)=incr; else {ERROR(900); return;} 
    var->old=malloc(sizeof(int));   if(var->old!=NULL) VINT(var->old)=VINT(var->val); else {ERROR(900); return;} 
    }
  else{ERROR(110);}
// ERROR    
  }

void _menu::addVar(MW_TYPE t, float* v, float low, float up, float incr){

  ERROR(0);
  if (t!=MW_AUTO_FLOAT)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_AUTO_FLOAT;
    var->val=v;
    var->lower=malloc(sizeof(float)); if(var->lower!=NULL) VFLOAT(var->lower)=low; else {ERROR(900); return;}
    var->upper=malloc(sizeof(float)); if(var->upper!=NULL) VFLOAT(var->upper)=up; else {ERROR(900); return;}
    var->incr=malloc(sizeof(float));  if(var->incr!=NULL) VFLOAT(var->incr)=incr; else {ERROR(900); return;} 
    var->old=malloc(sizeof(float));   if(var->old!=NULL) VFLOAT(var->old)=VFLOAT(var->val); else {ERROR(900); return;} 
    }
  else{ERROR(110);}
// ERROR    
  }

void _menu::addVar(MW_TYPE t, byte* v, byte low, byte up, byte incr){

  ERROR(0);
  if (t!=MW_AUTO_BYTE)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_AUTO_BYTE;
    var->val=v;
    var->lower=malloc(sizeof(byte)); if(var->lower!=NULL) VBYTE(var->lower)=low; else {ERROR(900); return;}
    var->upper=malloc(sizeof(byte)); if(var->upper!=NULL) VBYTE(var->upper)=up; else {ERROR(900); return;}
    var->incr=malloc(sizeof(byte));  if(var->incr!=NULL) VBYTE(var->incr)=incr; else {ERROR(900); return;} 
    var->old=malloc(sizeof(byte));   if(var->old!=NULL) VBYTE(var->old)=VBYTE(var->val); else {ERROR(900); return;} 
    }
  else{ERROR(110);}
// ERROR    
  }

void _menu::addVar(MW_TYPE t, boolean* v){

  ERROR(0);
  if (t!=MW_BOOLEAN)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_BOOLEAN;
    var->val=v;
    var->old=malloc(sizeof(boolean));  if(var->old!=NULL) VBOOL(var->old)=VBOOL(var->val); else {ERROR(900); return;} 
    }
  else{ERROR(110);}
// ERROR    
Serial.print("bool type=");Serial.println(t);
  }

void  _menu::addVar(MW_TYPE t,void (*f)()){

  ERROR(0);
  if (t!=MW_ACTION)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_ACTION;
    var->action=f;
    }
  else{ERROR(110);}
// ERROR    
Serial.print("action type=");Serial.println(t);
  }

void menwiz::addUsrNav(int (*f)()){
  usrNav.fl=true;
  usrNav.fi=f;
  }

void menwiz::begin(void *l,int c, int r){

  ERROR(0);
  tm_start=millis();
  row=r;
  col=c;
  flags=0;
  lcd=(LCD*)l; 
  lcd->begin(c,r);  // Size of the LCD
  lcd->setBacklight(HIGH);
  lcd->noCursor();
  lcd->createChar(0,(uint8_t*)c0);
//  sbuf=(char*)malloc(r*c+r); if(sbuf==NULL) ERROR(900);
//  buf=(char*)malloc(c+1); if(buf==NULL) ERROR(900);  
//  bitWrite(flags,FL_USRSCREEN_DRAW,0);
//  bitWrite(flags,FL_SPLASH_DRAW,0);
  }

char* menwiz::getVer(){

  ERROR(0);
  return (char*) MW_ver; 
  }


void menwiz::drawUsrScreen(char *scr){
  int i,j,k;

  ERROR(0);
  for (int i=0,j=0,k=0;(k<row)&&(i<(strlen(scr)+1));i++){
    if((scr[i]==MW_EOL_CHAR)||(scr[i]==0)){
      memset(&buf[min(j,col)],' ',col-j);
      buf[col]=0;
      lcd->setCursor(0,k);
      lcd->print(buf);
      j=0;
      k++;
      }
    else{
      buf[j]=scr[i];
      j++;
      }
    if(k==row)
      return;
    }
  }

void menwiz::draw(){
  int ret;

  ERROR(0);
  // get nav choice
  ret=usrNav.fl?usrNav.fi():scanNavButtons();    	//internal method or user defined callback?
  // if usrscreen is active, skip last button and switch to MENU mode
  if((cur_mode==MW_MODE_USRSCREEN)&&(ret!=MW_BTNULL)){
    cur_mode=MW_MODE_MENU;
    btx->last_button=MW_BTNULL;
    btx->tm_push=millis();
    } 
  // else run the action associated to selected button
  else
    actNavButtons(ret);
  
  int long lap1=(millis()-tm_start);
  int long lap2=(millis()-btx->tm_push); 

  // if defined splashscreen & not yet drawn & time window is ok, draw it  
  if((bitRead(flags,FL_SPLASH)==true) && (lap1<tm_splash)){
    cur_mode=MW_MODE_SPLASH;
    //draw only once
    if(bitRead(flags,FL_SPLASH_DRAW)==false){
      drawUsrScreen(sbuf);
      bitWrite(flags,FL_SPLASH_DRAW,1);
     }
   }
  // if defined usrscreen & time since last button push > user defined time, draw it  
  else if((usrScreen.fl) && (lap2>tm_usrScreen)){
    cur_mode=MW_MODE_USRSCREEN;
    bitWrite(flags,FL_USRSCREEN_DRAW,0);
    usrScreen.fv();
    }
  else{
  // if a button was pushed since last call, draw menu  
    cur_mode=MW_MODE_MENU;
    if((btx->last_button!=MW_BTNULL) || (!bitRead(flags,bitRead(flags,FL_USRSCREEN_DRAW)))){
      bitWrite(flags,FL_USRSCREEN_DRAW,1);
      drawMenu(cur_menu);
      }
    }
  }
  
void menwiz::drawMenu(_menu *mc){
  int rstart,rstop,i,j;
  _option *op;

  ERROR(0);
  lcd->setCursor(0,0);
  SFORM(buf,(char*)mc->label,(int) col);

  if (mc->type==MW_VAR){
    drawVar(mc);
    }
  else{
    rstart=max(0,mc->cur_item-(row-2));
    rstop=min((mc->idx_o),(rstart+row));
    for (i=1,j=rstart;i<row;i++,j++){// for all remaining lcd rows
      if(j<rstop){
        op=(_option*)mc->o[j];
        lcd->setCursor(0,i);
        lcd->write((j==mc->cur_item)?126:165);
        SFORM(buf,op->label,(int) col-1);
        }
      else{
        lcd->setCursor(0,i);
        FILLBLANK(buf,col);
        }
      }
    }    
  }

void menwiz::drawVar(_menu *mc){
  int rstart,rstop,i,j;
  MW_TYPE t;
  _option *op;

  ERROR(0);
  t=(MW_TYPE)(mc->var)->type;
  switch ((mc->var)->type){
    case MW_LIST:
      rstart=max(0,mc->cur_item-(row-2));
      rstop=min((mc->idx_o),rstart+(row));
      for (i=1,j=rstart;i<row;i++,j++){// for all remaining lcd rows
        if(j<rstop){
          op=(_option*)mc->o[j];
          lcd->setCursor(0,i);
          lcd->write((j==mc->cur_item)?0:165);
          SFORM(buf,op->label,col-1);
          }
        else{
          lcd->setCursor(0,i);
          FILLBLANK(buf,col);
          }
        }
      break;  
    case MW_AUTO_INT:
    case MW_AUTO_BYTE:
      for(i=2;i<row;i++){
        lcd->setCursor(0,i);
        FILLBLANK(buf,col);
        }
      lcd->setCursor(0,1);
      if(mc->var->type==MW_AUTO_INT){
      	sprintf(sbuf,MW_FMT_VARINT,VINT(mc->var->lower),VINT(mc->var->val),VINT(mc->var->upper));}
      else{
      	sprintf(sbuf,MW_FMT_VARINT,VBYTE(mc->var->lower),VBYTE(mc->var->val),VBYTE(mc->var->upper));}
      SFORM(buf,sbuf,col);
      break;      
    case MW_AUTO_FLOAT:
      for(i=2;i<row;i++){
        lcd->setCursor(0,i);
        FILLBLANK(buf,col);
        }
      lcd->setCursor(0,1);
      lcd->print(dtostrf(VFLOAT(mc->var->lower),0,MW_FLOAT_DEC,buf));
      lcd->print(F(" ["));
      lcd->print(dtostrf(VFLOAT(mc->var->val),0,MW_FLOAT_DEC,buf));
      lcd->print(F("] "));
      lcd->print(dtostrf(VFLOAT(mc->var->upper),0,MW_FLOAT_DEC,buf));
      break;Serial.println("ACTION");

    case MW_BOOLEAN:
      for(i=2;i<row;i++){
        lcd->setCursor(0,i);
        FILLBLANK(buf,col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,VBOOL(mc->var->val)?"ON":"OFF",(int) col);
      break;      
    case MW_ACTION:
      for(i=2;i<row;i++){
        lcd->setCursor(0,i);
        FILLBLANK(buf,col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,MW_STR_CONFIRM,(int) col);
      break;      
    default:
      ERROR(300);
    }
  }

void menwiz::addSplash(char *s, int lap){

  ERROR(0);
  strcpy(sbuf,s);
  tm_splash=lap;
  bitWrite(flags,FL_SPLASH,1);
  bitWrite(flags,FL_SPLASH_DRAW,0);
  }

void menwiz::addUsrScreen(void f(), unsigned long t){

  ERROR(0);
  usrScreen.fv=f;
  usrScreen.fl=true;
  tm_usrScreen=t;
  }

void menwiz::navButtons(int btu,int btd,int bte,int btc){
  
  ERROR(0);
  btx=(_nav*)malloc(sizeof(_nav));if(btx==NULL){ERROR(900); return;}

  if(btu!=0){btx->BTU.assign(btu);  btx->BTU.setMode(OneShot);  btx->BTU.turnOnPullUp();} 
  if(btd!=0){btx->BTD.assign(btd);  btx->BTD.setMode(OneShot);  btx->BTD.turnOnPullUp();} 
  if(bte!=0){btx->BTE.assign(bte);  btx->BTE.setMode(OneShot);  btx->BTE.turnOnPullUp();} 
  if(btc!=0){btx->BTC.assign(btc);  btx->BTC.setMode(OneShot);  btx->BTC.turnOnPullUp();} 

  // bouncing disarm 
  btx->BTU.check();
  btx->BTD.check(); 
  btx->BTE.check(); 
  btx->BTC.check(); 
  MW_navbtn=4;
  }

void menwiz::navButtons(int btu,int btd,int btl,int btr,int bte,int btc){
  
  ERROR(0);
  btx=(_nav*)malloc(sizeof(_nav));if(btx==NULL){ERROR(900); return;}
  if(btu!=0){btx->BTU.assign(btu);  btx->BTU.setMode(OneShot);  btx->BTU.turnOnPullUp();} 
  if(btd!=0){btx->BTD.assign(btd);  btx->BTD.setMode(OneShot);  btx->BTD.turnOnPullUp();} 
  if(btl!=0){btx->BTL.assign(btl);  btx->BTL.setMode(OneShot);  btx->BTL.turnOnPullUp();} 
  if(btr!=0){btx->BTR.assign(btr);  btx->BTR.setMode(OneShot);  btx->BTR.turnOnPullUp();} 
  if(bte!=0){btx->BTE.assign(bte);  btx->BTE.setMode(OneShot);  btx->BTE.turnOnPullUp();} 
  if(btc!=0){btx->BTC.assign(btc);  btx->BTC.setMode(OneShot);  btx->BTC.turnOnPullUp();} 

  // bouncing disarm 
  btx->BTU.check();
  btx->BTD.check(); 
  btx->BTL.check(); 
  btx->BTR.check(); 
  btx->BTE.check(); 
  btx->BTC.check(); 
  MW_navbtn=6;
  }

int menwiz::scanNavButtons(){ 
  if(btx->BTU.check()==ON){
    return MW_BTU;}
  else if (btx->BTD.check()==ON){
    return MW_BTD;}
  else if (btx->BTL.check()==ON){
    return MW_BTL;}
  else if (btx->BTR.check()==ON){
    return MW_BTR;}
  else if (btx->BTE.check()==ON){
    return MW_BTE;}
  else if (btx->BTC.check()==ON){
    return MW_BTC;}
  else
    return MW_BTNULL;
  }

int menwiz::actNavButtons(int button){  
  ERROR(0);
  if (button==MW_BTNULL) 
    return(button);
  else{
    btx->last_button=button;
    btx->tm_push=millis();
    switch(button){
      case MW_BTU: 
        if (MW_navbtn==6){ 
	  actBTU();}
        else{	  
          if(MW_invar){
             actBTR();}
          else
            actBTU();}
        break;   
      case MW_BTD:
        if (MW_navbtn==6){ 
	  actBTD();}
        else{
	  if(MW_invar){
             actBTL();}
          else
             actBTD();}  
        break; 
      case MW_BTL: actBTL();  break; 
      case MW_BTR: actBTR();  break; 
      case MW_BTE: actBTE();  break; 
      case MW_BTC: actBTC();  break; 
      }
    }
  }

void menwiz::actBTU(){ 
  if((cur_menu->type!=MW_VAR)||(cur_menu->var->type==MW_LIST))    
    cur_menu->cur_item=(cur_menu->cur_item-1)<0?(cur_menu->idx_o-1):cur_menu->cur_item-1;
  }

void menwiz::actBTD(){ 
  if((cur_menu->type!=MW_VAR)||(cur_menu->var->type==MW_LIST))    
    cur_menu->cur_item=(cur_menu->cur_item+1)%(cur_menu->idx_o);
  }

void menwiz::actBTL(){ 
  if(cur_menu->var->type==MW_AUTO_BYTE){
    VBYTE(cur_menu->var->val)=max((VBYTE(cur_menu->var->val)-VBYTE(cur_menu->var->incr)),VBYTE(cur_menu->var->lower));}
  else if(cur_menu->var->type==MW_AUTO_INT){
    VINT(cur_menu->var->val)=max((VINT(cur_menu->var->val)-VINT(cur_menu->var->incr)),VINT(cur_menu->var->lower));}    
  else if(cur_menu->var->type==MW_AUTO_FLOAT){
    VFLOAT(cur_menu->var->val)=max((VFLOAT(cur_menu->var->val)-VFLOAT(cur_menu->var->incr)),VFLOAT(cur_menu->var->lower));}    
  else if(cur_menu->var->type==MW_BOOLEAN){
    VBOOL(cur_menu->var->val)=!VBOOL(cur_menu->var->val);}    
  else if((cur_menu->var->type==MW_LIST)&& MW_invar)    
    cur_menu->cur_item=(cur_menu->cur_item+1)%(cur_menu->idx_o);
  }

void menwiz::actBTR(){ 
  if(cur_menu->var->type==MW_AUTO_INT){
    VINT(cur_menu->var->val)=min((VINT(cur_menu->var->val)+VINT(cur_menu->var->incr)),VINT(cur_menu->var->upper));}    
  else if(cur_menu->var->type==MW_AUTO_BYTE){
    VBYTE(cur_menu->var->val)=min((VBYTE(cur_menu->var->val)+VBYTE(cur_menu->var->incr)),VBYTE(cur_menu->var->upper));}    
  else if(cur_menu->var->type==MW_AUTO_FLOAT){
    VFLOAT(cur_menu->var->val)=min((VFLOAT(cur_menu->var->val)+VFLOAT(cur_menu->var->incr)),VFLOAT(cur_menu->var->upper));}    
  else if(cur_menu->var->type==MW_BOOLEAN){
    VBOOL(cur_menu->var->val)=!VBOOL(cur_menu->var->val);}    
  else if((cur_menu->var->type==MW_LIST)&&MW_invar)    
    cur_menu->cur_item=(cur_menu->cur_item-1)<0?(cur_menu->idx_o-1):cur_menu->cur_item-1;
  }

void menwiz::actBTE(){ 
  if(cur_menu->type==MW_VAR){
    if(cur_menu->var->type==MW_AUTO_INT){        
      VINT(cur_menu->var->val)=VINT(cur_menu->var->old);}
    else if(cur_menu->var->type==MW_AUTO_FLOAT){        
      VFLOAT(cur_menu->var->val)=VFLOAT(cur_menu->var->old);}
    else if(cur_menu->var->type==MW_AUTO_BYTE){        
      VBYTE(cur_menu->var->val)=VBYTE(cur_menu->var->old);}
    else if(cur_menu->var->type==MW_BOOLEAN){        
      VBOOL(cur_menu->var->val)=VBOOL(cur_menu->var->old);}
    }
  cur_menu=&m[cur_menu->parent];
  MW_invar=false;
  }

void menwiz::actBTC(){ 
  _option* oc;
  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 

  if((cur_menu->type==MW_SUBMENU)||(cur_menu->type==MW_ROOT)){
    VINT(cur_menu->var->val)=cur_menu->cur_item;
    cur_menu=&m[oc->sbm];
    if((cur_menu->type==MW_VAR)){
      MW_invar=true;
      }
    }
  else if(cur_menu->type==MW_VAR){
    cur_menu=&m[cur_menu->parent];
    MW_invar=false;
    }
  }

int menwiz::getErrorMessage(boolean fl){
  if (fl){
    switch(MW_error)
      {
      case 0:   break; 
      case 200: Serial.println(F("E200-Root undefined"));break; 
      case 100: Serial.println(F("E100-Too many items. Increment MAX_MENU"));break; 
      case 105: Serial.println(F("E105-Too many items. IncremenT MAX_OPTXMENU"));break; 
      case 110: Serial.println(F("E110-MW_VAR menu type required"));break; 
      case 120: Serial.println(F("E120-Bad 1st arg"));break; 
      case 300: Serial.println(F("E300-undefined variable type"));break; 
      case 900: Serial.println(F("E900-Out of memory"));break; 
      default:  Serial.println(F("E000-Unknown err"));break; 
      }
    }
  return MW_error;
  }

int menwiz::freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }

