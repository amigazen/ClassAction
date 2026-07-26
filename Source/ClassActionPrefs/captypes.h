#ifndef CAP_TYPES_H
#define CAP_TYPES_H
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File : captypes.h

        Goal : define new types

        Ver  : 4.2

****************************************/

#include "stormcompat.h"
#include <intuition/classes.h>
#include <libraries/amigaguide.h>
#include <libraries/asl.h>
#include <libraries/locale.h>
#include <libraries/resource.h>
#include <workbench/workbench.h>

/* Storm used ==/&&/||/! macros; those break C++ && NDK headers.
 * Prefer real C operators in the sources. */

#define NOACT GetCatalogStr( Main.Catalog,TXT_NOACT,TXT_NOACT_STR )
#define UNKNOWN GetCatalogStr( Main.Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR )

#define TPREFSFILE "T:ClassAction.prefs"
#define PREFSFILE  "PROGDIR:Config/ClassAction.prefs"
#define DPREFSFILE "PROGDIR:Config/Default/ClassAction.prefs"
#define OLDPREFSFILE "ENVARC:ClassAction.prefs"
#define CA_GUIDEFILE "PROGDIR:ClassAction.guide"


#define EOC "CLASS_END"
#define EOSF "FILE_END"

#define SHEADER "CAFS20"
#define GHEADER "CAFG20"
#define DHEADER "CAFD30"
#define CHEADER "CAFC40"

struct Action{
  Node *ln_Succ,*ln_Pred;
  UBYTE         ln_Type;
  BYTE          ln_Pri;
  char          *ln_Name;

  char          Name[33];
  char          Exec[256];
  char          Type;
  char          Stack[8];
  char          Delay[3];
  UBYTE         CD;
  BOOL          Async,ReadSrc,ReadDst;
};

struct TClass{
  Node *ln_Succ,*ln_Pred;
  UBYTE         ln_Type;
  BYTE          ln_Pri;
  char          *ln_Name;

  char          Name[33];
  BOOL          IsArchive;
  char          ImageFile[108];
  char          Suffix[100];
  char          Value[5][50];
  List          Actions;
  int           ActionCount;
};

/*
struct ClassList{
 struct Class    *head;
 struct Class    *tail;
 int             size;
};
*/

// flags for wbrun:
#define RUN_CLI          0      // start by SystemTags
#define RUN_WB           1      // start by OpenWorkbenchObject
#define RUN_NOCLI        2      // start by rx "..."
#define RUN_AREXX        3      // SystemTags, do not open a console
#define RUN_INTERNAL     4      // CA command
#define RUN_SPACE        5      // dummy, only space, no action

struct CommandNode{
 struct Node *ln_Succ;  /* Pointer to next (successor) */
 struct Node *ln_Pred;  /* Pointer to previous (predecessor) */
 UBYTE  ln_Type;
 BYTE   ln_Pri;         /* Priority, for sorting */
 char   *ln_Name;       /* ID string, null terminated */
 /* extended: */
 char           name[33];
 char           exec[256];
 char           help[256];
 char           icon[256];
 char           wbrun;
 char           stack[8];
 char           delay[3];
  BOOL          async,readsrc,readdst;
  UBYTE         CD;     // where to go: 0=source,1=destination,2=progdir
 UBYTE          key;
 BOOL           ascii;
};

struct NamedNode{
 struct Node *ln_Succ;  /* Pointer to next (successor) */
 struct Node *ln_Pred;  /* Pointer to previous (predecessor) */
 UBYTE  ln_Type;
 BYTE   ln_Pri;         /* Priority, for sorting */
 char   *ln_Name;       /* ID string, null terminated */
 /* extended: */
 char  Name[256];
};

struct TPathNode{
  Node *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 char   Name[100];
 char   Path[256];
};


struct TMain{
  Catalog        *Catalog;
  RESOURCEFILE   Resource;
  APTR           Visualinfo;
  Class          *IconClass;
  Screen         *Scr;
  Object         *Win,*LWin;
  Window         *IntuiWin;
  Object         **Root,**Gadgets,**LGadgets;
  MsgPort        *MsgPort,*AppPort;
  FileRequester  *FileReq;
  FontRequester  *FontReq;
  ScreenModeRequester  *ScrReq;
  Menu           *Menu;
  NewAmigaGuide          NewAG;
  AMIGAGUIDECONTEXT      GuideHandle;
  BOOL           Leave,ShowHelp,ClassEnabled,ActionEnabled;
  BOOL           CommandEnabled,ActionsActive;
  int            WinX,WinY,WinWidth,WinHeight;
  int            PrefsVersion;          // Version of the current file - compare PREFSVERS
  DiskObject     *Dobj;                  // ptr to the icon
  TClass         *SelectedClass;
  Action         *SelectedAction;
  TClass         *Volume;                 // ptr to volume class
  TClass         *Directory;              // ptr to directory class
  TClass         *Generic;                // ptr to generic class
  TClass         *OpenWith;               // ptr to OpenWith class
  char           CAPath[512];            // path of ClassAction
  List           Classes;
  int            ClassCount;
  char           *Ret[13];               // Requester texts
  char           *PrefsFile,*PublicScreen;
  TextFont       *MenuFont;
};

extern TMain Main;

#endif
