/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File: caini.cpp

        Aim:  Implement a class for
              preference ("ini") files

        Ver:  4.2

****************************************/

#include <stdlib.h>

#include <clib/dos_protos.h>
#include <dos/dos.h>
#include <clib/exec_protos.h>
#include <exec/memory.h>

#include "caini.h"

#define GROUP_BYTE 1

TIniFile::TIniFile(){
 Buffer=NULL;
 FileName=(char *)AllocMem( 512, MEMF_CLEAR );
 File=NULL;
 Current=NULL;
}

TIniFile::~TIniFile(){
 this->Free();
 FreeMem( FileName,512 );
}

BOOL TIniFile::Prepare( char *filename,BOOL forwrite ){
 if( File ) fclose( File );
 if( Buffer ) Free();
 FileInfoBlock *fib;
 Overwrite=forwrite;
 strcpy( FileName,filename );
 if( !forwrite ){
  fib=(FileInfoBlock *)AllocDosObject( DOS_FIB,NULL );
  if( !fib ) return( FALSE );
  BPTR lock=Lock( filename,ACCESS_READ );
  if( !Lock ) return( FALSE );
  Examine( lock,fib );
  UnLock( lock );
  BufferSize=fib->fib_Size+1;           // to be sure !!
  FreeDosObject( DOS_FIB,fib );
  Buffer=(char*)AllocMem( BufferSize,MEMF_CLEAR );
  if( !Buffer ) return( FALSE );
  File=fopen( filename,"r" );
  if( !File ) return( FALSE );
  fread( Buffer,1,BufferSize-1,File );     // get the file into buffer
  // now there is a 0 at the end.
  // We will replace all returns by 0 to work with strings:
  ReplaceInBuffer( 10,0 );
 }else{
  File=fopen( filename,"w" );
  if( !File ) return( FALSE );
  BufferSize=512;
  Buffer=(char*)AllocMem( BufferSize,MEMF_CLEAR );
  if( !Buffer ) return( FALSE );
 }
 Current=NULL;
 return( TRUE );
}

void TIniFile::Free(){
 if( File ){
  fclose( File );
  File=NULL;
 }
 if( Buffer ){
  FreeMem( Buffer, BufferSize );
  Buffer=NULL;
 }
}

void TIniFile::ReplaceInBuffer( char what,char by ){
 if( !File || !Buffer || Overwrite ) return;
 for( int i=0;i<BufferSize-1;i++ ){ // do not replace the last 0!
  if( Buffer[i]==what ) Buffer[i]=by;
 }
}

char *TIniFile::IdLine( char *group,char *id ){
// get a pointer to the line in the buffer that belongs to id
 if( !File || !Buffer || Overwrite ) return( NULL );
 int pos=0,len=strlen(group),tlen;
 char *str=Buffer;
 BOOL found=FALSE;
 // find group:
 while( (!found) &&(pos<BufferSize) ){
  if( (str[0]==GROUP_BYTE ) && ( strncmp(str+1,group,len)==0) ) found=TRUE;
  tlen=strlen(str)+1;
  str=&(str[tlen]);
  pos+=tlen;
 }
 if( !found ) return( NULL );
 len=strlen( id );
 while( (str[0]!=GROUP_BYTE)&&(pos<BufferSize) ){
  if( (str[len]=='=')&&(strncmp(str,id,len)==0) ){
   return( str );
  }else{
   tlen=strlen(str)+1;
   str=&(str[tlen]);
   pos+=tlen;
  }
 }
 return( NULL );
}

char *TIniFile::IdValue( char *group,char *id ){
// get a pointer to the value that belongs to id
 char *line;
 int len=strlen(id);
 if( line=IdLine( group,id ) ) return( line+len+1 );
 else return( NULL );
}

BOOL TIniFile::GetBool( char *group,char *id, BOOL def  ){
 char *str=IdValue( group,id );
 if( !str ) return( def );
 if( *str=='0' ) return( FALSE );
 else return( TRUE );
}

int  TIniFile::GetInt( char *group,char *id, int def  ){
 char *str=IdValue( group,id );
 if( str ) return( atoi(str) );
 else return( def );
}

char *TIniFile::GetString( char *group,char *id, char *def, char *buffer ){
 char *str=IdValue( group,id );
 if( str ) strcpy( buffer,str );
 else strcpy( buffer,def );
 return( buffer );
}

