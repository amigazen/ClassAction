/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File: calha.cpp

        Aim:  Archive class

        Ver:  4.1

****************************************/
#include "calha.h"

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <proto/xadmaster.h>
#include <libraries/xadmaster.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "cautils.h"

extern Library  *DOSBase;

TArchiveList *ArchiveList;

struct xadMasterBase *xadMasterBase=NULL;

//>"BOOL XAD_IsArchive( char *file,char **type=NULL )"
BOOL XAD_IsArchive( char *file,char **type ){
  if( xadMasterBase==NULL ){
    if( !(xadMasterBase =(struct xadMasterBase *) OpenLibrary("xadmaster.library", 7)) )
      return( FALSE );
  }
  BOOL Result=FALSE;
  BPTR Handle;
  char *Buffer;
  xadClient *client;
  ULONG size=xadMasterBase->xmb_RecogSize;
  if( Handle=Open(file,MODE_OLDFILE) ){
    FileInfoBlock *fib=(FileInfoBlock*)AllocDosObject(DOS_FIB,NULL);
    ExamineFH(Handle,fib);
    if( fib->fib_Size<size ) size=fib->fib_Size;
    Buffer=(char*)AllocVec(size,MEMF_CLEAR);
    Read( Handle,Buffer,size );
    if( client=xadRecogFileA(size,Buffer,NULL) ){
      if( type ) *type=client->xc_ArchiverName;
      Result=TRUE;
    }
    FreeVec(Buffer);
    FreeDosObject(DOS_FIB,fib);
    Close(Handle);
  }
  return( Result );
}
//<

//>"List *TArchive::InsertDir( char *dir, xadFileInfo *finfo  )"
List *TArchive::InsertDir( char *dir, xadFileInfo *finfo  ){
 char *ptr,*localname;
 List *ldir=NULL,*parent;
 FNode *node;
 ptr=new char[512];
 strcpy( ptr,dir );

 // divide the dir in parent and localname
 localname=strrchr( ptr,'/' );
 if( localname ){
  // parent exists, so get it or create it:
  *localname=0;
  localname++;
  if( ptr[0]!=0 ) parent=this->Directory( ptr,finfo );
  else parent=Files;   // it's possible that the name begins with '/'
  if( parent==NULL ) parent=this->InsertDir( ptr,finfo );
 }else{
  // we are in the root dir:
  localname=ptr;
  parent=Files;
 }
 if( node=(FNode*)AllocVec( sizeof(FNode),MEMF_CLEAR ) ){
  strcpy( node->Name,dir );
  strcpy( node->Fib.fib_FileName,localname );
  node->Fib.fib_Size=0;
  node->Fib.fib_DirEntryType=1;
  // we take the date of the given file, although this is probably not the right one:
  xadConvertDates( XAD_DATEXADDATE, &finfo->xfi_Date,
                  XAD_GETDATEDATESTAMP, &node->Fib.fib_Date, TAG_DONE);
  node->Fib.fib_Protection=FIBF_READ | FIBF_WRITE | FIBF_EXECUTE | FIBF_DELETE ;
  NewList( &(node->Children) );
  ldir=&(node->Children);
  AddTail( parent,(Node*)node );
 }
 delete ptr;
 return ldir;
}
//<

//>"void TArchive::InsertFNode( xadFileInfo *finfo )"
void TArchive::InsertFNode( xadFileInfo *finfo ){
 BOOL found=FALSE,dir=FALSE;
 char *ptr,*localname;
 List *parent;
 FNode *node;

 ptr=new char[512];
 strcpy( ptr,finfo->xfi_FileName );

 // strip a trailing slash:
 if( ptr[strlen(ptr)-1]=='/' ){
  ptr[strlen(ptr)-1]=0;
 }

 // if it is a dir, check if it already exists:
 if( this->Directory( ptr,finfo ) ){
  delete ptr;
  return;
 }

 // divide the name in parent and localname:
 localname=strrchr( ptr,'/' );
 if( localname ){
  // parent exists, so get it or create it:
  *localname=0;
  localname++;
  if( ptr[0]!=0 ) parent=this->Directory( ptr,finfo );
  else parent=Files;   // it's possible that the name begins with '/'
  if( parent==NULL ) parent=this->InsertDir( ptr,finfo );
 }else{
  // we are in the root dir:
  localname=ptr;
  parent=Files;
 }

 // now create the node:
 if( node=(FNode*)AllocVec( sizeof(FNode),MEMF_CLEAR ) ){
  strcpy( node->Name,finfo->xfi_FileName );
  strcpy( node->Fib.fib_FileName,localname );
  node->Fib.fib_Size=finfo->xfi_Size;
  node->EntryNumber=finfo->xfi_EntryNumber;
  if( finfo->xfi_Flags & XADFIF_DIRECTORY ) node->Fib.fib_DirEntryType=1;
  else node->Fib.fib_DirEntryType=-1;
  xadConvertDates( XAD_DATEXADDATE, &finfo->xfi_Date,
                  XAD_GETDATEDATESTAMP, &node->Fib.fib_Date, TAG_DONE);
  node->Fib.fib_Protection=finfo->xfi_Protection;
  NewList( &node->Children );
  AddTail( parent,(Node*)node );
 }

 delete ptr;
}
//<

