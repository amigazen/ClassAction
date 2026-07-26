#include "ca_begin.h"
/*

  General ARexx class !

*/

#include "unARexx.h"
#include <string.h>
#include <stdio.h>
#include <exec/types.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/rexxsyslib_protos.h>

//>"BOOL TARexx::Init( char *portname )"
BOOL TARexx::Init( char *portname ){
 short loopflag=TRUE;
 short count;
 char  *tmp;

 strncpy( PortName,portname,40 );
 strcpy( ErrorName,PortName );
 strcat( ErrorName,".LASTERROR" );
 Commands=0;
 Error=NULL;
 ErrorCode=0;

 /* We need to make a unique port name... */
 Forbid();
 tmp=&PortName[strlen(PortName)];
 for( count=1,Port=(struct MsgPort *)1; Port; count++ ){
  char buf[6];
  sprintf( buf,".%d",count );
  strcpy( tmp,buf );
  Port=FindPort( PortName );
 }
 Port=CreatePort( PortName,NULL );
 Permit();
 if( !Port ) return( FALSE );
 else return( TRUE );
}
//<

//>"BOOL TARexx::Free()"
BOOL TARexx::Free(){
 struct  RexxMsg *rmsg;
 if( !Port ) return( FALSE );
 // Clear port name so it can't be found...
 PortName[0]=0;
 // Clean out any outstanding messages we had sent out...
 while( Outstanding ){
  WaitPort( Port );
  while( rmsg=GetMessage() ){
   if( (ULONG)rmsg!=-1L ){
    // Any messages that come now are blown away...
    ReplyMessage( rmsg,NULL,100 );
   }
  }
 }
 // Clean up the port && delete it...
 while (rmsg=GetMessage()) ReplyMessage( rmsg,NULL,100L );
 DeletePort( Port );
 return( TRUE );
}
//<

//>"BOOL TARexx::AddCmd( char *title,(*func)(char*) )"
BOOL TARexx::AddCmd( char *title,void (*func)(char*) ){
 if( Commands==MAX_AREXX_COMMANDS ) return( FALSE );
 ARexxCmd[Commands].Name=title;
 ARexxCmd[Commands].Function=func;
 Commands++;
 return( TRUE );
}
//<

//>"ULONG TARexx::Signals()"
ULONG TARexx::Signals(){
 return( 1L << (Port->mp_SigBit) );
}
//<

//>"struct RexxMsg *TARexx::GetMessage()"
struct RexxMsg *TARexx::GetMessage(){
 struct  RexxMsg *rmsg=NULL;

 if( rmsg=(struct RexxMsg *)GetMsg(Port) ){
  if( rmsg->rm_Node.mn_Node.ln_Type==NT_REPLYMSG ){
   // this would be an answer to an arexx command send by us
   short error;
   if( rmsg->rm_Result1 ) error=TRUE; else error=FALSE;
   // delete the message
   DeleteArgstring( (UBYTE *)rmsg->rm_Args[0] );
   DeleteRexxMsg( rmsg );
   Outstanding-=1;
   if( error ) return( (struct RexxMsg*)-1L ); else return( NULL );
  }
 }
 return( rmsg );
}
//<

//>"void TARexx::ReplyMessage( struct RexxMsg *rmsg,char *RString,ULONG Error)"
void TARexx::ReplyMessage( struct RexxMsg *rmsg,char *RString,ULONG error){
 if( (rmsg)&&((ULONG)rmsg!=-1L) ){
  rmsg->rm_Result2=0;
  if( !(rmsg->rm_Result1=error) ){
   // return RString
   if( rmsg->rm_Action & (1L << RXFB_RESULT) ){
    if( RString ) rmsg->rm_Result2=(LONG)CreateArgstring( RString,(LONG)strlen(RString) );
   }
  }
  // && reply
  ReplyMsg( (struct Message *)rmsg );
 }
}
//<

//>"void TARexx::SetError( char* errtext )"
void TARexx::SetError( char* errtext ){
 ErrorCode=20;
 Error=errtext;
}
//<

//>"void TARexx::SetResult( char* result )"
void TARexx::SetResult( char* result ){
 ErrorCode=0;
 Error=result;
}
//<

//>"BOOL TARexx::HandleMsg()"
BOOL TARexx::HandleMsg(){
 struct RexxMsg *rmsg;
 int i;
 char FirstArg[512];
 BOOL rc=FALSE;
 char *ch;

 while( rmsg=GetMessage() ){
  strncpy( FirstArg,rmsg->rm_Args[0],512 );
  ch=strpbrk( FirstArg," ,");
  if( ch ) *ch=0;

  for( i=0;i<Commands;i++ ){
   if( stricmp( FirstArg,ARexxCmd[i].Name )==0 ){
    ARexxCmd[i].Function( rmsg->rm_Args[0] );
    i+=Commands;
  }}
  ReplyMessage( rmsg,Error,ErrorCode );
  Error=NULL;
  ErrorCode=0L;
  rc=TRUE;
 }
 return( rc );
}
//<

//>"char *TARexx::Name()"
char *TARexx::Name(){
  return( PortName );
}
//<


