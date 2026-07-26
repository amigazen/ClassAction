#ifndef CA_EXEC_H
#define CA_EXEC_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                  & Salim Gasmi

        File : caexec.h

****************************************/

#include "caFileTypes.h"
#include "catypes.h"

#define FT_COPY      0
#define FT_COPYAS    1
#define FT_MOVE      2
#define FT_MOVEAS    3
#define FT_DUPLICATE 4
#define FT_DELETE    5
#define FT_TRASHCAN  6
#define IC_RENAME    7
#define IC_ADDSUFFIX 8

// Constants for DeselectMode:
#define DESELECT_ALL    0
#define DESELECT_LIST   1       // only deselect the files in the list which have Selected=FALSE
#define DESELECT_NONE   2

int    CompareDate( char *f1, char*f2 );
BOOL   CopyArchiveFile( char *src,char *dest );
BOOL   DelFile( const char *src );
float  FileLength( char *file, FileInfoBlock *fib  );
BOOL   GetVersion( char *file,char *version );
int    IsAscii(char *);
int    MRECopy( char *src,char *dest,Object *gauge,UBYTE mode=FT_COPY );
void   ReleaseFileList( TFileList *list );
int    Runner( TAction *act, char *filename=NULL );
void   StoreFib( char *file, char *buffer,BOOL lha );

#endif
