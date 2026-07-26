#include "ca_begin.h"
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
        (c) by 1994 Salim Gasmi

        File : capreq.c

        Goal : Handle Requesters

        Ver  : 3.8

****************************************/

#include <classes/window.h>
#include <clib/alib_protos.h>
#include <ctype.h>
#include <gadgets/button.h>
#include <gadgets/integer.h>
#include <gadgets/layout.h>
#include <gadgets/slider.h>
#include <gadgets/string.h>
#include <images/label.h>
#include <proto/asl.h>
#include <proto/button.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/integer.h>
#include <proto/intuition.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/locale.h>
#include <proto/slider.h>
#include <proto/space.h>
#include <proto/string.h>
#include <proto/window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "captypes.h"
#define  CATCOMP_NUMBERS
#define  CATCOMP_STRINGS
#include "classactionprefscd.h"

Library *ColorWheelBase;
Library *GradientSliderBase;
Library *SliderBase;
Library *IntegerBase;
Library *LabelBase;


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
/*           a,b: not yet supported     */
/*                                      */
/* Output:   like in EasyRequest:       */
/*           number of Button in order  */
/*           1,2,...,n,0                */
/*                                      */
/****************************************/
ULONG ReactionRequest( Window *iwin, EasyStruct *req,APTR a,APTR b ){
 BOOL end,found;
 Object *win,*vlayout,*hlayout,*gadg;
 ULONG windowsignals,mask,code=0,result;
 ULONG selected;
 char buttons[256],*ptr,*button,act;
 char keys[10]; // for the activation keys
 UBYTE i,j;
 char bt[10][40];

 /* InitAll / OpenLibs failure: reaction classes may be missing. */
 if( !WindowBase || !LayoutBase || !ButtonBase || !SpaceBase )
  return( EasyRequest( iwin, req, (ULONG *)a, b ) );

 for( i=0;i<10;i++ ) keys[i]=0;
 // alloc vertical layout
 vlayout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                        LAYOUT_Orientation,LAYOUT_VERTICAL,
                        LAYOUT_DeferLayout,TRUE,
                        LAYOUT_SpaceInner,TRUE,
                        LAYOUT_SpaceOuter,TRUE,
                        TAG_END );
 // insert text
 strcpy( buttons, req->es_TextFormat );
 button=buttons;
 gadg=(Object*)NewObject( SPACE_GetClass(),NULL,TAG_END );
 SetGadgetAttrs( (Gadget*)vlayout,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
 do{
  ptr=strchr( button,(char)10 );
  if( ptr ) *ptr=0;     // delete |
  gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                        GA_Underscore,92,
                        GA_Text,button,
                        GA_ReadOnly,TRUE,
                        BUTTON_BevelStyle,BVS_NONE,
                        BUTTON_Transparent,TRUE,
                        TAG_END );
  SetGadgetAttrs( (Gadget*)vlayout,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
  button=ptr+1;
 }while( ptr );
 gadg=(Object*)NewObject( SPACE_GetClass(),NULL,TAG_END );
 SetGadgetAttrs( (Gadget*)vlayout,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );

 // alloc horizontal layout
 hlayout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                        LAYOUT_DeferLayout,TRUE,
                        LAYOUT_Orientation,LAYOUT_HORIZONTAL,
                        TAG_END );
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
   // and look if it is already used:
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
  gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                        GA_ID,code,
                        GA_RelVerify,TRUE,
                        GA_Text,bt[code],
                        TAG_END );
  SetGadgetAttrs( (Gadget*)hlayout,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
  button=ptr+1;
  code++;
 }while( ptr );

 // open window and wait till closing
 win=(Object*)NewObject( WINDOW_GetClass(),NULL,
                        WINDOW_Position,WPOS_CENTERMOUSE,
                        WINDOW_Layout,vlayout,
                        WA_PubScreen,Main.Scr,
                        WA_IDCMP,IDCMP_GADGETUP,
                        WA_Activate,TRUE,
                        WA_Title,req->es_Title,
                        WA_DragBar,TRUE,
                        WA_DepthGadget,TRUE,
                        WA_RMBTrap,TRUE,
                        TAG_END );

 if( !win ){
  if( vlayout ) DisposeObject( vlayout );
  return( EasyRequest( iwin, req, (ULONG *)a, b ) );
 }

 DoMethod( win,WM_OPEN );
 end=FALSE;
 selected=0;
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
/*            and yes/no buttons        */
/*                                      */
/*                                      */
/* Return Values : TRUE=Yes             */
/*                 FALSE=No             */
/*                                      */
/****************************************/