BOOL TIniFile::SeekGroup( char *group ){
// get a pointer to the line in the buffer that belongs to id
 if( !File || !Buffer || Overwrite ) return( NULL );
 int pos=0,len=strlen(group),tlen;
 char *str=Buffer;
 // find group:
 while( pos<BufferSize ){
  if( (str[0]==GROUP_BYTE ) && ( strncmp(str+1,group,len)==0) ){ Current=str; return( TRUE ); }
  tlen=strlen(str)+1;
  str=&(str[tlen]);
  pos+=tlen;
 }
 Current=NULL;
 return( FALSE );
}

char *TIniFile::GetNextGroupEntry(){
// returns NULL if no more entry
 if( Current==NULL ) return( NULL );
 int len=strlen(Current);
 Current+=(len+1);
 if( (Current-Buffer>BufferSize-2)||(Current[0]==GROUP_BYTE) ) Current=NULL;
 return( Current );
}

BOOL TIniFile::WriteBool( char *id, BOOL value ){
 if( !File || !Buffer || !Overwrite ) return( FALSE );
 sprintf( Buffer,"%s=%d",id,value?1:0 );
 int len=strlen(Buffer);
 Buffer[len]=10;     // we want to writeln !
 fwrite( Buffer,1,len+1,File );
 return( TRUE );
}

BOOL TIniFile::WriteInt( char *id, int value ){
 if( !File || !Buffer || !Overwrite ) return( FALSE );
 sprintf( Buffer,"%s=%d",id,value );
 int len=strlen(Buffer);
 Buffer[len]=10;     // we want to writeln !
 fwrite( Buffer,1,len+1,File );
 return( TRUE );
}

BOOL TIniFile::WriteString( char *id, char *value ){
 if( !File || !Buffer || !Overwrite ) return( FALSE );
 sprintf( Buffer,"%s=%s",id,value );
 int len=strlen(Buffer);
 Buffer[len]=10;     // we want to writeln !
 fwrite( Buffer,1,len+1,File );
 return( TRUE );
}

BOOL TIniFile::WriteGroup( char *title ){
 if( !File || !Buffer || !Overwrite ) return( FALSE );
 sprintf( Buffer,"[%s]",title );
 int len=strlen(Buffer);
 Buffer[len]=10;     // we want to writeln !
 fwrite( Buffer,1,len+1,File );
 return( TRUE );
}

BOOL TIniFile::WriteLine( char *line ){
 if( !File || !Buffer || !Overwrite ) return( FALSE );
 strcpy( Buffer,line );
 int len=strlen(Buffer);
 Buffer[len]=10;     // we want to writeln !
 fwrite( Buffer,1,len+1,File );
 return( TRUE );
}

BOOL TIniFile::ChangeValue( char *group,char *id, BOOL value ){
 char str[5];
 sprintf( str,"%d",value?1:0 );
 return( ChangeValue( group,id,str ) );
}

BOOL TIniFile::ChangeValue( char *group,char *id,int value ){
 char str[100];
 sprintf( str,"%d",value );
 return( ChangeValue( group,id,str ) );
}

BOOL TIniFile::ChangeValue( char *group,char *id,char *value ){
 char *newline;
 int newlen;
 if( !File || !Buffer || Overwrite ) return (FALSE);
 char *line=IdLine( group,id );
 if( !line ) return (FALSE);
 // Open the file for writing:
 fclose( File );
 File=fopen( FileName,"w" );
 if( !File ) return( FALSE );
 // Remember position and prepare the new line
 char *linend=line+strlen(line)+1;
 newline=(char*)AllocMem( 512,MEMF_CLEAR );
 sprintf( newline,"%s=%s",id,value );
 newlen=strlen( newline );
 // Replace the 0's by returns:
 newline[newlen]=10;
 ReplaceInBuffer( 0,10 );
 // Write first part, then new line, then the unchanged rest
 fwrite( Buffer,1,line-Buffer,File );
 fwrite( newline,1,newlen+1,File );
 fwrite( linend,1,&Buffer[BufferSize]-linend-1,File ); // without 0 at the end
 FreeMem( newline,512 );
 // now reset to the readable state
 Free();
 Prepare( FileName,FALSE );
}