//>"List *TArchive::Directory( char *dir, xadFileInfo *finfo  )"
List *TArchive::Directory( char *dir, xadFileInfo *finfo  ){
 char *ptr,*p2;
 List *ldir;
 Node *node;
 struct DateStamp dat;
 BOOL found=FALSE;

 if( dir[0]==0 ) return( Files );       // root dir!
 ptr=new char[512];
 strcpy( ptr,dir );
 p2=ptr;
 p2=strrchr( ptr,'/' ); // the last occurence of '/'
 if( p2 ){
  *p2=0;
  ldir=Directory( ptr,finfo );       // search the last dir
 }else ldir=Files;             // root dir
 if( ldir ){
  node=ldir->lh_Head;
  while( node->ln_Succ&&!found ){
   if( (stricmp(dir,((FNode*)node)->Name)==0)
        &&(((FNode*)node)->Fib.fib_DirEntryType>0) ){
    found=TRUE;
    // we confirm that the date of the file is not younger than that of the dir:
    if( finfo ){
      xadConvertDates(
        XAD_DATEXADDATE, &finfo->xfi_Date,
        XAD_GETDATEDATESTAMP, &dat, TAG_DONE
      );
      if( CompareDates(&((FNode*)node)->Fib.fib_Date,&dat)>0 ){
        memcpy(&((FNode*)node)->Fib.fib_Date,&dat,sizeof(struct DateStamp));
      }
    }
    ldir=&(((FNode*)node)->Children);
   }
   node=node->ln_Succ;
  }
 }
 delete ptr;
 if( found ) return( ldir );
 else return( NULL );
}
//<

//>"FNode *TArchive::File( char *src )"
FNode *TArchive::File( char *src ){
 char *ptr,*p2;
 List *parent;
 Node *node;
 BOOL found=FALSE;

 if( src[0]==0 ) return( NULL );       // root dir!
 ptr=new char[512];
 strcpy( ptr,src );
 p2=ptr;
 p2=strrchr( ptr,'/' ); // the last occurence of '/'
 if( p2 ){
  *p2=0;
  parent=Directory( ptr,NULL );       // search the last dir
 }else parent=Files;             // root dir
 delete ptr;

 if( parent ){
  node=parent->lh_Head;
  while( node->ln_Succ&&!found ){
   if( stricmp(src,((FNode*)node)->Name)==0 ) found=TRUE;
   if( !found ) node=node->ln_Succ;
  }
 }else return( NULL );
 if( found ) return( (FNode*)node );
 else return( NULL );
}
//<

//>"void TArchive::DeleteList( List *list )"
void TArchive::DeleteList( List *list ){
 FNode *cur;
 while( ! IsListEmpty( list ) ){
  cur=(FNode*)(list->lh_Head);
  if( !IsListEmpty(&(cur->Children)) ) DeleteList( &(cur->Children) );
  Remove( (Node*)cur );
  FreeVec( cur );
 }
}
//<

//>"TArchive::TArchive()"
TArchive::TArchive(){
 Files = NULL;
 CurrentPath=NULL;
 CurrentFile=NULL;
 Filename=new char[512];
 Path=new char[512];
 ArchiveInfo=NULL;
 lock=FALSE;
}
//<

//>"TArchive::~TArchive()"
TArchive::~TArchive(){
 delete [] Filename;
 delete [] Path;
 Error=0L;
}
//<

