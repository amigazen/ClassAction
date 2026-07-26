#include "ca_begin.h"
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
               & Salim Gasmi

        File: capopt.cpp

        Aim:  Handle Options

****************************************/

#include <proto/dos.h>
#include "capopt.h"
#include "capreq.h"
#include <classes/window.h>
#include <clib/alib_protos.h>
#include <gadgets/button.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <libraries/resource.h>
#include <proto/button.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <proto/listbrowser.h>
#include <proto/window.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TOptionList OptionList;

//>"void TOptionList::Init( Object *window,Object *listbrowser )"
void TOptionList::Init( Object *window,Object *listbrowser ){
  win=window;
  lb=listbrowser;
}
//<

//>"void TOptionList::Add( char *title,char type, char *tooltype, ULONG value )"
void TOptionList::Add( char *title,char type, char *tooltype, ULONG value ){
 List *list;
 lbn *optionnode;

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&list );
 SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
 optionnode=(lbn*)AllocListBrowserNode( 2,LBNA_NodeSize,sizeof(lbn),
                                LBNA_Generation,2,
                                LBNA_Flags,LBFLG_HIDDEN,
                                TAG_END );
 if( optionnode ){
  optionnode->type=type;
  strcpy( optionnode->title,title );
  strcpy( optionnode->tooltype,tooltype );
  if( type=='b' ){
   optionnode->value[0]=0;
   SetListBrowserNodeAttrs( (Node*)optionnode,
                         LBNA_Column,0,
                         LBNCA_Text,optionnode->title,
                         LBNA_CheckBox,TRUE,
                         LBNA_Checked,value,
                         LBNA_Column,1,
                         LBNCA_Text,"",
                         TAG_END );
  // chooser:
  }else if( type=='o' ){
   TChooser *sl=(TChooser*)value;
   optionnode->ptr=sl;
   strcpy( optionnode->value,sl->Items(sl->Selected) );
   SetListBrowserNodeAttrs( (Node*)optionnode,
                         LBNA_Column,0,
                         LBNCA_Text,optionnode->title,
                         LBNA_Column,1,
                         LBNCA_Text,optionnode->value,
                         TAG_END );
  // integer:
  }else if( type=='i' ){
   sprintf( optionnode->value,"%d",(int)value );
   SetListBrowserNodeAttrs( (Node*)optionnode,
                         LBNA_Column,0,
                         LBNCA_Text,optionnode->title,
                         LBNA_Column,1,
                         LBNCA_Text,optionnode->value,
                         TAG_END );
  }else{
   strcpy( optionnode->value,(char*)value );
   SetListBrowserNodeAttrs( (Node*)optionnode,
                         LBNA_Column,0,
                         LBNCA_Text,optionnode->title,
                         LBNA_Column,1,
                         LBNCA_Text,optionnode->value,
                         TAG_END );
  }
  AddTail( list,(Node*)optionnode );
  SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,list,TAG_END );
 }
}
//<

//>"void TOptionList::AddGroup( char *name,char *title )"
void TOptionList::AddGroup( char *name,char *title ){
 List *list;
 lbn *optionnode;

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&list );
 SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,list,TAG_END );
 optionnode=(lbn*)AllocListBrowserNode( 2,LBNA_NodeSize,sizeof(lbn),
                                LBNA_Generation,1,
                                LBNA_Flags,LBFLG_HASCHILDREN,
                                TAG_END );
 if( optionnode ){
  optionnode->type='g';
  strcpy( optionnode->title,title );
  optionnode->tooltype[0]=0;
  strcpy( optionnode->value,name );
  SetListBrowserNodeAttrs( (Node*)optionnode,
                         LBNA_Column,0,
                         LBNCA_Text,optionnode->title,
                         LBNA_Column,1,
                         LBNCA_Text,"",TAG_END );
  AddTail( list,(Node*)optionnode );
  SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,list,TAG_END );
 }
}
//<

