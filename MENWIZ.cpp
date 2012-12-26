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

#define SCREATE(p,s)     p=(char *)malloc(strlen((char *)s)+1); strcpy((char *)p,(char *)s)
#define SFORM(b,s,l)     memset(b,32,l); memcpy(b,s,strlen(s)); b[l]=NULL; lcd->print(b)
#define TSFORM(b,s,l)    memset(b,32,l);strcpy_P(b,(const prog_char*)s); b[strlen(b)]=' ';itoa(cur_menu->cur_item+1,tmp,10);strcat(tmp,"/");itoa(cur_menu->idx_o,tmp+strlen(tmp),10);b[col-strlen(tmp)-1]=126;memcpy(b+(col-strlen(tmp)),tmp,strlen(tmp));b[l]=NULL;lcd->print(b)
#define FSFORM(b,s,l)    memset(b,32,l);strcpy_P(b,(const prog_char*)s);b[strlen(b)]=' ';b[l]=NULL;lcd->print(b)
#define ERROR(a)         MW_error=a
#define BLANKLINE(b,r,c) memset(b,32,c);b[c]=NULL; lcd->setCursor(0,r);lcd->print(b)

// GLOBAL VARIABLES
// ---------------------------------------------------------------------------
int MW_FLOAT_DEC=1;  //decimal digits in float screen representation
static char *buf;
const char MW_ver[]={"1.2.0"};
char tmp[6];
//const char MW_FMT_VARINT[]={"%d [%d] %d"}; //@
const char MW_STR_CONFIRM[]={"[Confirm] to run."};
const uint8_t c0[8]={B00000, B00000, B00001, B00010, B10100, B01000, B00000, B00000}; 
const uint8_t c1[8]={B00000, B11100, B00100, B00100, B10101, B01110, B00100, B00000}; 
//const uint8_t c2[8]={B00000, B11111, B00000, B00000, B00000, B11111, B01110, B00100}; 
//const uint8_t c3[8]={B00100, B01110, B11111, B00000, B00000, B00000, B11111, B00000}; 
byte MW_error;
byte MW_navbtn=0;
boolean MW_invar=false;

menwiz::menwiz(){

  ERROR(0);
  bitWrite(flags,FL_SPLASH,false);
  bitWrite(flags,FL_SPLASH_DRAW,false);
  bitWrite(flags,FL_USRSCREEN_DRAW,false);
  bitWrite(flags,MW_MENU_INDEX,true);
  usrScreen.fl=false;
  usrNav.fl=false;
  last_button=MW_BTU;
  idx_m=0;
  row=0;
  col=0;
  tm_start=0;  
  root=NULL;
}

_menu::_menu(){

  ERROR(0);
  idx_o=0;
  cur_item=0;
  label=NULL;
  parent=NULL;  
  bitWrite(flags,FL_USRSCREEN_DRAW,false);
  }

_option::_option(){

  ERROR(0);
  label=NULL;
  }

_menu * menwiz::addMenu(int t,_menu * p, MW_LABEL lab){
static _option *op;

  ERROR(0);   
  if ((idx_m==0)&&(t!=MW_ROOT)){
    ERROR(200);
    return NULL;
    }
  //INSTANTIATE NEW MENU VARIABLES
  if (idx_m<MAX_MENU){   
    m[idx_m].type=(MW_TYPE)t;         // ROOT| SUBMENU| VAR
//    SCREATE(m[idx_m].label,lab);      // OPT:STORE STRING BY REFERENCE, NOT BY VALUE?
    m[idx_m].label=lab;
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
      if (m[p->cod].idx_o<MAX_OPTXMENU){
        op=(_option *) malloc(sizeof(_option));
        if(op==NULL){
          ERROR(900);
          }
        else {
	  op->sbm=idx_m;
          op->type=(MW_TYPE)t;
          m[p->cod].o[m[p->cod].idx_o]=(_option*)op;
          m[p->cod].idx_o++;
          }
        }
      else{ERROR(105);}
      }
    idx_m++;
    return &m[idx_m-1];
    }
  else{
// ERROR    
    ERROR(100);
    return NULL;}
   }