//>"BPTR TArchive::Lock( char *name )"
BPTR TArchive::Lock( char *name ){
 char *ptr;
 strcpy( Filename,name );
 xadFileInfo *Fifo;

 // now we will extract an internal path from the given name
 if( ptr=stristr(Filename,".lha") )        Type=TYPE_LHA;
 else if( ptr=stristr(Filename,".lzx") )   Type=TYPE_LZX;
 else if( ptr=stristr( Filename,".zip" ) ) Type=TYPE_ZIP;
 else if( ptr=stristr( Filename,".ha" ) )  Type=TYPE_HA;
 else Type=TYPE_UNKNOWN;

 // reduce name to the archive file:
 CheckPath( Filename,Filename );
 // the rest is the internal path:
 int n=strlen(Filename);
 if( name[n]==0 ) Path[0]=0;
 else strcpy( Path,&name[n+1] );

/*
 ptr2=strchr( ptr,'/' );
 if( ptr2==NULL ) Path[0]=0;
 else{
  *ptr2=0;
  ptr2++;
  strcpy( Path,ptr2 );
 }
*/
 if( Files=ArchiveList->GetList(Filename) ){
   lock=TRUE;
 }else{
   // not yet read, so start XAD:
   if( !(ArchiveInfo = (xadArchiveInfo *) xadAllocObjectA(XADOBJ_ARCHIVEINFO, 0)) ){
    Error=ARCHIVE_MEM;
    return( NULL );
   }
   Error=xadGetInfo( ArchiveInfo, XAD_INFILENAME, Filename, TAG_DONE );
   if( Error ){
     Error=ARCHIVE_NOINFO;
     xadFreeObjectA( ArchiveInfo,0 );
     return( NULL );
   }
   if( ArchiveInfo->xai_Flags & XADAIF_FILECORRUPT ){
     Error=ARCHIVE_CORRUPT;
     this->UnLock();
     return( NULL );
   }
   Files=ArchiveList->AddList(Filename);
   lock=TRUE;

   // now store all the files in the archive:
   Fifo=ArchiveInfo->xai_FileInfo;

   while( Fifo ){
    this->InsertFNode( Fifo );
    Fifo=Fifo->xfi_Next;
   }
 }

 // we have to set the pointer to the list of files of the given dir:
 CurrentPath=this->Directory( Path,NULL );
 // return a pointer to the file if it exists, just to
 // simulate the dos 'Lock'
 BPTR lock;
 if( CurrentPath ){
  CurrentFile=(FNode*)CurrentPath->lh_Head;
  return( (BPTR)CurrentPath );
 }else{
  CurrentFile=NULL;
  if( lock=(BPTR)this->File(Path) ) return( lock );
  else{
   this->UnLock();
   return( NULL );
 }}
}
//<

//>"void TArchive::UnLock()"
void TArchive::UnLock(){
 if( ArchiveInfo ){
  xadFreeInfo( ArchiveInfo );
  xadFreeObjectA( ArchiveInfo,0 );
  ArchiveInfo=NULL;
 }
 Files=NULL;
 lock=FALSE;
}
//<

//>"BOOL TArchive::Examine( FileInfoBlock *fib )"
BOOL TArchive::Examine( FileInfoBlock *fib ){
 FNode *file;

 if( !lock ){ Error=ARCHIVE_ERROR; return( FALSE ); }
 file=this->File( Path );
 if( file ) memcpy( fib, &(file->Fib),sizeof(FileInfoBlock) );

 return( TRUE );
}
//<

//>"BOOL TArchive::ExNext( FileInfoBlock *fib )"
BOOL TArchive::ExNext( FileInfoBlock *fib ){

 if( !lock ){ Error=ARCHIVE_ERROR; return( FALSE ); }
 if( CurrentFile->ln_Succ==NULL ) return( FALSE );

 memcpy( fib,&(CurrentFile->Fib),sizeof(FileInfoBlock) );

 CurrentFile=(FNode*)CurrentFile->ln_Succ;

 return( TRUE );
}
//<

//>"BOOL TArchive::PathValid( char * )"
BOOL TArchive::PathValid( char *name ){
 char *ptr,*ptr2;
 BOOL succ;

 strcpy( Filename,name );

 // now we will extract an internal path from the given name
 ptr=stristr( Filename,".lha" );
 if( ptr==NULL ) ptr=stristr( Filename,".lzx" );
 if( ptr==NULL ) ptr=stristr( Filename,".zip" );
 if( ptr==NULL ){ Error=ARCHIVE_ERROR; return( FALSE ); }
 ptr2=strchr( ptr,'/' );
 if( ptr2==NULL ) Path[0]=0;
 else{
  *ptr2=0;
  ptr2++;
  strcpy( Path,ptr2 );
 }

 if( !(ArchiveInfo = (xadArchiveInfo *) xadAllocObjectA(XADOBJ_ARCHIVEINFO, 0)) ){
  Error=ARCHIVE_MEM; return( FALSE );
 }

 Error=xadGetInfo( ArchiveInfo, XAD_INFILENAME, Filename, TAG_DONE );
 if( Error ){ Error=ARCHIVE_NOINFO; return( FALSE ); }

 if( ArchiveInfo->xai_Flags & XADAIF_FILECORRUPT ){ Error=ARCHIVE_CORRUPT; return( FALSE ); }

 FileInfoBlock *fib=(FileInfoBlock *)AllocDosObject( DOS_FIB,NULL );
 succ=this->ExNext( fib );
 FreeDosObject( DOS_FIB,fib );

 if( ArchiveInfo ){
  xadFreeInfo( ArchiveInfo );
  xadFreeObjectA( ArchiveInfo,0 );
 }
 Filename[0]=0;
 Path[0]=0;
 return( succ );
}
//<

