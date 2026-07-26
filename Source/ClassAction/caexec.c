/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
               & Salim Gasmi

        File: Exec.c

        Aim:  Kernel of the hard core code

****************************************/

#include <ctype.h>
#include <dos/datetime.h>
#include <dos/dostags.h>
#include <gadgets/fuelgauge.h>
#include <gadgets/listbrowser.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <proto/locale.h>
#include <proto/wb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cadisk.h"
#include "caglobal.h"
#include "cagui.h"
#include "calha.h"
#include "careq.h"
#include "cautils.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classactioncd.h"

#include "caexec.h"

char Quote=39;
char QuoteStr[]={39,0};

//>"void My_Free(char *buf,int len,int cr)"
void My_Free(char *buf,int cr){
 if(cr==1) FreeVec( buf ); else free( buf );
}
//<

//>"int CompareDate( char *f1, char*f2 )"
/****************************************/
/* Aim:      Compares the dates of the  */
/*           given files                */
/*                                      */
/* Input:    f1,f2: name of the files   */
/*            (full path)               */
/*                                      */
/* Output:   0: Dates equal             */
/*           <0: f1 younger than f2     */
/*           >0: f2 younger than f1     */
/****************************************/
int CompareDate( char *f1, char*f2 ){
 FileInfoBlock Fib1,Fib2;
 BPTR lock1,lock2;
 int ret;
 lock1=Lock( f1,ACCESS_READ  );
 lock2=Lock( f2,ACCESS_READ  );
 if( lock1 && lock2 ){
  Examine( lock1,&Fib1 );
  Examine( lock2,&Fib2 );
  ret=CompareDates( &(Fib1.fib_Date),&(Fib2.fib_Date) );
 }
 if( lock1 ) UnLock( lock1 );
 if( lock2 ) UnLock( lock2 );
 return( ret );
}
//<

