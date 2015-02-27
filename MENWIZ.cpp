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
#define TSFORM(b,s,l,c)  memset(b,32,l);strncpy_P(b,(const char PROGMEM*)s, min(col,strlen_P((char PROGMEM*) s))); b[strlen(b)]=' ';itoa(cur_menu->cur_item+1,tmp,10);strcat(tmp,"/");itoa(cur_menu->idx_o,tmp+strlen(tmp),10);b[col-strlen(tmp)-1]=c;memcpy(b+(col-strlen(tmp)),tmp,strlen(tmp));b[l]=NULL;lcd->print(b)
#define FSFORM(b,s,l)    memset(b,32,l);memcpy_P(b,(const char PROGMEM*)s,min(l,strlen_P((const char PROGMEM*)s)));buf[l]=NULL;lcd->print(b);
#define ERROR(a)         MW_error=a
#define BLANKLINE(b,r,c) memset(b,32,c);b[c]=NULL; lcd->setCursor(0,r);lcd->print(b)

// GLOBAL VARIABLES
// ---------------------------------------------------------------------------
const char MW_ver[]={"1.3.2 beta"};
int        MW_error;
byte       MW_navbtn=0;
bool    MW_invar=false;
//_menu    m[MAX_MENU];

static char *buf;
char tmp[6];

const char MW_STR_CONFIRM[]={"[Confirm] to run."};
const uint8_t c0[8]={B00000, B00000, B00001, B00010, B10100, B01000, B00000, B00000}; 
const uint8_t c1[8]={B00000, B11100, B00100, B00100, B10101, B01110, B00100, B00000}; 
const uint8_t c2[8]={B00000, B01010, B11111, B01110, B11111, B01010, B00000, B00000}; 
//const uint8_t c3[8]={B00100, B01110, B11111, B00000, B00000, B00000, B11111, B00000}; 


menwiz::menwiz(){

  ERROR(0);
  bitWrite(flags,FL_SPLASH,false);
  bitWrite(flags,FL_SPLASH_DRAW,false);
  bitWrite(flags,FL_USRSCREEN_DRAW,false);
  bitWrite(flags,MW_MENU_INDEX,true);

  cur_user=MW_GRANT_USER1;
  usrScreen.fl=false;
  usrNav.fl=false;
  last_button=MW_BTU;
  idx_m=0;
  row=0;
  col=0;
  tm_start=0;  
  root=NULL;
  #ifdef EEPROM_SUPPORT
    eeprom_offset=0;
    eeprom_write_on_confirm=false;
  #endif
}

