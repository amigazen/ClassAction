#ifndef CA_FILETYPES_H
#define CA_FILETYPES_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner & Salim Gasmi

        File: filetypes.h

****************************************/

#include <intuition/intuition.h>
#include <dos/dos.h>
#include <intuition/classusr.h>
#include <dos/notify.h>
#include "unBitmap.h"
#include "caactions.h"

struct TFileType{
 Node   *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;

 char           Name[33];       // TFileType Name
 BOOL           IsArchive;      // set if archive class, to be browsed like a dir
 char           ImageFile[108];       // FileName of the image
 int            ImageStatus;            // Image
 char           FilePattern[100];       // Used pattern
 char           ParsedPattern[210];     // Parsed pattern
 char           Value[5][50];   // Value of the 5 offsets
 List           Actions;        // Actions of the TFileType
 int            ActionCount;    // # of actions defined
 MREBitmap      *ListerImage;   // bitmap for display in lister
};

#define IMAGE_NULL              0
#define IMAGE_LOADED            1
#define IMAGE_NOTAVAILABLE      2

/*
struct TSuffixTypeNode{
 Node   *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 char   Suffix[108];
 char   ImageFileName[256];
 BOOL   ImageLoaded;
 Screen *ImageScreen;
 MREBitmap *ListerImage;
};
*/

class TcaFileTypes{
public:
  TcaFileTypes();
  ~TcaFileTypes();
  List                   FileTypes;               // all FileTypes
  int                    Count;
  List                   SuffixTypes;             // suffix types with icons
  TFileType              *OpenWith;               // ptr to Open With class
  TFileType              *Volume;                 // ptr to volume class
  TFileType              *Assign;                 // ptr to assign class
  TFileType              *Directory;              // ptr to directory class
  TFileType              *Generic;                // ptr to generic class
  TFileType              *Unknown;                // ptr to unknown class
  BOOL                   LittleImages;
  char                   Buffer[256];
  char                   Cruncher[30];           // name of cruncher

  void Clear();

  TFileType *GetFileType( char *full,char *name,BOOL simple,BOOL decrunch=FALSE );
  BOOL      ContainsArchive( char *path,char *archive );
  /* checks if path contains a part which exists, and that part is of an archive class.
     Fills archive with the name of the archive. */

  Image     *GetFileTypeImage( char *FileName,Screen *scr,BOOL simple=TRUE );
  BOOL      BelongsToFileType( char *FileName,TFileType *ft );
  BOOL      AddSuffix( char *FileName,TFileType *ft );

  TAction   *AddAction( TFileType *ft,char *name );
  TFileType *AddFileType( char *name );

  void RemoveAction( TAction *act,BOOL RemoveFromList=FALSE );
  void RemoveFileType( TFileType *ft,BOOL RemoveFromList=FALSE );
  void ClearFileTypes();

};

extern TcaFileTypes *caFileTypes;

#endif

