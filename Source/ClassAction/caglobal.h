#ifndef CA_GLOBAL_H
#define CA_GLOBAL_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                & Salim Gasmi

        File : caglobal.h

****************************************/

#include <libraries/amigaguide.h>
#include <libraries/asl.h>
#include <intuition/classes.h>
#include <libraries/locale.h>
#include <libraries/pm.h> // for struct popupmenu
#include <libraries/resource.h>
#include <workbench/workbench.h>
#include <devices/serial.h>
#include "catypes.h"
#include "IniFile.h"
#include "caccp.h"
#include "unARexx.h"
#include "unCommodities.h"
#include "unBitmap.h"

//>"struct str_Main"
struct TMain{
  Catalog                *Catalog;
  Locale                 *Locale;
  RESOURCEFILE           Resource;
  Screen                 *Scr;
  Object                 *Win,*ReqWin,*ARexxPort;
  short                  ZoomCoords[4];
  MREBitmap              AssignImage,DirImage,FileImage,VolumeImage,DeviceImage,FileLinkImage;
  Window                 *IntuiWin;
  Object                 **Gadgets,**Group5;

  MsgPort                *MsgPort,*AppPort,*NotifyPort;
  Hook                   IDCMPHook;              // for reacting on IDCMP events
  TIDCMPEvent            IDCMPEvent;             // buffer for IDCMP events

  FileRequester          *FileReq;
  NewAmigaGuide          NewAG;
  AMIGAGUIDECONTEXT      GuideHandle;
  TARexx                 ARexx;
  char                   *WindowTitle;
  char                   *SrcStartPath,*DstStartPath;
  char                   *OwnScreen,*PublicScreen, *BackgroundPattern;
 // Menu                   *IMenu; // intui
 // TMenu                  Menu;   // my class
  PopupMenu              *MainPopup;
  PopupMenu              *DirMenu;
  PopupMenu              *VolumeMenu;
  // Prefs:
  int                    PrefsVersion;  // version of the found prefs file
  BOOL                   ShowDrawersFirst,IconStart,ShowIcons;
  BOOL                   CreateIcons,CopyIcons,DeleteIcons;
  BOOL                   ConfirmReplace,ShowVersion,ReplaceVersion;
  BOOL                   DateString,DeleteProtected,ConfirmDelete,CountFiles;
  BOOL                   DisplayTime,ShowParent,ShowRefresh,LittleImages;
  BOOL                   ShowImages,ClassImages,CheckOnlyPatterns;
  BOOL                   ShowDevices,CheckFit,HighlightDirs,UseExAll;
  BOOL                   MuiQuit,PathNotify;
  BOOL                   VerticalLists,bPulldownMenu;
  BOOL                   TabButtonsTransparent;
  UBYTE                  DateFormat;
  int                    MaxTabs,MaxPaths,PathCount,BufferSize,ExAllFiles;
  int                    WinX,WinY,WinWidth,WinHeight,AppX,AppY,PopupWaitTime;
  DiskObject             *Dobj;                  // ptr to the icon
  List                   UserPaths;              // predefined paths containg UserPathNodes
  char                   *PopupKey,*HideKey;     // key definitions for commodities
  List                   History;                // visited directories
  List                   Paths;                  // Buffered paths and paths tabs
  List                   Tabs[2];                // dirs for the left and right tabs
  int                    CurrentList;            // the selected list (0 or 1)
  int                    HistorySize;            // number of paths in history
  char                   DecrunchFlag;           // =-1 if decrunch succeds
  char                   DevString[100];         // device string
  Library                *ConsoleDevice;         // console device base;
  IOExtSer               IOExtSer;               // for console device
  BOOL                   LhaPossible;
  TextFont               *MenuFont;              // font for the popup menu
  TextAttr               MenuAttr;              // font for the popup menu
  char                   *sMenuFont;             // buffer for the read value
  TextFont               *ListerFont;            // same for the lister
  TextAttr               ListerAttr;             // but additional textattr
  char                   *sListerFont;

  // runtime variables:
  BOOL Leave;    // Signal to leave the program
  BOOL Restart;  // Signal to restart after closing all
  BOOL OwnScreenOpen;
  BOOL                   IsIcon,HelpOpen;
  float                  SelectedFileSize[2];   // size of all selected files
  int                    AvailableFiles[2];     // number of all files
  int                    SelectedFiles[2];      // number of selected files
  int                    AvailableDirs[2];      // number of all dirs
  int                    SelectedDirs[2];       // number of selected dirs
  TCommodities           CX;
  struct{
    APTR  Buffer;
    char  *ReplaceText;         // the text for the replace requester
    char  *ProgressText;        // text to display in progress bar (only if needed)
    ULONG Count;                // counter for the transfer process
    UBYTE RequesterResult;      // se below for constants
    UBYTE DeprotectResult;      // se below for constants
  }Transfer;
};

extern TMain *Main;
//<

// consts for RequesterResult:
#define REPLACE_NONE 3
#define REPLACE_ALL  2
#define REPLACE_ONE  1
#define REPLACE_NOT  0
// consts for DeprotectResult:
#define DEPROTECT_NONE 3
#define DEPROTECT_ALL  2
#define DEPROTECT_ONE  1
#define DEPROTECT_NOT  0

// types for SortMode:
#define SM_SUFF 0
#define SM_NAME 1
#define SM_SIZE 2
#define SM_DATE 3

void AddInfoGadget();

void FreeAll();

void ListHelp( UBYTE which );

IClass *MakeIconClass();

void MC_Copy();

void MC_Help();

void MC_Paste();

void MC_Quit();

void Prefs();

void RenameFile( TFileList *list );

void RequesterVers();

void Swap();

#endif
