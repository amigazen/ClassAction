#ifndef CA_TYPES_H
#define CA_TYPES_H
/****************************************

        Class Action Source

        ? 2002 by Martin R. Elsner & Salim Gasmi

****************************************/

//#define BOBERG_TEST

#include "stormcompat.h"
#include <dos/dos.h>
#include <intuition/intuition.h>

/* Storm used ==/&&/||/! macros; those break C++ && NDK headers.
 * Use real C operators in the sources instead. */

#define MXDRV           20
#define TABCOUNT        5

#define TPREFSFILE "T:ClassAction.prefs"
/* Prefs and assets live under PROGDIR (no MRE: assign required). */
#define PREFSFILE  "PROGDIR:Config/ClassAction.prefs"
#define DPREFSFILE "PROGDIR:Config/Default/ClassAction.prefs"
#define CA_ICONFILE "PROGDIR:ClassAction"
#define CA_GUIDEFILE "PROGDIR:ClassAction.guide"
#define CA_PREFSAPP "PROGDIR:ClassActionPrefs"
#define CA_IMG_LISTER11 "PROGDIR:Images/Lister/"
#define CA_IMG_LISTER16 "PROGDIR:Images/Lister/"
#define CA_IMG_FILETYPES "PROGDIR:Images/FileTypes/"
#define CA_REQPATTERN "PROGDIR:Images/CAPatterns/CATile66x66.iff"

struct TFileList{
  List  Files;                  // list of selected files
  ULONG Count;                  // how many files?
  char  Source[512];            // source path (path for all select files)
  char  Destination[512];       // destination dir (for this action)
  ULONG Command;                // command to be executed for this list
  ULONG Reply;                  // reply from the executing function
  BOOL  Static;                 // whether the structure shall be hold in memory after
                                // calling the related function (-> asynchronous),
                                // wating for the reply by ARexx
  BOOL  ReadSource,ReadDestination; // read one of these listers
  UBYTE DeselectMode;           // which entries shall be deselected ?? see below
};


//Values for Type:
#define TYPE_FILE       1
#define TYPE_DIR        2
#define TYPE_ASSIGN     3
#define TYPE_VOLUME     4
#define TYPE_DEVICE     5

struct TNameNode{
 Node   *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 char   Name[256];
};

struct TFileEntry{
 Node   *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 char   Name[256];      // full filename
 BOOL   Selected;       // == file selected || deselected (can be toggled!)
 BOOL   Success;        // message returned from command
};

// event buffer for IDCMP events:
struct TIDCMPEvent{
 IntuiMessage Msg;
};


#endif

