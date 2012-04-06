//==========.cpp
#include "MENWIZ.h"

#define SCREATE(p,s) p=(char *)malloc(strlen(s)+1); strcpy(p,s)
#define SFORM(b,s,l) memset(b,32,l); memcpy(b,s,strlen(s)); b[l]=NULL; lcd->print(b)
#define ERROR(a)     MW_error=a

char buf[21];
char MW_ver[]={"0.1.4"};
byte MW_error;
byte c0[8]={B00000, B00000, B00001, B00010, B10100, B01000, B00000, B00000}; 

menwiz::menwiz(){
  fl_splash=false;
  fl_usrscreen=false;
  fl_splash_draw=false;
  fl_usrscreen_draw=false;
  btx.last_button=MW_BTU;
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
  fl_option=false;
}

_option::_option(){
  label=NULL;
}

_var::_var(){
  type=0;
  val=NULL;
}

_menu * menwiz::addMenu(int t,_menu * p, char *lab){
   _option *op;

  ERROR(0);   
   //INSTANTIATE NEW MENU VARIABLES
  if (idx_m!=MAX_MENUS){   
    m[idx_m].type=t;         // ROOT| SUBMENU| VAR
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
      op=p->addItem(MW_SUBMENU,(char *)lab);
      op->sbm=idx_m;
      }
    idx_m++;
    return &m[idx_m-1];
    }
  else{ERROR(100);}
// ERROR    
   }

_option *_menu::addItem(int t,char *lab){
  static _option *op;

  ERROR(0);
  if (idx_o!=MAX_OPTXMENU){
    op=(_option *) malloc(sizeof(_option));
    op->type=t;
    SCREATE(op->label,(char *)lab);
    o[idx_o]=(_option*)op;
    idx_o++;
    return op;
    }
  else{ERROR(100);}
// ERROR    
   } 

void _menu::addVar(int t, void* v){

  ERROR(0);
  if (t!=MW_LIST)
    ERROR(120);
  else if(type==MW_VAR){
    var.type=(int)t;
    var.val=v;
    cur_item=VBYTE(v);
    }
  else{ERROR(110);}
// ERROR    
  }
  
void _menu::addVar(int t, void* v, int low, int up, int incr){

  ERROR(0);
  if (t!=MW_AUTO_INT)
    ERROR(120);
  else if(type==MW_VAR){
    var.type=(int)t;
    var.val=v;
    var.lower=malloc(sizeof(int)); if(var.lower!=NULL) VINT(var.lower)=low; else {ERROR(900); return;}
    var.upper=malloc(sizeof(int)); if(var.upper!=NULL) VINT(var.upper)=up; else {ERROR(900); return;}
    var.incr=malloc(sizeof(int));  if(var.incr!=NULL) VINT(var.incr)=incr; else {ERROR(900); return;} 
    var.old=malloc(sizeof(int));   if(var.old!=NULL) VINT(var.old)=VINT(var.val); else {ERROR(900); return;} 
    }
  else{ERROR(110);}
// ERROR    
  }

void _menu::addVar(int t, boolean* v){

  ERROR(0);
  if (t!=MW_BOOLEAN)
    ERROR(120);
  else if(type==MW_VAR){
    var.type=(int)t;
    var.val=(void *)v;
    var.old=malloc(sizeof(boolean));  if(var.old!=NULL) VBOOL(var.old)=VBOOL(var.val); else {ERROR(900); return;} 
    }
  else{ERROR(110);}
// ERROR    
  }

void  _menu::addVar(int t,void (*f)()){
  ERROR(0);
  if (t!=MW_ACTION)
    ERROR(120);
  else if(type==MW_VAR){
    var.type=(int)t;
    var.action=f;
    }
  else{ERROR(110);}
// ERROR    

  }
  
void menwiz::begin(void *l,int c, int r){

  ERROR(0);
  tm_start=millis();
#ifdef I2C
  lcd=(LiquidCrystal_I2C*)l; row=r; col=c;
#else
  lcd=(LiquidCrystal*)l; row=r; col=c;
#endif 
  lcd=(LiquidCrystal_I2C*)l; row=r; col=c;
  lcd->init(); 
  lcd->setBacklight(HIGH);
  lcd->setCursor(0, 0);
  lcd->noCursor();
  lcd->createChar(0,c0);
  sbuf=(char*)malloc(r*c+r); if(sbuf==NULL) ERROR(900);
  fl_menu_draw=false;
  }

char* menwiz::getVer(){

  ERROR(0);
  return MW_ver; 
  }

