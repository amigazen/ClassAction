#include "ca_begin.h"
#include <string.h>
#include <ctype.h>
#include <exec/types.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include "cautils.h"

/* StormC runtime provided this; SAS/C does not. */
BPTR GetProgramDir( void )
{
  struct Process *pr;
  BPTR lock;

  pr = (struct Process *)FindTask( NULL );
  if( pr->pr_HomeDir )
  {
    lock = DupLock( pr->pr_HomeDir );
    if( lock ) return lock;
  }
  lock = Lock( "PROGDIR:", ACCESS_READ );
  if( lock ) return lock;
  if( pr->pr_CurrentDir )
    return DupLock( pr->pr_CurrentDir );
  return NULL;
}

//>"void AdjustPath( char *Path )"
void AdjustPath( char *Path ){
 if( !Path[0] ) return;
 if( (Path[0]!=0)&&(Path[strlen(Path)-1]!=':')&&(Path[strlen(Path)-1]!='/') ){
  strcat(Path,"/");
 }
}
//<

//>"int CheckPath( char *path,char *existing )"
/* function to check if path exisits || if it contains an existing file */
int CheckPath( char *path,char *existing ){
  int Result=-1;
  EnableRequesters( FALSE );
  if( !path[0] ) Result = PATH_NOTFOUND;
  else{
    BPTR lock=Lock( path,ACCESS_READ );
    if( lock ){
      NameFromLock( lock,existing,512 );
      FileInfoBlock *fib=(FileInfoBlock*)AllocDosObject(DOS_FIB,NULL);
      // we could have found a file, so look at it:
      Examine(lock,fib);
      if (fib->fib_DirEntryType<0) Result = PATH_PARTEXISTS; else Result = PATH_EXISTS;
      UnLock( lock );
    }
    if( Result==-1 ){
      // now we will look if there is an existing file inside the path:
      BOOL exists=FALSE;
      if( existing!=path ) strcpy( existing,path );
      char *ptr = existing+strlen(existing)-1;
      do{
        if( *ptr==':' ){
          Result = PATH_NOTFOUND;
          break;
        }
        while( (ptr>=existing) && (*ptr!=':') && (*ptr!='/') ){ *ptr=0;ptr--; }
        if( ptr<existing ){
          existing[0]=0;
          Result = PATH_NOTFOUND; // beginning reached, quit
          break;
        }
        if( *ptr=='/' ) *ptr=0;              // '/' found, so remove it
        lock = Lock( existing,ACCESS_READ );
        if( lock ){                       // BINGO!
          NameFromLock( lock,existing,512 );
          exists = TRUE;
          UnLock( lock );
        }
      }while( !exists );
      if( exists ) Result = PATH_PARTEXISTS; // we have really found an existing path
    }
  }
  EnableRequesters( TRUE );
  return Result;
}
//<

//>"void EnableRequesters( BOOL Allow )"
void EnableRequesters( BOOL Allow ){
  struct Process *proc = (Process *)FindTask(NULL);
  if( Allow ) proc->pr_WindowPtr = NULL;
  else proc->pr_WindowPtr = (Window*)-1;
  // else "proc->pr_WindowPtr = (struct Window*)win" to display on that screen
}
//<

//>"void InitHook( Hook *h,ULONG (*func)(), void *data )"
#include <utility/hooks.h>
void InitHook( Hook *h,ULONG (*func)(), void *data ){
 if(h){
  /* Avoid SAS/C C++ __stdargs mismatch on Hook.h_Entry vs plain C function ptr. */
  *(void **)&h->h_Entry = (void *)func;
  h->h_SubEntry=NULL;
  h->h_Data=data;
 }
}
//<

//>"int sign( int number )"
int sign( int number ){
 if( number>0 ) return( 1 );
 else if( number==0 ) return( 0 );
 else return( -1 );
}
//<

//>"int fsign( float number )"
int fsign( float number ){
 if( number>0 ) return( 1 );
 else if( number==0 ) return( 0 );
 else return( -1 );
}
//<