BOOL Req(char *mess){
 struct EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };

 Requester.es_Title=GetCatalogStr( Main.Catalog,TXT_ISSURE,TXT_ISSURE_STR );
 Requester.es_TextFormat=mess;
 Requester.es_GadgetFormat=GetCatalogStr( Main.Catalog,TXT_YESNO,TXT_YESNO_STR );
 if( ReactionRequest(Main.IntuiWin,&Requester,NULL,NULL) ) return( TRUE );
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

 Requester.es_Title=GetCatalogStr( Main.Catalog,TXT_INFORMATION,TXT_INFORMATION_STR );
 Requester.es_TextFormat=mess;
 Requester.es_GadgetFormat=GetCatalogStr( Main.Catalog,TXT_OK,TXT_OK_STR );
 ReactionRequest(Main.IntuiWin,&Requester,&iflags,NULL);
}
//<

//>"void Info(char *mess)"
/****************************************/
/*                                      */
/* Function : Info                      */
/*                                      */
/* Goal     : Pop Up a requester        */
/*            With text mess and a      */
/*            OKAY Button               */
/*                                      */
/* Return Values : NONE                 */
/*                                      */
/****************************************/

void Info(char *mess){
 struct EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };

 Requester.es_Title=GetCatalogStr( Main.Catalog,TXT_INFORMATION,TXT_INFORMATION_STR );
 Requester.es_TextFormat=mess;
 Requester.es_GadgetFormat=GetCatalogStr( Main.Catalog,TXT_OK,TXT_OK_STR );
 ReactionRequest(Main.IntuiWin,&Requester,NULL,NULL);
}
//<

//>"BOOL Freq(char *file,int mode,char *text)"
/****************************************/
/*                                      */
/* Function: Freq                       */
/*                                      */
/* Aim:      Open a File Requester      */
/*                                      */
/* Input:    file = file to show first  */
/*           mode : 0=files,            */
/*            1=dirs,2=volumes          */
/*           text = title of the Req.   */
/*                                      */
/* Output:   Result : FALSE = cancel    */
/*           file = chosen file         */
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
   result=AslRequestTags( Main.FileReq,
                          ASLFR_Screen,Main.Scr,
                          ASLFR_TitleText,text,
                          ASLFR_InitialFile,filename,
                          ASLFR_InitialDrawer,total,
                          ASLFR_DrawersOnly,FALSE,TAG_END );
   break;
  }
  case 1:{
   result=AslRequestTags( Main.FileReq,
                          ASLFR_Screen,Main.Scr,
                          ASLFR_TitleText,text,
                          ASLFR_InitialFile,filename,
                          ASLFR_InitialDrawer,total,
                          ASLFR_DrawersOnly,TRUE,TAG_END );
   break;
  }
  case 2:{
   result=AslRequestTags( Main.FileReq,
                          ASLFR_Screen,Main.Scr,
                          ASLFR_TitleText,text,
                          ASLFR_InitialShowVolumes,TAG_END );
   break;
  }
 }

 if( result ){
  strcpy( file, Main.FileReq->fr_Drawer );
  if( mode == 0 ){
    if(  Main.FileReq->fr_File[0] ) AddPart( file,Main.FileReq->fr_File,255 );
    else file[0]=0;
  }
  return( TRUE );
 }else return( FALSE );
}
//<

