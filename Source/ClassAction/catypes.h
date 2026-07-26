#ifndef CA_TYPES_H
#define CA_TYPES_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner & Salim Gasmi

****************************************/

//#define BOBERG_TEST

#include <dos/dos.h>
#include <intuition/intuition.h>

#define is              ==      // don't want to get errors from = and ==
#define isnot           !=
#define and             &&
#define or              ||
#define not             !
#define False           FALSE
#define True            TRUE

#define MXDRV           20
#define TABCOUNT        5

#define TPREFSFILE "T:ClassAction.prefs"
#define PREFSFILE  "MRE:Config/ClassAction.prefs"
#define DPREFSFILE "MRE:Config/Default/ClassAction.prefs"

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
 BOOL   Selected;       // is file selected or deselected (can be toggled!)
 BOOL   Success;        // message returned from command
};

// event buffer for IDCMP events:
struct TIDCMPEvent{
 IntuiMessage Msg;
};


#endif

