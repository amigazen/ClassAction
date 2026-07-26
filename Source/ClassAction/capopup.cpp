/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File: capopup.cpp

        Aim:  GUI interface code

        Ver:  4.3

****************************************/

#include <clib/alib_protos.h>
#include <dos/dosextens.h>
#include <intuition/intuitionbase.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/pm.h>
#include <stdio.h>
#include <string.h>

#include "caexec.h"
//#include "caextern.h"
#include "caglobal.h"
#include "cagui.h"
#include "caicons.h"
#include "calha.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classactioncd.h"

#include "capopup.h"

//>"TagItem *AddMenuItem( TagItem *tag, char *title, APTR userdata )"
TagItem *AddMenuItem( TagItem *tag, char *title, APTR userdata ){
 tag->ti_Tag  = PM_Item;
 tag->ti_Data = (ULONG)PM_MakeItem(PM_Title,title,PM_UserData,userdata,TAG_DONE);
 tag++;
 return( tag );
}
//<

//>"TagItem *AddMenuTitle( TagItem *tag, char *title )"
TagItem *AddMenuTitle( TagItem *tag, char *title ){
 tag->ti_Tag  = PM_Item;
 tag->ti_Data = (ULONG)PM_MakeItem( PM_Title,title,PM_NoSelect,TRUE,
//                                    PM_ShinePen,TRUE,PM_Shadowed,TRUE,PM_Center,TRUE,TAG_DONE );
                                    PM_Bold,TRUE,
//                                    PM_Center,TRUE,
                                    TAG_DONE );
 tag++;
 return( tag );
}
//<

//>"TagItem *AddBar( TagItem *tag )"
TagItem *AddBar( TagItem *tag ){
 tag->ti_Tag  = PM_Item;
 tag->ti_Data = (ULONG)PM_MakeItem(PM_TitleBar,TRUE, TAG_DONE);
 tag++;
 return( tag );
}
//<

//>"void OpenActionMenu( TFileType *SelectedClass,char *filename=NULL )"
void OpenActionMenu( TFileType *SelectedClass,char *filename ){
  BOOL UnlockScreen = FALSE, bActions=FALSE;
  int i;
  struct PopupMenu *ActionPMenu,*OpenWithMenu=NULL,*CopyMenu=NULL;
  TUserPathNode *nd;
  TagItem *Tags = (TagItem*)AllocVec( 300*sizeof(TagItem),MEMF_CLEAR  ); // should be enough
  TagItem *CurrentTag=Tags;
  BOOL IsDefinedClass =
    (SelectedClass is caFileTypes->Directory) or
    (SelectedClass is caFileTypes->Assign) or
    (SelectedClass is caFileTypes->Volume);
  // insert Open With actions:
  if( !IsDefinedClass ){
    if( caFileTypes->OpenWith->ActionCount>0 ){
      TAction *act = (TAction*)caFileTypes->OpenWith->Actions.lh_Head;
      while( act->ln_Succ ){
        CurrentTag=AddMenuItem( CurrentTag,act->Name,act );
        act = (TAction*)act->ln_Succ;
      }
      CurrentTag->ti_Tag = TAG_END;
      OpenWithMenu = PM_MakeMenuA( Tags );
      CurrentTag=Tags;
    }
  }
  // insert Copy To Paths:
  if( (SelectedClass isnot caFileTypes->Volume) and
      (SelectedClass isnot caFileTypes->Assign) ){
    for( nd=(TUserPathNode*)Main->UserPaths.lh_Head,i=1; nd->ln_Succ; nd=(TUserPathNode*)nd->ln_Succ,i++ ){
      if( nd->Name[0] and nd->Path[0]  ){
        CurrentTag=AddMenuItem( CurrentTag,nd->Name,(APTR)i );
      }
    }
    if( CurrentTag isnot Tags ){
      CurrentTag->ti_Tag = TAG_END;
      CopyMenu = PM_MakeMenuA( Tags );
      CurrentTag=Tags;
    }
  }

  // Actions
  if( SelectedClass->Name[0] )
    CurrentTag=AddMenuTitle( CurrentTag,SelectedClass->Name );
  else if( SelectedClass is caFileTypes->Directory )
    CurrentTag=AddMenuTitle( CurrentTag,GetCatalogStr(Main->Catalog,TXT_DIR,TXT_DIR_STR) );
  else if( SelectedClass is caFileTypes->Volume )
    CurrentTag=AddMenuTitle( CurrentTag,GetCatalogStr(Main->Catalog,TXT_VOLUME,TXT_VOLUME_STR) );
  else if( SelectedClass is caFileTypes->Assign )
    CurrentTag=AddMenuTitle( CurrentTag,GetCatalogStr(Main->Catalog,TXT_ASSIGN,TXT_ASSIGN_STR) );
  else{
    char *ptr;
    if( (!filename) || (!XAD_IsArchive(filename,&ptr)) ) ptr=GetCatalogStr( Main->Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR );
    CurrentTag=AddMenuTitle( CurrentTag,ptr );
  }
  if( SelectedClass isnot NULL ){
   BOOL AtLeastOneAction = FALSE;

   if( OpenWithMenu ){
     CurrentTag->ti_Tag  = PM_Item;
     CurrentTag->ti_Data = (ULONG)PM_MakeItem(
       PM_Title,GetCatalogStr(Main->Catalog,MSG_OPENWITH,MSG_OPENWITH_STR),PM_Sub,OpenWithMenu,TAG_DONE );
     CurrentTag++;
   }
   if( CopyMenu ){
     CurrentTag->ti_Tag  = PM_Item;
     CurrentTag->ti_Data = (ULONG)PM_MakeItem(
       PM_Title,GetCatalogStr(Main->Catalog,MSG_COPYTO,MSG_COPYTO_STR ),PM_Sub,CopyMenu,TAG_DONE );
     CurrentTag++;
   }

   //Main->Menu.AddItem( SelectedClass->name,NULL,NM_TITLE,NULL );
   for( TAction *act=(TAction*)SelectedClass->Actions.lh_Head;act->ln_Succ;act = (TAction*)act->ln_Succ ){
    if( !bActions ){
      CurrentTag=AddBar( CurrentTag );
      bActions = TRUE;
    }
    CurrentTag=AddMenuItem( CurrentTag,act->Name,act );
    AtLeastOneAction = TRUE;
   }
   if( (SelectedClass isnot caFileTypes->Generic) and not IsDefinedClass ){
    if( AtLeastOneAction ) CurrentTag=AddBar( CurrentTag );
    for( TAction *act=(TAction*)caFileTypes->Generic->Actions.lh_Head;act->ln_Succ;act=(TAction*)act->ln_Succ ){
      if( !bActions ){
        CurrentTag=AddBar( CurrentTag );
        bActions = TRUE;
      }
      CurrentTag=AddMenuItem( CurrentTag,act->Name,act );
    }
   }
  }
  CurrentTag->ti_Tag = TAG_END;

  Window *win;
  if( Main->Scr ){
   win = Main->IntuiWin;
   if( !win ) win=Main->Scr->FirstWindow;
  }else{
   char *ptr=Main->PublicScreen;
   if( ptr[0]==0 ) ptr=NULL;
   if( !(Main->Scr=LockPubScreen(ptr)) ){
    Main->Scr=LockPubScreen(NULL);
   }
   if( Main->Scr ){
     win = Main->Scr->FirstWindow;
     UnlockScreen = TRUE;
   }
  }

  ActionPMenu = PM_MakeMenuA( Tags );
  if( ActionPMenu ) {
   TAction *todo = (TAction*) PM_OpenPopupMenu( win,PM_Menu,ActionPMenu,
                                 Main->MenuFont ? PM_ForceFont : TAG_IGNORE,Main->MenuFont,TAG_DONE );
   if( todo ){
     if( (ULONG)todo < 100 ){    // we just assume that there are less than 100 user paths
       TAction *act = caActions->CreateAction( "" );
       sprintf( act->Exec,"COPY" );
       // look for the path todo-1:
       for( nd=(TUserPathNode*)Main->UserPaths.lh_Head,i=1; i<(ULONG)todo; nd=(TUserPathNode*)nd->ln_Succ,i++ );
       strcpy( act->Dest,nd->Path );
       act->Type  = RUN_INTERNAL;
       act->CD    = 0;
       act->Async = act->ReadSrc = act->ReadDst=FALSE;
       Runner( act );
       caActions->DeleteAction( act );
     }else Runner( todo,filename );
   }
   PM_FreePopupMenu( ActionPMenu );
  }
  if( UnlockScreen ){
    UnlockPubScreen( NULL, Main->Scr );
    Main->Scr = NULL;
  }
  FreeVec( Tags );
}
//<

//>"void OpenVolumeMenu( UBYTE which )"
void OpenVolumeMenu( UBYTE which ){
  PopupMenu *VolumeMenu;
  TagItem *Tags = (TagItem*)AllocVec( 300*sizeof(TagItem),MEMF_CLEAR  ); // should be enough
  TagItem *CurrentTag=Tags;
  char *volumetitles[100],*dummy,*Name;
  int i;
  BOOL sorted=FALSE;
  for( i=0;i<100;i++ ) volumetitles[i]=NULL;

  DosList *dl=LockDosList(LDF_VOLUMES | LDF_READ);       // first the volumes
  i=0;
  while( dl=NextDosEntry(dl, LDF_VOLUMES) ){
   Name=(char*)BADDR(dl->dol_Name);
   if( Name[0]<256 ){
     volumetitles[i] = (char*)AllocVec(256,MEMF_CLEAR);
     memcpy( &(volumetitles[i][0]), Name + 1, Name[0] );
     volumetitles[i][Name[0]]=(char)0;
     strcat( volumetitles[i],":" );
     i++;
  }}
  UnLockDosList( LDF_VOLUMES | LDF_READ );
  while( not sorted ){
    sorted = TRUE;
    for( i=1;i<100;i++ ){
      if( volumetitles[i] is NULL ) break;
      if( stricmp(volumetitles[i-1],volumetitles[i])>0 ){
        dummy = volumetitles[i-1];
        volumetitles[i-1] = volumetitles[i];
        volumetitles[i] = dummy;
        sorted = FALSE;
      }
    }
  }

  CurrentTag=AddMenuTitle( CurrentTag,GetCatalogStr(Main->Catalog,MSG_VOLUMES,MSG_VOLUMES_STR) );
  for( i=0;(i<100) and volumetitles[i];i++ ){
    CurrentTag=AddMenuItem( CurrentTag,volumetitles[i],volumetitles[i] );
  }
  CurrentTag->ti_Tag = TAG_END;

  VolumeMenu = PM_MakeMenuA( Tags );
  if( VolumeMenu ) {
  char *path = (char*) PM_OpenPopupMenu( Main->IntuiWin,PM_Menu,VolumeMenu,
                                Main->MenuFont ? PM_ForceFont : TAG_IGNORE,Main->MenuFont,TAG_DONE );
  if( path ) ListDir( which,path,0 );
  PM_FreePopupMenu( VolumeMenu );
 }
 for( i=0;i<100;i++ ){
   if( volumetitles[i] is NULL ) break;
   FreeVec( volumetitles[i] );
 }
 FreeVec( Tags );
}
//<

//>"BOOL IsGlobalVariable( APTR var )"
BOOL IsGlobalVariable( APTR var ){
 if( ((ULONG)var >= (ULONG)&Main) and ((ULONG)var < (ULONG)&Main+sizeof(TMain)) )
  return( TRUE );
 else return( FALSE );
}
//<

//>"void BuildMenu()"
void BuildMenu(){
  struct PopupMenu *menProject, *menEdit, *menSettings, *menKeyCom;
  menProject = PM_MakeMenu(
         PMItem( GetCatalogStr(Main->Catalog,TXT_PROJECT,TXT_PROJECT_STR) ), PMSimpleSub,
                PMItem(GetCatalogStr(Main->Catalog,TXT_HELP,TXT_HELP_STR)),
                PM_UserData,caActions->Item("HELP"),
                PM_CommKey,"H",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_PREFERENCES,TXT_PREFERENCES_STR)),
                PM_UserData,caActions->Item("PREFS"),
                PM_CommKey,"P",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_ABOUT,TXT_ABOUT_STR)),
                PM_UserData,caActions->Item("ABOUT"),
                PM_CommKey,"A",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_ICONIFY,TXT_ICONIFY_STR)),
                PM_UserData,caActions->Item("ICONIFY"),
                PM_CommKey,"I",End,
                PMBar, End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_QUIT,TXT_QUIT_STR)),
                PM_UserData,caActions->Item("QUIT"),
                PM_CommKey,"Q",End,
                End,
         End );
  menEdit = PM_MakeMenu(
         PMItem( GetCatalogStr(Main->Catalog,TXT_EDIT,TXT_EDIT_STR) ), PMSimpleSub,
                PMItem(GetCatalogStr(Main->Catalog,TXT_COPY,TXT_COPY_STR)),
                PM_UserData,caActions->Item("CLIPCOPY"),
                PM_CommKey,"C",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_PASTE,TXT_PASTE_STR)),
                PM_UserData,caActions->Item("CLIPPASTE"),
                PM_CommKey,"V",End,
                PMBar,End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SELALL,TXT_SELALL_STR)),
                PM_UserData,caActions->Item("ALL"),
                PM_CommKey,"+",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SELNONE,TXT_SELNONE_STR)),
                PM_UserData,caActions->Item("NONE"),
                PM_CommKey,"-",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SELPATTERN,TXT_SELPATTERN_STR)),
                PM_UserData,caActions->Item("PATTERN"),
                PM_CommKey,"?",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SELTOGGLE,TXT_SELTOGGLE_STR)),
                PM_UserData,caActions->Item("TOGGLE"),
                PM_CommKey,"%",End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SELTYPE,TXT_SELTYPE_STR)),
                PM_UserData,caActions->Item("TYPE"),
                PM_CommKey,"=",End,
                PMBar,End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SEARCH,TXT_SEARCH_STR)),
                PM_UserData,caActions->Item("SEARCH"),
                PM_CommKey,"F",End,
                End,
         End );
  menSettings = PM_MakeMenu(
         PMItem(GetCatalogStr(Main->Catalog,TXT_SETTINGS,TXT_SETTINGS_STR)), PMSimpleSub,
                PMItem(GetCatalogStr(Main->Catalog,TXT_GENERAL,TXT_GENERAL_STR)), PMSimpleSub,
                        PMItem(GetCatalogStr(Main->Catalog,MSG_PULLDOWNMENU,MSG_PULLDOWNMENU_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->bPulldownMenu,End,
                        End,
                End,
                PMItem(GetCatalogStr(Main->Catalog,MSG_COPYANDDELETE,MSG_COPYANDDELETE_STR)), PMSimpleSub,
                        PMItem(GetCatalogStr(Main->Catalog,MSG_COUNTFILES,MSG_COUNTFILES_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->CountFiles,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_CHECKFIT,TXT_CHECKFIT_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->CheckFit,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_CONFIRMREPLACE,TXT_CONFIRMREPLACE_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ConfirmReplace,End,
                        PMItem(GetCatalogStr(Main->Catalog,MSG_SHOWVERSION,MSG_SHOWVERSION_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ShowVersion,End,
                        PMItem(GetCatalogStr(Main->Catalog,MSG_REPLACEVERSION,MSG_REPLACEVERSION_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ReplaceVersion,End,
                        PMItem(GetCatalogStr(Main->Catalog,MSG_CONFIRMDELETE,MSG_CONFIRMDELETE_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ConfirmDelete,End,
                        End,
                End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_LISTERS,TXT_LISTERS_STR)), PMSimpleSub,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_SHOWIMAGES,TXT_SHOWIMAGES_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ShowImages,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_HIGHDIRS,TXT_HIGHDIRS_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->HighlightDirs,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_SHOWDRAWERSFIRST,TXT_SHOWDRAWERSFIRST_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ShowDrawersFirst,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_DATESTRING,TXT_DATESTRING_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->DateString,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_DISPLAYTIME,TXT_DISPLAYTIME_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->DisplayTime,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_PATHNOTIFY,TXT_PATHNOTIFY_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->PathNotify,End,
                        End,
                End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_ICONS,TXT_ICONS_STR)), PMSimpleSub,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_DISPLAYINFO,TXT_DISPLAYINFO_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->ShowIcons,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_CREATEICONS,TXT_CREATEICONS_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->CreateIcons,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_COPYICONS,TXT_COPYICONS_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->CopyIcons,End,
                        PMItem(GetCatalogStr(Main->Catalog,TXT_DELETEICONS,TXT_DELETEICONS_STR)),
                        PM_Checkit,TRUE,PM_AutoStore,&Main->DeleteIcons,End,
                        End,
                End,
                PMBar,End,
                PMItem(GetCatalogStr(Main->Catalog,TXT_SNAPSHOTWINDOW,TXT_SNAPSHOTWINDOW_STR)),
                PM_UserData,caActions->Item("SNAPSHOT"),
                PM_CommKey,"S",End,
                End,
        End );
  TagItem *Tags = (TagItem*)AllocVec( 100*sizeof(TagItem),MEMF_CLEAR  ); // should be enough
  TagItem *CurrentTag=Tags;
  Node *nd=caIcons->GetCommandList(2)->lh_Head;
  while( nd->ln_Succ ){
    CurrentTag=AddMenuItem( CurrentTag,((IconNode*)nd)->Action->Name,((IconNode*)nd)->Action );
    nd = nd->ln_Succ;
  }
  CurrentTag->ti_Tag = TAG_END;
  menKeyCom = PM_MakeMenuA( Tags );
  FreeVec( Tags );

  Main->MainPopup = PM_MakeMenu(
    PM_Item,menProject,PM_Item,menEdit,PM_Item,menSettings,
    PM_Item,PM_MakeItem(PM_Title,GetCatalogStr(Main->Catalog,MSG_TOOLS,MSG_TOOLS_STR),PM_Sub,menKeyCom,TAG_END),
    TAG_END);
}
//<

//>"void BuildDirMenu()"
void BuildDirMenu(){
  Main->DirMenu = PM_MakeMenu(
    PM_Item,PM_MakeItem(
      PM_Title,GetCatalogStr(Main->Catalog,TXT_DIR,TXT_DIR_STR),PM_NoSelect,TRUE,PM_Bold,TRUE,TAG_DONE
    ),
    PMItem(GetCatalogStr(Main->Catalog,MSG_ROOTDIR,MSG_ROOTDIR_STR)),
    PM_UserData,caActions->Item("ROOT"),End,
    PMItem(GetCatalogStr(Main->Catalog,MSG_VOLUMES,MSG_VOLUMES_STR)),
    PM_UserData,caActions->Item("LIST"),End,
    PMItem(GetCatalogStr(Main->Catalog,MSG_MKDIR,MSG_MKDIR_STR)),
    PM_UserData,caActions->Item("MAKEDIR"),End,
    PMItem(GetCatalogStr(Main->Catalog,MSG_OPENWINDOW,MSG_OPENWINDOW_STR)),
    PM_UserData,caActions->Item("WINDOW"),End,
    PMItem(GetCatalogStr(Main->Catalog,MSG_ASSIGNMAKE,MSG_ASSIGNMAKE_STR)),
    PM_UserData,caActions->Item("ASSIGN"),End,
  End;
}
//<

//>"void OpenMenu()"
void OpenMenu(){
 if( Main->MainPopup ) {
  long y = IntuitionBase->MouseY;
  ULONG font = Main->MenuFont and !Main->bPulldownMenu;
  TAction *todo = (TAction*) PM_OpenPopupMenu( Main->IntuiWin,PM_Menu,Main->MainPopup,
                                font ? PM_ForceFont : TAG_IGNORE,Main->MenuFont,
                                PM_PullDown,Main->bPulldownMenu,
                                TAG_DONE );
  if( todo ){
   if( IsGlobalVariable(todo) ){
    BOOL *var = (BOOL*)todo;
    *var = ! (*var);
   }else Runner( todo );
}}}
//<

//>"void OpenDirMenu()"
void OpenDirMenu(){
  if( Main->DirMenu ) {
    TAction *todo = (TAction*) PM_OpenPopupMenu( Main->IntuiWin,PM_Menu,Main->DirMenu,
                                 Main->MenuFont ? PM_ForceFont : TAG_IGNORE,Main->MenuFont,
                                 TAG_DONE );
    if( todo ) Runner( todo );
  }
}
//<

//>"BOOL HandlePopupMessage( IntuiMessage *msg )"
BOOL HandlePopupMessage( IntuiMessage *msg ){
 if( msg->Qualifier & AMIGARIGHT ){
  switch( msg->Code ){
   case 'a': Runner(caActions->Item("ABOUT"));     break;
   case 'c': Runner(caActions->Item("CLIPCOPY"));  break;
   case 'f': Runner(caActions->Item("SEARCH"));    break;
   case 'h': Runner(caActions->Item("HELP"));      break;
   case 'i': Runner(caActions->Item("ICONIFY"));   break;
   case 'p': Runner(caActions->Item("PREFS"));     break;
   case 'q': Runner(caActions->Item("QUIT"));      break;
   case 's': Runner(caActions->Item("SNAPSHOT"));  break;
   case 'v': Runner(caActions->Item("CLIPPASTE")); break;
   case '+': Runner(caActions->Item("ALL"));       break;
   case '-': Runner(caActions->Item("NONE"));      break;
   case '?': Runner(caActions->Item("PATTERN"));   break;
   case '%': Runner(caActions->Item("TOGGLE"));    break;
   case '=': Runner(caActions->Item("TYPE"));      break;
   default : return( FALSE );
  }
  return( TRUE );
 }else return( FALSE );
}
//<

//>"void FreeMenu()"
void FreeMenu(){
 if( Main->MainPopup ){
  PM_FreePopupMenu( Main->MainPopup );
  Main->MainPopup = NULL;
 }
}
//<

//>"void FreeDirMenu()"
void FreeDirMenu(){
 if( Main->DirMenu ){
  PM_FreePopupMenu( Main->DirMenu );
  Main->DirMenu = NULL;
 }
}
//<

//>"char *OpenPathMenu()"
char *OpenPathMenu(){
  PopupMenu *PathMenu;
  TUserPathNode *nd;
  char *Result;
  TagItem *Tags = (TagItem*)AllocVec( 300*sizeof(TagItem),MEMF_CLEAR  ); // should be enough
  TagItem *CurrentTag=Tags;
  CurrentTag=AddMenuTitle( CurrentTag,GetCatalogStr(Main->Catalog,MSG_PATHS,MSG_PATHS_STR) );

  for( nd=(TUserPathNode*)Main->UserPaths.lh_Head;nd->ln_Succ;nd=(TUserPathNode*)nd->ln_Succ ){
    if( nd->Name[0] and nd->Path[0]  ) CurrentTag=AddMenuItem( CurrentTag,nd->Name,nd->Path );
  }
  CurrentTag->ti_Tag = TAG_END;
  PathMenu = PM_MakeMenuA( Tags );
  if( PathMenu ) {
    Result = (char*)PM_OpenPopupMenu( Main->IntuiWin,PM_Menu,PathMenu,PM_ForceFont,Main->MenuFont,TAG_DONE );
    PM_FreePopupMenu( PathMenu );
  }else Result = NULL;
  FreeVec( Tags );
  return( Result );
}
//<

//>"char *OpenHistoryMenu()"
char *OpenHistoryMenu(){
 PopupMenu *HistoryMenu;
 char *Result;
 TagItem *Tags = (TagItem*)AllocVec( 300*sizeof(TagItem),MEMF_CLEAR  ); // should be enough
 TagItem *CurrentTag=Tags;
 CurrentTag=AddMenuTitle( CurrentTag,GetCatalogStr(Main->Catalog,MSG_VISITEDPATHS,MSG_VISITEDPATHS_STR) );
 for( TNameNode *node=(TNameNode*)Main->History.lh_Head;node->ln_Succ;node=(TNameNode*)node->ln_Succ ){
  CurrentTag=AddMenuItem( CurrentTag,node->Name,node->Name );
 }
 CurrentTag->ti_Tag = TAG_END;
 HistoryMenu = PM_MakeMenuA( Tags );
 if( HistoryMenu ) {
  Result = (char*)PM_OpenPopupMenu( Main->IntuiWin,PM_Menu,HistoryMenu,PM_ForceFont,Main->MenuFont,TAG_DONE );
  PM_FreePopupMenu( HistoryMenu );
 }else Result = NULL;
 FreeVec( Tags );
 return( Result );
}
//<
