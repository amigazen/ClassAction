/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File: caflist.c

        Aim:  implement class FileList

        Ver:  4.1

****************************************/

#include <clib/intuition_protos.h>
#include <clib/listbrowser_protos.h>
#include <gadgets/listbrowser.h>

#include "caextern.h"

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classaction.h"
#include "classactioncd.h"

#include "caflist.h"
/*
//>"FileList::FileList( int number )"
FileList::FileList( int number ){
 Node *fn;
 ULONG isselected;

 // get the list:
 if( number==0 ){
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LST_DIR_L], (ULONG*)&List );
 }else if( number==1 ){
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LST_DIR_R], (ULONG*)&List );
 }else return( -1 );

 if( !List ) return( -1 );
 // count selected nodes:
 Selected=0;
 fn=List->lh_Head;
 while( fn && fn->ln_Succ ){
  GetListBrowserNodeAttrs( fn,LBNA_Selected, &isselected,TAG_END );
  if( isselected ) Selected++;
  fn=fn->ln_Succ;
 }

 // set initial values:
 Number=number;
 CurrentFile=(TLBNode*)(List->lh_Head);
 return( 0 );
}
//<

//>"FileList::~FileList()"
FileList::~FileList(){
 return( 0 );
}
//<

//>"char *FileList::NextFile( TLBNode **fn )"
char *FileList::NextFile( TLBNode **fn ){
 char *f;

 if( CurrentFile && CurrentFile->ln_Succ ){
  if( fn ) *fn=CurrentFile;
  f=CurrentFile->Fn->Filename;
  CurrentFile=(TLBNode*)CurrentFile->Fn->ln_Succ;
  return( f );
 }else{
  CurrentFile=NULL;
  if( fn ) *fn=NULL;
  return( NULL );
 }
}
//<

//>"char *FileList::NextSFile( TLBNode **fn )"
char *FileList::NextSFile( TLBNode **fn ){
 ULONG selected=FALSE;
 char *f;

 if( CurrentFile && CurrentFile->ln_Succ ){
  while( !selected && CurrentFile->ln_Succ ){
   GetListBrowserNodeAttrs( (Node*)CurrentFile,LBNA_Selected, &selected,TAG_END );
   if( !selected ) CurrentFile=(TLBNode*)CurrentFile->ln_Succ;
  }
 }
 if( !selected ){
  CurrentFile=NULL;
  if( fn ) *fn=NULL;
  return( NULL );
 }else{
  if( fn ) *fn=CurrentFile;
  f=CurrentFile->Fn->Filename;
  CurrentFile=(TLBNode*)CurrentFile->ln_Succ;
  return( f );
 }
}
//<
*/
