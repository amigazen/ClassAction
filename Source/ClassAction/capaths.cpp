/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner & Salim Gasmi

        File: capaths.cpp

****************************************/

#include <clib/alib_protos.h>
#include <gadgets/button.h>
#include <gadgets/layout.h>
#include <intuition/classusr.h>
#include <intuition/intuition.h>
#include <proto/button.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <string.h>

#include "caglobal.h"
#include "classaction.h"

#include "capaths.h"

TTabNode *CurrentTab[2];         // the selected tabs

//>"Gadget *MakeNewTab( Object *layout, char *path, UBYTE num )"
Gadget *MakeNewTab( Object *layout, char *path, UBYTE num ){
 Gadget *Gadg;

 Gadg=(Gadget*)NewObject( BUTTON_GetClass(),NULL,
                                GA_Text,path,
                                GA_ID,num,              // id = 1,...,n
                                GA_RelVerify,TRUE,
                                GA_GadgetHelp,TRUE,
                                GA_Underscore,NULL,     // there may be _ in the path
                                BUTTON_PushButton,TRUE,
//                                BUTTON_BevelStyle,BVS_NONE,
                                BUTTON_BevelStyle,BVS_THIN,
                                BUTTON_Transparent,Main->TabButtonsTransparent,
                                TAG_END );
  SetGadgetAttrs( (Gadget*)layout,NULL, NULL,LAYOUT_AddChild, Gadg, TAG_DONE );
  return( Gadg );
}
//<

//>"void InitPaths()"
void InitPaths(){
 ULONG i;
 TTabNode *tnd;

 for( i=0;i<Main->MaxTabs;i++ ){
  tnd=(TTabNode*)AllocVec( sizeof(TTabNode),MEMF_CLEAR );
  strcpy( tnd->Path,"" );
  tnd->SortMode=SM_NAME;
  tnd->SortReverse=FALSE;
  tnd->LhaActive=FALSE;
  AddTail( &Main->Tabs[0],(Node*)tnd );

  if( Main->MaxTabs > 1 ) tnd->Gadget = MakeNewTab( Main->Gadgets[LAYOUT_TAB_L],tnd->Path,i+100 );
  else tnd->Gadget = NULL;

  tnd=(TTabNode*)AllocVec( sizeof(TTabNode),MEMF_CLEAR );
  strcpy( tnd->Path,"" );
  tnd->SortMode=SM_NAME;
  tnd->SortReverse=FALSE;
  tnd->LhaActive=FALSE;
  AddTail( &Main->Tabs[1],(Node*)tnd );

  if( Main->MaxTabs > 1 ) tnd->Gadget = MakeNewTab( Main->Gadgets[LAYOUT_TAB_R],tnd->Path,i+125 );
  else tnd->Gadget = NULL;
 }
 CurrentTab[0]=(TTabNode*)Main->Tabs[0].lh_Head;
 CurrentTab[1]=(TTabNode*)Main->Tabs[1].lh_Head;
 Main->CurrentList=1;
 Main->PathCount=0;
}
//<

//>"BOOL IsPathNodeValid( TPathNode *pn0 )"
BOOL IsPathNodeValid( TPathNode *pn0 ){
 TPathNode *pn=(TPathNode*)( Main->Paths.lh_Head );
 while( pn->ln_Succ ){
  if( pn is pn0 ) return( TRUE );       // found, return ok
  pn=(TPathNode*)pn->ln_Succ;
 }
 return( FALSE );                       // not found, return notok
}
//<

//>"void RemovePathNode( TPathNode *pn )"
void RemovePathNode( TPathNode *pn ){
 if( pn==NULL ){
  pn=(TPathNode*)( Main->Paths.lh_Head );
  while( pn->ln_Succ ){ // we must check, if the path is still used:
   if( (strcmp(pn->Path,CurrentTab[0]->Path) isnot 0) and
       (strcmp(pn->Path,CurrentTab[1]->Path) isnot 0) ){
    break;
   }else pn=(TPathNode*)pn->ln_Succ;
  }
  if( !pn->ln_Succ ) return; // empty list or all paths needed
 }
 if( pn->NotifyOn ) EndNotify( &pn->NotiReq ); // only if necessary !!
 Remove( (Node*)pn );
 FileNode *fn;
 while( !IsListEmpty(&pn->FList) ){
  fn=(FileNode*)RemHead(&pn->FList);
  FreeVec(fn);
 }
 FreeVec(pn);
 Main->PathCount--;
}
//<

//>"void ClearPaths()"
void ClearPaths(){
 TTabNode *tnd;
 while( !IsListEmpty(&Main->Tabs[0]) ){
  tnd=(TTabNode*)RemHead(&Main->Tabs[0]);
  FreeVec(tnd);
 }
 while( !IsListEmpty(&Main->Tabs[1]) ){
  tnd=(TTabNode*)RemHead(&Main->Tabs[1]);
  FreeVec(tnd);
 }
 while( !IsListEmpty(&Main->Paths) ){
  RemovePathNode( (TPathNode*)Main->Paths.lh_Head );
 }
}
//<

//>"void ClearUserPaths()"
void ClearUserPaths(){
 TUserPathNode *pnd;
 while( !IsListEmpty(&Main->UserPaths) ){
  pnd=(TUserPathNode*)RemHead(&Main->UserPaths);
  FreeVec(pnd);
 }
}
//<

