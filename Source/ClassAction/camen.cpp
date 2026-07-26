/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner

        File: camen.cpp

        Aim:  Implement the menu class

        Ver:  4.2

****************************************/
#include "caincludes.h"
#include "caextern.h"

#include "camen.h"

//>"TMenu::TMenu()"
TMenu::TMenu(){
 MenuData=(NewMenu*)AllocVec( MAX_MENUS*sizeof(NewMenu),MEMF_CLEAR );
 MenuIds=(ULONG*)AllocVec( MAX_MENUS*sizeof(ULONG),MEMF_CLEAR );
 Current=0;
 CurMenu=CurItem=CurSub=-1;
}
//<

//>"TMenu::~TMenu()"
TMenu::~TMenu(){
 FreeVec( MenuData ) ;
 FreeVec( MenuIds );
}
//<

//>"void TMenu::Clear()"
void TMenu::Clear(){
 Current=0;
 CurMenu=CurItem=CurSub=-1;
}
//<

//>"void TMenu::AddItem( char *title,char *key,UBYTE type,APTR ptr )"
void TMenu::AddItem( char *title,char *key,UBYTE type,APTR ptr ){
 if( type==NM_TITLE ){ CurMenu++; CurItem=CurSub=-1; }
 else if( type==NM_ITEM ){ CurItem++; CurSub=-1; }
 else if( type==NM_SUB ) CurSub++;
 MenuData[Current].nm_Type=type;
 MenuData[Current].nm_Label=title;
 MenuData[Current].nm_CommKey=key;
 if( type==NM_SUB ){            // used only for CheckItems
  MenuData[Current].nm_Flags=CHECKIT|MENUTOGGLE;
  if( *((BOOL*)ptr)==TRUE ){    // test if already set
   MenuData[Current].nm_Flags|=CHECKED;
  }
 }else MenuData[Current].nm_Flags=0;
 MenuData[Current].nm_MutualExclude=0;
 MenuData[Current].nm_UserData=ptr;
 switch( type ){
  case NM_TITLE: MenuIds[Current]=FULLMENUNUM(CurMenu,NOITEM,NOSUB);   break;
  case NM_ITEM:  MenuIds[Current]=FULLMENUNUM(CurMenu,CurItem,NOSUB);  break;
  case NM_SUB:   MenuIds[Current]=FULLMENUNUM(CurMenu,CurItem,CurSub); break;
 }
 Current++;
}
//<

//>"void TMenu::Handle( ULONG message,Menu *menu )"
void TMenu::Handle( ULONG message,Menu *menu ){
 int i,j;

 for( i=0;i<MAX_MENUS;i++ ){
  if( MenuData[i].nm_Type==NM_END ) return;
  if( message==MenuIds[i] ){
   switch( MenuData[i].nm_Type ){
    // item selected: execute the function
    case NM_ITEM:{
     Runner( (Action*)MenuData[i].nm_UserData );
     break;
    }
    // CheckItem selected: change the option
    case NM_SUB:{
     // first get the flags:
     Menu *men=menu;
     MenuItem *itm,*sub;
     for( j=0;j<MENUNUM(message);j++ ) men=men->NextMenu;
     itm=men->FirstItem;
     for( j=0;j<ITEMNUM(message);j++ ) itm=itm->NextItem;
     sub=itm->SubItem;
     for( j=0;j<SUBNUM(message);j++ ) sub=sub->NextItem;
     // then decide what has happened
     if( sub->Flags & CHECKED ) *((BOOL*)(MenuData[i].nm_UserData))=TRUE;
     else *((BOOL*)(MenuData[i].nm_UserData))=FALSE;
     break;
    }
}}}}
//<

//>"Menu *TMenu::Render( APTR visinfo )"
Menu *TMenu::Render( APTR visinfo ){
 MenuData[Current].nm_Type=NM_END;

 IMenu=CreateMenusA( MenuData,NULL );
 LayoutMenus( IMenu,visinfo,GTMN_NewLookMenus,TRUE,TAG_DONE );
 return( IMenu );
}
//<

//>"void TMenu::Update()"
void TMenu::Update(){
 int i=0;
 MenuItem *item,*sub;

 Menu *menu=Main.IMenu;
 while( menu ){
  item=menu->FirstItem;
  i++;
  while( item ){
   sub=item->SubItem;
   i++;
   while( sub ){
    // update the checkmark
    if( *((BOOL*)(MenuData[i].nm_UserData))==TRUE ) sub->Flags|=CHECKED;
    else sub->Flags&=~CHECKED;
    sub=sub->NextItem;
    i++;
   }
   item=item->NextItem;
  }
  menu=menu->NextMenu;
 }
}
//<

//>"TPopMenu::TPopMenu()"
TPopMenu::TPopMenu(){
 SelectedItem = 0;
 OpenSubMenu = NULL;
 NewList( &Items );
 ItemCount = 0;
 MenuWindow = NULL;
}
//<

//>"TPopMenu::~TPopMenu()"
TPopMenu::~TPopMenu(){
 Node *node;
 while( !IsListEmpty(&Items) ){
  node = RemHead( &Items );
  FreeVec( node );
 }
}
//<

