#include "ca_begin.h"
/****************************************

        Class Action Source

        � 2002 by Martin R. Elsner & Salim Gasmi

        File: filetypes.cpp

****************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clib/alib_protos.h>
#include <clib/datatypes_protos.h>
#include <datatypes/datatypesclass.h>
#include <clib/exec_protos.h>
#include <exec/types.h>
#include <exec/lists.h>
#include <clib/dos_protos.h>
#include <dos/dos.h>
#include <clib/intuition_protos.h>
#include <exec/memory.h>

#include "caxfd.h"
#include "calha.h"
#include "unBitmap.h"
#include "cafiletypes.h"
#include "catypes.h"
#include "cautils.h"

TcaFileTypes *caFileTypes;

//>"void TcaFileTypes::TcaFileTypes()"
/****************************************/
/*                                      */
/* Function : InitLists                 */
/*                                      */
/* Goal     : initialize the used lists */
/*                                      */
/* Return Values : NONE                 */
/*                                      */
/****************************************/
TcaFileTypes::TcaFileTypes(){
  NewList( &FileTypes );
  Count = 0;
}
//<

//>"TcaFileTypes::~TcaFileTypes()"
TcaFileTypes::~TcaFileTypes(){
  Clear();
}
//<

//>"void TcaFileTypes::Clear()"
void TcaFileTypes::Clear(){
  ClearFileTypes();
}
//<

//>"TFileType *TcaFileTypes::AddFileType( List *li,char *name )"
TFileType *TcaFileTypes::AddFileType( char *name ){
  TFileType *ft = (TFileType*)AllocVec(sizeof(TFileType),MEMF_CLEAR);
  if( ft ){
    strncpy( ft->Name,name,32 );
    ft->ImageFile[0]=0;
    ft->ImageStatus=IMAGE_NULL;
    NewList( &ft->Actions );
    ft->ActionCount = 0;
    AddTail( &FileTypes,(Node*)ft );
    Count++;
  }
  return( ft );
}
//<

//>"TAction *TcaFileTypes::AddAction( TFileType *ft,char *name )"
TAction *TcaFileTypes::AddAction( TFileType *ft,char *name ){
  TAction *act = caActions->CreateAction( name );
  if( ft ){
    AddTail( &ft->Actions,(Node*)act );
    ft->ActionCount++;
  }
  return( act );
}
//<

//>"void TcaFileTypes::RemoveFileType( TFileType *ft,BOOL RemoveFromList=FALSE )"
void TcaFileTypes::RemoveFileType( TFileType *ft,BOOL RemoveFromList ){
  Node *act;
  if( RemoveFromList ) Remove( (Node*)ft );
  while( act=RemHead(&ft->Actions) ) FreeVec( act );
  if( (ft->ImageStatus==IMAGE_LOADED) && ft->ListerImage ) delete ft->ListerImage;
  FreeVec( ft );
}
//<

//>"void TcaFileTypes::DeleteAction( TAction *act,BOOL RemoveFromList=FALSE )"
void TcaFileTypes::RemoveAction( TAction *act,BOOL RemoveFromList ){
  if( RemoveFromList ) Remove( (Node*)act );
  FreeVec( act );
}
//<
/*
//>"void TcaFileTypes::ClearSuffixTypes()"
void TcaFileTypes::ClearSuffixTypes(){
  TSuffixTypeNode *ftn;
  while( !IsListEmpty(&SuffixTypes) ){
    ftn = (TSuffixTypeNode*)RemHead(&SuffixTypes);
    if( ftn->ImageLoaded ) delete ftn->ListerImage;
    FreeMem( ftn,sizeof(TSuffixTypeNode) );
  }
}
//<
*/
//>"void TcaFileTypes::ClearFileTypes()"
void TcaFileTypes::ClearFileTypes(){
  while( !IsListEmpty(&FileTypes) )
    RemoveFileType( (TFileType*)FileTypes.lh_Head,TRUE );
}
//<

/*
//>"void TcaFileTypes::LoadSuffixTypes( Screen *s,BOOL LittleImages )"
void TcaFileTypes::LoadSuffixTypes( Screen *s,BOOL LittleImages ){
  BPTR lock;
  FileInfoBlock *fib;
  TSuffixTypeNode *ftn;
  char FileName[512];
  char *ptr,*fptr;
  ClearSuffixTypes();
  strcpy( FileName,CA_IMG_FILETYPES );
  if( LittleImages ) strcat( FileName,"/Size11/ext" );
  else strcat( FileName,"/Size16/ext" );
  if( lock=Lock(FileName,ACCESS_READ) ){
    fib=(FileInfoBlock *)AllocDosObject( DOS_FIB,NULL );
    Examine(lock,fib);
    strcat( FileName,"/" );
    fptr=FileName+strlen(FileName);
    while( ExNext(lock,fib) ){
      ftn=(TSuffixTypeNode*)AllocMem( sizeof(TSuffixTypeNode),MEMF_CLEAR );

      strcpy( ftn->Suffix,fib->fib_FileName );
      strcpy( fptr,fib->fib_FileName );
      strcpy( ftn->ImageFileName,FileName );
      if( ptr=strchr(ftn->Suffix,'.') ) *ptr=0;
      // ftn->ListerImage = NULL;
      ftn->ImageScreen = s;
      ftn->LittleImages = LittleImages;
      // ftn->ImageLoaded = FALSE;
      AddTail( &SuffixTypes,(Node*)ftn );
    }
    FreeDosObject( DOS_FIB,fib );
    UnLock( lock );
  }
}
//<
*/