//>"void TOptionList::HandleInput()"
void TOptionList::HandleInput(){
 List *list;
 lbn *node;
 ULONG event,nn,code;
 int i;
 char name[256],path[256],*ptr;
 BOOL show=FALSE;
 Window *intuiwin;
 GetAttr( WINDOW_Window,win,(ULONG*)&intuiwin );

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&list );
 GetAttr( LISTBROWSER_Selected,lb,&nn );
 GetAttr( LISTBROWSER_SelectedNode,lb,(ULONG*)&node );
 GetAttr( LISTBROWSER_RelEvent,lb,&event );
 if( node!=NULL ){
  switch( event ){
   case LBRE_NORMAL:{
    if( node->type=='g' ){
     SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
     GetListBrowserNodeAttrs( (Node*)node,LBNA_Flags,&code,TAG_END );
     if( code & LBFLG_SHOWCHILDREN ) HideListBrowserNodeChildren( (Node*)node );
     else ShowListBrowserNodeChildren( (Node*)node,1 );
     SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
    }else if( node->type=='b' ){
     SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
     GetListBrowserNodeAttrs( (Node*)node,LBNA_Checked,&code,TAG_END );
     SetListBrowserNodeAttrs( (Node*)node,LBNA_Checked,!code,TAG_END );
     SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
    }else if( node->type=='c' ){
     if( ColourReq(node->value,node->title) ) show=TRUE;
    }else if( node->type=='f' ){
     if( Freq(node->value,0,node->title) ){
      SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
      SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
      SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
     }
    }else if( node->type=='d' ){
     if( Freq(node->value,1,node->title) ){
      SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
      SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
      SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
     }
    }else if( node->type=='p' ){
     strcpy( name,node->value );
     ptr=strchr( name,',' );
     if( ptr ){ *ptr=0; strcpy( path,ptr+1 ); }
     else strcpy( path,name );
     if( Freq(path,1,node->title) ){
      if( path[strlen(path)-1]=='/' ) path[strlen(path)-1]=0;
      strcpy( name,FilePart(path) );
      if( GetText(name,256,node->title) ){
       sprintf( node->value,"%s,%s",name,path );
       SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
       SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
       SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
     }}
    }else if( node->type=='m' ){
     ScreenRequester(node->value,node->title);
     SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
     SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
     SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
    }else if( node->type=='o' ){
     TChooser *sl=(TChooser*)node->ptr;
     if( ChooserPopUp( sl ) ){
      strcpy( node->value,sl->Items(sl->Selected) );
      show=TRUE;
     }
    }else if( node->type=='z' ){
     if( FontReq(node->value,node->title) ){
      SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
      SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
      SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
     }
    }else if( node->type!='b' ){
     if( GetText( node->value,256,node->title ) ){
      if( node->type=='i' ){
       i=atoi( node->value );
       if( i<-1 ) i=-1;
       if( i>9999 ) i=9999;
       sprintf( node->value,"%d",i );
      }
      SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
      SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
      SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
     }
    }
    break;
   }
  }
  if( show ){
    SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,NULL,TAG_END );
    SetListBrowserNodeAttrs( (Node*)node,LBNA_Column,1,LBNCA_Text,node->value,TAG_END );
    SetGadgetAttrs( (Gadget*)lb,intuiwin,NULL,LISTBROWSER_Labels,list,TAG_END );
  }
 }
}
//<

//>"void TOptionList::WriteAll( TIniFile *IniFile )"
void TOptionList::WriteAll( TIniFile *IniFile ){
 ULONG checked;
 lbn *currentnode;
 char *group="";

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&currentnode );
 if( !currentnode ) return;
 currentnode=(lbn*)(currentnode->ln_Succ);
 if( !currentnode ) return;

 while( currentnode->ln_Succ ){
  // do type specific stuff:
  switch( currentnode->type ){
   case 'b':{
    GetListBrowserNodeAttrs( (Node*)currentnode,LBNA_Checked,&checked,TAG_END );
    IniFile->WriteBool( group,currentnode->tooltype,checked );
    break;
   }
   case 'g':{ // group reached
    group=currentnode->value;
    break;
   }
   default:{
    IniFile->WriteString( group,currentnode->tooltype,currentnode->value );
    break;
   }
  }
  currentnode=(lbn*)(currentnode->ln_Succ);
 }
}
//<

