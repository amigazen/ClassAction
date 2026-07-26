#include "ca_begin.h"
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner & Salim Gasmi

        File: caactions.cpp

****************************************/

#include <string.h>

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include "catypes.h"

#include "caActions.h"

TcaActions *caActions;

//>"TcaActions::TcaActions()"
TcaActions::TcaActions(){
  NewList( &Actions );
}
//<

//>"TcaActions::~TcaActions()"
TcaActions::~TcaActions(){
 Node *act;
 while( act = RemHead(&Actions) ) FreeVec( act );
}
//<

//>"TAction *TcaActions::CreateAction( char *name )"
TAction *TcaActions::CreateAction( char *name ){
  TAction *act = (TAction*)AllocVec(sizeof(TAction),MEMF_CLEAR);
  strncpy( act->Name,name,32 );
  act->Type = RUN_NOCLI;
  act->Async = FALSE;
  strcpy( act->Stack,"4096" );
  strcpy( act->Delay,"0" );
  return( act );
}
//<

//>"void TcaActions::DeleteAction( TAction *act,BOOL RemoveFromList=FALSE )"
void TcaActions::DeleteAction( TAction *act,BOOL RemoveFromList ){
  if( RemoveFromList ) Remove( (Node*)act );
  FreeVec( act );
}
//<

//>"TAction *TcaActions::Add( *Name,*Command,WBRun,Stack,sDelay,Async,ReadSource,ReadDestination )"
TAction *TcaActions::Add( char *Name, char *Command, char WBRun, char *Stack, char *sDelay,
  BOOL Async, BOOL ReadSource, BOOL ReadDestination ){
  TAction *added = CreateAction( "" );
  AddTail( &Actions,(Node*)added );
  strncpy( added->Name,Name,20 );
  strncpy( added->Exec,Command,250 );
  added->Type    = WBRun;
  added->Async   = Async;
  added->ReadSrc = ReadSource;
  added->ReadDst = ReadDestination;
  strncpy( added->Stack,Stack,8 );
  strncpy( added->Delay,sDelay,3 );
  return( added );
}
//<

//>"Action *TcaActions::Item( char *Name )"
TAction *TcaActions::Item( char *Name ){
 for( TAction *a = (TAction*)Actions.lh_Head;a->ln_Succ;a = (TAction*)a->ln_Succ ){
  if( strcmp(a->Name,Name) == 0 ) return( a );
 }
 return( NULL );
}
//<

