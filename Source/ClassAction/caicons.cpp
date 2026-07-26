/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                & Salim Gasmi

        File: caicons.cpp

        Aim:  prevent all functions
              for displaying and handling
              icon buttons

****************************************/


#include <classes/window.h>
#include <clib/alib_protos.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <gadgets/layout.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/space.h>
#include <workbench/icon.h>
#include <workbench/workbench.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caexec.h"
#include "caexec.h"
//#include "caextern.h"
#include "caFileTypes.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classaction.h"
#include "classactioncd.h"

#include "caicons.h"

IClass *MakeIconClass();


TcaIcons *caIcons;

//>"void TcaIcons::Init()"
void TcaIcons::Init(){
  // get iconclass:
  IconClass=(IClass*)MakeIconClass();
  NewList( &Icons[0] );
  NewList( &Icons[1] );
  NewList( &Icons[2] );
}
//<

//>"void TcaIcons::Free()"
void TcaIcons::Free(){
  if( IconClass ) FreeClass( IconClass );
}
//<

//>"void TcaIcons::LoadCommands( TIniFile *IniFile,Screen *scr,Object *Fuelgauge,*UpperLayout,*LowerLayout )"
void TcaIcons::LoadCommands( TIniFile *IniFile,Screen *scr,Object *Fuelgauge,Object *UpperLayout,Object *LowerLayout ){
 DiskObject *dob;
 IconNode *nd;
 Object *gadget;
 int i=150;
 BOOL end=FALSE,space;
 List *list;
 int mode=-1,width;
 Object *layout;
 char *entryline;

 oFuelgauge = Fuelgauge;
 oUpperLayout = UpperLayout;
 oLowerLayout = LowerLayout;

 if( !IniFile->SeekGroup( "Commands" ) ) return;

 do{
  entryline=IniFile->GetNextGroupEntry();
  if( entryline && entryline[0] ){
   if( mode isnot atoi(entryline) ){
    mode=atoi( entryline );
    list=&Icons[mode];
    switch( mode ){
     case 0: layout=UpperLayout; break;
     case 1: layout=LowerLayout; break;
     case 2: layout=NULL;                       break;
    }
   }

   nd=(IconNode*)AllocVec( sizeof( IconNode ),MEMF_CLEAR );
   nd->Id=i;


   if( entryline=IniFile->GetNextGroupEntry() ){ // name
     nd->Action = caActions->CreateAction( entryline );
   }

   if( entryline=IniFile->GetNextGroupEntry() ){ // help
    nd->LHlp=strlen( entryline )+1;
    nd->Help=(char*)AllocMem( nd->LHlp,MEMF_CLEAR );
    if( nd->Help ) strcpy( nd->Help,entryline );
    else{ nd->LHlp=0; nd->Help=""; }
   }else{
    nd->LHlp=0;
    nd->Help="";
   }
   
   if( entryline=IniFile->GetNextGroupEntry() ){ // command
     strcpy( nd->Action->Exec,entryline );
   }

   if( (entryline=IniFile->GetNextGroupEntry()) and (entryline[0] is 'M') ){ // wbrun
     nd->Action->Type = entryline[1]-48;
     nd->Action->CD   = entryline[2]-48;
     if( (nd->Action->CD<0) || (nd->Action->CD>2) ) nd->Action->CD=0;
     if( strchr(entryline,'A') ) nd->Action->Async=TRUE;
     if( strchr(entryline,'C') ) nd->Action->CD=1;
     if( strchr(entryline,'S') ) nd->Action->ReadSrc=TRUE;
     if( strchr(entryline,'D') ) nd->Action->ReadDst=TRUE;
   }
   space=( nd->Action->Type == RUN_SPACE );
   if( entryline=IniFile->GetNextGroupEntry() ){ // stack
     strcpy( nd->Action->Stack, entryline );
   }
   if( entryline=IniFile->GetNextGroupEntry() ){ // delay
     strcpy( nd->Action->Delay, entryline );
   }
   if( entryline=IniFile->GetNextGroupEntry() ){ // icon
    if( entryline[0] ) dob=GetIconTags( entryline,ICONGETA_Screen,scr,TAG_END );
    else dob=NULL;
   }else dob=NULL;
   if( !dob ) space=TRUE;       // icon not found, make a space gadget
   if( entryline=IniFile->GetNextGroupEntry() ){ // key
    if( entryline[0]=='"' ){
     nd->Key=entryline[1];
     nd->Ascii=TRUE;
    }else{
     nd->Key=atoi( entryline );
     nd->Ascii=FALSE;
    }
   }else{ nd->Key=0; nd->Ascii=FALSE; }

   if( (space || dob) && layout ){
    if( space ){
     gadget=(Object*)NewObject( SPACE_GetClass(),NULL,
                         CHILD_WeightedWidth,100L,
                        TAG_END );
     width = 100;
    }else{
     gadget=(Object*)NewObject( IconClass,NULL,
                        GA_ID,nd->Id, GA_RelVerify,TRUE,
                        GA_UserData,dob,
                        GA_GadgetHelp,TRUE,
                        TAG_END );
     width = 0;
    }
    SetGadgetAttrs( (Gadget*)layout,NULL,NULL,
                        LAYOUT_AddChild,gadget,CHILD_WeightedWidth,width,TAG_END );
    nd->Gadg=gadget;
   }else nd->Gadg=NULL;
   AddTail( list,(Node*)nd );
  }else i=300;
  i++;
 }while( i<256 );
}
//<

//>"void TcaIcons::FreeIconList( List *list )"
void TcaIcons::FreeIconList( List *list ){
 IconNode *nd;
 while( !IsListEmpty(list) ){
  nd=(IconNode*)RemHead( list );
  if( nd->LHlp>0 ) FreeMem( nd->Help,nd->LHlp );
  if( nd->Action ) caActions->DeleteAction( nd->Action );
  FreeVec( nd );
 }
}
//<

