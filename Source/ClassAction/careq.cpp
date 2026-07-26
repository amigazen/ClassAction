#include "ca_begin.h"
/****************************************

        Class Action Source

        (c) by 1994 Salim Gasmi

        File : req.c 

        Goal : Handle Requesters

        Ver  : 3.0

****************************************/

#include <classes/window.h>
#include <clib/alib_protos.h>
#include <ctype.h>
#include <dos/dos.h>
#include <gadgets/layout.h>
#include <gadgets/string.h>
#include <images/label.h>
#include <libraries/resource.h>
#include <proto/asl.h>
#include <proto/button.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/locale.h>
#include <proto/space.h>
#include <proto/window.h>

#include "caglobal.h"
#include "caextern.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classactioncd.h"
#include "classaction.h"

BOOL UseReactionRequest;

//>"ULONG ReactionRequest( Window *iwin, EasyStruct *req,APTR a,APTR b  )"
/****************************************/
/*                                      */
/* Function: ReactionRequest            */
/*                                      */
/* Aim:      Simulate EasyRequest       */
/*           with a reaction window     */
/*                                      */
/* Input:    iwin: Window to display on */
/*           req: struct with needed    */
/*                data                  */
/*           a,b: ! yet supported     */
/*                                      */
/* Output:   like in EasyRequest:       */
/*           number of Button in order  */
/*           1,2,...,n,0                */
/*                                      */
/****************************************/
ULONG ReactionRequest( Window *iwin, EasyStruct *req,APTR a,APTR b ){
  if( !UseReactionRequest ) return( EasyRequest(iwin,req,(ULONG*)a,b) );
  BOOL end,found;
  Object *win,*vlayout,*hlayout;
  ULONG windowsignals,mask,code=0,result;
  ULONG selected;
  char buttons[256],*ptr,*button,act;
  char keys[10]; // for the activation keys
  UBYTE i,j;
  char bt[10][40];


  for( i=0;i<10;i++ ) keys[i]=0;
 // alloc vertical layout
 vlayout=(Object*)NewObject(
   LAYOUT_GetClass(),NULL,
   LAYOUT_Orientation,LAYOUT_VERTICAL,
   LAYOUT_DeferLayout,TRUE,LAYOUT_SpaceInner,TRUE,LAYOUT_SpaceOuter,TRUE,
   LAYOUT_AddChild,NewObject(SPACE_GetClass(),NULL,TAG_END),
   LAYOUT_AddChild,NewObject(
     LAYOUT_GetClass(),NULL,
//     LAYOUT_BevelStyle,BVS_FIELD,
     LAYOUT_SpaceInner,TRUE,LAYOUT_SpaceOuter,TRUE,
     LAYOUT_AddImage,NewObject(
       LABEL_GetClass(),NULL,LABEL_Text,req->es_TextFormat,LABEL_Underscore,92,
       LABEL_Justification,LJ_CENTRE,TAG_END
     ),
     TAG_END
   ),
   LAYOUT_AddChild,NewObject(SPACE_GetClass(),NULL,TAG_END),
   TAG_END
 );

 // alloc horizontal layout
 hlayout=(Object*)NewObject( LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_HORIZONTAL,TAG_END );
 SetGadgetAttrs( (Gadget*)vlayout,NULL,NULL,LAYOUT_AddChild,hlayout,TAG_END );
 // insert buttons
 strcpy( buttons, req->es_GadgetFormat );
 button=&(buttons[0]);
 code=1;
 do{
  ptr=strchr( button,'|' );
  if( ptr ) *ptr=0;     // delete |
  else code=0;          // now only the last button
  // now create an activation key:
  i=0;
  found=FALSE;
  do{
   // get a character from the button text
   act=tolower(button[i]);
   // && look if it == already used:
   for( j=0;j<10;j++ ) if( keys[j]==act ) found=TRUE;
   if( found ) i++;
  }while( found&&(button[i]!=0) );
  // now build a gadget text with an underscore before the key:
  if( !found ){
   strncpy( bt[code],button,i );
   bt[code][i]=0;
   strcat( bt[code],"_" );
   strcat( bt[code],&(button[i]) );
   keys[code]=act;
  }else strcpy( bt[code],button );
  SetGadgetAttrs(
    (Gadget*)hlayout,NULL,NULL,
    LAYOUT_AddChild,NewObject( BUTTON_GetClass(),NULL,
      GA_ID,code,GA_RelVerify,TRUE,GA_Text,bt[code],TAG_END
    ),
    TAG_END
  );
  button=ptr+1;
  code++;
 }while( ptr );

 // open window && wait till closing
 if( a ){
  BPTR lock;
  if( lock=Lock(CA_REQPATTERN,ACCESS_READ) ) UnLock(lock);
  else a = NULL;
 }
 win=(Object*)NewObject(
   WINDOW_GetClass(),NULL,
   WINDOW_RefWindow,Main->IntuiWin,
   WINDOW_Position,WPOS_CENTERSCREEN,WINDOW_Layout,vlayout,
   a ? WINDOW_BackFillName : TAG_IGNORE,CA_REQPATTERN,   WA_PubScreen,Main->Scr,WA_IDCMP,IDCMP_GADGETUP,WA_Activate,TRUE,
   WA_Title,req->es_Title,WA_DragBar,TRUE,WA_DepthGadget,TRUE,WA_RMBTrap,TRUE,TAG_END
 );

 DoMethod( win,WM_OPEN );
 end=FALSE;
 GetAttr( WINDOW_SigMask, win, &windowsignals );
 do{
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) end=TRUE;
  while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
   switch( result & WMHI_CLASSMASK ){
    case WMHI_GADGETUP:{
     selected=(result & RL_GADGETMASK);
     end=TRUE;
     break;
    }
   }
  }
 }while( !end );
 DoMethod( win,WM_CLOSE );
 DisposeObject( win );
 return( selected );
}
//<

