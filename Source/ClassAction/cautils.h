#ifndef CA_UTILS_H
#define CA_UTILS_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                  & Salim Gasmi

        File : cautils.h

****************************************/

#include <dos/dos.h>
#include <intuition/intuition.h>
#include <utility/hooks.h>

void  AdjustPath( char *Path );

int   CheckPath( char *path,char *existing );
/* Checks if path exists.
Returns the existing part (without '/') in existing.
path and existing may point to the same space. Results:*/
#define PATH_EXISTS     0
#define PATH_PARTEXISTS 1
#define PATH_NOTFOUND   2

BOOL  CoordsInGadget( int x,int y,Gadget *gad );
/* Switch display of requesters on/off */
void  EnableRequesters( BOOL Allow );
ULONG FileCount( char *file, FileInfoBlock *fib );
BOOL  FileExists( char *filename );
int   fsign( float number );

BOOL  IsDirectory( char *path );
/* Checks if path exists and is a directory */

void  InitHook( Hook *h,ULONG (*func)(), void *data );
void  RemoveChar( char *s, char c );
void  RemoveQuotes( char *string );
int   sign( int number );
int   strincmp( char *s1,char *s2 );
char  *stristr( char *s1,char *s2 );
/* Check if subdir is a subdirectory of dir */
BOOL  SubDir( char *dir,char *subdir );
void  ToBig( char *str );

#endif