_option *_menu::addItem(int t,MW_LABEL lab){
static _option *op=NULL;

  ERROR(0);
  if (idx_o<MAX_OPTXMENU){
    op=(_option *) malloc(sizeof(_option));
    if(op==NULL){
      ERROR(900);
      }
    else {
      op->type=(MW_TYPE)t;
      op->label=lab;
      o[idx_o]=(_option*)op;
      idx_o++;
      }
    }
  else{
// ERROR
   ERROR(105);
   return NULL;
   }
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
    flags=0;
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    bitWrite(flags,MW_SCROLL_HORIZONTAL,false);   
    bitWrite(flags,MW_LIST_3COLUMNS,false);   
    bitWrite(flags,MW_LIST_2COLUMNS,false);   
    var->type=MW_LIST;
    var->val=v;
    cur_item=VBYTE(v);
    }
// ERROR    
  else{ERROR(110);}
  }
  
void _menu::addVar(MW_TYPE t, int* v, int low, int up, int incr){

  ERROR(0);
  if (t!=MW_AUTO_INT)
    ERROR(120);
  else if(type==MW_VAR){
    flags=0;
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_AUTO_INT;
    var->val=v;
    var->lower=malloc(sizeof(int)); if(var->lower!=NULL) VINT(var->lower)=low; else {ERROR(900); return;}
    var->upper=malloc(sizeof(int)); if(var->upper!=NULL) VINT(var->upper)=up; else {ERROR(900); return;}
    var->incr=malloc(sizeof(int));  if(var->incr!=NULL) VINT(var->incr)=incr; else {ERROR(900); return;} 
    var->old=malloc(sizeof(int));   if(var->old!=NULL) VINT(var->old)=VINT(var->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t, float* v, float low, float up, float incr){

  ERROR(0);
  if (t!=MW_AUTO_FLOAT)
    ERROR(120);
  else if(type==MW_VAR){
    flags=0;
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_AUTO_FLOAT;
    var->val=v;
    var->lower=malloc(sizeof(float)); if(var->lower!=NULL) VFLOAT(var->lower)=low; else {ERROR(900); return;}
    var->upper=malloc(sizeof(float)); if(var->upper!=NULL) VFLOAT(var->upper)=up; else {ERROR(900); return;}
    var->incr=malloc(sizeof(float));  if(var->incr!=NULL) VFLOAT(var->incr)=incr; else {ERROR(900); return;} 
    var->old=malloc(sizeof(float));   if(var->old!=NULL) VFLOAT(var->old)=VFLOAT(var->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t, byte* v, byte low, byte up, byte incr){

  ERROR(0);
  if (t!=MW_AUTO_BYTE)
    ERROR(120);
  else if(type==MW_VAR){
    flags=0;
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_AUTO_BYTE;
    var->val=v;
    var->lower=malloc(sizeof(byte)); if(var->lower!=NULL) VBYTE(var->lower)=low; else {ERROR(900); return;}
    var->upper=malloc(sizeof(byte)); if(var->upper!=NULL) VBYTE(var->upper)=up; else {ERROR(900); return;}
    var->incr=malloc(sizeof(byte));  if(var->incr!=NULL) VBYTE(var->incr)=incr; else {ERROR(900); return;} 
    var->old=malloc(sizeof(byte));   if(var->old!=NULL) VBYTE(var->old)=VBYTE(var->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t, boolean* v){

  ERROR(0);
  if (t!=MW_BOOLEAN)
    ERROR(120);
  else if(type==MW_VAR){
    flags=0;
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_BOOLEAN;
    var->val=v;
    var->old=malloc(sizeof(boolean));  if(var->old!=NULL) VBOOL(var->old)=VBOOL(var->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void  _menu::addVar(MW_TYPE t,void (*f)()){

  ERROR(0);
  if (t!=MW_ACTION)
    ERROR(120);
  else if(type==MW_VAR){
    flags=0;
    bitWrite(flags,MW_ACTION_CONFIRM,true);   
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    var->type=MW_ACTION;
    var->action=f;
    }
// ERROR    
  else{ERROR(110);}
  }

void menwiz::addUsrNav(int (*f)(), int nb){

  ERROR(0);
  usrNav.fl=true;

  usrNav.fi=f;

  if ((nb==4) or (nb==6))
     MW_navbtn=nb;
  else     
     ERROR(130);
  }

void menwiz::begin(void *l,int c, int r){

  ERROR(0);
  tm_start=millis();
  row=r;
  col=c;
  lcd=(MW_LCD*)l; 
  lcd->begin(c,r);  //  LCD size
  lcd->setBacklight(HIGH);
  lcd->noCursor();
  lcd->createChar(0,(uint8_t*)c0);
  lcd->createChar(1,(uint8_t*)c1);
  sbuf=(char*)malloc(r*c+r); if(sbuf==NULL) ERROR(900);
  buf =(char*)malloc(2*c);   if(buf==NULL) ERROR(900);  
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
  int long lap1,lap2; 

  ERROR(0);
  // get nav choice
  #ifdef BUTTON_SUPPORT 
    ret=usrNav.fl?usrNav.fi():scanNavButtons();    	//internal method or user defined callback?
  #else
    ret=usrNav.fi();    				//user defined callback
  #endif 

  // if usrscreen is active, skip last button and switch to MENU mode
  if((cur_mode==MW_MODE_USRSCREEN)&&(ret!=MW_BTNULL)){
    cur_mode=MW_MODE_MENU;
    last_button=MW_BTNULL;
    tm_push=millis();
    } 
  // else run the action associated to selected button
  else
    actNavButtons(ret);
    
  lap1=(millis()-tm_start);
  lap2=(millis()-tm_push); 

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
    if((last_button!=MW_BTNULL) || (!bitRead(flags,bitRead(flags,FL_USRSCREEN_DRAW)))){
      bitWrite(flags,FL_USRSCREEN_DRAW,1);
      drawMenu(cur_menu);
      }
    }
  }
  
void menwiz::drawMenu(_menu *mc){
  int rstart,rstop,i,j;
  _option *op;
  _menu   *mn;
  byte l,fl=0;
  
  ERROR(0);  
  lcd->setCursor(0,0);
  
  if(bitRead(flags,MW_MENU_INDEX)&&
  ((mc->type==MW_ROOT)||(mc->type==MW_SUBMENU)||(mc->var->type==MW_LIST))){
    TSFORM(buf,mc->label,(int) col);
    }
  else{
    FSFORM(buf,mc->label,(int) col);
    }
  
  if (mc->type==MW_VAR){
    drawVar(mc);
    }
  else{
    rstart=max(0,mc->cur_item-(row-2));
    rstop=min((mc->idx_o),(rstart+row));

    for (i=1,j=rstart;i<row;i++,j++){// for all remaining lcd rows
      if(j<rstop){
        lcd->setCursor(0,i); // POSITION TO LINE TO PRINT
	if(bitRead(mc->flags,MW_MENU_COLLAPSED)){
	  op=(_option*)mc->o[j];
	  mn=&m[op->sbm];
          if(j==mc->cur_item){
	    buf[0]=126;
	    fl=true;
	    }
	  else{
	    buf[0]=165;
	    fl=false;
	    }
	  strcpy_P(&buf[1],(const prog_char*)mn->label);
	  strcpy(sbuf,buf);//@
	  if(mn->type==MW_VAR){

	    switch(mn->var->type){
	      case MW_AUTO_INT:
                if (fl){strcat(sbuf,":[");strcat(sbuf,itoa(VINT(mn->var->val),buf,10));strcat(sbuf,"]");}
                else {strcat(sbuf,":");strcat(sbuf,itoa(VINT(mn->var->val),buf,10));}
		break;
		
	      case MW_LIST:
                strcat(sbuf,": ");l=strlen(sbuf);sbuf[l]=1;sbuf[l+1]=0;
	        break;
	        
	      case MW_BOOLEAN:
                if (fl) {strcat(sbuf,":[");strcat(sbuf,itoa(VBOOL(mn->var->val),buf,10));strcat(sbuf,"]");}
                else {strcat(sbuf,":");strcat(sbuf,itoa(VBOOL(mn->var->val),buf,10));}
		break;
		
	      case MW_AUTO_FLOAT:
                if (fl) {strcat(sbuf,":[");dtostrf(VFLOAT(mn->var->val),0,MW_FLOAT_DEC,buf);strcat(sbuf,buf);strcat(sbuf,"]");}
                else {strcat(sbuf,":");dtostrf(VFLOAT(mn->var->val),0,MW_FLOAT_DEC,buf);strcat(sbuf,buf);}        
                break;

	      case MW_AUTO_BYTE:
                if (fl) {strcat(sbuf,":[");strcat(sbuf,itoa(VBYTE(mn->var->val),buf,10));strcat(sbuf,"]");}
                else {strcat(sbuf,":");strcat(sbuf,itoa(VBYTE(mn->var->val),buf,10));}
		break;

	      case MW_ACTION:
                strcat(sbuf,": ");l=strlen(sbuf);sbuf[l]=1;sbuf[l+1]=0;
		break;
	      }
            memset(&sbuf[strlen(sbuf)],32,col-strlen(sbuf));
            sbuf[col]=0;
//	    lcd->setCursor(0,i);
	    lcd->print(sbuf);
            }
          else{ // NOT VARS (ROOT OR SUBMENU)
	    op=(_option*)mc->o[j];
//	    lcd->setCursor(0,i);
	    lcd->write((j==mc->cur_item)?126:165);
	    FSFORM(buf,m[op->sbm].label,(int) col-1);
	    }
	  }
	else{// NOT MENU COLLAPSED
	  op=(_option*)mc->o[j];
//	  lcd->setCursor(0,i);
	  lcd->write((j==mc->cur_item)?126:165);
	  FSFORM(buf,m[op->sbm].label,(int) col-1);
          }
        }
      else{// EMPTY LINE
        BLANKLINE(buf,i,col);
        }
      }// NEXT
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
      if(bitRead(mc->flags,MW_SCROLL_HORIZONTAL)){
         lcd->setCursor(0,1);
         op=(_option*)mc->o[mc->cur_item];
         strcpy_P(buf,(char*)pgm_read_word(op->label));
         strcpy(sbuf,"[");strcat(sbuf,buf);strcat(sbuf,"]");//@
         SFORM(buf,sbuf,col);
         for(i=2;i<row;i++){
            BLANKLINE(buf,i,col);
	    }
	 }
      else{
	 if(bitRead(mc->flags,MW_LIST_2COLUMNS)){
            drawList(mc,2);
            }
	 else if(bitRead(mc->flags,MW_LIST_3COLUMNS)){
            drawList(mc,3);
            }
	 else{
            rstart=max(0,mc->cur_item-(row-2));
            rstop=min((mc->idx_o),rstart+(row));
            for (i=1,j=rstart;i<row;i++,j++){
               if(j<rstop){
                  op=(_option*)mc->o[j];
                  lcd->setCursor(0,i);
                  lcd->write((j==mc->cur_item)?0:165);
                  FSFORM(buf,op->label,(int) col-1);
                  }
               else{
                  BLANKLINE(buf,i,col);
                  }
               }
	    }
         }
      break;  
    case MW_AUTO_INT:
    case MW_AUTO_BYTE:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      if(mc->var->type==MW_AUTO_INT){
	strcpy(sbuf,itoa(VINT(mc->var->lower),buf,10));
	strcat(sbuf," [");
	strcat(sbuf,itoa(VINT(mc->var->val),buf,10));
	strcat(sbuf,"] ");
	strcat(sbuf,itoa(VINT(mc->var->upper),buf,10));

      	}
      else{
	strcpy(sbuf,itoa(VBYTE(mc->var->lower),buf,10));
	strcat(sbuf," [");
	strcat(sbuf,itoa(VBYTE(mc->var->val),buf,10));
	strcat(sbuf,"] ");
	strcat(sbuf,itoa(VBYTE(mc->var->upper),buf,10));

      	}
      SFORM(buf,sbuf,col);
      break;      
    case MW_AUTO_FLOAT:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      lcd->print(dtostrf(VFLOAT(mc->var->lower),0,MW_FLOAT_DEC,buf));
      lcd->print(F(" ["));
      lcd->print(dtostrf(VFLOAT(mc->var->val),0,MW_FLOAT_DEC,buf));
      lcd->print(F("] "));
      lcd->print(dtostrf(VFLOAT(mc->var->upper),0,MW_FLOAT_DEC,buf));
      break;
    case MW_BOOLEAN:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,VBOOL(mc->var->val)?"ON":"OFF",(int) col);
      break;      
    case MW_ACTION:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,MW_STR_CONFIRM,(int) col);
      break;      
    default:
      ERROR(300);
    }
  }

void menwiz::drawList(_menu *mc, int nc){
int nr=row-1; int cw=col/nc;

   for(int i=0;i<(row*nc-nc);i++){
      int pc=i/nr; int pr=i%nr+1;
      lcd->setCursor(pc*cw,pr);
      if(i<mc->idx_o){
	 lcd->write((i==mc->cur_item)?0:165);
	 FSFORM(buf,((_option*)mc->o[i])->label,(int)cw-1);
	 }
      else{
	 memset(buf,32,cw);
	 buf[cw]=NULL;
	 lcd->print(buf);
	 }
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

#ifdef BUTTON_SUPPORT
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
  // check 6-buttonsmode uncompatibility
  for(int i=0;i<idx_m;i++){
	if (bitRead(m[i].flags,MW_MENU_COLLAPSED)){
		bitWrite(m[i].flags,MW_MENU_COLLAPSED,false);
		ERROR(410);
		}
	}
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
  // check 6-buttonsmode uncompatibility
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
#endif

int menwiz::actNavButtons(int button){  
int b;

  ERROR(0);
  if (button==MW_BTNULL){ 
    last_button=button;
    return(button);
    }
  else{
    switch(button){
      case MW_BTU: 
        if (MW_navbtn==6){ 
          actBTU();
          }
        else{	  
          if(MW_invar)
            actBTR();
          else
            actBTU();
          }
        break;   
      case MW_BTD:
        if (MW_navbtn==6){ 
	  actBTD();
          }
        else{
	  if(MW_invar)
            actBTL();
          else
            actBTD();
          }  
        break; 
      case MW_BTL: 
        actBTL();  
        break; 
      case MW_BTR: 
        actBTR();  
        break; 
      case MW_BTE: 
        actBTE();  
        break; 
      case MW_BTC: 
        actBTC();  
        break; 
      }
    }
  tm_push=millis();
  }

void menwiz::actBTU(){ 
  last_button=MW_BTU;

  if((cur_menu->type!=MW_VAR)||(cur_menu->var->type==MW_LIST)){    
    cur_menu->cur_item=(cur_menu->cur_item-1)<0?(cur_menu->idx_o-1):cur_menu->cur_item-1;
    }
  }

void menwiz::actBTD(){ 
  last_button=MW_BTD;

  if((cur_menu->type!=MW_VAR)||(cur_menu->var->type==MW_LIST)){    
    cur_menu->cur_item=(cur_menu->cur_item+1)%(cur_menu->idx_o);
    }
  }

void menwiz::actBTL(){ 
  last_button=MW_BTL;
  _menu* cm;
  _option* oc;

  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 
  cm=bitRead(cur_menu->flags,MW_MENU_COLLAPSED)? &m[oc->sbm]:cur_menu;
  if(cm->var->type==MW_AUTO_BYTE){
    VBYTE(cm->var->val)=max((VBYTE(cm->var->val)-VBYTE(cm->var->incr)),VBYTE(cm->var->lower));}
  else if(cm->var->type==MW_AUTO_INT){
    VINT(cm->var->val)=max((VINT(cm->var->val)-VINT(cm->var->incr)),VINT(cm->var->lower));}    
  else if(cm->var->type==MW_AUTO_FLOAT){
    VFLOAT(cm->var->val)=max((VFLOAT(cm->var->val)-VFLOAT(cm->var->incr)),VFLOAT(cm->var->lower));}    
  else if(cm->var->type==MW_BOOLEAN){
    VBOOL(cm->var->val)=!VBOOL(cm->var->val);}    
  else if((cm->var->type==MW_LIST)&& MW_invar)    
    cm->cur_item=(cm->cur_item+1)%(cm->idx_o);
  
  }

void menwiz::actBTR(){ 
  _menu *cm;
  _option* oc;

  last_button=MW_BTR;

  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 
  cm=bitRead(cur_menu->flags,MW_MENU_COLLAPSED)? &m[oc->sbm]:cur_menu;
  if(cm->var->type==MW_AUTO_INT){
    VINT(cm->var->val)=min((VINT(cm->var->val)+VINT(cm->var->incr)),VINT(cm->var->upper));}    
  else if(cm->var->type==MW_AUTO_BYTE){
    VBYTE(cm->var->val)=min((VBYTE(cm->var->val)+VBYTE(cm->var->incr)),VBYTE(cm->var->upper));}    
  else if(cm->var->type==MW_AUTO_FLOAT){
    VFLOAT(cm->var->val)=min((VFLOAT(cm->var->val)+VFLOAT(cm->var->incr)),VFLOAT(cm->var->upper));}    
  else if(cm->var->type==MW_BOOLEAN){
    VBOOL(cm->var->val)=!VBOOL(cm->var->val);}    
  else if((cm->var->type==MW_LIST)&&MW_invar)    
    cm->cur_item=(cm->cur_item-1)<0?(cm->idx_o-1):cm->cur_item-1;
  }

void menwiz::actBTE(){ 
  _menu *cm;
  _option* oc;

  last_button=MW_BTE;

  cm=cur_menu;
  if(cm->type==MW_VAR){
    if(cm->var->type==MW_AUTO_INT){        
      VINT(cm->var->val)=VINT(cm->var->old);}
    else if(cm->var->type==MW_AUTO_FLOAT){        
      VFLOAT(cm->var->val)=VFLOAT(cm->var->old);}
    else if(cm->var->type==MW_AUTO_BYTE){        
      VBYTE(cm->var->val)=VBYTE(cm->var->old);}
    else if(cm->var->type==MW_BOOLEAN){        
      VBOOL(cm->var->val)=VBOOL(cm->var->old);}
    }
  cur_menu=&m[cur_menu->parent];
  MW_invar=false;
  }

void menwiz::actBTC(){ 
  _option* oc;

  last_button=MW_BTC;

  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 
  if((cur_menu->type==MW_SUBMENU)||(cur_menu->type==MW_ROOT)){
//    VINT(cur_menu->var->val)=cur_menu->cur_item;  //? commented senza sapere perchè ...
    Serial.println("PASS1");
    cur_menu=&m[oc->sbm];
    if(cur_menu->type==MW_VAR){
    Serial.println("PASS2");
      if(bitRead(m[cur_menu->parent].flags,MW_MENU_COLLAPSED)){
    Serial.println("PASS3");
	if ((cur_menu->var->type!=MW_LIST)&&((cur_menu->var->type!=MW_ACTION))) {
    Serial.println("PASS4");
	  cur_menu=&m[cur_menu->parent];
	  MW_invar=false;
	  }
	else{
    Serial.println("PASS5");
	  MW_invar=true;	  
	  }
    Serial.println("PASS6");
	return;
        }
      else if((cur_menu->var->type==MW_ACTION) && (!bitRead(cur_menu->flags,MW_ACTION_CONFIRM))){
      Serial.println("NOT CONFIRMED");
        cur_menu->var->action();
	cur_menu=&m[cur_menu->parent];
	MW_invar=false;  
	return;
	}
      else{
    Serial.println("PASS7");
	MW_invar=true;
  	return;
        }
      }
    } 
  else if(cur_menu->type==MW_VAR){
    Serial.println("PASS8");
    if(cur_menu->var->type==MW_LIST){        
      VINT(cur_menu->var->val)=cur_menu->cur_item;}
    else if(cur_menu->var->type==MW_AUTO_INT){        
      VINT(cur_menu->var->old)=VINT(cur_menu->var->val);}
    else if(cur_menu->var->type==MW_AUTO_FLOAT){        
      VFLOAT(cur_menu->var->old)=VFLOAT(cur_menu->var->val);}
    else if(cur_menu->var->type==MW_AUTO_BYTE){        
      VBYTE(cur_menu->var->old)=VBYTE(cur_menu->var->val);}
    else if(cur_menu->var->type==MW_BOOLEAN){        
      VBOOL(cur_menu->var->old)=VBOOL(cur_menu->var->val);}
    else if((cur_menu->var->type==MW_ACTION)&&(bitRead(cur_menu->flags,MW_ACTION_CONFIRM))){
      Serial.println("CONFIRMED");
      cur_menu->var->action();}
    cur_menu=&m[cur_menu->parent];
    MW_invar=false;
    }
  }

int menwiz::getErrorMessage(boolean fl){
/*
  if (fl){
    switch(MW_error){
      case 0:   break; 
      case 100: Serial.println(F("E100-Too many items. Increment MAX_MENU"));break; 
      case 130: Serial.println(F("E130-Invalid buttons number: allowed 4 or 6"));break; 
      case 200: Serial.println(F("E200-Root undefined"));break; 
      case 105: Serial.println(F("E105-Too many items. Increment MAX_OPTXMENU"));break; 
      case 110: Serial.println(F("E110-MW_VAR menu type required"));break; 
      case 120: Serial.println(F("E120-Bad 1st arg"));break; 
      case 300: Serial.println(F("E300-Undefined variable type"));break; 
      case 310: Serial.println(F("E310-Unknown behaviour"));break; 
      case 410: Serial.println(F("E410-Behavihour available only with 6 buttons. ignored"));break; 
      case 900: Serial.println(F("E900-Out of memory"));break; 
      default:  Serial.println(F("E000-Unknown err"));break; 
      }
    }
*/
  if (fl){Serial.print(F("ERROR "));Serial.println(MW_error);}
  return MW_error;
  }

void menwiz::setBehaviour(MW_FLAGS b, boolean v){

  ERROR(0);  
  bitWrite(flags,b,v);
  }
  
void _menu::setBehaviour(MW_FLAGS b, boolean v){

  ERROR(0);
  bitWrite(flags,b,v);
  if(v&&(b==MW_SCROLL_HORIZONTAL)){
     bitWrite(flags,MW_LIST_2COLUMNS,false);
     bitWrite(flags,MW_LIST_3COLUMNS,false);
     }
  else if(v&&(b==MW_MENU_COLLAPSED)){
     bitWrite(flags,MW_MENU_COLLAPSED,v);
     }
  else if(v&&(b==MW_LIST_2COLUMNS)){
     bitWrite(flags,MW_SCROLL_HORIZONTAL,false);
     bitWrite(flags,MW_LIST_3COLUMNS,false);
     }
  else if(v&&(b==MW_LIST_3COLUMNS)){
     bitWrite(flags,MW_SCROLL_HORIZONTAL,false);
     bitWrite(flags,MW_LIST_2COLUMNS,false);
     }
  }

int menwiz::freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }

#ifdef EEPROM_SUPPORT
void  menwiz::writeEeprom(){
  int addr=0;
  Etype temp;

  ERROR(0);
  for (int i=0;i<idx_m;i++){
    if(m[i].type==MW_VAR){
	switch(m[i].var->type){
	    case MW_AUTO_BYTE:{
		    temp.b = VBYTE(m[i].var->val);
		    EEPROM.write(addr, temp.bytes[0]);
		    addr++;
                    }
		    break;
	    case MW_BOOLEAN:{
		    temp.bl = VBOOL(m[i].var->val);
		    EEPROM.write(addr, temp.bytes[0]);
		    addr++;
                    }
		    break;
	    case MW_AUTO_FLOAT:{
		    temp.f = VFLOAT(m[i].var->val);
		    for (int i=0; i<4; i++) {
		      EEPROM.write(addr +i, temp.bytes[i]);
		      }
		    addr=addr+4;
                    }
		    break;
	    case MW_LIST:{
		    temp.i = (int)m[i].cur_item;
		    for (int i=0; i<2; i++) {
		      EEPROM.write(addr +i, temp.bytes[i]);
		      }    
		    addr=addr+2;
                    }
	   	    break;
	    case MW_AUTO_INT:{
		    temp.i = VINT(m[i].var->val);
		    for (int i=0; i<2; i++) {
		      EEPROM.write(addr +i, temp.bytes[i]);
		      }    
		    addr=addr+2;
                    }
	   	    break;
	 }
       }
     }
   }

void  menwiz::readEeprom(){
  int addr=0;
  Etype temp;

  ERROR(0);
  for (int i=0;i<idx_m;i++){
    if(m[i].type==MW_VAR){
	switch(m[i].var->type){
	    case MW_AUTO_BYTE:{
		    temp.bytes[0] = EEPROM.read(addr);
		    VBYTE(m[i].var->val)=temp.b;
		    VBYTE(m[i].var->old)=temp.b;
		    addr++;
                    }
		    break;
	    case MW_BOOLEAN:{
		    temp.bytes[0] = EEPROM.read(addr);
		    VBOOL(m[i].var->val)=temp.bl;
		    VBOOL(m[i].var->old)=temp.bl;
		    addr++;
                    }
		    break;
	    case MW_AUTO_FLOAT:{
		    for (int n=0; n<4; n++) {
		     temp.bytes[n] = EEPROM.read(addr+n);
		     }
		    VFLOAT(m[i].var->val)=temp.f;
		    VFLOAT(m[i].var->old)=temp.f;
		    addr=addr+4;
                    }
		    break;
	    case MW_AUTO_INT:{
		    for (int n=0; n<2; n++) {
		     temp.bytes[n] = EEPROM.read(addr+n);
		     }
		    VINT(m[i].var->val)=temp.i;
		    VINT(m[i].var->old)=temp.i;
		    addr=addr+2;
                    }
		    break;
	    case MW_LIST:{
		    for (int n=0; n<2; n++) {
		     temp.bytes[n] = EEPROM.read(addr+n);
		     }
		    VINT(m[i].var->val)=temp.i;
		    VINT(m[i].var->old)=temp.i;
		    m[i].cur_item=(byte) temp.i;
		    addr=addr+2;
                    }
		    break;
	 }
       }
     }
   }

#endif