//>"BOOL Req(char *mess)"
/****************************************/
/*                                      */
/* Function : Req                       */
/*                                      */
/* Goal     : Pop Up a requester        */
/*            With text 'mess'          */
/*            && yes/no buttons        */
/*                                      */
/*                                      */
/* Return Values : TRUE=Yes             */
/*                 FALSE=No             */
/*                                      */
/****************************************/

BOOL Req(char *mess){
 EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };

 Requester.es_Title=GetCatalogStr( Main->Catalog,TXT_SURE,TXT_SURE_STR );
 Requester.es_TextFormat=mess;
 Requester.es_GadgetFormat=GetCatalogStr( Main->Catalog,TXT_YESNO,TXT_YESNO_STR );
 if( ReactionRequest(Main->IntuiWin,&Requester,NULL,NULL) ) return( TRUE );
 else return( FALSE );
}
//<

//>"void DInfo(char *mess)"
/****************************************/
/*                                      */
/* Function : DInfo                     */
/*                                      */
/* Goal     : Pop Up a requester        */
/*            With text mess and a      */
/*            OKAY Button               */
/*            return if DISKINSERTED    */
/*                                      */
/* Return Values : NONE                 */
/*                                      */
/****************************************/

void DInfo(char *mess){
 struct EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };
 ULONG iflags = IDCMP_DISKINSERTED;

 Requester.es_Title=GetCatalogStr( Main->Catalog,TXT_INFORMATION,TXT_INFORMATION_STR );
 Requester.es_TextFormat=mess;
 Requester.es_GadgetFormat=GetCatalogStr( Main->Catalog,TXT_OK,TXT_OK_STR );
 ReactionRequest(Main->IntuiWin,&Requester,&iflags,NULL);
}
//<

//>"void Info( char *mess,BOOL cabackground=FALSE )"
/****************************************/
/*                                      */
/* Function: Info                       */
/*                                      */
/* Aim:      Pop Up a requester         */
/*           With text mess and a       */
/*           OKAY Button                */
/*                                      */
/* In:       mess  = Message to display */
/*           cabackground = true,       */
/*            if you want a special     */
/*            background in the window  */
/*                                      */
/****************************************/

void Info( char *mess,BOOL cabackground ){
 struct EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };

 Requester.es_Title=GetCatalogStr( Main->Catalog,TXT_INFORMATION,TXT_INFORMATION_STR );
 Requester.es_TextFormat=mess;
 Requester.es_GadgetFormat=GetCatalogStr( Main->Catalog,TXT_OK,TXT_OK_STR );
 ReactionRequest( Main->IntuiWin,&Requester,(APTR)cabackground,NULL );
}
//<

//>"int SureReq(char *st,char *button)"
/****************************************/
/*                                      */
/* Function:  SureReq                   */
/*                                      */
/* Aim:       Display a requester with  */
/*            the string st and the     */
/*            buttons specified by      */
/*            button                    */
/*                                      */
/* Output:    Number of selected button */
/*            (1,...,n,0)               */
/*                                      */
/****************************************/
int SureReq(char *st, char *button){
 struct EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };
 Requester.es_Title=GetCatalogStr( Main->Catalog,TXT_SURE,TXT_SURE_STR );
 Requester.es_TextFormat=st;
 Requester.es_GadgetFormat=button;
 return( ReactionRequest( Main->IntuiWin,&Requester,NULL,NULL ) );
}
//<