//>"void TArchive::ArchiveName( char *string )"
void TArchive::ArchiveName( char *string ){
  CheckPath( string,string );
}
//<

//>"void TArchive::LocalName( char *string )"
void TArchive::LocalName( char *string ){
  char *lhafile=new char[512];

  if( CheckPath(string,lhafile)==PATH_PARTEXISTS ){
    int lha_len=strlen(lhafile);
    if( lhafile[lha_len-1]==':' ) strcpy( lhafile,&string[lha_len] );
    else strcpy( lhafile,&string[lha_len+1] );
    strcpy( string,lhafile );
  }else string[0]=0;
  delete [] lhafile;
}
//<

//>"BOOL TArchive::UnpackFile( char *src, char *filename )"
BOOL TArchive::UnpackFile( char *src, char *filename ){
 FNode *srcnode,*child;
 char *childdir;

 if( !lock ) return( FALSE );

 if( !(srcnode=this->File( src )) ) return( FALSE );

 if( IsListEmpty( &(srcnode->Children) ) ){
   if( !ArchiveInfo ){
     if( !(ArchiveInfo = (xadArchiveInfo*)xadAllocObjectA(XADOBJ_ARCHIVEINFO, 0)) ){
       Error=ARCHIVE_MEM;
       return( FALSE );
     }
     Error=xadGetInfo( ArchiveInfo, XAD_INFILENAME, Filename, TAG_DONE );
     if( Error ){
       Error=ARCHIVE_NOINFO;
       xadFreeObjectA( ArchiveInfo,0 );
       return( NULL );
     }
   }
   xadFileUnArc( this->ArchiveInfo,
                XAD_ENTRYNUMBER,srcnode->EntryNumber,
                XAD_OUTFILENAME,filename,
                XAD_OVERWRITE,TRUE,
                XAD_MAKEDIRECTORY,TRUE,TAG_END );
 }else{
  childdir=new char[512];
  child=(FNode*)(srcnode->Children.lh_Head);
  while( child->ln_Succ!=NULL ){
   sprintf( childdir,"%s/%s",filename,child->Fib.fib_FileName );
   this->UnpackFile( child->Name,childdir );
   child=(FNode*)(child->ln_Succ);
  }
  delete childdir;
 }
 return( TRUE );
}
//<

TArchiveList::TArchiveList(){
  if (xadMasterBase==NULL){
    xadMasterBase = (struct xadMasterBase*) OpenLibrary("xadmaster.library", 7);
  }
  NewList( &Archives );
  Count=0;
}

TArchiveList::~TArchiveList(){
  while( Count>0 ) RemoveList( (TFileListNode*)Archives.lh_Head );
  if (xadMasterBase) CloseLibrary( (Library*) xadMasterBase );
}

List *TArchiveList::AddList( char *Name ){
  TFileListNode *fln;
  if( Count>9 ) RemoveList( (TFileListNode*)&Archives.lh_Head );
  fln = (TFileListNode*)AllocVec( sizeof(TFileListNode),MEMF_CLEAR );
  strcpy( fln->Name,Name );
  NewList( &fln->Files );
  AddTail( &Archives,(Node*)fln );
  Count++;
  return &fln->Files;
}

List *TArchiveList::GetList( char *Name ){
  for( TFileListNode *fln=(TFileListNode*)Archives.lh_Head;fln->ln_Succ;fln=(TFileListNode*)fln->ln_Succ ){
    if( stricmp(fln->Name,Name)==0 ) return &fln->Files;
  }
  return NULL;
}

void TArchiveList::RemoveList( char *Name ){
  for( TFileListNode *fln=(TFileListNode*)Archives.lh_Head;fln->ln_Succ;fln=(TFileListNode*)fln->ln_Succ ){
    if( stricmp(fln->Name,Name)==0 ){
      RemoveList( fln );
      return;
    }
  }
}

void TArchiveList::RemoveDir( List *list ){
  FNode *cur;
  while( !IsListEmpty( list ) ){
    cur=(FNode*)(list->lh_Head);
    if( !IsListEmpty(&(cur->Children)) ) RemoveDir( &(cur->Children) );
    Remove( (Node*)cur );
    FreeVec( cur );
  }
}

void TArchiveList::RemoveList( TFileListNode *list ){
  RemoveDir( &list->Files );
  Remove( (Node*)list );
  FreeVec( list );
  Count--;
}
