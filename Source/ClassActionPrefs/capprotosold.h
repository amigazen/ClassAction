/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
        (c) by 1994 Salim Gasmi

        File: capprotos.h

        Aim:  define external prototypes

        Ver:  3.8

****************************************/
/*
#include "captypes.h"

ULONG AppendAction();                                   // cagui.c
void  AppendClass();                                   // cagui.c
void  AddGenDirList();                          // calist.c
void  AddPath( char *Path=NULL );               // calist.c
void  AdjustCDelay();                           // cagui.c
void  AdjustCDir();                             // cagui.c
void  AdjustCHelp();                            // cagui.c
void  AdjustCMode();                            // cagui.c
void  AdjustCName();                            // cagui.c
void  AdjustCommand();                          // cagui.c
void  AdjustCStack();                           // cagui.c
void  AdjustComGadgets();                       // cagui.c
void  AdjustKey();                              // cagui.c
void  AdjustKeyMode();                          // cagui.c
void  AdjustPathButton();                       // cagui.c
void  AdjustPathGadgets();                      // cagui.c
int   AddList(struct List *,char *,int);        // calist.c

void  CheckArchiveOnClick();
ULONG ChRescan();
Action *CreateAction( Class *cla,char *name );
Class *CreateClass( List *li,char *name );
ULONG CyMode();
ULONG CyOffset();

ULONG DeleteAction();
ULONG DeleteClass();
ULONG Down();

void EnterKey();        // cagui.c

BOOL FontReq( char *font, char *text );         // capreq.c
void FreeList(struct List *);
void FreeClassList();
BOOL Freq(char *file,int mode,char *text);      // capreq.c

void GetAppIcon();      // cagui.c
int  Getfile( char * );
void GetGuide();        // cagui.c
void GetIcon();         // cagui.c
void GetPathButton();   // cagui.c
void GetPathName();     // cagui.c
BOOL GetText(char *buffer,int len,char *msg);                           // capreq.c
int  GetToolType(struct DiskObject *dobj,char *tool,char *result);      // cadisk.c

void ImageFileOnChange();
void ImageFileRequest();
void Info( char*mess ); // capreq.c
void InitLists();
void InsertCom();       // capgui.c
void InsertComA();      // capgui.c
void InsertVar();       // capgui.c

void  Learn();
void  ListClasses();
void  LoadClasses( TIniFile *IniFile );    // cagui.c
void  LoadCommands( TIniFile *IniFile );   // cagui.c
BOOL  LoadOptions( TIniFile *IniFile );    // cagui.c
void  LoadCom();        // cagui.c
//BOOL  LoadGen();
//BOOL  LoadMem();
void  LoadPaths( TIniFile *IniFile );
ULONG Lwa();
void  Lwc();
ULONG LwCom();

IClass *MakeIconClass();        // capbutclass.c
int     Make_List(UBYTE **,struct List *,struct Remember *);
void    MemToList();
void    MenuCrPic();
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

void NotifyMaster();

void OpenIconWindow(); // cagui.c
void PathListDblClick(); //capgui.c
void Preview(); // cagui.c

void Quit();
BOOL ReadToolTypes();
void RemoveCommand();   // cagui.c
void RemovePath();      // cagui.c
BOOL Req( char *mess ); // careq.c
void RequesterVers();
void ResetCommands();   // capgui.c
void RInfo(char *);

void  Save();
void  SaveClasses( TIniFile *Inifile );
void  SaveCommands( TIniFile *Inifile );
void  SaveOptions( TIniFile *Inifile );
void  SavePaths( TIniFile *Inifile );
BOOL ScreenRequester( char *modestring, char *title );
int   SortList();
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
*/