//>"int FileOffsetCorrect(struct TFileType *ft,BPTR File)"
/****************************************/
/*                                      */
/* Function:  FileOffsetCorrect         */
/*                                      */
/* Aim:       determine if the file     */
/*            handled by 'File' has the */
/*            offsets of  Class 'cl'    */
/*                                      */
/* Output:   0  : no offsets given      */
/*           -1 : offsets wrong         */
/*           else: number of offsets    */
/*                                      */
/****************************************/

int FileOffsetCorrect(struct TFileType *ft,BPTR File){
 int i=0,j;
 char tmp[1024],val[1024];
 char hex[3];
 int Offset;
 unsigned char c;
 char *x;
 char Value[200];
 char AsciiString[200];
 int  found=0;

 while( i<5 ){
  strcpy( val,"");
  strcpy( tmp,ft->Value[i] );
  x=strchr(tmp,',');
  if( x!=NULL ){
   found++;
   *x=0;
   Offset=atoi( tmp );
   strcpy( Value,x+1 );

   if( Seek( File,Offset,OFFSET_BEGINNING )==-1 ) return( -1 );

   if( Value[0]=='"' ){
    strcpy( AsciiString,Value+1 );
    AsciiString[strlen(AsciiString)-1]=0;
    Read( File,tmp,strlen(AsciiString) );
    if( strnicmp(AsciiString,tmp,strlen(AsciiString))!=0 ) return( -1 );
   }else if( Value[0]==39 ){ // simple '
    strcpy( AsciiString,Value+1 );
    AsciiString[strlen(AsciiString)-1]=0;
    Read( File,tmp,strlen(AsciiString) );
    if( strncmp(AsciiString,tmp,strlen(AsciiString))!=0 ) return( -1 );
   }else{
    Read( File,tmp,strlen(Value)/2 );
    for(j=0;j<strlen(Value)/2;j=j+1){
     c=tmp[j];
     sprintf(hex,"%02x",c);
     strcat(val,hex);
    }
    if( stricmp(val,Value)!=0 ) return( -1 );
   }
  }
  i++;
 }
 return( found );
}
//<

//>"int CheckBufferOffset(struct TFileType *ft,char *buff,int buflen)"
/****************************************/
/*                                      */
/* Function:  CheckBufferOffset         */
/*                                      */
/* Aim:       determine if the buffer   */
/*            'buff' has valid offsets  */
/*            with the FileType 'ft'    */
/*                                      */
/*                                      */
/* Return Values:                       */
/*                                      */
/* 0 = No Valid Offsets found           */
/* 1 = Valid Offsets found              */
/*                                      */
/****************************************/
int CheckBufferOffset(struct TFileType *ft,char *buff,int buflen){
 int good;
 int i,j;
 char tmp[1024],val[1024];
 char hex[3];
 int off;
 unsigned char c;
 char *x;
 char vax[200];
 char asc[200];

 good=0;

 for(i=0;i<5;i++){
  strcpy(val,"");
  strcpy(tmp,ft->Value[i]);
  x=strchr(tmp,',');
  if(x!=NULL){
   *x=0;
   off=atoi(tmp);
   strcpy(vax,x+1);

   if(buflen<off) return(0);

   if(vax[0]=='"'){
    strcpy(asc,vax+1);
    asc[strlen(asc)-1]=0;
    if(strnicmp(asc,buff+off,strlen(asc))!=0) return(0);
    else good++;
   }

   if(vax[0]==39){ // simple '
    strcpy(asc,vax+1);
    asc[strlen(asc)-1]=0;
    if(strncmp(asc,buff+off,strlen(asc))!=0) return(0);
    else good++;
   }

   if(vax[0]!='"' && vax[0]!=39){ // '
    for(j=0;j<strlen(vax)/2;j=j+1){
     c=buff[j+off];
     sprintf(hex,"%02x",c);
     strcat(val,hex);
    }
    if(stricmp(val,vax)!=0) return(0); else good++;
   }
  }
 }
 if(good==0) return(0); else return(1);
}
//<

