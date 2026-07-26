#include "ca_begin.h"

/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
        (c) by 1994 Salim Gasmi

        File : caplist.c

        Goal : Handle Exec lists

        Ver  : 3.8

****************************************/

#include "caplist.h"
#include <clib/alib_protos.h>
#include <gadgets/listbrowser.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <string.h>

#include "captypes.h"
#include "classactionprefs.h"


//>"int AddList(struct List *li,char *name,int pri)"
/*
int AddList(struct List *li,char *name,int pri){
 struct Node *nod;

 nod=(Node*)AllocMem(sizeof(Node),MEMF_CLEAR|MEMF_PUBLIC);
 if(nod==NULL) return(-1);

 nod->ln_Name=name;
 nod->ln_Pri=pri;
 nod->ln_Type = NT_UNKNOWN;
 AddTail(li,nod);
 return(0);
}
*/
//<

//>"void FreeList(struct List *li)"
void FreeList(struct List *li){
 struct Node *nod;
 while( nod=RemHead(li) ) FreeMem(nod,sizeof(struct Node));
}
//<

//>"void FreeClassList(void)"
void FreeClassList(void){
 struct TClass *cla;//,*ctmp;
 struct Action *act;//,*atmp;
 List *lbl;

 /* freeing listbrowser list (skip if InitAll failed before GUI) */
 if( Main.Gadgets ){
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lbl );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                  LISTBROWSER_Labels, NULL, TAG_DONE );
  FreeListBrowserList( lbl );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                  LISTBROWSER_Labels, lbl, TAG_DONE );
 }

 /* freeing generic list */
/*
 if( Main.MemList.head&&Main.MemList.head->next&&Main.MemList.head->next->next&&
     Main.MemList.head->next->next->next ){
  cla=Main.MemList.head->next->next->next;
  Main.MemList.head->next=cla;  // on the top is still the unknown class

  act=Main.Generic.action;
  while(act!=NULL){
   atmp=act->next;
   FreeVec(act);
   act=atmp;
  }
 }

 act=Main.Directory.action;
 while(act!=NULL){
  atmp=act->next;
  FreeVec(act);
  act=atmp;
 }

 act=Main.Directory.action;
 while(act!=NULL){
  atmp=act->next;
  FreeVec(act);
  act=atmp;
 }

  */
 /* Freeing Normal List */
 while( cla = (TClass*)RemHead(&Main.Classes) ){
   while( act = (Action*)RemHead(&cla->Actions) ) FreeVec( act );
   FreeVec( cla );
 }
 Main.ClassCount = 0;
}
//<

//>"void AddGenDirList()"
/*
void AddGenDirList(){
 struct Class *cla;

 // Adding Generic class to list
 cla=Main.MemList.head->next;
 Main.MemList.head->next=&Main.Generic;
 Main.Generic.next=cla;
 if( Main.MemList.tail==Main.MemList.head) Main.MemList.tail=&Main.Generic;
 Main.MemList.size++;

 // Adding Directory class to list
 cla=Main.MemList.head->next->next;
 Main.MemList.head->next->next=&Main.Directory;
 Main.Directory.next=cla;
 Main.MemList.size++;
}
*/
//<

//>"int SortList()"
int SortList(){
 struct TClass *cla,*buf[200];
 int i,n;
 BOOL sorted=FALSE;

 n=Main.ClassCount-4;
 if( n == 0 ) return(0);

 cla = (TClass*)Main.Classes.lh_Head->ln_Succ->ln_Succ->ln_Succ->ln_Succ;
 for( i=0;i<n;i++ ){
   buf[i] = cla;
   Remove( (Node*)cla );
   cla = (TClass*)cla->ln_Succ;
 }

 while( ! sorted ){
  sorted = TRUE;
  for( i=1;i<n-1;i++ ) if( strcmp(buf[i]->Name,buf[i+1]->Name)>0 ){
    sorted   = FALSE;
    cla      = buf[i];
    buf[i]   = buf[i+1];
    buf[i+1] = cla;
  }
 }

 for(i=0;i<n;i++){
   AddTail( &Main.Classes,(Node*)buf[i] );
 }

 return(0);
}
//<

//>"TClass *CreateClass( List *li,char *name )"
TClass *CreateClass( List *li,char *name ){
  TClass *cla = (TClass*)AllocVec(sizeof(TClass),MEMF_CLEAR);
  if( cla ){
    strncpy( cla->Name,name,32 );
    cla->ImageFile[0]=0;
    NewList( &cla->Actions );
    cla->ActionCount = 0;
    AddTail( li,(Node*)cla );
    Main.ClassCount++;
  }
  return( cla );
}
//<

//>"Action *CreateAction( TClass *cla,char *name )"
Action *CreateAction( TClass *cla,char *name ){
  Action *act = (Action*)AllocVec(sizeof(Action),MEMF_CLEAR);
  strncpy( act->Name,name,32 );
  AddTail( &cla->Actions,(Node*)act );
  cla->ActionCount++;
  return( act );
}
//<