//>"BOOL ScreenRequester( char *modestring, char *title )"
// modestring must consist of : "<id> <width> <height> <depth> <overscan> <scroll>"
BOOL ScreenRequester( char *modestring, char *title ){

  BOOL result;
  ULONG id,width,height,depth,overscan,scroll;

  if( modestring[0] ) sscanf( modestring,"%d %d %d %d %d %d",&id,&width,&height,&depth,&overscan,&scroll );

  if( modestring[0] ){
    result=AslRequestTags( Main.ScrReq,
      ASLSM_TitleText,title,
      ASLSM_Window,Main.IntuiWin,
      ASLSM_PrivateIDCMP,TRUE,
      ASLSM_SleepWindow,TRUE,
      ASLSM_DoWidth,TRUE,
      ASLSM_DoHeight,TRUE,
      ASLSM_DoDepth,TRUE,
//      ASLSM_DoOverscanType,TRUE,
//      ASLSM_DoAutoScroll,TRUE,
      ASLSM_InitialDisplayID,id,
      ASLSM_InitialDisplayWidth,width,
      ASLSM_InitialDisplayHeight,height,
      ASLSM_InitialDisplayDepth,depth,
      ASLSM_InitialOverscanType,overscan,
      ASLSM_InitialAutoScroll,scroll != 0,
      TAG_END );
  }else{
    result=AslRequestTags( Main.ScrReq,
      ASLSM_TitleText,title,
      ASLSM_Window,Main.IntuiWin,
      ASLSM_PrivateIDCMP,TRUE,
      ASLSM_SleepWindow,TRUE,
      ASLSM_DoWidth,TRUE,
      ASLSM_DoHeight,TRUE,
      ASLSM_DoDepth,TRUE,
//      ASLSM_DoOverscanType,TRUE,
//      ASLSM_DoAutoScroll,TRUE,
      TAG_END );
  }

  if( result ){
    sprintf( modestring,"%d %d %d %d %d %d",
      Main.ScrReq->sm_DisplayID,
      Main.ScrReq->sm_DisplayWidth,
      Main.ScrReq->sm_DisplayHeight,
      Main.ScrReq->sm_DisplayDepth,
      Main.ScrReq->sm_OverscanType,
      Main.ScrReq->sm_AutoScroll );
    return( TRUE );
  }else{
    modestring[0] = 0;
    return( FALSE );
  }
}
//<

//>"BOOL FontReq(char *font, char *text)"
//////////////////////////////////////////////////////////////////
// FontReq                                                      //
//  requests a font from the user                               //
// In:                                                          //
//  font : font choosen by user, formatted as "fontname,size"   //
//  text : title of the Requester                               //
// Out:                                                         //
//  Result : false = canceled                                   //
//  font   : requested font as above                            //
//////////////////////////////////////////////////////////////////
BOOL FontReq( char *font, char *text ){
 char *ptr;
 BOOL result;
 int size;

 // build TextAttr from string <font>:
 if( font[0] ){
  ptr = strchr( font,',' );
  if( ptr ){
   *ptr = 0; // cut comma and size
   size = atoi( ptr+1 ); // compute size
  }else size = 8;
 }
 result=AslRequestTags( Main.FontReq,
                        ASLFO_Screen,Main.Scr,
                        ASLFO_TitleText,text,
                        ASLFO_InitialName,font,
                        ASLFO_InitialSize,size,
                        TAG_END );
 if( result ){
  sprintf( font,"%s,%d",Main.FontReq->fo_Attr.ta_Name,Main.FontReq->fo_Attr.ta_YSize );
  return( TRUE );
 }else return( FALSE );
}
//<

