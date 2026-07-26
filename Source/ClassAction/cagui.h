#ifndef CA_GUI_H
#define CA_GUI_H
/****************************************

        Class Action Source

        (c) 2002 by Martin R. Elsner
                  & Salim Gasmi

        File: cagui.h

        Aim:  define functions prototypes

****************************************/

#include <intuition/intuition.h>
#include "capaths.h"

void Act();
void ActivateBrowser( UBYTE which );
void ActivatePathGadget( UBYTE which );
void AddIcon();
void AddInfoGadget();
void All();
int  AskReplace( char *f1, char *f2, char *reqtext );
void Assign( char *arguments );

BOOL CheckAbort();
BOOL ChoixDirList( UBYTE which );
void ClearActionList();
void ClearHistory();

void DeselectList( TFileList *list );
void DoubleClickDirList( UBYTE which );

void EnableAbort( BOOL enable );

void FileTransfer( TFileList *list );

Object        *GetCurrentListBrowser();
char          *GetDst();
void          GetL2Dir(char *p);
Object        *GetListBrowser( UBYTE which );
char *GetPath( UBYTE which );
TPathNode     *GetPathNode( char *path, BOOL read=TRUE );
char          *GetSrc();
float         GetSize(BOOL req=FALSE,BOOL refresh=TRUE);
BOOL          GetText(char *buffer,int len,char *msg);

void HandleAppMessage();
void HandleDiskNotify( ULONG Class );
void HandleNotify();

void IconifyWindow();
void InsertInActionList( char **actionlist,int realactions );

void Key( char p );
void KeyMove( UBYTE key,short Qualifier );

void LhaPopup( char *file );
BOOL ListBrowserKeyMove( IntuiMessage *im );
BOOL ListDir( UBYTE which,char *path,int check,ULONG top=-1 );

void MkDir( TFileList *list,UBYTE Arg1 );

void None();

void OpenDirWindow();
void OpenOld( UBYTE which );
void OpenPath( UBYTE which );

void Pack();
void Parent( UBYTE which );
void Pat();
void PopupOnCurrentFiles();
void Prefs();

TPathNode *ReadDirIntoList( char *path );
void RearrangeBrowserSize();
void Refresh( UBYTE which );
void RenameFile();
void RequesterVers();
void Root();

void Search();
void ShowOtherTab( UBYTE which,UBYTE num );
void SnapshotWindow();

void StoreFib( char *file, char *buffer,BOOL lha );
void Swap( char which=0 );

void TitleClickDirList( int list );
void Toggle();
void Type();

void UnIconifyWindow();

void WbInfo();
void WbInfoDir();

#endif