//>"char *stristr( char *s1,char *s2 )"
char *stristr( char *s1,char *s2 ){
 char *p1,*p2,*p,*ret;
 p1=new char[strlen(s1)+1];
 p2=new char[strlen(s2)+1];
 strcpy( p1,s1 );
 strcpy( p2,s2 );
 for( p=p1;(*p)!=0;p++ ) (*p)=toupper(*p);
 for( p=p2;(*p)!=0;p++ ) (*p)=toupper(*p);
 p=strstr( p1,p2 );
 if( p ) ret=s1+(p-p1);
 else ret=NULL;
 delete p1;
 delete p2;
 return( ret );
}
//<

//>"int strincmp( char *s1,char *s2 )"
int strincmp( char *s1,char *s2 ){
 char *p1=s1,*p2=s2;
 int ret;

 while( (*p1)&&(*p2)&&(toupper(*p1)==toupper(*p2)) ){ p1++;p2++; }

 if( (*p1==0)||(*p2==0) ) ret=0;
 else if( *p1<*p2 ) ret=-1;
 else ret=1;

 return( ret );
}
//<

//>"void RemoveChar( char *s, char c )"
void RemoveChar( char *s, char c ){
  char *src,*dst=s;
  for( src=s;*src;src++ ){
    if( *src != c ){
      *dst=*src;
      dst++;
    }
  }
  *dst = 0;
}
//<

//>"BOOL CoordsInGadget( int x,int y,Gadget* gad )"
#include <intuition/intuition.h>
BOOL CoordsInGadget( int x,int y,Gadget *gad ){
 if( (x >= gad->LeftEdge)&&(x < gad->LeftEdge+gad->Width)
  && (y >= gad->TopEdge) &&(y < gad->TopEdge+gad->Height) ) return( TRUE );
 else return( FALSE );
}
//<

//>"BOOL FileExists( char *filename )"
BOOL FileExists( char *filename ){
 BPTR lock=Lock( filename,ACCESS_READ );
 if( lock ){
  UnLock( lock );
  return( TRUE );
 }else return( FALSE );
}
//<

//>"BOOL IsDirectory( char *path )"
BOOL IsDirectory( char *path ){
  BOOL Result=FALSE;

  BPTR lock=Lock( path,ACCESS_READ );
  if( lock ){
    FileInfoBlock *fib=(FileInfoBlock*)AllocDosObject(DOS_FIB,NULL);
    Examine( lock,fib );
    if( fib->fib_DirEntryType>0 ) Result = TRUE;
    FreeDosObject( DOS_FIB,fib );
    UnLock( lock );
  }
  return( Result );
}
//<

//>"ULONG FileCount( char *file, FileInfoBlock *fib )"
ULONG FileCount( char *file, FileInfoBlock *fib ){
 ULONG size=1;
 if( fib->fib_DirEntryType>0 ){
  BPTR lock=Lock( file,ACCESS_READ );
  if( lock ){
   FileInfoBlock *newfib=(FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
   Examine( lock, newfib );
   char *child=new char[512];
   while( ExNext( lock,newfib ) ){
    strcpy( child, file );
    AddPart( child, newfib->fib_FileName,512 );
    size += FileCount( child,newfib );
   }
   delete [] child;
   FreeDosObject( DOS_FIB,newfib );
   UnLock( lock );
  }
 }
 return( size );
}
//<

//>"void RemoveQuotes( char *string )"
void RemoveQuotes( char *string ){
  RemoveChar( string,'"' );
}
//<

//>"BOOL SubDir( char *dir,char *subdir )"
BOOL SubDir( char *dir,char *subdir ){
  BPTR ld,ls;
  BOOL result=FALSE;
  ld=Lock(dir,ACCESS_READ);
  if( ld ){
    ls=Lock(subdir,ACCESS_READ);
    if( ls ){
      char *realdir=new char[512];
      char *realsubdir=new char[512];
      NameFromLock( ld,realdir,511 );
      NameFromLock( ls,realsubdir,511 );
      // test if dir contains subdir:
      if( strstr(realsubdir,realdir) ) result=TRUE;
      delete [] realdir;
      delete [] realsubdir;
      UnLock( ls );
    }
    UnLock( ld );
  }
  return result;
}
//<

//>"void ToBig( char *str )"
void ToBig( char *str ){
 char *ptr=str;
 while( *ptr ){
  *ptr=toupper(*ptr);
  ptr++;
 }
}
//<