//>"BOOL GetText(char *buffer,int len,char *msg)"
BOOL GetText(char *buffer,int len,char *msg){
 ULONG windowsignals, mask,result,code;
 BOOL Leave=FALSE,rc=FALSE;
 char *txt;
 char ok[100],cancel[100];
 int i;
 struct Window *iwin;
 Object *win,*layout,*layout2,*gadg,*strgadg;
 
 txt=GetCatalogStr( Main.Catalog,TXT_OK,TXT_OK_STR);
 ok[0]='_';
 strcpy( ok+1,txt );
 txt=GetCatalogStr( Main.Catalog,TXT_CANCEL,TXT_CANCEL_STR);
 i=0;
 while( (*(txt+i)!=0) && (*(txt+i)==ok[1]) ){
  i++;
 }
 if( *(txt+i)==0 ) strcpy( cancel,txt );
 else{
  strncpy( cancel,txt,i );
  cancel[i]='_';
  strcpy( cancel+i+1,txt+i );
 }

 SetAttrs( Main.Win, WA_BusyPointer, TRUE, TAG_END );


 layout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                                LAYOUT_DeferLayout,TRUE,
                                LAYOUT_Orientation,LAYOUT_VERTICAL,
                                TAG_END );
 strgadg=(Object*)NewObject( STRING_GetClass(),NULL,
                                GA_ID,0,
                                GA_RelVerify,TRUE,
                                STRINGA_TextVal,buffer,
                                STRINGA_MaxChars,256,
                                STRINGA_MinVisible,20L,
                                TAG_END );
 SetGadgetAttrs( (Gadget*)layout,NULL,NULL,LAYOUT_AddChild,strgadg,TAG_END );
 layout2=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                                LAYOUT_Orientation,LAYOUT_HORIZONTAL,
                                TAG_END );
 SetGadgetAttrs( (Gadget*)layout,NULL,NULL,LAYOUT_AddChild,layout2,TAG_END );
 gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                                GA_Text,ok,
                                GA_ID,1,
                                GA_RelVerify,TRUE,
                                TAG_END );
 SetGadgetAttrs( (Gadget*)layout2,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
 gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                                GA_Text,cancel,
                                GA_ID,2,
                                GA_RelVerify,TRUE,
                                TAG_END );
 SetGadgetAttrs( (Gadget*)layout2,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
 win=(Object*)NewObject( WINDOW_GetClass(),NULL,
                                WA_PubScreen,Main.Scr,
                                WA_Title,msg,
                                WA_Activate,TRUE,
                                WINDOW_Layout,layout,
                                WINDOW_Position,WPOS_CENTERMOUSE,
                                TAG_DONE );
 DoMethod( win,WM_OPEN );
 GetAttr( WINDOW_Window,win,(ULONG*)&iwin );
 ActivateLayoutGadget( (Gadget*)layout,iwin,NULL,(ULONG)strgadg );
 GetAttr( WINDOW_SigMask, win, &windowsignals );
 while( !Leave ){
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Leave=TRUE;
  if( mask & windowsignals ){
   while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_RAWKEY: break;
     case WMHI_GADGETUP:{
      switch( result & RL_GADGETMASK ){
       case 0:
       case 1:{
        rc=TRUE;
        Leave=TRUE;
        break;
       }
       case 2:{
        Leave=TRUE;
       }
 }}}}}}
 if( rc ){
  GetAttr( STRINGA_TextVal,strgadg,(ULONG*)&txt );
  strncpy( buffer,txt,len);
  DoMethod( win,WM_CLOSE );
 }
 DisposeObject( win );
 SetAttrs( Main.Win, WA_BusyPointer, FALSE, TAG_END );

 return( rc );
}
//<