//>"int IsAscii(char *file)"
/****************************************/
/* Function: IsAscii                    */
/*                                      */
/* Aim:      Determine if 'file' is an  */
/*            ASCII file                */
/*                                      */
/* Return Values:                       */
/*                                      */
/*      1: File == ASCII                */
/*      0: File == ! ASCII            */
/*     -1: File ! found               */
/*     -3: GetDatatype Failed           */
/*                                      */
/****************************************/
int IsAscii(char *file){
 struct DataTypeHeader *dth;
 struct DataType *dtn;
 BPTR lock;
 char *s;
 char ret;

 lock = Lock (file, ACCESS_READ);
 if(lock==NULL) return(-1);

 dtn = ObtainDataTypeA (DTST_FILE, (APTR) lock, NULL);
 if(dtn==NULL){
  UnLock(lock);
  return(-3);
 }

 dth = dtn->dtn_Header;

 s=GetDTString ((dth->dth_Flags & DTF_TYPE_MASK) + DTMSG_TYPE_OFFSET);
 for(ret=0;ret<strlen(s);ret++) s[ret]=tolower(s[ret]);
 if(strstr(s,"ascii")!=NULL) ret=1; else ret=0;

 ReleaseDataType (dtn);
 UnLock (lock);
 return(ret);
}
//<

//>"TFileType *TcaFileTypes::GetFileType( char *full,char *name,BOOL simple,BOOL decrunch=FALSE )"
/****************************************/
/*                                      */
/* Function:  GetClass                  */
/*                                      */
/* Aim:       Determine the Class of    */
/*            the file 'fullname'       */
/*            with local name 'name'.   */
/*            If the file == Crunched   */
/*            we try to decrunch it.    */
/* Input:     full = full name of file  */
/*            name = name of file       */
/*                                      */
/* Output:    pointer to the determined */
/*             Class                    */
/*                                      */
/****************************************/
TFileType *TcaFileTypes::GetFileType( char *full,char *name,BOOL simple,BOOL decrunch ){
 TFileType *ft,*maxft,*FirstFileType;
 char *buff,tmp[1024];
 int len,newlen,offsetsfound,maxoffsetsfound;
 APTR dec;
 BPTR han;

 strcpy(Cruncher,"");

 /* We search with Matchnames */

 ft = (TFileType*)FileTypes.lh_Head;
 // now leave the internal classes:
 while( ft->ln_Succ && (ft->Name[0] == 0) ) ft = (TFileType*)ft->ln_Succ;
 FirstFileType = ft;
 while( ft->ln_Succ ){
  if( ft->ParsedPattern[0] ){
   if( MatchPatternNoCase((UBYTE *)ft->ParsedPattern,name) ) return(ft);
  }
  ft = (TFileType*)ft->ln_Succ;
 }
 if( simple ) return( Unknown );

 // we search with offsets
 han = Open(full,MODE_OLDFILE);
 maxoffsetsfound=0;
 ft = FirstFileType;
 while( ft->ln_Succ ){
  offsetsfound = FileOffsetCorrect( ft,han );
  if( offsetsfound>maxoffsetsfound ){
   maxoffsetsfound = offsetsfound;
   maxft = ft;
  }
  ft = (TFileType*)ft->ln_Succ;
 }
 Close( han );
 if( maxoffsetsfound>0 ) return( maxft );

 /* se search with offsets && decrunching */

 if( decrunch ){
  FileInfoBlock *fib;
  BPTR flock;

  len = 0;
  flock = Lock( full,ACCESS_READ );
  if( flock ){
   fib = (FileInfoBlock *)AllocDosObject( DOS_FIB,NULL );
   if( fib ){
    if( Examine( flock,fib ) ) len = fib->fib_Size;
    FreeDosObject( DOS_FIB,fib );
   }
   UnLock( flock );
  }

  han=Open(full,MODE_OLDFILE);

  buff=(char *)AllocVec( len,MEMF_CLEAR );
  if(buff==NULL && len!=0){
   Close(han);
   return( Unknown );
  }

  Read( han,buff,len );
  Close( han );

  dec=Decrunch( buff,len,tmp,&newlen );
  FreeVec( buff );
  if(dec!=NULL){
   strncpy( Cruncher,tmp,30 );
   buff=(char*)dec;
   len=newlen;

   ft = FirstFileType;
   while( ft->ln_Succ ){
    if(CheckBufferOffset(ft,buff,newlen)==1){
     FreeMem( buff,newlen );
     return(ft);
    }
    ft = (TFileType*)ft->ln_Succ;
   }
   FreeMem( buff,newlen );
  }else{
   strcpy( Cruncher,"Unknown" );
  }
 }

 // we test if it's an ASCII file
 ft = FirstFileType;
 while( ft->ln_Succ ){
  if( strcmp(ft->Value[0],"ASCII[]" )==0 && IsAscii(full)==1)  return(ft);
  ft = (TFileType*)ft->ln_Succ;
 }

 // Unknown Class
 return( Unknown );
}
//<