void menwiz::draw(){
  
  ERROR(0);
  scanButtons();
  int long lap1=(millis()-tm_start);
  int long lap2=(millis()-btx.tm_push);

  // if defined splashscreen & not yet drawn & time window is ok, draw it  
  if((fl_splash==true) && (lap1<tm_splash)){
    cur_mode=MW_MODE_SPLASH;
    //draw only once
    if(fl_splash_draw==false){
      draw_splash();
      fl_splash_draw=true;
      }
    }
  // if defined usrscreen & time since last button push > user defined time, draw it  
  else if((fl_usrscreen==true) && (lap2>tm_usrscreen)){
    cur_mode=MW_MODE_USRSCREEN;
    fl_menu_draw=false;
    UsrScreen();
    }
  else{
  // if a button was pushed since last call, draw menu  
    cur_mode=MW_MODE_MENU;
    if((btx.last_button!=MW_BTNULL) || (!fl_menu_draw))
	fl_menu_draw=true;
	draw_menu(cur_menu);
    }
  }

void menwiz::draw_splash(){
  int i,j,k;

  ERROR(0);
  for (int i=0,j=0,k=0;i<(strlen(sbuf)+1);i++){
    if((sbuf[i]=='#')||(sbuf[i]==0)){
      buf[j]=0;
      lcd->setCursor(0,k);
      lcd->print(buf);
      j=0;
      k++;
      }
    else{
      buf[j]=sbuf[i];
      j++;
      }
    if(k==row)
      return;
    }
  }
  
void menwiz::draw_menu(_menu *mc){
  int rstart,rstop,i,j;
  _option *op;

  ERROR(0);
  lcd->setCursor(0,0);
  SFORM(buf,(char*)mc->label,(int) col);

  if (mc->type==MW_VAR){
    draw_val(mc);
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
        SFORM(buf," ",(int) col);
        }
      }
    }    
  }