//>"void StoreFib( char *file, char *buffer, BOOL lha )"
/****************************************/
/*                                      */
/* Function: StoreFib                   */
/*                                      */
/* Aim:      Stores the information     */
/*           found in the FileInfoBlock */
/*           of the given file          */
/*           in the string              */
/*                                      */
/* Input:    file: filename             */
/*            (full path)               */
/*           buffer: pointer to the     */
/*            buffer (reserve 40 B)     */
/*           lha: if file is in an arch.*/
/*            or not in an archive      */
/*                                      */
/****************************************/
void StoreFib( char *file, char *buffer, BOOL lha ){
 FileInfoBlock *fib;
 DateTime Dat;
 char *tempdate=new char[80],*temptime=new char[80];
 BPTR lock;
 TArchive Archive;

 if( lha ) lock=Archive.Lock( file );
 else lock=Lock( file,ACCESS_READ  );
 if( lock ){
  fib=(FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
  if( lha ) Archive.Examine( fib );
  else Examine( lock,fib );
  memcpy( &(Dat.dat_Stamp),&(fib->fib_Date),sizeof(struct DateStamp) );
  Dat.dat_StrDay  = NULL;
  Dat.dat_StrDate = tempdate;
  Dat.dat_StrTime = temptime;
  Dat.dat_Format  = Main->DateFormat;
  if( Main->DateString ) Dat.dat_Flags=DTF_SUBST;
  else Dat.dat_Flags=0;
  DateToStr( &Dat );
  sprintf( buffer,"%ld Bytes, %s  %s",fib->fib_Size,temptime,tempdate );
  FreeDosObject( DOS_FIB,fib );
  if( lha ) Archive.UnLock();
  else UnLock( lock );
 }else buffer[0]=0;
 delete [] temptime;
 delete [] tempdate;
}
//<

//>"float FileLength( char *file, FileInfoBlock *fib )"
float FileLength( char *file, FileInfoBlock *fib ){
  float size=0;
  if( fib->fib_DirEntryType>0 ){
   BPTR lock=Lock( file,ACCESS_READ );
   if( lock ){
    FileInfoBlock *newfib=(FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
    Examine( lock, newfib );
    char *child=new char[512];
    while( ExNext( lock,newfib ) ){
     strcpy( child, file );
     AddPart( child, newfib->fib_FileName,512 );
     size+=FileLength( child,newfib );
    }
    delete [] child;
    FreeDosObject( DOS_FIB,newfib );
    UnLock( lock );
   }
  }else size=fib->fib_Size;
  return( size );
}
//<

//>"BOOL GetVersion( char *file,char *version )"
BOOL GetVersion( char *file,char *version ){
  long bytes=Main->BufferSize;
  BPTR Handle;
  char *buffer,*ptr;
  BOOL result=FALSE;
  if( Handle=Open(file,MODE_OLDFILE ) ){        // try to open file
    buffer = (char*)AllocMem( Main->BufferSize+10,MEMF_CLEAR );     // alloc buffer
    if( buffer ){
      strcpy( buffer,"      " );                                        // prepare first bytes
      while( (result is FALSE) and (bytes == Main->BufferSize) ){
        bytes = Read( Handle,buffer+5,Main->BufferSize );
        if( bytes>0 ){
          buffer[bytes+5] = 0;                  // let the string have an end
          ptr=buffer-1;
          do{
            ptr=(char*)memchr(ptr+1,'$',buffer+bytes+6-ptr);
          }while( ptr and (memcmp(ptr,"$VER",4) isnot 0) );      // search for VER
          if( ptr ){      
            if( (ptr=strchr(ptr,' ')) and
                (ptr=strchr(ptr+1, ' ')) ){     // go to the version number
              strncpy(version,ptr+1,20);        // to be sure, copy only 20 bytes
              version[20]=0;
              if( ptr=strchr(version,' ') ) *ptr=0;
              result=TRUE;
            }
          }
        }else break;
        memcpy( buffer,buffer+Main->BufferSize-5,5 );
      }
      FreeMem( buffer,Main->BufferSize+10 );
    }
    Close( Handle );
  }
  return( result );
}
//<

//>"BOOL CopyArchiveFile( char *src,char *dest )"
BOOL CopyArchiveFile( char *src, char *dest ){
  BPTR handler;
  char comm[1024];
  int res,result=0;
  TArchive Arch;
  BPTR lock;
  BOOL dest_lha = caFileTypes->ContainsArchive(dest,comm);
  BOOL src_lha  = caFileTypes->ContainsArchive(src,comm);

  if( Main->ConfirmReplace ){
    if( dest_lha  ) lock=Arch.Lock(dest);
    else lock=Lock( dest,ACCESS_READ );
    if( lock ){
      if( dest_lha ) Arch.UnLock();
      else UnLock( lock );
      if( (Main->Transfer.RequesterResult is REPLACE_NOT) or
          (Main->Transfer.RequesterResult is REPLACE_ONE) ){    // we must ask the user
        Main->Transfer.RequesterResult = AskReplace( src,dest,Main->Transfer.ReplaceText );
      }
      if( (Main->Transfer.RequesterResult is REPLACE_NOT) or
          (Main->Transfer.RequesterResult is REPLACE_NONE) ) result=1;
    }
  }
  if( result isnot 0 ) return(TRUE);

  if( src_lha ){
    char *file=new char[512];
    strcpy( file,src );
    if (Arch.Lock(file)){
      Arch.LocalName( file );
      res=Arch.UnpackFile( file,dest );
      Arch.UnLock();
    }else res=FALSE;
    delete [] file;
    return( res );
  }else if( dest_lha ){
    char *archive=new char[512];
    char *path=new char[512];
    char *ptr;
    BPTR lock,oldlock;
    strcpy( archive,dest );
    Arch.ArchiveName( archive );
    strcpy( path,src );
    ptr=PathPart( path );
    *ptr=0;
    ptr=FilePart( src );
    lock=Lock( path,ACCESS_READ );
    if( lock ){
      oldlock=CurrentDir( lock );
      Arch.Lock( dest );
      if( Arch.Type==TYPE_LHA ){
        // options: Recursively Add file
        sprintf( comm,"lha -r a %c%s%c %c%s%c",'"',archive,'"','"',ptr,'"' );
      }else if( Arch.Type==TYPE_LZX ){
        // options: Recursively Add file
        sprintf( comm,"lzx -r a %c%s%c %c%s%c",'"',archive,'"','"',ptr,'"' );
      }else if( Arch.Type==TYPE_ZIP ){
        // options: Grow archive, Recursive add
        sprintf( comm,"zip -gr %c%s%c %c%s%c",'"',archive,'"','"',ptr,'"' );
      }else{ Arch.UnLock(); UnLock(lock); return( FALSE ); }
      Arch.UnLock();
      handler=Open("NIL:",MODE_NEWFILE);
      res=SystemTags( comm,SYS_Input,handler,SYS_Output,NULL,SYS_Asynch,FALSE,NP_HomeDir,path,TAG_DONE );
      Close( handler );
      CurrentDir( oldlock );
      UnLock( lock );
      if( res==0 ) return( TRUE );
      else return( FALSE );
    }
    delete path;
    delete archive;
    if( res==0 ) return( TRUE );
    else return( FALSE );
  }
/*
 else{
  sprintf( comm, "copy %c%s%c to %c%s%c all quiet clone",'"',src,'"','"',dest,'"' );
  handler=Open("NIL:",MODE_NEWFILE);
  
  res=SystemTags( comm,SYS_Input,handler,SYS_Output,NULL,SYS_Asynch,FALSE,TAG_DONE );
  Close( handler );
  // ftpmount seems to return the wrong code, so we repair this here:
  if( stristr(dest,"ftp:") || stristr(dest,"ftpmount:") ) res=0;
  if( res==0 ) return( TRUE );
  else return( FALSE );
 }
*/
}
//<

//>"void TryDeprotectFile( char *file )"
void TryDeprotectFile( char *file ){
  FileInfoBlock *fib;
  fib = (FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
  BPTR lock = Lock( file,ACCESS_READ );
  if( lock ){
    Examine( lock,fib );
    if( (fib->fib_Protection & FIBF_WRITE) or
        (fib->fib_Protection & FIBF_DELETE) ){
      if( (Main->Transfer.DeprotectResult is DEPROTECT_ONE) or
          (Main->Transfer.DeprotectResult is DEPROTECT_NOT) ){
        char *msg=new char[512];
        sprintf( msg,GetCatalogStr(Main->Catalog,MSG_DEPROTECTFILE,MSG_DEPROTECTFILE_STR),file );
        Main->Transfer.DeprotectResult = SureReq( msg,Main->Transfer.ReplaceText );
        delete [] msg;
      }
      if( (Main->Transfer.DeprotectResult is DEPROTECT_ONE) or
          (Main->Transfer.DeprotectResult is DEPROTECT_ALL) ){
        fib->fib_Protection &= ~ FIBF_WRITE;
        fib->fib_Protection &= ~ FIBF_DELETE;
        SetProtection( file,fib->fib_Protection );
      }
    }
    UnLock( lock );
  }
  FreeDosObject( DOS_FIB,fib );
}
//<

//>"int MRECopy( char *src,char *dest,Object *gauge,UBYTE mode=FT_COPY )"
/****************************************/
/* Aim:    Copy, move etc. src to dest  */
/* Output: 0  = ok                      */
/*         -1 = abort by user           */
/*         -2 = copy dir to file error  */
/*         -3 = create dir error        */
/*         -4 = write error             */
/*         -5 = delete error            */
/*         -6 = read error              */
/*         1 = replacement cancel       */
/****************************************/
int MRECopy( char *src,char *dest,Object *gauge,UBYTE mode ){
  BPTR srcfile,dstfile,lock;
  long copiedbytes=Main->BufferSize;
  float progress=0;
  BOOL result=0,childresult=0;
  FileInfoBlock *info;

  if( CheckAbort() ) return( -1 );

  if( !(lock = Lock( src,ACCESS_READ )) ) return( FALSE );
  info = (FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
  Examine( lock,info );

  SetGadgetAttrs( (Gadget *)gauge,Main->IntuiWin, NULL,
    Main->CountFiles ? FUELGAUGE_Level : TAG_IGNORE,Main->Transfer.Count,GA_Text,src,TAG_END );

  if( info->fib_DirEntryType>0 ){
    // directory:
    char *nextsrc=new char[1024],*nextdest=new char[1024],*ptrs,*ptrd;
    BPTR destlock;
    // prepare path for sourcefile:
    strcpy( nextsrc,src );
    AdjustPath( nextsrc );
    ptrs = nextsrc + strlen(nextsrc); // remember the end of the string to add each file there
    if( mode is FT_DELETE ){
      nextdest[0] = 0;
      ptrd = nextdest;
    }else{
      // prepare path for destfile:
      strcpy( nextdest,dest );
      destlock = Lock( nextdest,ACCESS_READ );
      if( destlock ){
        FileInfoBlock *destinfo=(FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
        Examine( destlock,destinfo );
        if( destinfo->fib_DirEntryType<0 ){
          char *msg=new char[512];
          sprintf( msg,GetCatalogStr(Main->Catalog,ERR_NOMKDIR,ERR_NOMKDIR_STR),dest );
          Info( msg );
          delete [] msg;
          result = -2;
        }
        FreeDosObject( DOS_FIB,destinfo );
        UnLock( destlock );
      }else{
        if( destlock = CreateDir( nextdest ) ){
          UnLock( destlock );
          SetProtection( nextdest,info->fib_Protection );
          SetComment( nextdest,info->fib_Comment );
        }else{
          char *msg=new char[512];
          sprintf( msg,GetCatalogStr(Main->Catalog,ERR_NOMKDIR,ERR_NOMKDIR_STR),dest );
          Info( msg );
          delete [] msg;
          result = -3;
        }
      }
      AdjustPath( nextdest );
      ptrd = nextdest + strlen(nextdest);
    }
    if( result is 0 ){
      while( (childresult > -1) and ExNext(lock,info) ){
        strcpy( ptrs,info->fib_FileName );
        strcpy( ptrd,info->fib_FileName );
        childresult = MRECopy( nextsrc,nextdest,gauge,mode );
      }
      UnLock( lock );
      if( (childresult is 0) and ((mode is FT_DELETE)or(mode is FT_MOVE)or(mode is FT_MOVEAS)) ){
        childresult = DeleteFile( src );
        if( not childresult ){
          TryDeprotectFile( src );
          if( (Main->Transfer.DeprotectResult is DEPROTECT_ONE) or
              (Main->Transfer.DeprotectResult is DEPROTECT_ALL) ) childresult = DeleteFile( src );
        }
        if( not childresult  ){
          char *msg=new char[512];
          sprintf( msg,GetCatalogStr(Main->Catalog,ERR_NODEL,ERR_NODEL_STR),src );
          Info( msg );
          delete [] msg;
          result = -5;
        }
      }else if( childresult is -1 ) result = -1; // abort is valid for all !!
    }else UnLock( lock );
    delete [] nextsrc;
    delete [] nextdest;
  }else{
    // file:
    UnLock( lock );
    if( mode is FT_DELETE ){
      childresult = DeleteFile( src );
      if( not childresult ){
        TryDeprotectFile( src );
        if( (Main->Transfer.DeprotectResult is DEPROTECT_ONE) or
            (Main->Transfer.DeprotectResult is DEPROTECT_ALL) ) childresult = DeleteFile( src );
      }
      if( not childresult ){
        char *msg=new char[512];
        sprintf( msg,GetCatalogStr(Main->Catalog,ERR_NODEL,ERR_NODEL_STR),src );
        Info( msg );
        delete [] msg;
        result = -5;
      }
    }else{
      srcfile = Open( src,MODE_OLDFILE );
      if( !srcfile ) result = -6;
      else{
        // here test for replace !!
        if( Main->ConfirmReplace ){
          if( lock=Lock( dest,ACCESS_READ ) ){
            UnLock( lock );
            if( (Main->Transfer.RequesterResult is REPLACE_NOT) or
                (Main->Transfer.RequesterResult is REPLACE_ONE) ){    // we must ask the user
              Main->Transfer.RequesterResult = AskReplace( src,dest,Main->Transfer.ReplaceText );
            }
            if( (Main->Transfer.RequesterResult is REPLACE_NOT) or
                (Main->Transfer.RequesterResult is REPLACE_NONE) ) result=1;
          }
        }
        if( result is 0 ){
          dstfile = Open( dest,MODE_NEWFILE );
          if( !dstfile ){
            TryDeprotectFile( dest );
            if( (Main->Transfer.DeprotectResult is DEPROTECT_ONE) or
                (Main->Transfer.DeprotectResult is DEPROTECT_ALL) ) dstfile = Open( dest,MODE_NEWFILE );
          }
          if( !dstfile ){
            char *msg=new char[512];
            result = -4;
            sprintf( msg,GetCatalogStr(Main->Catalog,ERR_NOCOPY,ERR_NOCOPY_STR),src );
            Info( msg );
            delete [] msg;
          }else{
            while( copiedbytes == Main->BufferSize ){
              copiedbytes = Read( srcfile,Main->Transfer.Buffer,Main->BufferSize );
              // returns 0 if EOF and -1 if error
              if( copiedbytes>0L ) Write( dstfile,Main->Transfer.Buffer,copiedbytes );
              if( copiedbytes==Main->BufferSize ){
                progress += copiedbytes;
                sprintf(Main->Transfer.ProgressText,"%s %.0f%%",src,(100*progress)/info->fib_Size);
                SetGadgetAttrs( (Gadget *)gauge,Main->IntuiWin, NULL,
                  Main->CountFiles ? FUELGAUGE_Level : TAG_IGNORE,Main->Transfer.Count,
                  GA_Text,Main->Transfer.ProgressText,TAG_END );
              }
            }
            Close( dstfile );
          }
        }
        Close( srcfile );
      }
      if( result is 0 ){
        if( copiedbytes == -1 ){
          char *msg=new char[512];
          sprintf( msg,GetCatalogStr(Main->Catalog,ERR_NOCOPY,ERR_NOCOPY_STR),src );
          Info( msg );
          delete [] msg;
          DeleteFile( dest );
          result = -4;
        }else{
          SetProtection( dest,info->fib_Protection );
          SetFileDate( dest,&info->fib_Date );
          SetComment( dest,info->fib_Comment );
          if( (mode is FT_MOVE) or (mode is FT_MOVEAS) ) DeleteFile( src );
        }
      }
    }
  }
  FreeDosObject( DOS_FIB,info );
  Main->Transfer.Count++;
  return( result );
}
//<

//>"BOOL DelFile( const char *src )"
BOOL DelFile( const char *src ){
 BPTR handler;
 char comm[512];
 int res;

 sprintf( comm,"c:delete %c%s%c all",'"',src,'"' );
 if( Main->DeleteProtected ) strcat( comm," FORCE" );
 handler=Open("NIL:",MODE_OLDFILE);

 res=SystemTags( comm,SYS_Input,handler,SYS_Output,NULL,SYS_Asynch,FALSE,TAG_DONE );
 Close( handler );
 if( res==0 ) return( TRUE );
 else return( FALSE );
}
//<

//>"void CutSuffix( char *Filename )"
void CutSuffix( char *Filename ){
  BOOL conti=TRUE;
  for( char *ptr=Filename+strlen(Filename)-1;ptr>Filename;ptr-- ){
    switch( *ptr ){
      case '/': case ':': case '"': case ' ': conti=FALSE; break;
      case '.': *ptr=0; conti=FALSE; break;
    }
    if( not conti ) break;
  }
}
//<

//>"void AppendFilesToString( char *String,TFileList *list,char Specifier )"
void AppendFilesToString( char *String,TFileList *list,char Specifier ){
  char sep[]={'"',0};
  char dir[512],*ptr;
  strcpy( dir,list->Source );
  AdjustPath( dir );
  TFileEntry* File = (TFileEntry*)list->Files.lh_Head;
  while( File->ln_Succ ){
    if( File->Selected ){
      if( not isupper(Specifier) ) strcat( String,sep );
      switch( toupper(Specifier) ){
        case 'F':{
          strcat( String,dir );
          strcat( String,File->Name );
          break;
        }
        case 'B': strcat( String,File->Name ); break;
        case 'S':{
          ptr=String+strlen(String);
          strcat( String,dir );
          strcat( String,File->Name );
          CutSuffix( ptr );
          break;
        }
        case 'X':{
          ptr=String+strlen(String);
          strcat( String,File->Name );
          CutSuffix( ptr );
          break;
        }
      }
      if( not isupper(Specifier) ) strcat( String,sep );
      strcat( String," " );
    }
    File = (TFileEntry*)File->ln_Succ;
  }
  if( String[0] and (String[strlen(String)-1] is ' ') ) String[strlen(String)-1] = 0;
}
//<

//>"int ParseArgs(char *exec,char *file,TFileList *list,char *res)"
/****************************************/
/*                                      */
/* Function:  ParseArgs                 */
/*                                      */
/* Aim:       interprete exec string    */
/*            with selected file 'file' */
/*            and fill up 'res' with    */
/*            the result                */
/*                                      */
/* Return Values :                      */
/*                                      */
/* -2 = Unable to parse                 */
/* -1 = Nothing to change               */
/*  0 = Parsed Succesfully              */
/*  1 = Parsed Succesfully,             */
/*      filename used                   */
/*                                      */
/****************************************/
int ParseArgs(char *exec,char *file,TFileList *list,char *res){
  if( !strchr(exec,'[') ){
    strcpy(res,exec);
    return(-1);
  }
  char *found,*x;
  char sep[]={'"',0};
  char tmpe[1024];
  char tmpf[1024];
  char *suf,*mimi;

  strcpy(tmpe,exec);

  found=strstr(tmpe,"[a");
  if( !found ) found=strstr(tmpe,"[A");
  if( found and (found[3] is ']') ){
   *found=0;
   strcpy(res,tmpe);
   AppendFilesToString( res,list,found[2] );
   strcat(res,found+4);
   return(0);
  }
  found=strstr(tmpe,"[f]"); // filename with whole path and quotes
  if(found!=NULL){
   *found=0;
   strcpy(res,tmpe);
   strcat(res,sep);
   strcat(res,file);
   strcat(res,sep);
   strcat(res,found+3);
   return(1);
  }
  found=strstr(tmpe,"[F]"); // same without quotes
  if(found!=NULL){
   *found=0;
   strcpy(res,tmpe);
   strcat(res,file);
   strcat(res,found+3);
   return(1);
  }
  found=strstr(tmpe,"[b]"); // filename without path with quotes
  if(found!=NULL){
   *found=0;
   strcpy(res,tmpe);
   strcat(res,sep);
   strcat(res,(char *)FilePart(file));
   strcat(res,sep);
   strcat(res,found+3);
   return(1);
  }
  found=strstr(tmpe,"[B]"); // same without quotes
  if(found!=NULL){
   *found=0;
   strcpy(res,tmpe);
   strcat(res,(char *)FilePart(file));
   strcat(res,found+3);
   return(1);
  }
  strcpy(tmpe,exec);
  strcpy(tmpf,file);

  found=strstr(tmpe,"[s]"); // filename with path without suffix with quotes
  if(found!=NULL){
   x=strchr(tmpf,'.');
   if(x!=NULL){
    suf=x+1;
    while((mimi=strchr(suf,'.'))!=NULL) suf=mimi+1;
    *(suf-1)=0;
   }
   *found=0;
   strcpy(res,tmpe);
   strcat(res,sep);
   strcat(res,tmpf);
   strcat(res,sep);
   strcat(res,found+3);
   return(1);
  }

  found=strstr(tmpe,"[S]"); // same without quotes
  if(found!=NULL){
   x=strchr(tmpf,'.');
   if(x!=NULL){
   suf=x+1;
   while((mimi=strchr(suf,'.'))!=NULL) suf=mimi+1;
   *(suf-1)=0;
   }
   *found=0;
   strcpy(res,tmpe);
   strcat(res,tmpf);
   strcat(res,found+3);
   return(1);
  }

  found=strstr(tmpe,"[x]"); // filename without path and suffix with quotes
  if(found!=NULL){
    x=strchr(tmpf,'.');
    if(x!=NULL){
      suf=x+1;
      while((mimi=strchr(suf,'.'))!=NULL) suf=mimi+1;
      *(suf-1)=0;
    }
    *found=0;
    strcpy(res,tmpe);
    strcat(res,sep);
    strcat(res,(char *)FilePart(tmpf));
    strcat(res,sep);
    strcat(res,found+3);
    return(1);
  }

  found=strstr(tmpe,"[X]"); // same without quotes
  if(found!=NULL){
   x=strchr(tmpf,'.');
   if(x!=NULL){
    suf=x+1;
    while((mimi=strchr(suf,'.'))!=NULL) suf=mimi+1;
    *(suf-1)=0;
   }
   *found=0;
   strcpy(res,tmpe);
   strcat(res,(char *)FilePart(tmpf));
   strcat(res,found+3);
   return(1);
  }

  found=strstr(tmpe,"[SRC]");
  if(found!=NULL){
   *found=0;
   strcpy( res,tmpe );
   strcat( res,GetSrc() );
   strcat( res,found+5 );
   return( 0 );
  }
  found=strstr(tmpe,"[SRCS]");
  if(found!=NULL){
   *found=0;
   strcpy( tmpf,GetSrc() );
   if( tmpf[0]!=0 ){
    x=&(tmpf[strlen(tmpf)-1]);
    if( (*x!='/')&&(*x!=':') ) strcat( tmpf,"/" );
   }
   strcpy( res,tmpe );
   strcat( res,tmpf );
   strcat( res,found+6 );
   return( 0 );
  }
  found=strstr(tmpe,"[DST]");
  if(found!=NULL){
   *found=0;
   strcpy( res,tmpe );
   strcat( res,GetDst() );
   strcat( res,found+5 );
   return( 0 );
  }
  found=strstr(tmpe,"[DSTS]");
  if(found!=NULL){
   *found=0;
   strcpy( tmpf,GetDst() );
   if( tmpf[0]!=0 ){
    x=&(tmpf[strlen(tmpf)-1]);
    if( (*x!='/')&&(*x!=':') ) strcat( tmpf,"/" );
   }
   strcpy( res,tmpe );
   strcat( res,tmpf );
   strcat( res,found+6 );
   return( 0 );
  }
  found=strstr(tmpe,"[LDIR]");
  if(found!=NULL){
   *found=0;
   strcpy( res,tmpe );
   strcat( res,CurrentTab[0]->Path );
   strcat( res,found+6 );
   return( 0 );
  }
  found=strstr(tmpe,"[LDIRS]");
  if(found!=NULL){
   *found=0;
   strcpy( tmpf,CurrentTab[0]->Path );
   if( tmpf[0]!=0 ){
    x=&(tmpf[strlen(tmpf)-1]);
    if( (*x!='/')&&(*x!=':') ) strcat( tmpf,"/" );
   }
   strcpy( res,tmpe );
   strcat( res,tmpf );
   strcat( res,found+7 );
   return( 0 );
  }
  found=strstr(tmpe,"[RDIR]");
  if(found!=NULL){
   *found=0;
   strcpy( res,tmpe );
   strcat( res,CurrentTab[1]->Path );
   strcat( res,found+6 );
   return( 0 );
  }
  found=strstr(tmpe,"[RDIRS]");
  if(found!=NULL){
   *found=0;
   strcpy( tmpf,CurrentTab[1]->Path );
   if( tmpf[0]!=0 ){
    x=&(tmpf[strlen(tmpf)-1]);
    if( (*x!='/')&&(*x!=':') ) strcat( tmpf,"/" );
   }
   strcpy( res,tmpe );
   strcat( res,tmpf );
   strcat( res,found+7 );
   return( 0 );
  }

  strcpy(res,exec);
  return(-1);
}
//<

//>"void BstrToStr( BSTR bs,char *r )"
/****************************************/
/*                                      */
/* Function : BstrToStr                 */
/*                                      */
/* Goal     : Convert a BCPL String     */
/*            into a C String           */
/*                                      */
/****************************************/
void BstrToStr( BSTR bs,char *r ){
 ULONG *x;
 char *c;
 int i;

 x=(ULONG *)bs;
 x=(ULONG*)((ULONG)x<<2);
 c=(char *)x;

 for(i=1;i<=c[0];i++) r[i-1]=c[i];
 r[c[0]]=0;
}
//<

//>"int ParseReq(char *exec,char *fname,char **reqbuffer,char *res)"
/****************************************/
/*                                      */
/* Function: ParseReq                   */
/*                                      */
/* Aim:      interprete exec string     */
/*           with REQ Commands          */
/*           and fill up 'res' with     */
/*           the result                 */
/*                                      */
/* Return Values :                      */
/*                                      */
/* -2 = Unable to parse                 */
/* -1 = Nothing to change               */
/*  0 = Parsed Succesfully              */
/*                                      */
/****************************************/
int ParseReq(char *exec,char *fname,char **reqbuffer,char *res){
 char tmp[1024];
 char *found,*end,*ptr;
 int mode=-1;
 char text[1024];
 BOOL result;

 strcpy(res,exec);
 strcpy(tmp,exec);

 found=strstr(tmp,"[SURE:");
 if(found!=NULL){
  end=strstr(found,"]");
  if( end==NULL ){
   Info( GetCatalogStr(Main->Catalog,TXT_SUREERROR,TXT_SUREERROR_STR) );
   return(-2);
  }
  strcpy(text,found+6);
  text[end-found-6]=0;
  result=Req( text );
  if( !result ) return(-2);
  *found=0;
  strcpy(res,tmp);
  strcat(res,end+1);
  return(0);
 }
 found=strstr(tmp,"[OSURE:");
 if(found!=NULL){
  end=strstr(found,"]");
  if( end==NULL ){
   Info( GetCatalogStr(Main->Catalog,TXT_SUREERROR,TXT_SUREERROR_STR) );
   return(-2);
  }
  strcpy(text,found+7);
  text[end-found-7]=0;
  // now to the roots: the first byte says if we have alrady asked,
  // and the second holds the result:
  if( !reqbuffer[4][0] ){
    result=Req( text );
    reqbuffer[4][0]=1;
    reqbuffer[4][1]=result;
  }else result = reqbuffer[4][1];
  if( !result ) return(-2);
  *found=0;
  strcpy(res,tmp);
  strcat(res,end+1);
  return(0);
 }

 found=strstr(tmp,"[REQF:");
 if( !found ) found=strstr(tmp,"[OREQF:");
 if(found!=NULL) mode=0;

 if(mode==-1){
  found=strstr(tmp,"[REQD:");
  if( !found ) found=strstr(tmp,"[OREQD:");
  if(found!=NULL) mode=1;
 }

 if(mode==-1){
  found=strstr(tmp,"[REQV:");
  if( !found ) found=strstr(tmp,"[OREQV:");
  if(found!=NULL) mode=2;
 }

 if(mode==-1){
  found=strstr(tmp,"[REQT:");
  if( !found ) found=strstr(tmp,"[OREQT:");
  if(found!=NULL) mode=3;
 }

 if(mode==-1) return(-1);

 end=strchr(found,']');
 if(end==NULL){
  Info( GetCatalogStr(Main->Catalog,TXT_REQERROR,TXT_REQERROR_STR) );
  return(-2);
 }

 if( found[1] == 'O' ){
   strcpy(text,found+7);
   text[end-found-7]=0;
 }else{
   strcpy(text,found+6);
   text[end-found-6]=0;
 }
 if( (found[1] != 'O') || (reqbuffer[mode][0] == 0) ){
   if(mode<3){
     // we will first take the dest dir:
     strcpy( reqbuffer[mode],GetDst() );
     if( !reqbuffer[mode][0] ) strcpy( reqbuffer[mode],GetSrc() ); // or the source, if dest=volumes
     if( !reqbuffer[mode][0] ) strcpy( reqbuffer[mode],"Sys:" ); // or the source, if dest=volumes
     if( mode is 0 ){
       // and for a reqbuffer[i] request add the reqbuffer[i]name:
       if( ptr=FilePart(fname) ) AddPart( reqbuffer[mode],ptr,255 );
     }else if( mode is 1 ){
       if( reqbuffer[mode][strlen(reqbuffer[mode])-1] isnot ':' ) strcat( reqbuffer[mode],"/" );
     }
     if( !Freq(reqbuffer[mode],mode,text) ) return(-2);
   }else{
     strcpy(reqbuffer[mode],"");
     if( !GetText(reqbuffer[mode],255,text) ) return(-2);
   }
 }
 *found=0;

 sprintf(res,"%s%s",tmp,reqbuffer[mode]);
 if(end[1]!=0) strcat(res,end+1);
 return(0);
}
//<

//>"void StoreCommandProgramPath( char *Command, char *Path )"
void StoreCommandProgramPath( char *Command, char *Path ){
  char *src=Command,*dst=Path;
  BOOL WaitForAp=FALSE,Stop=FALSE;
  if( *src=='"' ){
    WaitForAp=TRUE;
    src++;
  }
  while( (!Stop) && (*src) ){
    switch( *src ){
      case ' ':{
        if( !WaitForAp ) Stop=TRUE;
        else{
          *dst=' ';
          src++;
          dst++;
        }
        break;
      }
      case '"': if( WaitForAp ) Stop=TRUE; else src++; break;
      default:{
        *dst=*src;
        src++;
        dst++;
      }
    }
  }
  *dst=0;
  dst=PathPart(Path);
  *dst=0;
}
//<

//>"void WbStart( char *text )"
/****************************************/
/* Function:  WbStart                   */
/*                                      */
/* Aim:       Start a WB-object with    */
/*            WB-arguments as specified */
/*            in 'text'                 */
/*                                      */
/****************************************/
void WbStart( char *text ){
  struct TagItem taglist[11];
  char *object,*next,*ptr;
  char buffer[512];
  char file[5][109];
  char dir[5][256];
  int i=0,n=0;
  BOOL done=FALSE;
  char *path=new char[512];
  BPTR lock,oldlock;

  strcpy( buffer,text );
  object=buffer;
  // get the object:
  if( object[0]=='"' ){
    // enclosed by "
    object++;
    if( next=strchr(object,'"') ){
      *next=0;
      if( (*(++next)==0)||(*(++next)==0) ) done=TRUE;
    }else done=TRUE;
  }else{
    // take the text til the next blank
    if( next=strchr(object,' ') ){
      *next=0;
      if( *(++next)==0 ) done=TRUE;
    }else done=TRUE;
  }
  // collect all arguments:
  while( (!done)&&(n<5) ){
    if( *next=='"' ){
      next++;
      ptr=next;
      if( next=strchr( next,'"' ) ){
        *next=0;
        if( (*(++next)==0)||(*(++next)==0) ) done=TRUE;
      }else done=TRUE;
    }else{
      ptr=next;
      if( next=strchr(next,' ') ){
        *next=0;
        if( *(++next)==0 ) done=TRUE;
      }else done=TRUE;
    }
    strcpy( dir[i],ptr );
    strcpy( file[n],FilePart(dir[n]) );   // divide file
    ptr=PathPart(dir[n]);
    *ptr=0;                               // and path
    taglist[i].ti_Tag=WBOPENA_ArgLock;
    taglist[i].ti_Data=Lock( dir[n],SHARED_LOCK );
    i++;
    taglist[i].ti_Tag=WBOPENA_ArgName;
    taglist[i].ti_Data=(ULONG)file[n];
    i++;
    n++;
  }

  taglist[i].ti_Tag=TAG_END;
  strcpy( path,object );
  ptr=PathPart(path);
  *ptr=0;
  if( !path[0] ) strcpy( path,"c:" );
  lock=Lock(path,ACCESS_READ);
  if(lock) oldlock=CurrentDir(lock);
  OpenWorkbenchObjectA( (STRPTR)object,taglist );
  if(lock){ CurrentDir(oldlock); UnLock( lock ); }
  for( i=0;taglist[i].ti_Tag!=TAG_END;i++ ){
    if( (taglist[i].ti_Tag==WBOPENA_ArgLock)
     && (taglist[i].ti_Data!=NULL) ) UnLock( (BPTR)taglist[i].ti_Data );
  }
  delete path;
}
//<

//>"void InternalCommand( char *CommandLine,TFileList *list )"
void InternalCommand( char *CommandLine,TFileList *list ){
 char Command[64],Arg1[256],Arg2[256],dummy;
 int  IntArg=0;
 int  Count,i;

 Arg1[0]=0; Arg2[0]=0; IntArg=0;
 Count = sscanf( CommandLine,"%s %s %n",Command,Arg1,&IntArg ) - 1 ;
 if( IntArg ) strcpy( Arg2,&CommandLine[IntArg] );
 for( i=0;i<strlen(Command);i++ ) Command[i]=toupper(Command[i]);
 if( Count is 0 ) IntArg=Main->CurrentList;
 else{
  dummy = toupper(Arg1[0]);
  switch( dummy ){
    case 'L': IntArg=0; break; // left
    case 'R': IntArg=1; break; // right
    case 'S': IntArg=Main->CurrentList;   break; // source
    case 'D': IntArg=1-Main->CurrentList; break; // destination
    default:{
      IntArg=atoi(Arg1);
      if( IntArg is 1 ) IntArg = 1-Main->CurrentList; // destination
      else IntArg = Main->CurrentList;                // source
      break;
    }
  }
 }

 if( strcmp(Command,"ABOUT")==0 )               {RequesterVers();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"ADDICON")==0 )        AddIcon();
 else if( strcmp(Command,"ADDSUFFIX")==0 )      {list->Command=IC_ADDSUFFIX;RenameFile(list);}
 else if( strcmp(Command,"ALL")==0 )            {All();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"ASSIGN")==0 )         Assign( &CommandLine[7] );
 else if( strcmp(Command,"CLIPCOPY")==0 )       MC_Copy();
 else if( strcmp(Command,"CLIPPASTE")==0 )      MC_Paste();
 else if( strcmp(Command,"COPY")==0 )           {list->Command=FT_COPY; FileTransfer(list);}
 else if( strcmp(Command,"COPYAS")==0 )         {list->Command=FT_COPYAS; FileTransfer(list);}
 else if( strcmp(Command,"DELETE")==0 )         {list->Command=FT_DELETE; FileTransfer(list);}
 else if( strcmp(Command,"DUPLICATE")==0 )      {list->Command=FT_DUPLICATE; FileTransfer(list);}
 else if( strcmp(Command,"GETSIZE")==0 )        {GetSize( TRUE );list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"HELP")==0 )           {MC_Help();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"ICONIFY")==0 )        {IconifyWindow();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"LIST")==0 )
 {RemoveQuotes(Arg2);ListDir( IntArg,Arg2,0 );list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"MAKEDIR")==0 )        MkDir(list,IntArg);
 else if( strcmp(Command,"MOVE")==0 )           {list->Command=FT_MOVE; FileTransfer(list);}
 else if( strcmp(Command,"MOVEAS")==0 )         {list->Command=FT_MOVEAS; FileTransfer(list);}
 else if( strcmp(Command,"NONE")==0 )           {None();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"PARENT")==0 )         {Parent( IntArg );list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"PATTERN")==0 )        {Pat();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"POPUP")==0 )          PopupOnCurrentFiles();
 else if( strcmp(Command,"PREFS")==0 )          {Prefs();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"QUIT")==0 )           MC_Quit();
 else if( strcmp(Command,"REFRESH")==0 )        {Refresh( IntArg );list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"RENAME")==0 )         {list->Command=IC_RENAME;RenameFile(list);}
 else if( strcmp(Command,"ROOT")==0 )           {Root();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"SEARCH")==0 )         {Search();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"SNAPSHOT")==0 )       {SnapshotWindow();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"SWAP")==0 )           {Swap(Arg1[0]);list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"TOGGLE")==0 )         {Toggle();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"TRASH")==0 )          {list->Command=FT_TRASHCAN; FileTransfer(list);}
 else if( strcmp(Command,"TYPE")==0 )           {Type();list->DeselectMode=DESELECT_NONE;}
 else if( strcmp(Command,"WBINFO")==0 )         WbInfo();
 else if( strcmp(Command,"WINDOW")==0 )         {OpenDirWindow();list->DeselectMode=DESELECT_NONE;}

}
//<

