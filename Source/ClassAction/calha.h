#ifndef CA_LHA_H
#define CA_LHA_H
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File: calha.h

        Aim:  defs for calha.cpp

        Ver:  4.1

****************************************/
#include <proto/xadmaster.h>
#include <libraries/xadmaster.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include <dos/dos.h>
/*
#include <exec/memory.h>
#include <dos/dosasl.h>
#include <utility/hooks.h>
*/

struct FNode{
 Node   *ln_Succ;  /* Pointer to next (successor) */
 Node   *ln_Pred;  /* Pointer to previous (predecessor) */
 UBYTE  ln_Type;
 BYTE   ln_Pri;         /* Priority, for sorting */
 char   *ln_Name;       /* ID string, null terminated */
 /* extended: */
 ULONG  EntryNumber;
 char   Name[256];
 List   Children;       // files in this directory
 FileInfoBlock Fib;     // what this file contains
};

BOOL XAD_IsArchive( char *file,char **type=NULL );
/* Checks if the given file is an archive. type can be a pointer to a strptr which will a pointer
to the archive type name.*/

// Lha == a class for lha archives. First Lock an archive, than operate like with
// devices
class TArchive{
public:
 TArchive();
 ~TArchive();
 BPTR Lock( char* );
 void UnLock();
 BOOL Examine( FileInfoBlock* );
 BOOL ExNext( FileInfoBlock* );
 BOOL PathValid( char * );
 void ArchiveName( char* );             // returns in the argument the ar name
 void LocalName( char* );               // returns in the argument the name in the ar
                                        // && as returnvalue the fib_DirEntryType
 BOOL UnpackFile( char*,char* );        // unpack the src file (first) to the dest file
 ULONG Type;                            // the type of the archive
 ULONG Error;                           // what has happened
private:
 void  InsertFNode( xadFileInfo * );   // insert the file in the tree
 BOOL  DirExists( char * );            // check if a specific dir exists
 List  *InsertDir( char *,xadFileInfo * );// insert a directory if it == ! yet present
                                        // && return a pointer to it
 List  *Directory( char*,xadFileInfo *  );// returns a pointer to the given dir (list)
 FNode *File( char* );          // returns a pointer to the given file (list)
 void   DeleteList( List *list );        // removes  node (&& all subnodes recursive)

 char  *Filename;                        // the name of the archive
 char  *Path;                            // the path we are in
 List  *Files;                           // a tree of the files in the archive, contains FNodes
                                         // Administrated inside ArchiveList!
 List  *CurrentPath;                     // the dir we are in (for ExNext)
 FNode *CurrentFile;                     // the actual file (for ExNext)

 xadArchiveInfo *ArchiveInfo;           // address of the Info
 BOOL lock;                             // have we locked the archive?
};


// Types:
#define TYPE_LHA    1L
#define TYPE_ZIP    2L
#define TYPE_LZX    3L
#define TYPE_HA     3L
#define TYPE_UNKNOWN    99L
// Errors:
#define ARCHIVE_NOLIB   1L
#define ARCHIVE_CORRUPT 2L
#define ARCHIVE_MEM     3L
#define ARCHIVE_NOINFO  4L
#define ARCHIVE_ERROR   5L

struct TFileListNode{
  Node   *ln_Succ,*ln_Pred;
  UBYTE  ln_Type;
  BYTE   ln_Pri;
  char   *ln_Name;
  char Name[512]; // Filename of the archive, without trailing slash
  List Files; // List of files, contains FNode's
};

class TArchiveList{
public:
  TArchiveList();
  ~TArchiveList();
  List Archives;                // read archives, contains TFileListNode's
  List *AddList( char *Name );  // create a list of an archive
  List *GetList( char *Name );  // get the list of an archive if the list exists
  void RemoveList( char *Name );  // remove the list of an archive if the list exists
private:
  int Count;    // number of archives
  void RemoveDir( List *list );  // remove the list of an archive if the list exists
  void RemoveList( TFileListNode *list );  // remove the list of an archive if the list exists
};

extern TArchiveList *ArchiveList;

#endif