_menu::_menu(){
  ERROR(0);
/*
initialized in addMenu
*/
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
  //INITIALIZE NEW MENU VARIABLES
  if (idx_m<MAX_MENU){   
    m[idx_m].type=(MW_TYPE)t;   // ROOT| SUBMENU| VAR
    m[idx_m].label=lab;
    m[idx_m].cod=idx_m;      	// unique menu id
    m[idx_m].idx_o=0;        	// reset option index
    m[idx_m].cur_item=0;     	// reset cur item 

    bitWrite(m[idx_m].flags,MW_GRANT_USER1,true);
    bitWrite(m[idx_m].flags,MW_GRANT_USER2,true);
    bitWrite(m[idx_m].flags,MW_GRANT_USER3,true);

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
        else{
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
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    bitWrite(flags,MW_SCROLL_HORIZONTAL,false);   
    bitWrite(flags,MW_LIST_3COLUMNS,false);   
    bitWrite(flags,MW_LIST_2COLUMNS,false);   
    ((_var*)var)->type=MW_LIST;
    ((_var*)var)->val=v;
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
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    ((_var*)var)->type=MW_AUTO_INT;
    ((_var*)var)->val=v;
    ((_var*)var)->lower=malloc(sizeof(int)); if(((_var*)var)->lower!=NULL) VINT(((_var*)var)->lower)=low; else {ERROR(900); return;}
    ((_var*)var)->upper=malloc(sizeof(int)); if(((_var*)var)->upper!=NULL) VINT(((_var*)var)->upper)=up; else {ERROR(900); return;}
    ((_var*)var)->incr=malloc(sizeof(int));  if(((_var*)var)->incr!=NULL) VINT(((_var*)var)->incr)=incr; else {ERROR(900); return;} 
    ((_var*)var)->old=malloc(sizeof(int));   if(((_var*)var)->old!=NULL) VINT(((_var*)var)->old)=VINT(((_var*)var)->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t, float* v, float low, float up, float incr){

  ERROR(0);
  if (t!=MW_AUTO_FLOAT)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    ((_var*)var)->type=MW_AUTO_FLOAT;
    ((_var*)var)->val=v;
    ((_var*)var)->lower=malloc(sizeof(float)); if(((_var*)var)->lower!=NULL) VFLOAT(((_var*)var)->lower)=low; else {ERROR(900); return;}
    ((_var*)var)->upper=malloc(sizeof(float)); if(((_var*)var)->upper!=NULL) VFLOAT(((_var*)var)->upper)=up; else {ERROR(900); return;}
    ((_var*)var)->incr=malloc(sizeof(float));  if(((_var*)var)->incr!=NULL) VFLOAT(((_var*)var)->incr)=incr; else {ERROR(900); return;} 
    ((_var*)var)->old=malloc(sizeof(float));   if(((_var*)var)->old!=NULL) VFLOAT(((_var*)var)->old)=VFLOAT(((_var*)var)->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t, byte* v, byte low, byte up, byte incr){

  ERROR(0);
  if (t!=MW_AUTO_BYTE)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    ((_var*)var)->type=MW_AUTO_BYTE;
    ((_var*)var)->val=v;
    ((_var*)var)->lower=malloc(sizeof(byte)); if(((_var*)var)->lower!=NULL) VBYTE(((_var*)var)->lower)=low; else {ERROR(900); return;}
    ((_var*)var)->upper=malloc(sizeof(byte)); if(((_var*)var)->upper!=NULL) VBYTE(((_var*)var)->upper)=up; else {ERROR(900); return;}
    ((_var*)var)->incr=malloc(sizeof(byte));  if(((_var*)var)->incr!=NULL) VBYTE(((_var*)var)->incr)=incr; else {ERROR(900); return;} 
    ((_var*)var)->old=malloc(sizeof(byte));   if(((_var*)var)->old!=NULL) VBYTE(((_var*)var)->old)=VBYTE(((_var*)var)->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t, bool* v){

  ERROR(0);
  if (t!=MW_BOOL)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var));if(var==NULL){ERROR(900); return;}
    ((_var*)var)->type=MW_BOOL;
    ((_var*)var)->val=v;
    ((_var*)var)->old=malloc(sizeof(bool));  if(((_var*)var)->old!=NULL) VBOOL(((_var*)var)->old)=VBOOL(((_var*)var)->val); else {ERROR(900); return;} 
    }
// ERROR    
  else{ERROR(110);}
  }

void _menu::addVar(MW_TYPE t,char *s){

  ERROR(0);
  if (t!=MW_EDIT_TEXT)
    ERROR(120);
  else if(type==MW_VAR){
    var=(_var*)malloc(sizeof(_var)); if(var==NULL){ERROR(320); return;}
    ((_var*)var)->type=MW_EDIT_TEXT;
    ((_var*)var)->val=s;
    }
// ERROR    
  else{ERROR(110);}
  }

void  _menu::addVar(MW_TYPE t,void (*f)()){

  ERROR(0);
  if (t!=MW_ACTION)
    ERROR(120);
  else if(type==MW_VAR){
    bitWrite(flags,MW_ACTION_CONFIRM,true);   
    var=(_act*)malloc(sizeof(_act));if(var==NULL){ERROR(900); return;}
    ((_act*)var)->type=MW_ACTION;
    ((_act*)var)->action=f;
    }
// ERROR    
  else{ERROR(110);}
  }

void menwiz::setCurrentUser(int u){

  ERROR(0);
  if((u>=MW_GRANT_USER1)&&(u<=MW_GRANT_USER3))
     cur_user=u;
  else
     ERROR(500);
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
//  lcd->noCursor();
  lcd->noBlink();
  lcd->createChar(0,(uint8_t*)c0);
  lcd->createChar(1,(uint8_t*)c1);
  lcd->createChar(2,(uint8_t*)c2);
  sbuf=(char*)malloc(r*c+r+1); if(sbuf==NULL) ERROR(900);
  memset(sbuf,' ',r*c+r);sbuf[r*c+r]=0;
  buf =(char*)malloc(c+1);   if(buf==NULL) ERROR(900);  
  memset(buf,' ',c);buf[c]=0;
  }

void menwiz::drawUsrScreen(char *scr){

  ERROR(0);
  int start=0, cur=0;
  lcd->noBlink();
  for(int i=0;i<row;i++){
    while((scr[cur]!=MW_EOL_CHAR)&&(scr[cur]!=0)){
	  cur++;
	  }
	memset(buf,32,col); 
	memcpy(buf,&scr[start],cur-start); 
	buf[col]=NULL; 
    lcd->setCursor(0,i);
	lcd->print(buf);
	if (scr[cur]==0){
	  return;
	  }
	cur++;
	start=cur;
	}  
  }

void menwiz::draw(){
  int ret;
  int long lap1,lap2; 

//  ERROR(0);
  // get nav choice
  #ifdef BUTTON_SUPPORT 
    ret=usrNav.fl?usrNav.fi():scanNavButtons();    	//internal method or user defined callback?
  #else
    ret=usrNav.fi();    				            //user defined callback
  #endif 
  
    if (ret) {
     Serial.print(ret);
     Serial.println(" Button pressed");
    }
  if (ret) {
     Serial.print(cur_mode);
     Serial.println(" cur_mode");
     Serial.print(cur_menu->type);
     Serial.println(" cur_menu->type");
    }
  // ------ SPECIAL CASES START --------
  //ENTER MENU SCREEN IF ANY BUTTON IS PUSHED
  // if a button was pushed while running usrscreen or splash screen , skip last button, switch to MENU mode, draw menu and exit
  if(  (ret!=MW_BTNULL) && ((cur_mode==MW_MODE_USRSCREEN)||(cur_mode==MW_MODE_SPLASH))){
    bitWrite(flags,FL_SPLASH_DRAW,1);      //mark as drawn once
    bitWrite(flags,FL_USRSCREEN_DRAW,1);   //disable userscreen draw
    cur_mode=MW_MODE_MENU;
    tm_push=millis();
    drawMenu(cur_menu);
    return;
  } 
  //EXIT MENU SCREEN IF CANCEL BUTTON IS PUSHED
  // if the cancel button is pushed while in menu root, let the userscreen come up
  if ( (ret==MW_BTE) && ((cur_mode==MW_MODE_MENU)) && (cur_menu->type==MW_ROOT)){
    tm_push=millis()-tm_usrScreen-1;   //we just set tm_push, so that the userscreen comes up
    ret = MW_BTNULL;                   //skip cancel button
  } 
  // ------ SPECIAL CASES END --------
  
  actNavButtons(ret);
    
  lap1=(millis()-tm_start);
  lap2=(millis()-tm_push); 

  //SPLASHSCREEN
  // if defined splashscreen & not yet drawn & time window is ok, draw it  
  if(bitRead(flags,FL_SPLASH)&& (!bitRead(flags,FL_SPLASH_DRAW)) && (lap1<tm_splash)){
    cur_mode=MW_MODE_SPLASH;
    drawUsrScreen(sbuf);
    if(lap1>=tm_splash) bitWrite(flags,FL_SPLASH_DRAW,1);//mark as drawn once    }
  }
  //USERSCREEN
  // if defined usrscreen & time since last button push > user defined time, draw it  
  else if((usrScreen.fl) && (lap2>tm_usrScreen)){
    cur_mode=MW_MODE_USRSCREEN;
    bitWrite(flags,FL_USRSCREEN_DRAW,0);
    usrScreen.fv();
  }
  //MENU
  else{
    // if a button was pushed since last call, draw menu  
    cur_mode=MW_MODE_MENU;
    if((last_button!=MW_BTNULL) || (!bitRead(flags,FL_USRSCREEN_DRAW)) ){
      drawMenu(cur_menu);
      }
    }
  }
  
void menwiz::drawMenu(_menu *mc){
  int rstart,rstop,i,j;
  _option *op;
  _menu   *mn;
  byte top,fl=0;
  
//  ERROR(0);  
  lcd->setCursor(0,0);
  // DRAW THE FIRST LINE 
  
  if((mc->type==MW_ROOT)||(mc->type==MW_SUBMENU)||(((_act*)mc->var)->type==MW_LIST)){
    if(bitRead(flags,MW_MENU_INDEX)){
      if(((_act*)mc->var)->type==MW_LIST){
      	top=126;}
      else if(bitRead( m[((_option*)mc->o[mc->cur_item])->sbm].flags,cur_user)==false){
        top=2;}
      else
        top=126;
      TSFORM(buf,mc->label,(int) col,top);
      }
    else{
      FSFORM(buf,mc->label,(int) col);
      }
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
    //DRAW ALL REMAINNING LINES
    for (i=1,j=rstart;i<row;i++,j++){
      if(j<rstop){
        lcd->setCursor(0,i); 
	if(bitRead(mc->flags,MW_MENU_COLLAPSED)){
	  op=(_option*)mc->o[j];
	  mn=&m[op->sbm];
          if(j==mc->cur_item){
            if(bitRead( m[((_option*)mc->o[mc->cur_item])->sbm].flags,cur_user)==false)
            	buf[0]=2;
            else 
	    	buf[0]=126;
	    fl=true;
	    }
	  else{
            if(bitRead( m[((_option*)mc->o[mc->cur_item])->sbm].flags,cur_user)==false)
            	buf[0]=2;
            else 
	    	buf[0]=165;
	    fl=false;
	    }
	  strncpy_P(&buf[1],(const char PROGMEM*)mn->label,min(col,strlen_P((const char PROGMEM*)mn->label)));
	  strcpy(sbuf,buf);
	  if(mn->type==MW_VAR){

	    switch( ((_act*)mn->var)->type ){
	      case MW_AUTO_INT:       
                if (fl){strcat(sbuf,":[");strcat(sbuf,itoa( VINT(((_var*)mn->var)->val),buf,10));strcat(sbuf,"]");}
                else {strcat(sbuf,":");strcat(sbuf,itoa(VINT(((_var*)mn->var)->val),buf,10));}
		break;
		
	      case MW_LIST:
                strcat(sbuf,": ");sbuf[strlen(sbuf)]=1;sbuf[strlen(sbuf)+1]=0;
	        break;
	        
	      case MW_BOOL:
                if (fl) {strcat(sbuf,":[");strcat(sbuf,itoa(VBOOL(((_var*)mn->var)->val),buf,10));strcat(sbuf,"]");}
                else {strcat(sbuf,":");strcat(sbuf,itoa(VBOOL(((_var*)mn->var)->val),buf,10));}
		break;
		
	      case MW_AUTO_FLOAT:
                if (fl) {strcat(sbuf,":[");dtostrf(VFLOAT( ((_var*)mn->var)->val),0,MW_FLOAT_DEC,buf);strcat(sbuf,buf);strcat(sbuf,"]");}
                else {strcat(sbuf,":");dtostrf(VFLOAT( ((_var*)mn->var)->val),0,MW_FLOAT_DEC,buf);strcat(sbuf,buf);}        
                break;

	      case MW_AUTO_BYTE:
                if (fl) {strcat(sbuf,":[");strcat(sbuf,itoa(VBYTE( ((_var*)mn->var)->val),buf,10));strcat(sbuf,"]");}
                else {strcat(sbuf,":");strcat(sbuf,itoa(VBYTE( ((_var*)mn->var)->val),buf,10));}
		break;

	      case MW_ACTION:
                strcat(sbuf,": ");sbuf[strlen(sbuf)]=1;sbuf[strlen(sbuf)+1]=0;
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

	    lcd->write((j==mc->cur_item)?126:(bitRead(m[op->sbm].flags,cur_user)?165:2));
	    FSFORM(buf,m[op->sbm].label,(int) col-1);
	    }
	  }
	else{// NOT MENU COLLAPSED
	  op=(_option*)mc->o[j];
//	  lcd->setCursor(0,i);
          if(bitRead( m[((_option*)mc->o[mc->cur_item])->sbm].flags,cur_user)==false)
	    lcd->write((j==mc->cur_item)?2:165);
          else 
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

//  ERROR(0);
  t=(MW_TYPE)((_var*)mc->var)->type;
  switch (((_var*)mc->var)->type){
    case MW_LIST:
      if(bitRead(mc->flags,MW_SCROLL_HORIZONTAL)){
         lcd->setCursor(0,1);
         op=(_option*)mc->o[mc->cur_item];
         strncpy_P(buf,(const char PROGMEM*)pgm_read_word(op->label),min(col,strlen_P((const char PROGMEM*)op->label)));
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
          rstop=min((mc->idx_o),rstart+(row-1));
          for (i=1,j=rstart;i<row;i++,j++){
            if(j<rstop){
              op=(_option*)mc->o[j];
              lcd->setCursor(0,i);
              lcd->write((j==mc->cur_item)?0:165);
              FSFORM(buf,op->label,col-1);
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
      if(((_var*)mc->var)->type==MW_AUTO_INT){
		strcpy(sbuf,itoa(VINT(((_var*)mc->var)->lower),buf,10));
		strcat(sbuf," [");
		strcat(sbuf,itoa(VINT(((_var*)mc->var)->val),buf,10));
		strcat(sbuf,"] ");
		strcat(sbuf,itoa(VINT(((_var*)mc->var)->upper),buf,10));
      	}
      else{
		strcpy(sbuf,itoa(VBYTE(((_var*)mc->var)->lower),buf,10));
		strcat(sbuf," [");
		strcat(sbuf,itoa(VBYTE(((_var*)mc->var)->val),buf,10));
		strcat(sbuf,"] ");
		strcat(sbuf,itoa(VBYTE(((_var*)mc->var)->upper),buf,10));
      	}
      SFORM(buf,sbuf,col);
      break;      
    case MW_AUTO_FLOAT:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      lcd->print(dtostrf(VFLOAT(((_var*)mc->var)->lower),0,MW_FLOAT_DEC,buf));
      lcd->print(F(" ["));
      lcd->print(dtostrf(VFLOAT(((_var*)mc->var)->val),0,MW_FLOAT_DEC,buf));
      lcd->print(F("] "));
      lcd->print(dtostrf(VFLOAT(((_var*)mc->var)->upper),0,MW_FLOAT_DEC,buf));
      break;
    case MW_BOOL:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,VBOOL(((_var*)mc->var)->val)?"ON":"OFF",(int) col);
      break;      
    case MW_ACTION:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,MW_STR_CONFIRM,(int) col);
      break;      
    case MW_EDIT_TEXT:
      for(i=2;i<row;i++){
        BLANKLINE(buf,i,col);
        }
      lcd->setCursor(0,1);
      sbuf[0]='[';
      strcpy(sbuf+1,(char*)((_var*)mc->var)->val);
      strcat(sbuf,"]");
      SFORM(buf,sbuf,col);
	  lcd->blink();
      lcd->setCursor(mc->cur_item+1,1);
      break;
    default:
      ERROR(300);
    }
  }

void menwiz::drawList(_menu *mc, int nc){
  int nr=row-1; int cw=col/nc;

//   ERROR(0);
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
int l;
  
  ERROR(0);
  l=min(row*col+row,strlen(s));
  strncpy(sbuf,s,l);
  sbuf[l]=0; 
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
  char* s;

  s=(char*)((_var*)cur_menu->var)->val;
  if((cur_menu->type!=MW_VAR)||(((_var*)cur_menu->var)->type==MW_LIST)){    
    cur_menu->cur_item=(cur_menu->cur_item-1)<0?(cur_menu->idx_o-1):cur_menu->cur_item-1;
    }
  else if((((_var*)cur_menu->var)->type==MW_EDIT_TEXT)&&(MW_navbtn==6)){
    if(s[cur_menu->cur_item]<MW_MAX_CHARLIST)
      s[cur_menu->cur_item]++;
    else
      s[cur_menu->cur_item]=MW_MIN_CHARLIST;
    }
  }

void menwiz::actBTD(){ 
  last_button=MW_BTD;
  char* s;

  s=(char*)((_var*)cur_menu->var)->val;  
  if((cur_menu->type!=MW_VAR)||(((_var*)cur_menu->var)->type==MW_LIST)){    
    cur_menu->cur_item=(cur_menu->cur_item+1)%(cur_menu->idx_o);
    }
  else if((((_var*)cur_menu->var)->type==MW_EDIT_TEXT)&&(MW_navbtn==6)){
    if(s[cur_menu->cur_item]>MW_MIN_CHARLIST)
      s[cur_menu->cur_item]--;
    else
      s[cur_menu->cur_item]=MW_MAX_CHARLIST;
	}    
  }

void menwiz::actBTL(){ 
  last_button=MW_BTL;
  _menu* cm;
  _option* oc;

  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 
  cm=bitRead(cur_menu->flags,MW_MENU_COLLAPSED)? &m[oc->sbm]:cur_menu;

  if(((_var*)cm->var)->type==MW_AUTO_BYTE){
    VBYTE(((_var*)cm->var)->val)=max((VBYTE(((_var*)cm->var)->val)-VBYTE(((_var*)cm->var)->incr)),VBYTE(((_var*)cm->var)->lower));}
  else if(((_var*)cm->var)->type==MW_AUTO_INT){
    VINT(((_var*)cm->var)->val)=max((VINT(((_var*)cm->var)->val)-VINT(((_var*)cm->var)->incr)),VINT(((_var*)cm->var)->lower));}    
  else if(((_var*)cm->var)->type==MW_AUTO_FLOAT){
    VFLOAT(((_var*)cm->var)->val)=max((VFLOAT(((_var*)cm->var)->val)-VFLOAT(((_var*)cm->var)->incr)),VFLOAT(((_var*)cm->var)->lower));}    
  else if(((_var*)cm->var)->type==MW_BOOL){
    VBOOL(((_var*)cm->var)->val)=!VBOOL(((_var*)cm->var)->val);}    
  else if((((_var*)cm->var)->type==MW_LIST)&& MW_invar)    
    cm->cur_item=(cm->cur_item+1)%(cm->idx_o); 
  else if((((_var*)cur_menu->var)->type==MW_EDIT_TEXT)&&(MW_navbtn==6)){
    cur_menu->cur_item=(cur_menu->cur_item-1<0)?(strlen((char*)((_var*)cur_menu->var)->val)-1):(cur_menu->cur_item-1);
//@
    }    
  }

void menwiz::actBTR(){ 
  _menu *cm;
  _option* oc;

  last_button=MW_BTR;

  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 
  cm=bitRead(cur_menu->flags,MW_MENU_COLLAPSED)? &m[oc->sbm]:cur_menu;

  if(((_var*)cm->var)->type==MW_AUTO_INT){
    VINT(((_var*)cm->var)->val)=min((VINT(((_var*)cm->var)->val)+VINT(((_var*)cm->var)->incr)),VINT(((_var*)cm->var)->upper));}    
  else if(((_var*)cm->var)->type==MW_AUTO_BYTE){
    VBYTE(((_var*)cm->var)->val)=min((VBYTE(((_var*)cm->var)->val)+VBYTE(((_var*)cm->var)->incr)),VBYTE(((_var*)cm->var)->upper));}    
  else if(((_var*)cm->var)->type==MW_AUTO_FLOAT){
    VFLOAT(((_var*)cm->var)->val)=min((VFLOAT(((_var*)cm->var)->val)+VFLOAT(((_var*)cm->var)->incr)),VFLOAT(((_var*)cm->var)->upper));}    
  else if(((_var*)cm->var)->type==MW_BOOL){
    VBOOL(((_var*)cm->var)->val)=!VBOOL(((_var*)cm->var)->val);}    
  else if((((_var*)cm->var)->type==MW_LIST)&&MW_invar)    
    cm->cur_item=(cm->cur_item-1)<0?(cm->idx_o-1):cm->cur_item-1;
  else if((((_var*)cur_menu->var)->type==MW_EDIT_TEXT)&&(MW_navbtn==6)){
    cur_menu->cur_item=(cur_menu->cur_item+1) % (strlen((char*)((_var*)cur_menu->var)->val));
//@
    }    
  }

void menwiz::actBTE(){ 
  _menu *cm;
  _option* oc;

  last_button=MW_BTE;
  cm=cur_menu;
  
  if(cm->type==MW_VAR){
    if(((_var*)cm->var)->type==MW_AUTO_INT){        
      VINT(((_var*)cm->var)->val)=VINT(((_var*)cm->var)->old);}
    else if(((_var*)cm->var)->type==MW_AUTO_FLOAT){        
      VFLOAT(((_var*)cm->var)->val)=VFLOAT(((_var*)cm->var)->old);}
    else if(((_var*)cm->var)->type==MW_AUTO_BYTE){        
      VBYTE(((_var*)cm->var)->val)=VBYTE(((_var*)cm->var)->old);}
    else if(((_var*)cm->var)->type==MW_BOOL){        
      VBOOL(((_var*)cm->var)->val)=VBOOL(((_var*)cm->var)->old);}
    else if((((_var*)cm->var)->type==MW_EDIT_TEXT)){
      lcd->noBlink();
      }    
    }
  cur_menu=&m[cur_menu->parent];
  MW_invar=false;
  }

void menwiz::actBTC(){ 
  _menu *mc;
  _option* oc;

  last_button=MW_BTC;
     
  oc=(_option*)cur_menu->o[cur_menu->cur_item]; 
  mc=&m[oc->sbm];

  if((cur_menu->type==MW_SUBMENU)||(cur_menu->type==MW_ROOT)){

    if(bitRead(mc->flags,cur_user)==false){
       return;
       }

    cur_menu=mc;

    if(cur_menu->type==MW_VAR){
      if(bitRead(m[cur_menu->parent].flags,MW_MENU_COLLAPSED)){
		if ((((_var*)cur_menu->var)->type!=MW_LIST)&&((((_var*)cur_menu->var)->type!=MW_ACTION))) {
			cur_menu=&m[cur_menu->parent];
			MW_invar=false;
			}
		else{
			MW_invar=true;	  
			}
		return;
		}
	  else if((((_var*)cur_menu->var)->type==MW_ACTION) && (!bitRead(cur_menu->flags,MW_ACTION_CONFIRM))){
        ((_act*)cur_menu->var)->action();
		cur_menu=&m[cur_menu->parent];
		MW_invar=false;  
		return;
		}
      else if((((_var*)cur_menu->var)->type==MW_EDIT_TEXT)){
		MW_invar=false;  
		return;
		}
      else{
		MW_invar=true;
		return;
        }
      }
    } 
  else if(cur_menu->type==MW_VAR){
    if(((_var*)cur_menu->var)->type==MW_LIST){        
      VINT(((_var*)cur_menu->var)->val)=cur_menu->cur_item;}
    else if(((_var*)cur_menu->var)->type==MW_AUTO_INT){        
      VINT(((_var*)cur_menu->var)->old)=VINT(((_var*)cur_menu->var)->val);}
    else if(((_var*)cur_menu->var)->type==MW_AUTO_FLOAT){        
      VFLOAT(((_var*)cur_menu->var)->old)=VFLOAT(((_var*)cur_menu->var)->val);}
    else if(((_var*)cur_menu->var)->type==MW_AUTO_BYTE){        
      VBYTE(((_var*)cur_menu->var)->old)=VBYTE(((_var*)cur_menu->var)->val);}
    else if(((_var*)cur_menu->var)->type==MW_BOOL){        
      VBOOL(((_var*)cur_menu->var)->old)=VBOOL(((_var*)cur_menu->var)->val);}
    else if((((_var*)cur_menu->var)->type==MW_ACTION)&&(bitRead(cur_menu->flags,MW_ACTION_CONFIRM))){
      ((_act*)cur_menu->var)->action();}
    else if(((_var*)cur_menu->var)->type==MW_EDIT_TEXT){        
      }
	cur_menu=&m[cur_menu->parent];
    MW_invar=false;
    #ifdef EEPROM_SUPPORT
      if (eeprom_write_on_confirm) writeEeprom();
    #endif
    }
  }

int menwiz::getErrorMessage(bool fl){
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
      case 320: Serial.println(F("E320-Text variable pointer is NULL"));break;       
      case 410: Serial.println(F("E410-Behavihour available only with 6 buttons. ignored"));break; 
      case 500: Serial.println(F("E500-out of user range (1-3)"));break;
      case 900: Serial.println(F("E900-Out of memory"));break; 
      default:  Serial.println(F("E000-Unknown err"));break; 
      }
    }
*/
  if (fl){Serial.print(F("ERROR:"));Serial.println(MW_error);}
  return MW_error;
  }

void menwiz::setBehaviour(MW_FLAGS b, bool v){

  ERROR(0);  
  bitWrite(flags,b,v);
  }
  
void _menu::setBehaviour(MW_FLAGS b, bool v){

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
  else if(b==MW_GRANT_USER1){
     bitWrite(flags,MW_GRANT_USER1,v);
     }
  else if(b==MW_GRANT_USER2){
     bitWrite(flags,MW_GRANT_USER2,v);
     }
  else if(b==MW_GRANT_USER3){
     bitWrite(flags,MW_GRANT_USER3,v);
     }
  }

int menwiz::freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }

#ifdef EEPROM_SUPPORT
void  menwiz::writeEeprom(){
  Etype temp;
  int addr=eeprom_offset;

  ERROR(0);
  for (int i=0;i<idx_m;i++){
		if(m[i].type==MW_VAR){
			switch(((_var*)m[i].var)->type){
				case MW_AUTO_BYTE:{
					temp.b = VBYTE(((_var*)m[i].var)->val);
					EEPROM.update(addr, temp.bytes[0]); //define update in your EEPROM.h and EEPROM.cpp !!!
					addr++;
					}
					break;
				case MW_BOOL:{
					temp.bl = VBOOL(((_var*)m[i].var)->val);
					EEPROM.update(addr, temp.bytes[0]);
					addr++;
					}
					break;
				case MW_AUTO_FLOAT:{
					temp.f = VFLOAT(((_var*)m[i].var)->val);
					for (int i=0; i<4; i++){
					  EEPROM.update(addr +i, temp.bytes[i]);
					  }
					addr=addr+4;
					}
					break;
				case MW_LIST:{
					temp.i = (int)m[i].cur_item;
					for (int i=0; i<2; i++) {
					  EEPROM.update(addr +i, temp.bytes[i]);
					  }    
					addr=addr+2;
					}
					break;
				case MW_AUTO_INT:{
					temp.i = VINT(((_var*)m[i].var)->val);
					for (int i=0; i<2; i++) {
					  EEPROM.update(addr +i, temp.bytes[i]);
					  }    
					addr=addr+2;
					}
					break;
				case MW_EDIT_TEXT:{
				    int l;
					char *cp=(char*)((_var*)m[i].var)->val;
					l=strlen(cp);
					for (int i=0; i<l; i++) {
					  EEPROM.update(addr +i, cp[i]);
					  }
					EEPROM.update(addr +l+1, 0);
					addr=addr+l+1;
					}
					break;
				default:
				    break;
				}//switch
		   }//if
		}//for
   }//function

void  menwiz::readEeprom(){
  Etype temp;
  int addr=eeprom_offset;

  ERROR(0);
  for (int i=0;i<idx_m;i++){
    if(m[i].type==MW_VAR){
	switch(((_var*)m[i].var)->type){
	    case MW_AUTO_BYTE:{
		    temp.bytes[0] = EEPROM.read(addr);
		    VBYTE(((_var*)m[i].var)->val)=temp.b;
		    VBYTE(((_var*)m[i].var)->old)=temp.b;
		    addr++;
            }
		    break;
	    case MW_BOOL:{
		    temp.bytes[0] = EEPROM.read(addr);
		    VBOOL(((_var*)m[i].var)->val)=temp.bl;
		    VBOOL(((_var*)m[i].var)->old)=temp.bl;
		    addr++;
            }
		    break;
	    case MW_AUTO_FLOAT:{
		    for (int n=0; n<4; n++) {
		      temp.bytes[n] = EEPROM.read(addr+n);
		      }
		    VFLOAT(((_var*)m[i].var)->val)=temp.f;
		    VFLOAT(((_var*)m[i].var)->old)=temp.f;
		    addr=addr+4;
            }
		    break;
	    case MW_AUTO_INT:{
		    for (int n=0; n<2; n++) {
		     temp.bytes[n] = EEPROM.read(addr+n);
		     }
		    VINT(((_var*)m[i].var)->val)=temp.i;
		    VINT(((_var*)m[i].var)->old)=temp.i;
		    addr=addr+2;
            }
		    break;
	    case MW_LIST:{
		    for (int n=0; n<2; n++) {
		      temp.bytes[n] = EEPROM.read(addr+n);
		      }
		    VINT(((_var*)m[i].var)->val)=temp.i;
		    VINT(((_var*)m[i].var)->old)=temp.i;
		    m[i].cur_item=(byte) temp.i;
		    addr=addr+2;
            }
		    break;
	    case MW_EDIT_TEXT:{
 		    char c,*cp; 
			int l,n;
			cp=(char*)((_var*)m[i].var)->val;
			l=strlen(cp);
			memset(cp,32,l);
			cp[l]=0;
			for(n=0;n<l;n++){
               c=EEPROM.read(addr+n);
			   if ((int)c==0){ 
				  n=l;
				  }
			   else
				  cp[n]=c;
			   }
			addr=addr+l+1;
            }
	        break;
	    }
      }
    }
  }

#endif