//>"BOOL TcaFileTypes::ContainsArchive( char *path,char *archive ) <<<<<<<<<<<<<!!!!!"
BOOL TcaFileTypes::ContainsArchive( char *path,char *archive ){
  char *lhafile=new char[512];
  BOOL Result=FALSE;
  if( CheckPath(path,lhafile)==PATH_PARTEXISTS ){
    TFileType *type = GetFileType( lhafile,FilePart(lhafile),FALSE );
    Result = type->IsArchive || XAD_IsArchive(lhafile);
  }
  delete [] lhafile;
  return( Result );
}
//<

//>"BOOL TcaFileTypes::BelongsToFileType( char *FileName,TFileType *ft )"
/****************************************/
/*                                      */
/* Function:  BelongsToClass            */
/*                                      */
/* Aim:       Determine if File belongs */
/*            to the class cls          */
/* Input:     file = full name of file  */
/*            cls  = pointer to class   */
/*                                      */
/* Output:    TRUE = right class        */
/*                                      */
/****************************************/
BOOL TcaFileTypes::BelongsToFileType( char *FileName,TFileType *ft ){
  if( ft->ParsedPattern[0] ){
    if( MatchPatternNoCase((UBYTE *)ft->ParsedPattern,FilePart(FileName)) ) return(TRUE);
    else return(FALSE);
  }else return(FALSE);
}
//<

//>"Image *TcaFileTypes::GetFileTypeImage( char *FileName,Screen *scr,BOOL simple=TRUE )"
Image *TcaFileTypes::GetFileTypeImage( char *FileName,Screen *scr,BOOL simple ){
  TFileType *ftn=GetFileType( FileName,FilePart(FileName),simple );

  if( ftn==Unknown ) return(NULL);
  if( ftn->ImageStatus==IMAGE_NOTAVAILABLE ) return( NULL );
  if( ftn->ImageStatus==IMAGE_LOADED ) return( ftn->ListerImage->IntuiImage() );

  if( !ftn->ImageFile[0] ){
    ftn->ImageStatus = IMAGE_NOTAVAILABLE;
  }

  strcpy( Buffer,CA_IMG_FILETYPES );
  strcat( Buffer,ftn->ImageFile );

  ftn->ListerImage = new MREBitmap;
  ftn->ListerImage->LoadFromFile( Buffer,scr );
  ftn->ImageStatus = IMAGE_LOADED;
  return( ftn->ListerImage->IntuiImage() );

/*
  for( ptr=FileName+strlen(FileName)-1;(ptr>FileName)&&(*ptr!='.');ptr-- );
  if( *ptr='.' ) ptr++;
  ftn=(TSuffixTypeNode*)SuffixTypes.lh_Head;
  while( ftn->ln_Succ && (stricmp(ptr,ftn->Suffix)!=0) ) ftn=(TSuffixTypeNode*)ftn->ln_Succ;
  if( ftn->ln_Succ ){
    if( !ftn->ImageLoaded ){
      ftn->ListerImage = new MREBitmap;
      ftn->ListerImage->LoadFromFile( ftn->ImageFileName,ftn->ImageScreen );
      ftn->ImageLoaded = TRUE;
    }
    return( ftn->ListerImage->IntuiImage() );
  }
  else return(NULL);
*/
}
//<

//>"BOOL TcaFileTypes::AddSuffix( char *FileName,TFileType *ft )"
BOOL TcaFileTypes::AddSuffix( char *FileName,TFileType *ft ){
  char Suffix[100];
  char *ptr,i=0;
  BOOL end=FALSE;
  // get Suffix:
  for( ptr=ft->FilePattern;*ptr && (*ptr!='.');ptr++ );
  if( ptr == 0 ) return( FALSE );
  Suffix[0]=0;
  while( !end ){
    switch( *ptr ){
      case '#': case '?': return( FALSE );
      case 0: case ')': case '|': case ' ': end=TRUE;break;
      default: Suffix[i]=*ptr;i++;Suffix[i]=0;break;
    }
    ptr++;
  }
  if( !Suffix[0] ) return( FALSE );
  // now get FileSuffix && compare:
  end=FALSE;
  for( ptr=FileName+strlen(FileName)-1;(ptr>FileName) && (!end);ptr-- ){
    switch( *ptr ){
      case '/': case ':': case '"': case ' ': case '.': end=TRUE; break;
    }
  }
  if( *ptr == '.' ){
    if( stricmp(ptr+1,Suffix) == 0 ) return( FALSE );
    *ptr=0;
  }
  strcat( FileName,Suffix );
  return( TRUE );
}
//<