//>"void TPopMenu::AddItem( char *title,UBYTE type,APTR ptr )"
void TPopMenu::AddItem( char *title,UBYTE type,APTR ptr ){
 TPopNode *node;
 node = (TPopNode*)AllocVec( sizeof(TPopNode),MEMF_CLEAR );
 strncpy( node->Title,title,99 );
 node->Action  = (void(*)())ptr;
 node->SubMenu = (TPopMenu*)ptr;
 node->Type    = type;
 AddTail( &Items,(Node*)node );
}
//<

//>"int TPopMenu::Show()"
// Result > -1 : Item selected, all done
// Result = -1 : Window closed
// Result = -2 : Whole menu closed
int TPopMenu::Show(){
 ULONG windowsignals, mask,result,code,selgadget=-1;
 BOOL Leave=FALSE,ClickediClick=FALSE;
 int count=0,n=0,Result=-3;
 Gadget *gadgarray[40];
 void (*action)() = NULL;
 TPopNode *node;

 Object *Layout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                        LAYOUT_BevelStyle,BVS_BUTTON,
                        LAYOUT_FillPen,0,
                        LAYOUT_Orientation,LAYOUT_ORIENT_VERT,
                        LAYOUT_EvenSize,FALSE,
                        TAG_END );
 // prepare window:
 WindowObject=(Object*)NewObject( WINDOW_GetClass(),NULL,
                        WINDOW_Layout,Layout,
                        WINDOW_Position,WPOS_CENTERMOUSE,
                        WA_Activate,TRUE,
                        WA_IDCMP,IDCMP_GADGETUP
                                |IDCMP_INACTIVEWINDOW
                                |IDCMP_MOUSEMOVE
                                |IDCMP_MOUSEBUTTONS, // only for right mb
                        WA_RMBTrap,TRUE,
                        WA_Borderless,TRUE,
                        TAG_END );
 int i=0;
 Object *Gadg;
 node = (TPopNode*)Items.lh_Head;
 while( node->ln_Succ ){
  Gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                                GA_Text,node->Title,
                                GA_ID,i+1,              // id = 1,...,n
                                GA_RelVerify,TRUE,
                                BUTTON_BevelStyle,BVS_NONE,
                                TAG_END );
  SetGadgetAttrs( (Gadget*)Layout,NULL, NULL,LAYOUT_AddChild, Gadg, TAG_DONE );
  gadgarray[i]=(Gadget*)Gadg;
  i++;
  node = (TPopNode*)node->ln_Succ;
 }
 gadgarray[i]=NULL;
 
 DoMethod( WindowObject,WM_OPEN );

 GetAttr( WINDOW_Window,WindowObject,(ULONG*)&MenuWindow );
 GetAttr( WINDOW_SigMask, WindowObject, &windowsignals );
 while( Result is -3 ){
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Leave=TRUE;
  if( mask & windowsignals ){
   while( (result = DoMethod( WindowObject, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_INACTIVE:
     case WMHI_MOUSEBUTTONS:{
      Result = -2;
      break;
     }
/*
     case WMHI_INTUITICK:{
      if( Main.PopupWaitTime>0 ){ // 10 ticks per second
       if( ++count>10*Main.PopupWaitTime ) =TRUE;
      }
      break;
     }
*/
     case WMHI_GADGETUP:{
      selgadget=(result & RL_GADGETMASK)-1;     // id = 1,...,n -> selgadget = 0,...,n-1
      node = (TPopNode*)Items.lh_Head;
      for( i=0;i<selgadget-1;i++ ) node = (TPopNode*)node->ln_Succ;
      if( node->Type is POP_MENU ){
       OpenSubMenu = node->SubMenu;
       Result = OpenSubMenu->Show();
       if( Result is -1 ) Result = -3; // do not close, if only window closed
       OpenSubMenu = NULL;
      }else{
       Result = selgadget;
       if( node->Action ) action = node->Action;
      }
      break;
     }
     case WMHI_MOUSEMOVE:{
      for( int i=n-1;i>-1;i-- ){
       if( (MenuWindow->MouseX > gadgarray[i]->LeftEdge)
       and (MenuWindow->MouseX < gadgarray[i]->LeftEdge+gadgarray[i]->Width)
       and (MenuWindow->MouseY > gadgarray[i]->TopEdge)
       and (MenuWindow->MouseY < gadgarray[i]->TopEdge+gadgarray[i]->Height) ){
        if( selgadget isnot i ){
         if( selgadget isnot -1 ) SetGadgetAttrs( gadgarray[selgadget],MenuWindow,NULL,BUTTON_BevelStyle,BVS_NONE,GA_Selected,FALSE,TAG_END );
         selgadget=i;
         SetGadgetAttrs( gadgarray[i],MenuWindow,NULL,BUTTON_BevelStyle,BVS_THIN,GA_Selected,TRUE,TAG_END );
        }
        break;
       }
       if( i is 0 ) Result = -1;
 }}}}}}
 DoMethod( WindowObject,WM_CLOSE );
 DisposeObject( WindowObject );
 switch( Result ){
  case -2 : break;
  case -1 : break;
  default : if( action ) action(); break;
 }
 return( Result );
}
//<
