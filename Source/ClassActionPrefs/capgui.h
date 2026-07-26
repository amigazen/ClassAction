/****************************************

        Class Action Source

        � 2002 by Martin R. Elsner
               & Salim Gasmi

        File : capgui.h

        Aim  : Graphical interface for
               ClassActionPrefs

****************************************/


#include "captypes.h"
#include "IniFile.h"
#include "capopt.h"

class TcapGui{
public:
  TcapGui();
  ~TcapGui();
  void DuplicateClass();
  void ImportClass();
  void ExportClass();
  void CopyAction();
  void PasteAction();

  BOOL SavePrefs( char *FileName );
  TChooser DateFormats;
private:
  Action   ActionBuffer;
  BOOL     ActionCopied;
};

extern TcapGui capGui;

void  AddCommand();
void  AddPath( char *Path=NULL );
ULONG AppendAction();
void  AppendClass();
void  AdjustCCD();
void  AdjustCD();
void  AdjustCDelay();
void  AdjustCDir();
void  AdjustCHelp();
void  AdjustCMode();
void  AdjustCName();
void  AdjustCommand();
void  AdjustCStack();
void  AdjustComGadgets();
void  AdjustKey();
void  AdjustKeyMode();
void  AdjustPathButton();
void  AdjustPathGadgets();

void  CheckArchiveOnClick();
ULONG ChRescan();
ULONG CyMode();
ULONG CyOffset();

ULONG DeleteAction();
ULONG DeleteClass();
ULONG Down();

void EnterKey();

void GetAppIcon();
void GetGuide();
void GetIcon();
void GetPathButton();
void GetPathName();

void ImageFileOnChange();
void ImageFileRequest();
void InsertCom();
void InsertComA();
void InsertVar();

void  ListClasses();
void  LoadClasses( TIniFile *IniFile );
void  LoadCommands( TIniFile *IniFile );
BOOL  LoadOptions( TIniFile *IniFile );
void  LoadCom();
void  LoadPaths( TIniFile *IniFile );
ULONG Lwa();
void  Lwc();
ULONG LwCom();

void    MenuLastSaved();
void    MenuOldPrefs( BOOL refresh );
void    MenuOpen();
void    MenuReset();
void    MenuRestore( BOOL refresh );
void    MenuSaveAs();
void    MenuShowHelp( char *link="LINK main" );
void    MoveComDown();          // cagui.c
void    MoveComUp();            // cagui.c
void    MoveClassLister( ULONG key ); // cagui.c
void    MoveCommandLister( ULONG key ); // cagui.c
void    MovePathDown();          // cagui.c
void    MovePathUp();            // cagui.c

void OpenIconWindow();
void PathListDblClick();
void Preview();
void Quit();

void RemoveCommand();
void RemovePath();
void ResetCommands();

void  Save();
void  SaveClasses( TIniFile *Inifile );
void  SaveCommands( TIniFile *Inifile );
void  SaveOptions( TIniFile *Inifile );
void  SavePaths( TIniFile *Inifile );
ULONG Sel();
ULONG StDelay();
ULONG StExec();
ULONG StFiltre();
ULONG StNoma();
ULONG StNomc();
ULONG StOffset();
ULONG StPile();
ULONG Up();
void  Use();