//>"BOOL ColourReq( char *colour,char *title )"
BOOL ColourReq( char *colour,char *title ){
 BOOL end;
 Object *win,*vlayout,*slider[3],*preview,*ok,*cancel,*string[3];
 Window *IntuiWin;
 ULONG selected,windowsignals,mask,code=0,result;
 ULONG Red,Green,Blue;
 ColorMap *ColMap=Main.Scr->ViewPort.ColorMap;
 long Pen;
 BOOL Result = FALSE;

 ColorWheelBase = 0;
 GradientSliderBase = (Library*)1;
 SliderBase     = 0;
 IntegerBase    = 0;
 LabelBase      = 0;

 ColorWheelBase = OpenLibrary("gadgets/colorwheel.gadget", 44 );
// if( !(GradientSliderBase = OpenLibrary("gadgets/gradientslider.gadget", 44))) return(FALSE);
 SliderBase     = OpenLibrary("gadgets/slider.gadget", 44 );
 IntegerBase    = OpenLibrary("gadgets/integer.gadget", 44 );
 LabelBase      = OpenLibrary("images/label.image", 44 );
 if( !(ColorWheelBase && GradientSliderBase && SliderBase && IntegerBase && LabelBase) ){
   CloseLibrary( LabelBase );
   CloseLibrary( IntegerBase );
   CloseLibrary( SliderBase );
   CloseLibrary( ColorWheelBase );
   CloseLibrary( GradientSliderBase );
   return( FALSE );
 }

 sscanf( colour,"%x",&code );
 Red   = (code & 0xFF0000) >> 16;
 Green = (code & 0xFF00)   >> 8;
 Blue  = (code & 0xFF);
 Pen   = FindColor( ColMap,Red << 24,Green << 24,Blue << 24,-1 );

 // alloc vertical layout
 vlayout=(Object*)NewObject(
   LAYOUT_GetClass(),NULL,
   LAYOUT_Orientation,LAYOUT_VERTICAL,
   LAYOUT_DeferLayout,TRUE,
   LAYOUT_SpaceInner,TRUE,
   LAYOUT_SpaceOuter,TRUE,
   // alloc wheel
   /*
    colourwheel=(Object*)NewObject( (IClass*)ColorWheelBase,NULL,
                        GA_ID,1,GA_RelVerify,TRUE, TAG_END );
    SetGadgetAttrs( (Gadget*)hlayout,NULL,NULL,LAYOUT_AddChild,colourwheel,TAG_END );
   // alloc gradientslider
    gradient=(Object*)NewObject( (IClass*)GradientSliderBase,NULL,
                        GA_ID,1,GA_RelVerify,TRUE, GRAD_MaxVal,255,GRAD_SkipVal,1, TAG_END );
    SetGadgetAttrs( (Gadget*)hlayout,NULL,NULL,LAYOUT_AddChild,gradient,TAG_END );
   */
   LAYOUT_AddChild,
     NewObject(
       LAYOUT_GetClass(),NULL,
       LAYOUT_Orientation,LAYOUT_HORIZONTAL,
       LAYOUT_AddChild,
         NewObject(
           LAYOUT_GetClass(),NULL,
           LAYOUT_Orientation,LAYOUT_VERTICAL,
           LAYOUT_AddImage, NewObject(
             LABEL_GetClass(),NULL,
             LABEL_Text,GetCatalogStr(Main.Catalog,MSG_RED,MSG_RED_STR),TAG_END
           ),
           LAYOUT_AddImage, NewObject(
             LABEL_GetClass(),NULL,
             LABEL_Text,GetCatalogStr(Main.Catalog,MSG_GREEN,MSG_GREEN_STR),TAG_END
           ),
           LAYOUT_AddImage, NewObject(
             LABEL_GetClass(),NULL,
             LABEL_Text,GetCatalogStr(Main.Catalog,MSG_BLUE,MSG_BLUE_STR),TAG_END
           ),
           TAG_END
         ),
       LAYOUT_AddChild,
         NewObject(
           LAYOUT_GetClass(),NULL,
           LAYOUT_Orientation,LAYOUT_VERTICAL,
           LAYOUT_AddChild,
             slider[0]=(Object*)NewObject(
               SLIDER_GetClass(),NULL,
               GA_ID,1,GA_RelVerify,TRUE,SLIDER_Orientation,SLIDER_HORIZONTAL,
               SLIDER_Min, 0,SLIDER_Max,255,SLIDER_Level,Red,TAG_END
             ),
           LAYOUT_AddChild,
             slider[1]=(Object*)NewObject(
               SLIDER_GetClass(),NULL,
               GA_ID,3,GA_RelVerify,TRUE,SLIDER_Orientation,SLIDER_HORIZONTAL,
               SLIDER_Min, 0,SLIDER_Max,255,SLIDER_Level,Green,TAG_END
             ),
           LAYOUT_AddChild,
             slider[2]=(Object*)NewObject(
               SLIDER_GetClass(),NULL,
               GA_ID,5,GA_RelVerify,TRUE,SLIDER_Orientation,SLIDER_HORIZONTAL,
               SLIDER_Min,0,SLIDER_Max,255,SLIDER_Level,Blue,TAG_END
             ),
           TAG_END
         ),
       CHILD_MinWidth,150,
       LAYOUT_AddChild,
         NewObject(
           LAYOUT_GetClass(),NULL,
           LAYOUT_Orientation,LAYOUT_VERTICAL,
           LAYOUT_AddChild,
             string[0]=(Object*)NewObject(
               INTEGER_GetClass(),NULL,INTEGER_Minimum,0,INTEGER_Maximum,255,
               GA_ID,2,GA_RelVerify,TRUE,INTEGER_Number,Red,TAG_END
             ),
           LAYOUT_AddChild,
             string[1]=(Object*)NewObject(
               INTEGER_GetClass(),NULL,INTEGER_Minimum,0,INTEGER_Maximum,255,
               GA_ID,4,GA_RelVerify,TRUE,INTEGER_Number,Green,TAG_END
             ),
           LAYOUT_AddChild,
             string[2]=(Object*)NewObject(
               INTEGER_GetClass(),NULL,INTEGER_Minimum,0,INTEGER_Maximum,255,
               GA_ID,6,GA_RelVerify,TRUE,INTEGER_Number,Blue,TAG_END
             ),
           TAG_END
         ),
       TAG_END
     ),
   LAYOUT_AddChild,
     NewObject(
       LAYOUT_GetClass(),NULL,
       LAYOUT_Orientation,LAYOUT_HORIZONTAL,
       LAYOUT_AddChild,
         ok=(Object*)NewObject(
           BUTTON_GetClass(),NULL,
           GA_ID,7,GA_RelVerify,TRUE,
           GA_Text,GetCatalogStr(Main.Catalog,TXT_OK,TXT_OK_STR),TAG_END
         ),
       CHILD_WeightedWidth,0,
       LAYOUT_AddChild,
         preview=(Object*)NewObject(
           BUTTON_GetClass(),NULL,
//           GA_Text,title,
           BUTTON_BackgroundPen,Pen,
           BUTTON_BevelStyle,BVS_FIELD,TAG_END
         ),
       CHILD_WeightedWidth,100,
       LAYOUT_AddChild,
         cancel=(Object*)NewObject(
           BUTTON_GetClass(),NULL,
           GA_ID,8,GA_RelVerify,TRUE,
           GA_Text,GetCatalogStr(Main.Catalog,TXT_CANCEL,TXT_CANCEL_STR),TAG_END
         ),
       CHILD_WeightedWidth,0,
       TAG_END
     ),
   TAG_END
 );


 // open window and wait till closing
 win=(Object*)NewObject(
   WINDOW_GetClass(),NULL,
   WINDOW_Position,WPOS_CENTERMOUSE,
   WINDOW_Layout,vlayout,
   WA_PubScreen,Main.Scr,
   WA_IDCMP,IDCMP_GADGETUP,
   WA_Activate,TRUE,
   WA_Title,title, //GetCatalogStr( Main.Catalog,TXT_CHOOSECOLOUR,TXT_CHOOSECOLOUR_STR ),
   WA_DragBar,TRUE,
   WA_DepthGadget,TRUE,
   WA_RMBTrap,TRUE,
   TAG_END
 );

 DoMethod( win,WM_OPEN );
 end=FALSE;
 GetAttr( WINDOW_Window, win, (ULONG*)&IntuiWin );
 GetAttr( WINDOW_SigMask, win, &windowsignals );
 do{
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) end=TRUE;
  while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
   switch( result & WMHI_CLASSMASK ){
    case WMHI_GADGETUP:{
     selected=(result & RL_GADGETMASK);
     switch( selected ){
       case 1: case 3: case 5:{
         GetAttr( SLIDER_Level, slider[0],&Red );
         GetAttr( SLIDER_Level, slider[1],&Green );
         GetAttr( SLIDER_Level, slider[2],&Blue );
         SetGadgetAttrs( (Gadget*)string[0],IntuiWin,NULL,INTEGER_Number,Red,TAG_END );
         SetGadgetAttrs( (Gadget*)string[1],IntuiWin,NULL,INTEGER_Number,Green,TAG_END );
         SetGadgetAttrs( (Gadget*)string[2],IntuiWin,NULL,INTEGER_Number,Blue,TAG_END );
         Pen = FindColor( ColMap,Red << 24,Green << 24,Blue << 24,-1 );
         SetGadgetAttrs( (Gadget*)preview,IntuiWin,NULL,BUTTON_BackgroundPen,Pen,TAG_END );
         break;
       }
       case 2: case 4: case 6:{
         GetAttr( INTEGER_Number, string[0],&Red );
         GetAttr( INTEGER_Number, string[1],&Green );
         GetAttr( INTEGER_Number, string[2],&Blue );
         SetGadgetAttrs( (Gadget*)slider[0],IntuiWin,NULL,SLIDER_Level,Red,TAG_END );
         SetGadgetAttrs( (Gadget*)slider[1],IntuiWin,NULL,SLIDER_Level,Green,TAG_END );
         SetGadgetAttrs( (Gadget*)slider[2],IntuiWin,NULL,SLIDER_Level,Blue,TAG_END );
         Pen = FindColor( ColMap,Red << 24,Green << 24,Blue << 24,-1 );
         SetGadgetAttrs( (Gadget*)preview,IntuiWin,NULL,BUTTON_BackgroundPen,Pen,TAG_END );
         break;
       }
       case 8: end = TRUE; break;
       case 7:{
         sprintf( colour,"%.6X",(Red<<16) + (Green<<8) + Blue );
         end = TRUE;
         Result = TRUE;
         break;
       }
     }
     break;
    }
   }
  }
 }while( !end );
 DoMethod( win,WM_CLOSE );
 DisposeObject( win );
 CloseLibrary( LabelBase );
 CloseLibrary( IntegerBase );
 CloseLibrary( SliderBase );
 CloseLibrary( ColorWheelBase );
// CloseLibrary( GradientSliderBase );
 return( Result );
}
//<