//>"void TOptionList::Reset()"
void TOptionList::Reset(){
 List *list;
 Window *intuiwin;
 GetAttr( WINDOW_Window,win,(ULONG*)&intuiwin );
 if( intuiwin ){
  GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&list );
  if( list ){
   FreeListBrowserList( list );
   SetGadgetAttrs( (Gadget*)lb,NULL,NULL,LISTBROWSER_Labels,list,TAG_END );
 }}
}
//<

//>"BOOL TOptionList::ChooserPopUp( TChooser *Chooser )"
/****************************************/
/*                                      */
/* Function: ChooserPopUp               */
/*                                      */
/* Aim:      Display chooser list       */
/*                                      */
/* Input:    StringList = list to act on*/
/*                                      */
/* Output:   ptr to selected string     */
/*           or NULL                    */
/*                                      */
/****************************************/
BOOL TOptionList::ChooserPopUp( TChooser *Chooser ){
 Gadget *gadgarray[50];
 ULONG windowsignals, mask,result,code;
 BOOL Leave=FALSE;
 Window *win;
 ULONG a=-1;
 int n=0,count=0,selgadget=-1;

 Object *Layout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                        LAYOUT_BevelStyle,BVS_THIN,
                        LAYOUT_Orientation,LAYOUT_ORIENT_VERT,
                        TAG_END );
 Object *Win=(Object*)NewObject( WINDOW_GetClass(),NULL,
                        WINDOW_Layout,Layout,
                        WINDOW_Position,WPOS_CENTERMOUSE,
                        WA_Activate,TRUE,
                        WA_IDCMP,IDCMP_GADGETUP
                                |IDCMP_INACTIVEWINDOW
                                |IDCMP_MOUSEBUTTONS|
                                IDCMP_GADGETHELP,
                        WA_Borderless,TRUE,
                        WINDOW_GadgetHelp,TRUE,
                        WA_RMBTrap,TRUE,
                        TAG_END );

 Object *Gadg;
 for( int i=0;i<Chooser->ItemCount();i++ ){
  Gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                                GA_Text,Chooser->Items(i),
                                GA_ID,i+1,              // id = 1,...,n
                                GA_RelVerify,TRUE,
                                GA_GadgetHelp,TRUE,
                                BUTTON_BevelStyle,BVS_NONE,
                                BUTTON_Transparent,TRUE,
                                TAG_END );
  SetGadgetAttrs( (Gadget*)Layout,NULL, NULL,LAYOUT_AddChild, Gadg, TAG_DONE );
  gadgarray[i]=(Gadget*)Gadg;
 }

 DoMethod( Win,WM_OPEN );

 GetAttr( WINDOW_Window,Win,(ULONG*)&win );
 GetAttr( WINDOW_SigMask, Win, &windowsignals );
 while( !Leave ){
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Leave=TRUE;
  if( mask & windowsignals ){
   while( (result = DoMethod( Win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_INACTIVE:
     case WMHI_MOUSEBUTTONS:{
      Leave=TRUE;
      break;
     }
     case WMHI_GADGETHELP:{
      if( selgadget!=-1 ) SetGadgetAttrs( gadgarray[selgadget-1],win,NULL,BUTTON_TextPen, 1L,TAG_END );
      selgadget=result & RL_GADGETMASK;
      if( (selgadget==0)||(selgadget==RL_GADGETMASK) ) selgadget=-1;
      else{
       SetGadgetAttrs( gadgarray[selgadget-1],win,NULL,BUTTON_TextPen, 2L,TAG_END );
      }
      break;
     }
     case WMHI_GADGETUP:{
      a=(result & RL_GADGETMASK)-1;     // id = 1,...,n -> a = 0,...,n-1
      Leave=TRUE;
     }
     break;
 }}}}
 DoMethod( Win,WM_CLOSE );
 DisposeObject( Win );
 if( a==-1 ) return( FALSE );
 else{
  Chooser->Selected=a;
  return( TRUE );
 }
}
//<