//>"void TcaIcons::FreeIcons()"
void TcaIcons::FreeIcons(){
 for( int i=0;i<3;i++ ){
  FreeIconList( &Icons[i] );
 }
}
//<

//>"List *TcaIcons::GetCommandList( int Index )"
List *TcaIcons::GetCommandList( int Index ){
  if( (Index>-1) and (Index<3) ) return &Icons[Index];
}
//<

//>"BOOL TcaIcons::DoIconCommand( Window *win,ULONG msg )"
/****************************************/
/*                                      */
/* Function: DoIconCommand              */
/*                                      */
/* Aim:      Execute the command for    */
/*           gadget or key that was     */
/*           activated.                 */
/*                                      */
/* Input:    msg = result of            */
/*                WM_HANDLEINPUT        */
/*                                      */
/* Output:   TRUE: command found        */
/*           FALSE: not found           */
/*                                      */
/****************************************/
BOOL TcaIcons::DoIconCommand( Window *win,ULONG msg ){
 IconNode *node;
 int count=0;
 ULONG id;

 switch( msg & WMHI_CLASSMASK ){
  case WMHI_GADGETUP:{
   id = msg & WMHI_GADGETMASK;
   do{
    node=(IconNode*)(Icons[count].lh_Head);
    while( (node->ln_Succ!=NULL)&&(node->Id!=id) ) node=(IconNode*)(node->ln_Succ);
    if( node->ln_Succ==NULL ) node=NULL;
    count++;
   }while( (count<3) && (node==NULL) );
   break;
  }
  case WMHI_RAWKEY:{
   id = msg & WMHI_KEYMASK;
   do{
    node=(IconNode*)(Icons[count].lh_Head);
    while( (node->ln_Succ!=NULL)&&((node->Key!=id)||(node->Ascii)) ){
     node=(IconNode*)(node->ln_Succ);
    }
    if( node->ln_Succ==NULL ) node=NULL;
    count++;
   }while( (count<3) && (node==NULL) );
   break;
  }
  case WMHI_VANILLAKEY:{
   id = msg & WMHI_KEYMASK;
   do{
    node=(IconNode*)(Icons[count].lh_Head);
    while( (node->ln_Succ!=NULL)&&((node->Key!=id)||(!node->Ascii)) ){
     node=(IconNode*)(node->ln_Succ);
    }
    if( node->ln_Succ==NULL ) node=NULL;
    count++;
   }while( (count<3) && (node==NULL) );
   break;
  }
 }
 if( node ){
   Runner( node->Action );
   if( (msg & WMHI_CLASSMASK)==WMHI_GADGETUP ){
     SetGadgetAttrs( (Gadget*)(node->Gadg),win,NULL,GA_Selected,FALSE,TAG_END );
     RefreshGList( (Gadget*)(node->Gadg),win,NULL,1 );
   }
/*
  FileList FL(Main.CurrentList);
  char *filename=new char[1000];
  char *ptr;
  // determine how many commands must be executed:
  if( strstr(node->Command,"[f]")||strstr(node->Command,"[F]") ) count=FL.Selected;
  else count=1;
  for( int i=0;i<count;i++ ){
   strcpy( filename,GetSrc() );
   if( FL.Selected ){
    // build filename:
    AddPart( filename,FL.NextSFile(NULL),500 );
   }else{
    if( filename[0]!=0 ){
     ptr=&(filename[strlen(filename)-1]);
     if( (*ptr!='/')&&(*ptr!=':') ) strcat(filename,"/");
    }else strcpy( filename,"SYS:" );
   }
   Action act;
   act.wbrun=node->Type;
   sprintf(act.stack,"%d",node->Stack);
   sprintf(act.delay,"%d",node->Delay);
   strncpy( act.exec,node->Command,250 );
   Runner( &act,filename );
  }
  if( (msg & WMHI_CLASSMASK)==WMHI_GADGETUP ){
   SetGadgetAttrs( (Gadget*)(node->Gadg),Main.IntuiWin,NULL,GA_Selected,FALSE,TAG_END );
   RefreshGList( (Gadget*)(node->Gadg),Main.IntuiWin,NULL,1 );
  }
  if( node->Type>9 ){
   ListDir( 0,NULL,0 );
   ListDir( 1,NULL,0 );
  }
  delete filename;
  */
  return( TRUE );
 }else return( FALSE );
}
//<

//>"void TcaIcons::ShowIconHelp( Window *win,ULONG id )"
void TcaIcons::ShowIconHelp( Window *win,ULONG id ){
 IconNode *node;

 node=(IconNode*)(Icons[0].lh_Head);
 while( (node->ln_Succ!=NULL)&&(node->Id!=id) ) node=(IconNode*)(node->ln_Succ);
 if( node->ln_Succ==NULL ){
  node=(IconNode*)(Icons[1].lh_Head);
  while( (node->ln_Succ!=NULL)&&(node->Id!=id) ) node=(IconNode*)(node->ln_Succ);
  if( node->ln_Succ==NULL ){
   node=(IconNode*)(Icons[2].lh_Head);
   while( (node->ln_Succ!=NULL)&&(node->Id!=id) ) node=(IconNode*)(node->ln_Succ);
 }}
 if( node->ln_Succ!=NULL ){
  if( node->LHlp>0 ){
   SetGadgetAttrs( (Gadget*)oFuelgauge,win, NULL,
                   GA_Text,node->Help,TAG_DONE );
  }else{
   SetGadgetAttrs( (Gadget*)oFuelgauge,win, NULL,
                   GA_Text,"?",TAG_DONE );
  }
 }
}
//<