//>"void ReleaseFileList( TFileList *list )"
void ReleaseFileList( TFileList *list ){
  while( !IsListEmpty(&list->Files) ) FreeVec( RemHead(&list->Files) );
  FreeMem( list,sizeof(TFileList) );
}
//<

//>"int Runner( TAction *act,char *filename=NULL )"
/****************************************/
/*                                      */
/* Function : Runner                    */
/*                                      */
/* Goal     : run the action act        */
/*            with filename             */
/*                                      */
/****************************************/
int Runner( TAction *act, char *filename ){
  BPTR handler,OldDirLock,CurrentDirLock;
  char oktodo, *reqbuffer[5];
//  char sta[20];
  char *command,*result,*SystemCommand;
  int  stack,delay,FilesSelected=0,i;
  BOOL bSuccess=FALSE,bFilenameUsed=FALSE, lha,lhad;
//  NotifyRequest NotifySrc,NotifyDst;
  TFileList *FiList;
  Object *lb  = GetCurrentListBrowser();
  TLBNode *node = NULL;
  List *Labels;
  TFileEntry *next;

  command = (char*)malloc(4000);
  result  = (char*)malloc(4000);
  SystemCommand    = (char*)malloc(4000);
  char *sCurrentDir = (char*)malloc(512);
  for( i=0;i<5;i++ ){
    reqbuffer[i] = (char*)malloc(512);
    reqbuffer[i][0] = 0;
  }

  FiList  = (TFileList*)AllocMem( sizeof(TFileList),MEMF_CLEAR );
  NewList( &FiList->Files );
  strcpy( FiList->Source,GetSrc() );
  if( act->Dest[0] ) strcpy( FiList->Destination,act->Dest );
  else strcpy( FiList->Destination,GetDst() );
  FiList->ReadSource = act->ReadSrc;
  FiList->ReadDestination = act->ReadDst;

  lha = (CheckPath(FiList->Source,result)==PATH_PARTEXISTS);
  lhad = (CheckPath(FiList->Destination,result)==PATH_PARTEXISTS);

  sCurrentDir[0]=0;
  switch( act->CD ){
    case 0:{
      if( (not lha) and FiList->Source[0] )
        strcpy( sCurrentDir,FiList->Source );
      break;
    }
    case 1:{
      if( (not lhad) and FiList->Destination[0] )
        strcpy( sCurrentDir,FiList->Destination );
      break;
    }
    case 2:{
      // extract path from command:
      StoreCommandProgramPath( act->Exec,sCurrentDir );
      break;
    }
  }
  RemoveQuotes( sCurrentDir );

  // now set the NotifyRequest:
/*
  memset( &NotifySrc,0,sizeof(NotifyRequest) );
  memset( &NotifyDst,0,sizeof(NotifyRequest) );
  NotifySrc.nr_Name  = FiList->Source;
  NotifySrc.nr_Flags = NRF_SEND_MESSAGE | NRF_WAIT_REPLY;
  NotifySrc.nr_stuff.nr_Msg.nr_Port = Main->NotifyPort;
  NotifySrc.nr_UserData = 0;
  if( !StartNotify( &NotifySrc ) ) FiList->ReadSource=TRUE;
  NotifyDst.nr_Name  = FiList->Destination;
  NotifyDst.nr_Flags = NRF_SEND_MESSAGE | NRF_WAIT_REPLY;
  NotifyDst.nr_stuff.nr_Msg.nr_Port = Main->NotifyPort;
  NotifyDst.nr_UserData = 1;
  if( !StartNotify( &NotifyDst ) ) FiList->ReadDestination=TRUE;
*/


//  if( !filename ){
    GetAttr( LISTBROWSER_Labels,(Gadget*)lb,(ULONG*)&Labels );
    ULONG selected;
    node = (TLBNode*)Labels->lh_Head;
    while( node->ln_Succ ){
      GetListBrowserNodeAttrs( (Node*)node,LBNA_Selected,&selected,TAG_END );
      if( selected ){
        next = (TFileEntry*)AllocVec( sizeof(TFileEntry),MEMF_CLEAR );
        AddTail( &FiList->Files,(Node*)next );
        FiList->Count++;
        strcpy( next->Name,node->Fn->Filename );
        next->Selected = TRUE;
        next->Success  = FALSE;
      }
      node = (TLBNode*)node->ln_Succ;
    }
//  }

  next = (TFileEntry*)FiList->Files.lh_Head;
  do{
   if( filename ){               // filename as argument
    strcpy( SystemCommand,filename );
   }else if( next->ln_Succ ){             // filename from the current listbrowser
    strcpy( SystemCommand,FiList->Source );
    AddPart( SystemCommand,next->Name,512 );
   }else SystemCommand[0]=0;              // no filename at all - hope it isn't needed

   strcpy( command,act->Exec );

   // first replace arguments
   while( (oktodo=ParseArgs(command,SystemCommand,FiList,result)) > -1 ){
    strcpy(command,result);
    if( oktodo is 1 ) bFilenameUsed = TRUE;
   }
   // then replace requesters
   while( (oktodo=ParseReq(command,SystemCommand,reqbuffer,result)) > -1 ) strcpy(command,result);
   if( oktodo > -2 ){
    if( act->Type is RUN_WB ){
     WbStart( result );
    }else if( act->Type is RUN_INTERNAL ){
      if( !Main->IsIcon ) InternalCommand( result,FiList );
    }else{
     stack = atoi( act->Stack );
     if( stack<30000 ) stack = 30000;
     if( act->Type is RUN_CLI ){
      // open the console:
      delay = atoi(act->Delay);
      char *charbuffer=(char*)malloc(500);
      if( delay < 0 ) sprintf(charbuffer,"%s/AUTO/WAIT/CLOSE",Main->DevString);
      else sprintf(charbuffer,"%s/AUTO",Main->DevString);
      if( !(handler=Open(charbuffer,MODE_OLDFILE)) )
        handler=Open("NIL:",MODE_OLDFILE);
      free(charbuffer);
      // build the cli command:
/*
      if( act->CdDest and (not lhad) and FiList->Destination[0] and (FiList->Destination[1] isnot Quote) ){
        sprintf(charbuffer,"cd %c%s%c",'"',FiList->Destination,'"');        // change dir
      }else if( (not act->CdDest) and (not lha) and FiList->Source[0] and (FiList->Source[1] isnot Quote) ){
        sprintf(charbuffer,"cd %c%s%c",'"',FiList->Source,'"');        // change dir
      }else charbuffer[0]=0;
*/
      if( delay > 0 )
       sprintf(SystemCommand,"%s%cwait %d",result,10,delay); // all together
      else strcpy(SystemCommand,result);
//         sprintf(command,"wait %d",delay); else strcpy(command,";");      // wait
//      sprintf(SystemCommand,"%s%c%s%c%s%c",charbuffer,10,result,10,command,10); // all together
//      sprintf(SystemCommand,"%s%c%s",result,10,command); // all together
     }else{
      handler=Open("NIL:",MODE_OLDFILE);
      if( act->Type is RUN_NOCLI ){
/*
        char *charbuffer=(char*)malloc(500);
        if( act->CdDest and (not lhad) and FiList->Destination[0] and (FiList->Destination[1] isnot Quote) ){
          sprintf(charbuffer,"cd %c%s%c",'"',FiList->Destination,'"');        // change dir
        }else if( (not act->CdDest) and (not lha) and FiList->Source[0] and (FiList->Source[1] isnot Quote) ){
          sprintf(charbuffer,"cd %c%s%c",'"',FiList->Source,'"');        // change dir
        }else strcpy( charbuffer,";" );
        sprintf( SystemCommand,"%s%c%s%c",charbuffer,10,result,10 ); // all together
*/
        strcpy( SystemCommand,result );
//        free( charbuffer );
      }else if( act->Type is RUN_AREXX ) sprintf( SystemCommand,"sys:rexxc/rx %s",result );
     }

     if( (sCurrentDir[0] is 0) or !(CurrentDirLock=Lock(sCurrentDir,ACCESS_READ)) )
       CurrentDirLock=Lock("sys:",ACCESS_READ);
     if( CurrentDirLock ) OldDirLock = CurrentDir(CurrentDirLock);
     if( act->Async ){
       // do not reread directory, so start asynchronous:
       if( SystemTags(SystemCommand,SYS_Input,handler,SYS_Output,NULL,SYS_Asynch,TRUE,
           NP_StackSize,stack,TAG_DONE)!=0 ){
         Close(handler);
        // if SystemTags is successfull, it does that itself
       }
     }else{
       SystemTags( SystemCommand,SYS_Input,handler,SYS_Output,NULL,SYS_Asynch,FALSE,
         NP_StackSize,stack,TAG_DONE );
       Close( handler );
     }
     CurrentDir( OldDirLock );
     UnLock( CurrentDirLock );
    }
   }
   bSuccess = TRUE;
   if( next->ln_Succ ) next = (TFileEntry*)next->ln_Succ;
  }while( bFilenameUsed and next->ln_Succ ); // only continue if we must process all files

/*
  NotifyMessage *msg;
  while( msg=(NotifyMessage*)GetMsg(Main->NotifyPort) ){
    if( msg->nm_NReq->nr_UserData is 0 ) FiList->ReadSource=TRUE;
    else FiList->ReadDestination=TRUE;
    ReplyMsg( (Message*)msg );
  }
*/

  if( FiList->ReadSource ){
    Refresh(Main->CurrentList);
    if( strcmp(GetSrc(),FiList->Destination) is 0 ) FiList->ReadDestination = FALSE;
  }else if( FiList->DeselectMode is DESELECT_ALL ) None();
  else if( FiList->DeselectMode is DESELECT_LIST ) DeselectList( FiList );

  // if action on other dir, don't read dest:
  CheckPath( act->Dest,act->Dest ); // get correct name to compare with destination path
  if( act->Dest[0] and (stricmp(act->Dest,GetDst()) isnot 0) ) FiList->ReadDestination = FALSE;
  if( FiList->ReadDestination ) Refresh(1-Main->CurrentList);

  if( not FiList->Static ){
    ReleaseFileList( FiList );
  }
/*
  EndNotify( &NotifySrc );
  EndNotify( &NotifyDst );
*/

  for( i=0;i<5;i++ ) free(reqbuffer[i]);
  free(sCurrentDir);
  free(command);
  free(result);
  free(SystemCommand);
  return(0);
}
//<