//>"BOOL Freq(char *file,int mode,char *text)"
/****************************************/
/*                                      */
/* Function : Freq                      */
/*                                      */
/* Goal     : Open an RT Requester      */
/*                                      */
/* file : file choosen by user          */
/* mode : 0=files,1=dirs,2=volumes      */
/* text : title of the Requester        */
/*                                      */
/* Return Values :                      */
/*                                      */
/*  FALSE : User canceled the requester */
/*  TRUE  : Okay                        */
/*                                      */
/****************************************/

BOOL Freq(char *file,int mode,char *text){
 char filename[256],total[512],*ptr;
 BOOL result;

 strcpy( filename,FilePart(file) );
 if( file[0]==0 ) strcpy( total,"SYS:" );
 else strcpy( total,file );
 ptr=PathPart( total );
 *ptr=0;
 switch( mode ){
  case 0:{
   result=AslRequestTags( Main->FileReq,
                          ASLFR_Screen,Main->Scr,
                          ASLFR_TitleText,text,
                          ASLFR_InitialFile,filename,
                          ASLFR_InitialDrawer,total,
                          ASLFR_DrawersOnly,FALSE,TAG_END );
   break;
  }
  case 1:{
   result=AslRequestTags( Main->FileReq,
                          ASLFR_Screen,Main->Scr,
                          ASLFR_TitleText,text,
                          ASLFR_InitialFile,filename,
                          ASLFR_InitialDrawer,total,
                          ASLFR_DrawersOnly,TRUE,TAG_END );
   break;
  }
  case 2:{
   result=AslRequestTags( Main->FileReq,
                          ASLFR_Screen,Main->Scr,
                          ASLFR_TitleText,text,
                          ASLFR_InitialShowVolumes,TAG_END );
   break;
  }
 }

 if( result ){
  strcpy( total, Main->FileReq->fr_Drawer );
  if( mode==0 ) strcpy( filename, Main->FileReq->fr_File );
  else filename[0]=0;
  AddPart( total,filename,512 );
  sprintf(file,"%c%s%c",'"',total,'"');
  return( TRUE );
 }else return( FALSE );
}
//<

//>"BOOL GetText(char *buffer,int len,char *msg)"
BOOL GetText(char *buffer,int len,char *msg){
 ULONG windowsignals, mask,result,code;
 BOOL Leave=FALSE,rc=FALSE;
 char *txt;
 struct Window *win;
 char oktext[40],canceltext[40],*ptr;

 SetAttrs( Main->Win, WA_BusyPointer, TRUE, TAG_END );

 SetAttrs( Main->ReqWin,WA_Title,msg,WINDOW_Position,WPOS_CENTERSCREEN,TAG_DONE );
 SetGadgetAttrs( (struct Gadget *)Main->Group5[STR_REQ],NULL, NULL,
                 STRINGA_TextVal, buffer,
                 STRINGA_MinVisible,20L, TAG_DONE );
 strcpy( oktext,"_" );
 strcat( oktext,GetCatalogStr(Main->Catalog,TXT_OK,TXT_OK_STR) );
 ptr = GetCatalogStr( Main->Catalog,TXT_CANCEL,TXT_CANCEL_STR );
 if( *ptr!=oktext[1] ) strcpy( canceltext,"_" ); // only a shortcut if not the same
 else canceltext[0]=0;
 strcat( canceltext,ptr );
 SetGadgetAttrs( (struct Gadget *)Main->Group5[BUTTON_REQ_OK],NULL, NULL,
                 GA_Text, oktext,TAG_END );
 SetGadgetAttrs( (struct Gadget *)Main->Group5[BUTTON_REQ_CANCEL],NULL, NULL,
                 GA_Text, canceltext,TAG_END );
 DoMethod( Main->ReqWin,WM_OPEN );
 GetAttr( WINDOW_Window,Main->ReqWin,(ULONG*)&win );
 ActivateLayoutGadget( (struct Gadget *)Main->Group5[LAYOUT_REQ],win,NULL,
                        (ULONG)Main->Group5[STR_REQ] );
 GetAttr( WINDOW_SigMask, Main->ReqWin, &windowsignals );
 while( !Leave ){
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Leave=TRUE;
  if( mask & windowsignals ){
   while( (result = DoMethod( Main->ReqWin, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_GADGETUP:{
      switch( result & RL_GADGETMASK ){
       case STR_REQ:
       case BUTTON_REQ_OK:{
        rc=TRUE;
        Leave=TRUE;
        break;
       }
       case BUTTON_REQ_CANCEL:{
        Leave=TRUE;
       }
 }}}}}}
 GetAttr( STRINGA_TextVal,Main->Group5[STR_REQ],(ULONG*)&txt );
 if( *txt == 0 ) rc = FALSE;
 strncpy( buffer,txt,len);
 DoMethod( Main->ReqWin,WM_CLOSE );

 SetAttrs( Main->Win, WA_BusyPointer, FALSE, TAG_END );

 return( rc );
}
//<