void menwiz::draw_val(_menu *mc){
  int rstart,rstop,i,j;
  _option *op;
  
  ERROR(0);
  switch ((int)mc->var.type){
    case MW_LIST:
      rstart=max(0,mc->cur_item-(row-2));
      rstop=min((mc->idx_o),rstart+(row));
      for (i=1,j=rstart;i<row;i++,j++){// for all remaining lcd rows
        if(j<rstop){
          op=(_option*)mc->o[j];
          lcd->setCursor(0,i);
          lcd->write((j==mc->cur_item)?0:165);
          SFORM(buf,op->label,(int) col-1);
          }
        else{
          lcd->setCursor(0,i);
          SFORM(buf," ",(int) col);
          }
        }
      break;  
    case MW_AUTO_INT:
      for(i=2;i<row;i++){
        lcd->setCursor(0,i);
        SFORM(buf," ",(int) col);
        }
      lcd->setCursor(0,1);
      sprintf(sbuf,"%d< %d <%d",VINT(mc->var.lower),VINT(mc->var.old),VINT(mc->var.upper));
      SFORM(buf,sbuf,(int) col);
      break;      
    case MW_BOOLEAN:
      for(i=2;i<row;i++){
        lcd->setCursor(0,i);
        SFORM(buf," ",(int) col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,VBOOL(mc->var.old)?"ON":"OFF",(int) col);
      break;      
    case MW_ACTION:
      for(i=1;i<row;i++){
        lcd->setCursor(0,i);
        SFORM(buf," ",(int) col);
        }
      lcd->setCursor(0,1);
      SFORM(buf,"Confirm to run...",(int) col);
      break;      
    default:{}
    }
  }

void menwiz::addSplash(char *s, int lap){

  ERROR(0);
  strcpy(sbuf,s);
  tm_splash=lap;
  fl_splash=true;
  fl_splash_draw=false;
  }

void menwiz::addUsrScreen(void f(), unsigned long t){

  ERROR(0);
  UsrScreen=f;
  fl_usrscreen=true;
  tm_usrscreen=t;
  }

void menwiz::navButtons(int btu,int btd,int btl,int btr,int bte,int btc){
  
  ERROR(0);
  if(btu!=0){btx.BTU.assign(btu);  btx.BTU.setMode(OneShot);  btx.BTU.turnOnPullUp();} 
  if(btd!=0){btx.BTD.assign(btd);  btx.BTD.setMode(OneShot);  btx.BTD.turnOnPullUp();} 
  if(btl!=0){btx.BTL.assign(btl);  btx.BTL.setMode(OneShot);  btx.BTL.turnOnPullUp();} 
  if(btr!=0){btx.BTR.assign(btr);  btx.BTR.setMode(OneShot);  btx.BTR.turnOnPullUp();} 
  if(bte!=0){btx.BTE.assign(bte);  btx.BTE.setMode(OneShot);  btx.BTE.turnOnPullUp();} 
  if(btc!=0){btx.BTC.assign(btc);  btx.BTC.setMode(OneShot);  btx.BTC.turnOnPullUp();} 

  // bouncing disarm 
  btx.BTU.check();
  btx.BTD.check(); 
  btx.BTL.check(); 
  btx.BTR.check(); 
  btx.BTE.check(); 
  btx.BTC.check(); 
  }

int menwiz::scanButtons(){ 
  _option* oc;
  int b=0;
  
  ERROR(0);
  oc=(_option*)cur_menu->o[cur_menu->cur_item];

  //skip last button push and set current status to menu if usrscreen is active
  if(cur_mode==MW_MODE_USRSCREEN){
    b=btx.BTU.check()+btx.BTD.check()+btx.BTL.check()+btx.BTR.check()+btx.BTE.check()+btx.BTC.check();
    if(b!=0){ 
       cur_mode=MW_MODE_MENU;
       btx.last_button=MW_BTE;}
    else{
       btx.last_button=MW_BTNULL;
       return btx.last_button;
       }
    }
  else if(btx.BTU.check()==ON){
    if((cur_menu->type!=MW_VAR)||(cur_menu->var.type==MW_LIST))    
      cur_menu->cur_item=(cur_menu->cur_item-1)<0?(cur_menu->idx_o-1):cur_menu->cur_item-1;
//      cur_menu->cur_item=(cur_menu->cur_item-1)%(cur_menu->idx_o);
    btx.last_button=MW_BTU;}
  else if (btx.BTD.check()==ON){
    if((cur_menu->type!=MW_VAR)||(cur_menu->var.type==MW_LIST))    
      cur_menu->cur_item=(cur_menu->cur_item+1)%(cur_menu->idx_o);
    btx.last_button=MW_BTD;}
  else if (btx.BTL.check()==ON){
    if(cur_menu->var.type==MW_AUTO_INT){
      VINT(cur_menu->var.old)=max((VINT(cur_menu->var.old)-VINT(cur_menu->var.incr)),VINT(cur_menu->var.lower));
      }    
    else if(cur_menu->var.type==MW_BOOLEAN){
      VBOOL(cur_menu->var.old)=!VBOOL(cur_menu->var.old);
      }    
    btx.last_button=MW_BTL;}
  else if (btx.BTR.check()==ON){
    if(cur_menu->var.type==MW_AUTO_INT){
      VINT(cur_menu->var.old)=min((VINT(cur_menu->var.old)+VINT(cur_menu->var.incr)),VINT(cur_menu->var.upper));
      }    
    else if(cur_menu->var.type==MW_BOOLEAN){
      VBOOL(cur_menu->var.old)=!VBOOL(cur_menu->var.old);
      }    
    btx.last_button=MW_BTR;}
  else if (btx.BTE.check()==ON){
    if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_AUTO_INT)){        
      VBOOL(cur_menu->var.old)=VBOOL(cur_menu->var.val);
      }
    if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_BOOLEAN)){        
      VBOOL(cur_menu->var.old)=VBOOL(cur_menu->var.val);
      }
    cur_menu=&m[cur_menu->parent];
    btx.last_button=MW_BTE;}
  else if (btx.BTC.check()==ON){
    if((cur_menu->type==MW_SUBMENU)||(cur_menu->type==MW_ROOT)){
      cur_menu=(_menu*)&m[oc->sbm];
      if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_LIST))        
	cur_menu->cur_item=VINT(cur_menu->var.val);
      }
    else if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_LIST)){        
      VINT(cur_menu->var.val)=(int)cur_menu->cur_item;
      cur_menu=&m[cur_menu->parent];
      }
    else if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_AUTO_INT)){        
      VINT(cur_menu->var.val)=VINT(cur_menu->var.old);
      cur_menu=&m[cur_menu->parent];
      }
    else if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_BOOLEAN)){        
      VBOOL(cur_menu->var.val)=VBOOL(cur_menu->var.incr);
      cur_menu=&m[cur_menu->parent];
      }
    else if((cur_menu->type==MW_VAR)&&(cur_menu->var.type==MW_ACTION)){        
      cur_menu->var.action();	      
      cur_menu=&m[cur_menu->parent];
      }
    btx.last_button=MW_BTC;}
  else{
    btx.last_button=MW_BTNULL;
    return (btx.last_button);
    }
  btx.tm_push=millis();
  return (btx.last_button);
  }

int menwiz::getErrorMessage(boolean fl){
  if (fl){
    switch(MW_error)
      {
      case 0:   break; 
      case 100: Serial.println(F("ERR 100-too many items"));break; 
      case 110: Serial.println(F("ERR 110-MW_VAR menu type required"));break; 
      case 120: Serial.println(F("ERR 120-Bad function 1st arg"));break; 
      case 900: Serial.println(F("ERR 900-No memory available"));break; 
      default:  Serial.println(F("ERR 000-Unknown err"));break; 
      }
    }
  return MW_error;
  }

int menwiz::freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }
