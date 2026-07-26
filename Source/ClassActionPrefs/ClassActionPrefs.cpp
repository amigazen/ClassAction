#include <clib/alib_protos.h>
#include <classes/window.h>
#include <gadgets/chooser.h>
#include <gadgets/clicktab.h>
#include <gadgets/listbrowser.h>
#include <intuition/intuitionbase.h>
#include <libraries/gadtools.h>
#include <libraries/pm.h>
#include <proto/amigaguide.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/listbrowser.h>
#include <proto/locale.h>
#include <proto/pm.h>
#include <proto/resource.h>
#include <stdlib.h>
#include <string.h>
#include <workbench/icon.h>

#define  CATCOMP_NUMBERS
#define  CATCOMP_STRINGS
#include "classactionprefscd.h"

#include "classactionprefs.h"
#include "classactionprefs_rev.h"

#include "capgui.h"
#include "caplearn.h"
#include "capreq.h"
#include "capopt.h"
#include "capdisk.h"
#include "caplist.h"
#include "capbuttclass.h"

char Soft[]=VERSTAG;

Library    *DosBase;
Library    *IconBase;
Library    *GadToolsBase;
Library    *ResourceBase;
Library    *WorkbenchBase;
Library    *WindowBase;
Library    *LayoutBase;
Library    *ListBrowserBase;
Library    *ChooserBase;
Library    *ButtonBase;
Library    *StringBase;
Library    *SpaceBase;
Library    *AslBase;
Library    *AmigaguideBase;
Library    *DataBase;
Library    *RexxSysBase;
tPopupMenuBase *PopupMenuBase;

TMain Main;

//>"structs"

BOOL DosStart=TRUE;
BPTR Oldlock,Olddir;

#define MENU_ID_Open      FULLMENUNUM(0,0,NOSUB)
#define MENU_ID_SaveAs    FULLMENUNUM(0,1,NOSUB)
#define MENU_ID_Help      FULLMENUNUM(0,2,NOSUB)
#define MENU_ID_Quit      FULLMENUNUM(0,4,NOSUB)
#define MENU_ID_Reset     FULLMENUNUM(1,0,NOSUB)
#define MENU_ID_LastSaved FULLMENUNUM(1,1,NOSUB)
#define MENU_ID_Restore   FULLMENUNUM(1,2,NOSUB)
#define MENU_ID_ShowHelp  FULLMENUNUM(2,0,NOSUB)
#define MENU_PTR_ShowHelp MenuFlagPtr(2,0)

char Quot[2];

struct NewMenu MenuData[] ={
  NM_TITLE, (STRPTR)TXT_PROJECT_STR,   NULL, 0, 0, (APTR)TXT_PROJECT,
  NM_ITEM,  (STRPTR)MEN_OPEN_STR,      NULL, 0, 0, (APTR)MEN_OPEN,
  NM_ITEM,  (STRPTR)MEN_SAVEAS_STR,    NULL, 0, 0, (APTR)MEN_SAVEAS,
  NM_ITEM,  (STRPTR)MEN_HELP_STR,      NULL, 0, 0, (APTR)MEN_HELP,
  NM_ITEM,  NM_BARLABEL,               NULL, 0, 0, NULL,
  NM_ITEM,  (STRPTR)MEN_QUIT_STR,      NULL, 0, 0, (APTR)MEN_QUIT,

  NM_TITLE, (STRPTR)TXT_EDIT_STR,      NULL, 0, 0, (APTR)TXT_EDIT,
  NM_ITEM,  (STRPTR)MEN_RESET_STR,     NULL, 0, 0, (APTR)MEN_RESET,
  NM_ITEM,  (STRPTR)MEN_LASTSAVED_STR, NULL, 0, 0, (APTR)MEN_LASTSAVED,
  NM_ITEM,  (STRPTR)MEN_RESTORE_STR,   NULL, 0, 0, (APTR)MEN_RESTORE,

  NM_TITLE, (STRPTR)TXT_OPTIONS_STR,   NULL, 0, 0, (APTR)TXT_OPTIONS,
  NM_ITEM,  (STRPTR)MEN_SHOWHELP_STR,  NULL, CHECKIT|MENUTOGGLE, 0, (APTR)MEN_SHOWHELP,
  NM_END, NULL, NULL, 0, 0, NULL
 };

struct TagItem ReqTags[]={
 {ASLFR_Screen, NULL },
 {ASLFR_RejectIcons, TRUE },
};

char MenuKeys[20];

//<

//>"void ShowHint( int HintCat,char *Hint )"
void ShowHint( int HintCat,char *Hint ){
 PopupMenu *Pop = PM_MakeMenu(
   PMItem(GetCatalogStr(Main.Catalog,HintCat,Hint)),End,
 End;
 if( Pop ) {
  PM_OpenPopupMenu(
   Main.IntuiWin,PM_Menu,Pop,PM_Left,Main.IntuiWin->MouseX,
   PM_Bottom,Main.IntuiWin->MouseY,PM_HintBox,TRUE,TAG_DONE
  );
  PM_FreePopupMenu( Pop );
 }
}
//<

//>"void MenuShowHelp( char *link=LINK main )"
void MenuShowHelp( char *link ){
 if( Main.GuideHandle ) SendAmigaGuideCmd( Main.GuideHandle,link,NULL );
}
//<

//>"UWORD MenuFlagPtr( int menu, int item )"
UWORD *MenuFlagPtr( int menu, int item ){
 int i;
 struct Menu *men=Main.Menu;
 struct MenuItem *itm;
 for( i=0;i<menu;i++ ) men=men->NextMenu;
 itm=men->FirstItem;
 for( i=0;i<item;i++ ) itm=itm->NextItem;
 return( &(itm->Flags) );
}
//<

//>"void HandleAppMessage()"
/****************************************/
/*                                      */
/* Function : HandleAppMessage          */
/*                                      */
/* Goal     : Handle App Messages       */
/*                                      */
/****************************************/

void HandleAppMessage(){

 AppMessage *appmsg;
 WBArg *argptr;
 int i;
 ULONG sel;
 CommandNode *node;
 List *l;

 while( appmsg=(AppMessage*)GetMsg( Main.AppPort ) ){
  switch( appmsg->am_Type ){
   case AMTYPE_APPWINDOW:{
    argptr=appmsg->am_ArgList;
    char *f=new char[512];
    char *name=new char[512];
    NameFromLock( argptr->wa_Lock,f,512 );
    if( argptr->wa_Name[0] ){
     strcpy( name,argptr->wa_Name );
     AddPart( f,name,512 );
    }else{
     char *ptr;
     if( ptr=strchr(f,'/') ) strcpy( name,ptr+1 );      // normal directory
     else{
      ptr=strchr(f,':');
      if( ptr ){
       if( *(ptr+1)==0 ) strcpy( name,f );               // device
       else strcpy( name,ptr+1 );                        // directory in root dir
     }}
    }
    GetAttr( CLICKTAB_Current,Main.Root[CLICKTAB],&sel );
    if( sel==1 ){
     AddCommand();
     GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );
     GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&l );
     strcpy( node->name,name );
     strcpy( node->help,name );
     strcpy( node->exec,f );
     strcpy( node->icon,f );
     node->wbrun=RUN_WB;
     SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
                    LISTBROWSER_Labels,NULL,TAG_END );
     SetListBrowserNodeAttrs( (Node*)node,LBNCA_Text,node->name,TAG_END );
     SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                         LISTBROWSER_Labels,l,TAG_END );
     AdjustComGadgets();
    }else if( sel==2 ){
     if( Main.SelectedClass ){
      AppendAction();
      strncpy( Main.SelectedAction->Name,name,20 );
      strncpy( Main.SelectedAction->Exec,f,250 );
      Main.SelectedAction->Type=RUN_WB;
      GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_ACTIONS],(ULONG*)&node );
      GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS],(ULONG*)&l );
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_ACTIONS],NULL,NULL,
                     LISTBROWSER_Labels,NULL,TAG_END );
      SetListBrowserNodeAttrs( (Node*)node,LBNCA_Text,Main.SelectedAction->Name,TAG_END );
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin,NULL,
                          LISTBROWSER_Labels,l,TAG_END );
      Lwa();
     }
    }else if( sel==3 ){
      AddPath( f );
    }
    for( i=0;i<appmsg->am_NumArgs;i++ ) UnLock( appmsg->am_ArgList[i].wa_Lock );
    delete name;
    delete f;
 }}}
}
//<

//>"void MessageLoop()"
void MessageLoop(){
 ULONG windowsignals,mask, result, code;
 struct EasyStruct Requester={ sizeof(struct EasyStruct),0,NULL,NULL,NULL };

 Main.Leave=FALSE;
 GetAttr( WINDOW_SigMask, Main.Win, &windowsignals );
 while( !Main.Leave ){
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Main.Leave=TRUE;
  if( (!Main.Leave)&&(mask & windowsignals) ){
   HandleAppMessage();
   while( (result = DoMethod( Main.Win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_CLOSEWINDOW:{
      Main.Leave=TRUE;
      break;
     }
     case WMHI_RAWKEY:{
      ULONG click;
      GetAttr( CLICKTAB_Current,Main.Root[CLICKTAB],&click );
      if( click==1 ) MoveCommandLister( result & WMHI_KEYMASK );
      else if( click==2 ) MoveClassLister( result & WMHI_KEYMASK );
      break;
     }
//MENUPICK---------------------------------------------
     case WMHI_MENUPICK:{
      switch(result & WMHI_MENUMASK){
       case MENU_ID_Open:       MenuOpen();             break;
       case MENU_ID_SaveAs:     MenuSaveAs();           break;
       case MENU_ID_Help:{
         ULONG click;
         GetAttr( CLICKTAB_Current,Main.Root[CLICKTAB],&click );
         switch( click ){
           case 0: MenuShowHelp("LINK options"); break;
           case 1: MenuShowHelp("LINK commands");break;
           case 2: MenuShowHelp("LINK classes"); break;
           case 3: MenuShowHelp("LINK paths");   break;
         }
         break;
       }
       case MENU_ID_Reset:      MenuReset();            break;
       case MENU_ID_LastSaved:  MenuLastSaved();        break;
       case MENU_ID_Restore:    MenuRestore( TRUE );    break;
       case MENU_ID_Quit:       Main.Leave=TRUE;        break;
       case MENU_ID_ShowHelp:{
         Main.ShowHelp = (*MENU_PTR_ShowHelp & CHECKED);
         break;
       }
      }
      break;
     }
//GADGETHELP-------------------------------------------
     case WMHI_GADGETHELP:{
      if( Main.ShowHelp ){
       switch( result & RL_GADGETMASK ){
        // Options:
//        case LIST_OPTIONS:       SetHString(TXT_BUTTOPTIONS,TXT_BUTTOPTIONS_STR); break;
        case LIST_OPTIONS:       ShowHint(TXT_BUTTOPTIONS,TXT_BUTTOPTIONS_STR); break;
        // Commands:
        case LIST_COMMANDS:      ShowHint(TXT_BUTTLCOM,TXT_BUTTLCOM_STR);     break;
        case BUTTON_CUP:         ShowHint(TXT_BUTTCUP,TXT_BUTTCUP_STR);       break;
        case BUTTON_CDOWN:       ShowHint(TXT_BUTTCDOWN,TXT_BUTTCDOWN_STR);   break;
        case BUTTON_CADD:        ShowHint(TXT_BUTTCADD,TXT_BUTTCADD_STR);     break;
        case BUTTON_CREM:        ShowHint(TXT_BUTTCREM,TXT_BUTTCREM_STR);     break;
        case STRING_CNAME:       ShowHint(TXT_BUTTCNAME,TXT_BUTTCNAME_STR);   break;
        case STRING_CHELP:       ShowHint(TXT_BUTTCHELP,TXT_BUTTCHELP_STR);   break;
        case GETFILE_ICON:       ShowHint(TXT_BUTTGTICON,TXT_BUTTGTICON_STR); break;
        case BUTTON_PREVIEW:     ShowHint(TXT_BUTTPREVIEW,TXT_BUTTPREVIEW_STR); break;
        case BUTTON_OPENICONS:   ShowHint(MSG_BUTTOPENICONS,MSG_BUTTOPENICONS_STR); break;
        case BUTTON_KEY:         ShowHint(TXT_BUTTKEY,TXT_BUTTKEY_STR);       break;
        case STRING_KEY:         ShowHint(TXT_BUTTSTRKEY,TXT_BUTTSTRKEY_STR); break;
        case CHOOSER_KEY:        ShowHint(TXT_BUTTCHKEY,TXT_BUTTCHKEY_STR);   break;
        // Classes:
        case LIST_CLASSES:       ShowHint(TXT_BUTTLST1,TXT_BUTTLST1_STR);     break;
        case LIST_ACTIONS:       ShowHint(TXT_BUTTLST2,TXT_BUTTLST2_STR);     break;
        case STRING_CLASS:       ShowHint(TXT_BUTTCLASS,TXT_BUTTCLASS_STR);   break;
        case STRING_CLASSIMAGE:  ShowHint(MSG_BUTTCLASSIMAGE,MSG_BUTTCLASSIMAGE_STR); break;
        case BUTTON_LOADIMAGE:   ShowHint(MSG_BUTTLOADIMAGE,MSG_BUTTLOADIMAGE_STR);   break;
        case STRING_MATCH:       ShowHint(TXT_BUTTMATCH,TXT_BUTTMATCH_STR);   break;
        case STRING_OFFSET:      ShowHint(TXT_BUTTOFFSET,TXT_BUTTOFFSET_STR); break;
        case STRING_ANAME:       ShowHint(TXT_BUTTANAME,TXT_BUTTANAME_STR);   break;
        case STRING_COMMAND:
        case STRING_ACOMMAND:    ShowHint(TXT_BUTTCOMMAND,TXT_BUTTCOMMAND_STR);      break;
        case STRING_CSTACK:
        case STRING_STACK:       ShowHint(TXT_BUTTSTACK,TXT_BUTTSTACK_STR);   break;
        case STRING_CDELAY:
        case STRING_DELAY:       ShowHint(TXT_BUTTDELAY,TXT_BUTTDELAY_STR);   break;
        case BUTTON_ADD:         ShowHint(TXT_BUTTADD,TXT_BUTTADD_STR);       break;
        case BUTTON_REMOVE:      ShowHint(TXT_BUTTREMOVE,TXT_BUTTREMOVE_STR); break;
        case BUTTON_UP:          ShowHint(TXT_BUTTUP,TXT_BUTTUP_STR);         break;
        case BUTTON_DOWN:        ShowHint(TXT_BUTTDOWN,TXT_BUTTDOWN_STR);     break;
        case BUTTON_ADD2:        ShowHint(TXT_BUTTADD2,TXT_BUTTADD2_STR);     break;
        case BUTTON_REMOVE2:     ShowHint(TXT_BUTTREMOVE2,TXT_BUTTREMOVE2_STR);      break;
        case BUTTON_CLOAD:
        case BUTTON_LOAD:        ShowHint(TXT_BUTTLOAD,TXT_BUTTLOAD_STR);     break;
        case BUTTON_SAVE:        ShowHint(TXT_BUTTSAVE,TXT_BUTTSAVE_STR);     break;
        case BUTTON_USE:         ShowHint(TXT_BUTTUSE,TXT_BUTTUSE_STR);       break;
        case BUTTON_CANCEL:      ShowHint(TXT_BUTTCANCEL,TXT_BUTTCANCEL_STR); break;
        case CHOOSER_OFFSET:     ShowHint(TXT_BUTTCHOFF,TXT_BUTTCHOFF_STR);   break;
        case CHOOSER_CMODE:
        case CHOOSER_MODE:       ShowHint(TXT_BUTTMODE,TXT_BUTTMODE_STR);     break;
        case CHOOSER_CDIR:
        case CHOOSER_DIR:        ShowHint(MSG_BUTTDIR,MSG_BUTTDIR_STR);       break;
        case CHOOSER_INT:
        case CHOOSER_AINT:       ShowHint(TXT_BUTTINT,TXT_BUTTINT_STR);       break;
        case CHOOSER_CVARIABLES:
        case CHOOSER_AVARIABLES: ShowHint(TXT_BUTTCOMM,TXT_BUTTCOMM_STR);      break;
        // Paths:
        case LIST_PATHS:         ShowHint(TXT_BUTTLPATHS,TXT_BUTTLPATHS_STR);     break;
        case BUTTON_PUP:         ShowHint(TXT_BUTTPUP,TXT_BUTTPUP_STR);       break;
        case BUTTON_PDOWN:       ShowHint(TXT_BUTTPDOWN,TXT_BUTTPDOWN_STR);   break;
        case BUTTON_PADD:        ShowHint(TXT_BUTTPADD,TXT_BUTTPADD_STR);     break;
        case BUTTON_PREM:        ShowHint(TXT_BUTTPREM,TXT_BUTTPREM_STR);     break;
      }}
      break;
     }
//GADGETUP---------------------------------------------
     case WMHI_GADGETUP:{
      switch( result & RL_GADGETMASK ){
       // Options:
       case LIST_OPTIONS:       OptionList.HandleInput();break;
       // Commands:
       case LIST_COMMANDS:{
         GetAttr( LISTBROWSER_RelEvent,Main.Gadgets[LIST_COMMANDS], &code );
         if( code==LBRE_NORMAL ) AdjustComGadgets();
         break;}
       case BUTTON_CUP:         MoveComUp();     break;
       case BUTTON_CDOWN:       MoveComDown();   break;
       case BUTTON_CADD:        AddCommand();    break;
       case BUTTON_CREM:        RemoveCommand(); break;
       case STRING_CNAME:       AdjustCName();   break;
       case STRING_CHELP:       AdjustCHelp();   break;
       case STRING_COMMAND:     AdjustCommand(); break;
       case CHOOSER_CMODE:      AdjustCMode();   break;
       case CHOOSER_CDIR:       AdjustCCD();     break;
       case STRING_CSTACK:      AdjustCStack();  break;
       case STRING_CDELAY:      AdjustCDelay();  break;
       case CHECK_CRESOURCE:
       case CHECK_CREDEST:
       case CHECK_CASYNC:       AdjustCDir();   break;
       case BUTTON_CLOAD:       LoadCom();      break;
       case CHOOSER_INT:        InsertCom();    break;
       case CHOOSER_CVARIABLES: InsertVar();    break;
       case GETFILE_ICON:       GetIcon();      break;
       case BUTTON_PREVIEW:     Preview();      break;
       case BUTTON_OPENICONS:   OpenIconWindow(); break;
       case BUTTON_KEY:         EnterKey();     break;
       case STRING_KEY:         AdjustKey();    break;
       case CHOOSER_KEY:        AdjustKeyMode(); break;
       // Classes:
       case LIST_CLASSES:{
         GetAttr( LISTBROWSER_RelEvent,Main.Gadgets[LIST_CLASSES], &code );
         if( code==LBRE_NORMAL ) Lwc();
         break;}
       case STRING_CLASS:       StNomc();       break;
       case CHECK_CARCHIVE:     CheckArchiveOnClick();break;
       case STRING_CLASSIMAGE:  ImageFileOnChange();  break;
       case BUTTON_LOADIMAGE:   ImageFileRequest();   break;
       case STRING_MATCH:       StFiltre();     break;
       case STRING_OFFSET:      StOffset();     break;
       case BUTTON_ADD:         AppendClass();  break;
       case BUTTON_REMOVE:      DeleteClass();  break;
       case CHOOSER_CLASSCP:{
         ULONG Sel;
         GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_CLASSCP], &Sel );
         switch( Sel ){
           case 0: capGui.DuplicateClass();     break;
           case 1: Learn();     break;
           case 2: capGui.ImportClass();        break;
           case 3: capGui.ExportClass();        break;}
         break;}
       case BUTTON_UP:          Up();           break;
       case BUTTON_DOWN:        Down();         break;
       // Actions:
       case LIST_ACTIONS:{
         GetAttr( LISTBROWSER_RelEvent,Main.Gadgets[LIST_ACTIONS], &code );
         if( code==LBRE_NORMAL ) Lwa();
         break;}
       case STRING_ANAME:       StNoma();  break;
       case STRING_ACOMMAND:    StExec();  break;
       case STRING_STACK:       StPile();  break;
       case STRING_DELAY:       StDelay(); break;
       case CHOOSER_OFFSET:     CyOffset();     break;
       case CHOOSER_MODE:       CyMode();       break;
       case CHOOSER_DIR:        AdjustCD();     break;
       case CHOOSER_AINT:       InsertComA();   break;
       case CHOOSER_AVARIABLES: LwCom();        break;
       case BUTTON_LOAD:        Sel();  break;
       case CHECK_RESOURCE:
       case CHECK_REDEST:
       case CHECK_ASYNC:     ChRescan();     break;
       case BUTTON_ADD2:        AppendAction(); break;
       case BUTTON_REMOVE2:     DeleteAction(); break;
       case CHOOSER_ACTIONCP:{
         ULONG Sel;
         GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_ACTIONCP], &Sel );
         switch( Sel ){
           case 0: capGui.CopyAction(); break;
           case 1: capGui.PasteAction();break;}
         break;}

        // Paths:
       case LIST_PATHS:{
         GetAttr( LISTBROWSER_RelEvent,Main.Gadgets[LIST_PATHS], &code );
         if( code==LBRE_DOUBLECLICK ) PathListDblClick();
         else if( code==LBRE_NORMAL ) AdjustPathGadgets();
         break;}
       case BUTTON_PUP:         MovePathUp();   break;
       case BUTTON_PDOWN:       MovePathDown(); break;
       case BUTTON_PADD:        AddPath();      break;
       case BUTTON_PREM:        RemovePath();   break;

       // General:
       case BUTTON_SAVE:        Save(); break;
       case BUTTON_USE:         Use();  break;
       case BUTTON_CANCEL:      Quit();         break;
      }
      break;
     }
    }
   }
 }}
}
//<

//>"void CreateMenu()"
void CreateMenu(){
 char *ptr;
 int i=0;

 struct NewMenu *item;
 for( item=&MenuData[0]; item->nm_Type!=NM_END; item++ ){
  if( item->nm_UserData ){
   ptr=(STRPTR)GetCatalogStr( Main.Catalog,(ULONG)item->nm_UserData,(STRPTR)item->nm_Label );
   if( item->nm_Type==NM_ITEM ){
    MenuKeys[i]=*ptr;
    MenuKeys[i+1]=0;
    item->nm_CommKey=&(MenuKeys[i]);
    i+=2;
    item->nm_Label = ptr+2;     // name begins after x_
   }else{
    item->nm_CommKey=NULL;
    item->nm_Label = ptr;
}}}}
//<

//>"BOOL OpenLibs()"
BOOL OpenLibs(){
 if( !(DosBase=OpenLibrary( "dos.library", 39L ))) return(FALSE);
 if( !(IconBase=OpenLibrary( "icon.library", 39L ))) return(FALSE);
 if( !(GadToolsBase=OpenLibrary( "gadtools.library", 39L ))) return(FALSE);
 if( !(ResourceBase=OpenLibrary( "resource.library", 39L ))) return(FALSE);
 if( !(WorkbenchBase=OpenLibrary( "workbench.library", 39L ))) return(FALSE);
 if( !(AslBase=OpenLibrary( "asl.library", 39L ))) return(FALSE);
 if( !(AmigaguideBase=OpenLibrary( "amigaguide.library", 39L ))) return(FALSE);
 if( !(WindowBase = OpenLibrary("window.class", 44))) return(FALSE);
 if( !(LayoutBase = OpenLibrary("gadgets/layout.gadget", 44))) return(FALSE);
 if( !(ListBrowserBase = OpenLibrary("gadgets/listbrowser.gadget", 44))) return(FALSE);
 if( !(ChooserBase = OpenLibrary("gadgets/chooser.gadget", 44))) return(FALSE);
 if( !(ButtonBase = OpenLibrary("gadgets/button.gadget", 44))) return(FALSE);
 if( !(StringBase = OpenLibrary("gadgets/string.gadget", 44))) return(FALSE);
 if( !(SpaceBase = OpenLibrary("gadgets/space.gadget", 44))) return(FALSE);
 if( !(DataBase=OpenLibrary("datatypes.library",0)) ) return(FALSE);
 if( !(RexxSysBase=OpenLibrary("rexxsyslib.library",0)) ) return(FALSE);
 if( !(PopupMenuBase=(tPopupMenuBase*)OpenLibrary("popupmenu.library",0)) ) return(FALSE);
 return( TRUE );
}
//<

//>"void CloseLibs()"
void CloseLibs(){
 CloseLibrary( (Library*)PopupMenuBase );
 CloseLibrary( RexxSysBase );
 CloseLibrary( DataBase );
 CloseLibrary( SpaceBase );
 CloseLibrary( StringBase );
 CloseLibrary( ButtonBase );
 CloseLibrary( ChooserBase );
 CloseLibrary( ListBrowserBase );
 CloseLibrary( LayoutBase );
 CloseLibrary( WindowBase );
 CloseLibrary( AmigaguideBase );
 CloseLibrary( AslBase );
 CloseLibrary( WorkbenchBase );
 CloseLibrary( ResourceBase );
 CloseLibrary( GadToolsBase );
 CloseLibrary( IconBase );
 CloseLibrary( DosBase );
}
//<

//>"BOOL InitAll()"
/****************************************/
/*                                      */
/* Function : InitAll                   */
/*                                      */
/* Goal     : Allocate ALL needed ress  */
/*                                      */
/****************************************/
BOOL InitAll(){
 BPTR lock;
 UWORD wpos=WPOS_CENTERSCREEN;

 Main.IconClass=NULL;
 Main.Scr=NULL; Main.AppPort=Main.MsgPort=NULL; Main.Resource=NULL;
 Main.Win=Main.LWin=NULL;
 Main.IntuiWin=NULL;
 Main.Dobj=NULL;
 Main.SelectedClass=NULL;
 Main.SelectedAction=NULL;
 Main.CommandEnabled=FALSE;
 Main.Ret[0]="[F]";
 Main.Ret[1]="[S]";
 Main.Ret[2]="[B]";
 Main.Ret[3]="[X]";
 Main.Ret[4]="[SRC]";
 Main.Ret[5]="[SRCS]";
 Main.Ret[6]="[DST]";
 Main.Ret[7]="[DSTS]";
 Main.Ret[8]="[REQF:";
 Main.Ret[9]="[REQD:";
 Main.Ret[10]="[REQT:";
 Main.Ret[11]="[SURE:";
 Main.ClassEnabled  = TRUE;
 Main.ActionsActive = FALSE;
 Main.PrefsFile =(char*)malloc( 512 );
 Main.PublicScreen =(char*)malloc( 512 );
 NewList( &Main.Classes );
 Main.ClassCount = 0;

 if( !OpenLibs() ){
   Info( "Not all required libraries have been found." );
   return( FALSE );}
// if( DosStart ){
  Olddir = CurrentDir(GetProgramDir());
// }else{
//  Olddir = CurrentDir(Oldlock);       // i thought this was right but it seems not to work
// }

 Main.Catalog=OpenCatalog( NULL,"ClassActionPrefs.catalog",OC_BuiltInLanguage,"english",
   OC_Version,CATVERS,TAG_DONE );

 // first we need our own icon ...
 if( !(Main.Dobj=GetIconTags( "classactionprefs", ICONGETA_FailIfUnavailable, FALSE, TAG_END)) ){
   Info( GetCatalogStr(Main.Catalog,ERR_NOICON,ERR_NOICON_STR) );
   return( FALSE );}

 // then we check the tooltypes:
 if( !ReadToolTypes() ) return( FALSE );


 TIniFile IniFile;
 strcpy( Main.PrefsFile,TPREFSFILE );
 if( !IniFile.LoadFromFile( Main.PrefsFile ) ){
   if( !IniFile.LoadFromFile( TPREFSFILE ) ){
     IniFile.LoadFromFile( PREFSFILE ); // if this fails, the standard values are taken
   }
 }
 Main.PrefsVersion = IniFile.GetInt("INFO","VERSION",0);
 // this is the only value we need now:
 IniFile.GetString( "General","PUBLICSCREEN","",Main.PublicScreen );

 if( ((struct IntuitionBase*)IntuitionBase)->ActiveScreen->Flags & PUBLICSCREEN ){
   Screen *scr=LockPubScreen(NULL);
   Main.Scr=scr;
   do{
     NextPubScreen(Main.Scr,Main.PublicScreen);
     Main.Scr=LockPubScreen(Main.PublicScreen);
   }while( (Main.Scr!=scr) && (Main.Scr!=IntuitionBase->ActiveScreen) );
 }
 if( !Main.Scr ) if( !(Main.Scr=LockPubScreen(NULL)) ) return( FALSE );
/*
 if( Main.PublicScreen[0] ){
  if( !(Main.Scr=LockPubScreen(Main.PublicScreen)) ){
   if( !(Main.Scr=LockPubScreen(NULL)) ) return( FALSE );
  }
 }else if( !(Main.Scr=LockPubScreen(NULL)) ) return( FALSE );
*/
 if( !(Main.AppPort=CreateMsgPort()) ) return( FALSE );
 if( !(Main.MsgPort=CreateMsgPort()) ) return( FALSE );
 if( !(Main.Resource=RL_OpenResource(RCTResource,Main.Scr,Main.Catalog)) ) return( FALSE );
 CreateMenu();
 Main.Visualinfo=GetVisualInfoA( Main.Scr,TAG_DONE );
 Main.Menu=CreateMenusA( MenuData,NULL );
 LayoutMenus( Main.Menu,Main.Visualinfo,GTMN_NewLookMenus,TRUE,TAG_DONE );
 if( (Main.WinX!=-1)||(Main.WinY!=-1) ) wpos=0;
 if( !(Main.Win=RL_NewObject( Main.Resource,WIN_1_ID,
                              WA_Title,GetCatalogStr(Main.Catalog,TXT_PROG,TXT_PROG_STR),
                              WA_Top,Main.WinY,WA_Left,Main.WinX,
                              WA_Width,Main.WinWidth,WA_Height,Main.WinHeight,
                              WA_PubScreen,Main.Scr,
                              WINDOW_AppPort,Main.AppPort,
                              WINDOW_AppWindow,TRUE,
                              WINDOW_SharedPort,Main.MsgPort,
                              WINDOW_MenuStrip,Main.Menu,
                              WINDOW_MenuUserData,WGUD_IGNORE,
                              WINDOW_Position,wpos,
                              TAG_END)) ) return( FALSE );
 if( !(Main.LWin=RL_NewObject( Main.Resource,WIN_2_ID,
                               WA_PubScreen,Main.Scr,
                               WINDOW_SharedPort,Main.MsgPort,
                               TAG_END)) ) return( FALSE );

 Main.Root=RL_GetObjectArray( Main.Resource, Main.Win, GROUP_0_ID );
 Main.Gadgets=RL_GetObjectArray( Main.Resource, Main.Win, GROUP_1_ID );
 Main.LGadgets=RL_GetObjectArray( Main.Resource, Main.LWin, GROUP_2_ID );

 Main.IconClass=MakeIconClass();

 // now we get the rest:
 LoadOptions( &IniFile );
 LoadPaths( &IniFile );
 LoadClasses( &IniFile );
 LoadCommands( &IniFile );
 ListClasses();

 Main.GuideHandle = NULL;
 Main.NewAG.nag_Screen=Main.Scr;
 Main.NewAG.nag_Name="MRE:Doc/ClassAction.guide";
 if( !(lock=Lock(Main.NewAG.nag_Name,ACCESS_READ)) ){
 Main.NewAG.nag_Name="HELP:ClassAction.guide";
   lock=Lock(Main.NewAG.nag_Name,ACCESS_READ);
 }
 if( lock ){
  UnLock( lock );
  Main.GuideHandle = OpenAmigaGuideAsyncA( &Main.NewAG, NULL );
 }

 DoMethod( Main.Win, WM_OPEN );
 GetAttr( WINDOW_Window, Main.Win, (ULONG *)&Main.IntuiWin );

 ReqTags[0].ti_Data = (ULONG)Main.Scr;
 Main.FileReq = (FileRequester *)AllocAslRequest( ASL_FileRequest, ReqTags );
 ReqTags[1].ti_Tag  = TAG_END; // now we don't need the hide icon tag any more
 ReqTags[0].ti_Tag  = ASLFO_Screen;
 Main.FontReq = (FontRequester *)AllocAslRequest( ASL_FontRequest, ReqTags );
 ReqTags[0].ti_Tag  = ASLSM_Screen;
 Main.ScrReq  = (ScreenModeRequester *)AllocAslRequest( ASL_ScreenModeRequest, ReqTags );

 return( TRUE );
}
//<

//>"void FreeAll()"
/****************************************/
/*                                      */
/* Function : FreeAll                   */
/*                                      */
/* Goal     : Free ALL allocated ress   */
/*                                      */
/****************************************/
void FreeAll(){
 FreeClassList();
 FreeAslRequest( Main.ScrReq );
 FreeAslRequest( Main.FontReq );
 FreeAslRequest( Main.FileReq );
 DoMethod( Main.Win, WM_CLOSE );
 FreeMenus( Main.Menu );
 FreeVisualInfo( Main.Visualinfo );
 if( Main.Resource ) RL_CloseResource( Main.Resource );
 if( Main.MsgPort )  DeleteMsgPort( Main.MsgPort );
 if( Main.AppPort )  DeleteMsgPort( Main.AppPort );
 if( Main.Scr )      UnlockPubScreen( NULL, Main.Scr );
 CloseCatalog(Main.Catalog);
 if( Main.GuideHandle ) CloseAmigaGuide( Main.GuideHandle );
 if( DosStart ) CurrentDir( Olddir );
 free( Main.PublicScreen );
 free( Main.PrefsFile );
 RemoveIconClass( Main.IconClass );
 CloseLibs();
}
//<

//>"int main()"
int main(){
 Quot[0]=(char)34; Quot[1]=(char)0;
 if( InitAll() ){
  MessageLoop();
  FreeAll();
  return( 0 );
 }else{
  Info( GetCatalogStr(Main.Catalog,ERR_GENERAL,ERR_GENERAL_STR) );
  FreeAll();
  return( 20 );
 }
}
//<

//>"void wbmain( struct WBStartup *wbmsg )"
void wbmain( struct WBStartup *wbmsg ){
 DosStart=FALSE;
 Oldlock = wbmsg->sm_ArgList->wa_Lock;
 main();
}
//<


