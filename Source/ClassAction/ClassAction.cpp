#include "ca_begin.h"
#include <classes/window.h>
#include <proto/alib.h>
#include <exec/libraries.h>
#include <gadgets/fuelgauge.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <libraries/locale.h>
#include <proto/amigaguide.h>
#include <proto/asl.h>
#include <proto/commodities.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/listbrowser.h>
#include <proto/locale.h>
#include <proto/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <workbench/icon.h>
#include <workbench/startup.h>

#include "caglobal.h"
#include "calha.h"
#include "cavers.h"
#include "cagui.h"
#include "caFileTypes.h"
#include "capopup.h"
#include "caIcons.h"
#include "caexec.h"
#include "cautils.h"
#include "careq.h"
#include "cadisk.h"
#include "cadebug.h"
#include "newmouse.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classaction.h"
#include "classactioncd.h"

Library    *DosBase;
Library    *DiskfontBase;
Library    *IFFParseBase;
Library    *IconBase;
Library    *GadToolsBase;
Library    *ResourceBase;
Library    *WorkbenchBase;
Library    *ButtonBase;
Library    *SpaceBase;
Library    *LabelBase;
Library    *BitMapBase;
Library    *StringBase;
Library    *CheckBoxBase;
Library    *WindowBase;
Library    *LayoutBase;
Library    *ListBrowserBase;
Library    *AslBase;
Library    *AmigaguideBase;
Library    *DataBase;
Library    *RexxSysBase;
Library    *KeymapBase;
Library    *CxBase;
#include <libraries/pm.h>
struct PopupMenuBase *PopupMenuBase;

TMain *Main;
char Soft[]=VERSTAG;

BOOL DosStart=TRUE;
BPTR Olddir;

char Quote=34;
char QuoteStr[]={34,0};
struct TDragDrop{
  BOOL Pressed;
  int  SLeft,STop,ELeft,ETop; // Areas where the mouse was on start/end of the dragging
  int  DragLister,DropLister;
}DragDrop;

#define CXCMD_SHOW 1
#define CXCMD_HIDE 2

short Pens_3D_Struct[]={ 6,0,1,2,1,0,-1 };

TagItem ReqTags[]={
 {ASLFR_Screen, NULL },
 {ASLFR_RejectIcons, TRUE },
};

//>"void ShowHelp( char *link=LINK main )"
/* Open AmigaGuide lazily. NewAmigaGuide must be fully zeroed (nag_Client
 * MUST be NULL); opening Async at InitAll with a garbage struct crashes the
 * amigaguide.library process and then CloseAmigaGuide hangs on exit. */
static char sGuideName[128];

static BOOL CA_OpenHelp( void )
{
  BPTR lock;

  if( Main->HelpOpen && Main->GuideHandle ) return TRUE;

  memset( &Main->NewAG, 0, sizeof(Main->NewAG) );

  strcpy( sGuideName, CA_GUIDEFILE );
  lock = Lock( sGuideName, ACCESS_READ );
  if( !lock )
  {
    strcpy( sGuideName, "HELP:ClassAction.guide" );
    lock = Lock( sGuideName, ACCESS_READ );
  }
  if( !lock )
  {
    CA_D(("CA_OpenHelp: no guide file\n"));
    Main->HelpOpen = FALSE;
    Main->GuideHandle = NULL;
    return FALSE;
  }
  UnLock( lock );

  Main->NewAG.nag_Name = sGuideName;
  Main->NewAG.nag_Screen = Main->Scr;

  CA_D(("CA_OpenHelp: OpenAmigaGuideAsyncA '%s'\n", sGuideName));
  Main->GuideHandle = OpenAmigaGuideAsyncA( &Main->NewAG, NULL );
  Main->HelpOpen = (Main->GuideHandle != NULL);
  CA_D(("CA_OpenHelp: handle=%ld\n", (long)Main->GuideHandle));
  return Main->HelpOpen;
}

void ShowHelp( char *link="LINK main" ){
 if( !CA_OpenHelp() ) return;
 SendAmigaGuideCmd( Main->GuideHandle, link, NULL );
}
//<

//>"void SetHString( int n, char *text )"
void SetHString( int n, char *text ){
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   GA_Text,(ULONG)GetCatalogStr(Main->Catalog,n,text),TAG_DONE );
}
//<

//>"void MakeParent( char *path )"
void MakeParent( char *path ){
 char *position;
 if( (position=strrchr(path,'/'))!=NULL ) *position=0;
//      no '/' found:
 else if( (position=strrchr(path,':'))!=&path[strlen(path)-1] ) *(++position)=0;
//      ':' found on last position: return;
}
//<

//>"void MakeFile( char *path )"
void MakeFile( char *path ){
 char *position;
 if( (position=strrchr(path,'/'))!=NULL ){ strncpy( path,(++position),256 ); }
//      no '/' found:
 else if( (position=strrchr(path,':'))!=NULL ){ strcpy( path,(++position) ); }
//      no '/', no ':' then return;
}
//<

//>"void PathHelp( UBYTE which )"
void PathHelp( UBYTE which ){
 char *path;
 path=GetPath( which );
 if( !path[0] ) path=GetCatalogStr( Main->Catalog,MSG_VOLUMES,MSG_VOLUMES_STR );
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   GA_Text,path,TAG_DONE );
}
//<

//>"void TabHelp( UBYTE which,UBYTE num )"
void TabHelp( UBYTE which,UBYTE num ){
  TTabNode *tn=(TTabNode*)Main->Tabs[which].lh_Head;
  for( int i=0;i<num;i++ ) tn=(TTabNode*)tn->ln_Succ;
  char *ptr = tn->Path;
  if( !ptr[0] ) ptr = GetCatalogStr( Main->Catalog,MSG_VOLUMES,MSG_VOLUMES_STR );
  SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   GA_Text,ptr,TAG_DONE );
}
//<

//>"void ListHelp( UBYTE which )"
void ListHelp( UBYTE which ){
 ULONG files,selfiles;
 static char help[256];
 Object *list=GetListBrowser(which);
 GetAttr( LISTBROWSER_TotalNodes,list,&files );
 GetAttr( LISTBROWSER_NumSelected,list,&selfiles );
 if( which == Main->CurrentList )
  strcpy( help,GetCatalogStr(Main->Catalog,TXT_BUTTDIR,TXT_BUTTDIR_STR) );
 else strcpy( help,GetCatalogStr(Main->Catalog,TXT_BUTTDIR2,TXT_BUTTDIR2_STR) );
 strcat( help,": " );
 sprintf( &help[strlen(help)],GetCatalogStr(Main->Catalog,MSG_DDIRSDFILES,MSG_DDIRSDFILES_STR),
   Main->SelectedDirs[which],Main->AvailableDirs[which],Main->SelectedFiles[which],Main->AvailableFiles[which] );
 if( *(GetPath(which)) ){
   sprintf(
     &help[strlen(help)]," (%.0f/%.0f kb)",
     Main->SelectedFileSize[which]/1000,CurrentTab[which]->CurrentPath->Size/1000
   );
 }
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   GA_Text,help,TAG_DONE );
}
//<

/* Hook entry: raw __asm/__saveds/register — ca_begin empties NDK macros. */
ULONG __asm __saveds IDCMPHandler( register __a0 Hook *h ,
                             register __a2 void *o ,
                             register __a1 void *msg ){
 memcpy( &((TIDCMPEvent*)h->h_Data)->Msg, msg,sizeof(IntuiMessage) );
 return(1);
}
//<

#include <clib/keymap_protos.h>
//>"void HandleRawKey( IntuiMessage *msg )"
void HandleRawKey( IntuiMessage *msg ){
// if( !(msg->Code & IECODE_UP_PREFIX) ) return;
  if( msg->Code & IECODE_UP_PREFIX ){
//    msg->Code -= IECODE_UP_PREFIX;
//    if( (msg->Code<76) || (msg->Code>79) )
        return;
  }
        /*
   msg->Code = msg->Code - IECODE_UP_PREFIX;
   if( (msg->Code!=NM_WHEEL_UP)&&(msg->Code!=NM_WHEEL_DOWN)&&(msg->Code!=NM_WHEEL_LEFT)&&(msg->Code!=NM_WHEEL_RIGHT) )
     return;
 }*/
 InputEvent ie;
 IntuiMessage im;
 char Chars[10],p;
 BOOL Alone,Shift,Alt,Ctrl;
 
 ie.ie_Class           = IECLASS_RAWKEY;
 ie.ie_SubClass        = 0;
 ie.ie_Code            = msg->Code;
 ie.ie_Qualifier       = msg->Qualifier;
 ie.ie_EventAddress    = (APTR) msg->IAddress;

 // first try VANILLA (ASCII):
 if( MapRawKey( &ie,Chars,9,NULL ) > 0 ){
  p = Chars[0];
  if( p >= ' ' ){
   // we modify the event to IDCMP_VANILLAKEY:
   im.Code      = p;
   im.Qualifier = msg->Qualifier;
   im.Class     = IDCMP_VANILLAKEY;
   if( HandlePopupMessage( &im ) ) return;
   else if( caIcons->DoIconCommand( Main->IntuiWin,p + WMHI_VANILLAKEY ) ) return;
   else if( ((p>='a')&&(p<='z'))||((p>='A')&&(p<='Z')) ){
    Key( p );
    return;
   }
  }
 }
 // now RAWKEY:
 p = msg->Code;
 if( ListBrowserKeyMove( msg ) ) return;
 if( ! caIcons->DoIconCommand( Main->IntuiWin,p + WMHI_RAWKEY ) ){
  Shift = ( msg->Qualifier & (IEQUALIFIER_LSHIFT + IEQUALIFIER_RSHIFT) );
  Alt   = ( msg->Qualifier & (IEQUALIFIER_LALT + IEQUALIFIER_RALT) );
  Ctrl  = ( msg->Qualifier & IEQUALIFIER_CONTROL );
  Alone = ! (Shift || Alt || Ctrl);
  switch( p ){
   case 64: if( Alone ) ActivatePathGadget( Main->CurrentList ); break;      // SPACE
   case 66: if( Alone ) ActivateBrowser( 1-Main->CurrentList ); break;       // TAB
   case 47:                                                     // 6 on NUMBLOCK
   case 46:                                                     // 5 on NUMBLOCK
   case 68: if( Alone ) DoubleClickDirList( Main->CurrentList ); break;      // RETURN
   case 45:                                                     // 4 on NUMBLOCK
   case 65:{
     if( Alt ) ListDir( Main->CurrentList,"",0 );                // ALT BACKSPACE
     else Shift?Root():Parent( Main->CurrentList ); break;       // BACKSPACE
     break;
   }
   case 69: if( Alone ) PopupOnCurrentFiles(); break;                       // ENTER
   case 70: if( Alone ) Runner(caActions->Item("DELETE"));  break;     // DEL
   case NM_WHEEL_UP: case NM_WHEEL_DOWN: case NM_WHEEL_LEFT: case NM_WHEEL_RIGHT: // mouse wheel
   case 76: case 77: case 78:
   case 79: KeyMove( p,msg->Qualifier );                break;  // cursor keys
   case 80:                                                     // F1
   case 95: if( Alone ) ShowHelp(); break;                                  // HELP
   case 81: if( Alone ) Refresh(Main->CurrentList);                  break;  // F2
   case 82: if( Alone ) Root();                                     break;  // F3
   case 83: if( Alone ) ListDir( Main->CurrentList,"",0 );           break;  // F4
   case 84:{
     if( Alt ) Runner(caActions->Item("DUPLICATE"));                       // ALT F5
     else      Runner(caActions->Item(Shift?"COPYAS":"COPY"));             // (SHIFT) F5
     break;       
   }
   case 85: if( Alone ) Runner(caActions->Item(Shift?"MOVEAS":"MOVE"));   break;       // F6
   case 86: if( Alone ) Runner(caActions->Item("MAKEDIR"));    break;  // F7
   case 87: if( Alone ) Runner(caActions->Item(Shift?"TRASH":"DELETE")); break;        // F8
   case 88: if( Alone ) Runner(caActions->Item("RENAME"));     break;  // F9
   case 89:                                             break;  // F10 // what could we add ??
  }
 }
}
//<

//>"void Drag{int X, int Y )"
/* Puts X && Y in global variables and set Pressed, || set Unpressed */
void Drag( int X, int Y ){
  DragDrop.SLeft = X;
  DragDrop.STop  = Y;
  if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)GetListBrowser(0) ) ){
    DragDrop.DragLister = 0;
    DragDrop.Pressed    = TRUE;
  }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
    (Gadget*)GetListBrowser(1)) ) {
    DragDrop.DragLister = 1;
    DragDrop.Pressed    = TRUE;
  }
}
//<

//>"BOOL Drop{int X, int Y}"
/* React on a Drag'nDrop event */
BOOL Drop( int X, int Y ){
  if( DragDrop.Pressed ){
    // only react if we have got the button down event
    DragDrop.ELeft   = X;
    DragDrop.ETop    = Y;
    DragDrop.Pressed = FALSE;
    if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
      (Gadget*)GetListBrowser(0) ) )
      DragDrop.DropLister = 0;
    else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
      (Gadget*)GetListBrowser(1) ) )
      DragDrop.DropLister = 1;
    else return FALSE;
    TAction *a;
    if( DragDrop.DragLister!=DragDrop.DropLister )
      a=caActions->Add( "COPY","COPY",RUN_NOCLI,"30000",0,FALSE,FALSE,TRUE );
    else a=caActions->Add( "DUPLICATE","DUPLICATE",RUN_NOCLI,"30000",0,FALSE,FALSE,TRUE );
    Runner( a );
    caActions->DeleteAction( a,TRUE );
    return TRUE;
  }
  return FALSE;
}
//<

//>"void MessageLoop()"
void MessageLoop(){
 ULONG windowsignals,arxsignals,notifysignals, mask, result, code, GadgetId,data;
 int CxId;
 EasyStruct Requester={ sizeof(EasyStruct),0,NULL,NULL,NULL };

#ifdef DEBUG_VERSION
cout << "Entering message loop" << endl;
#endif
 Main->Leave = FALSE;
 GetAttr( WINDOW_SigMask, Main->Win, &windowsignals );
 while( !Main->Leave ){
  GetAttr( WINDOW_SigMask, Main->Win, &windowsignals );
  arxsignals=Main->ARexx.Signals();
  notifysignals = 1L << Main->NotifyPort->mp_SigBit;
  mask = Wait( windowsignals | arxsignals | notifysignals | Main->CX.PortSignal | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Main->Leave=TRUE;
  if( mask & arxsignals ) Main->ARexx.HandleMsg();
  if( mask & notifysignals ) HandleNotify();
  if( mask & Main->CX.PortSignal ){
    while( CxId = Main->CX.GetCXId() ){
      switch( CxId ){
        case CXCMD_KILL:        Main->Leave=TRUE; break;
        case CXCMD_SHOW:
        case CXCMD_APPEAR:{
          if( ! Main->IntuiWin ) UnIconifyWindow();
          else{
            WindowToFront(  Main->IntuiWin );
            ActivateWindow( Main->IntuiWin );
          }
          break;
        }
        case CXCMD_HIDE:
        case CXCMD_DISAPPEAR:   if( Main->IntuiWin ) IconifyWindow(); break;
      }
    }
  }
  if( (!Main->Leave)&&(mask & windowsignals) ){
   HandleAppMessage();
   while( (result = DoMethod( Main->Win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
//CLOSEWINDOW------------------------------------------
     case WMHI_CLOSEWINDOW:{
      if( Main->MuiQuit ) IconifyWindow();
      else Main->Leave=TRUE;
      break;
     }
//GADGETHELP-------------------------------------------
     case WMHI_GADGETHELP:{
       GadgetId=result & RL_GADGETMASK;
       if( GadgetId>149 ) caIcons->ShowIconHelp( Main->IntuiWin,GadgetId );
       else if( GadgetId>124 ) TabHelp( 1,GadgetId-125 );
       else if( GadgetId>99 )  TabHelp( 0,GadgetId-100 );
       else switch( GadgetId ){
         case LST_DIR_L:          ListHelp(0);   break;
         case LST_DIR_R:          ListHelp(1);   break;
         case STR_PATH_L:         PathHelp(0);   break;
         case STR_PATH_R:         PathHelp(1);   break;
         case BUTTON_ABORT:       SetHString(TXT_BUTTABORT,TXT_BUTTABORT_STR);    break;
         case BUTTON_ACTIVE_L:    if( Main->CurrentList == 0 ) SetHString(TXT_BUTTDEACTIVATE,TXT_BUTTDEACTIVATE_STR);
                                  else SetHString(TXT_BUTTACTIVATE,TXT_BUTTACTIVATE_STR); break;
         case BUTTON_ACTIVE_R:    if( Main->CurrentList == 1 ) SetHString(TXT_BUTTDEACTIVATE,TXT_BUTTDEACTIVATE_STR);
                                  else SetHString(TXT_BUTTACTIVATE,TXT_BUTTACTIVATE_STR); break;
         case BUTTON_REFRESH_L:
         case BUTTON_REFRESH_R:   SetHString(TXT_BUTTREFRESH,TXT_BUTTREFRESH_STR); break;
         case BUTTON_PARENT_L:
         case BUTTON_PARENT_R:    SetHString(TXT_BUTTPARENT,TXT_BUTTPARENT_STR);  break;
         case FUELGAUGE:          SetHString(MSG_BUTTFUELGAUGE,MSG_BUTTFUELGAUGE_STR);  break;
         default:{
           SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,GA_Text," ",TAG_DONE );
           break;
         }
       }
       break;
     }
//GADGETUP---------------------------------------------
     case WMHI_GADGETUP:{
      GadgetId=result & RL_GADGETMASK;
      if( GadgetId>149 ) caIcons->DoIconCommand( Main->IntuiWin,result );
      else if( GadgetId>124 ) ShowOtherTab( 1,GadgetId-125 );
      else if( GadgetId>99 )  ShowOtherTab( 0,GadgetId-100 );
      else switch( GadgetId ){
       case LST_DIR_L:{
         if( Drop( Main->IntuiWin->LeftEdge,Main->IntuiWin->TopEdge ) ) break;
         ActivateBrowser( 0 );
         GetAttr( LISTBROWSER_RelEvent,Main->Gadgets[LST_DIR_L], &code );
         if( code==LBRE_DOUBLECLICK ) DoubleClickDirList( 0 );
         else if( (code==LBRE_NORMAL)||(code==LBRE_COLUMNADJUST) ) ChoixDirList( 0 );
         else if( code==LBRE_TITLECLICK ) TitleClickDirList( 0 );
         break;
       }
       case LST_DIR_R:{
        if( Drop( Main->IntuiWin->LeftEdge,Main->IntuiWin->TopEdge ) ) break;
        ActivateBrowser( 1 );
        GetAttr( LISTBROWSER_RelEvent,Main->Gadgets[LST_DIR_R], &code );
        if( code==LBRE_DOUBLECLICK ) DoubleClickDirList( 1 );
        else if( (code==LBRE_NORMAL)||(code==LBRE_COLUMNADJUST) ) ChoixDirList( 1 );
        else if( code==LBRE_TITLECLICK ) TitleClickDirList( 1 );
        break;
       }
       case BUTTON_ACTIVE_L:     ActivateBrowser(0);                        break;
       case BUTTON_ACTIVE_R:     ActivateBrowser(1);                        break;
       case BUTTON_REFRESH_L:    ActivateBrowser(0);Refresh( 0 );           break;
       case BUTTON_REFRESH_R:    ActivateBrowser(1);Refresh( 1 );           break;
       case BUTTON_PARENT_L:     ActivateBrowser(0);Parent( 0 );            break;
       case BUTTON_PARENT_R:     ActivateBrowser(1);Parent( 1 );            break;
       case STR_PATH_L:{
        ActivateBrowser(0);
        GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_L],&data );
//        strcpy( CurrentTab[0]->Path,(char*)data );
        ListDir( 0,(char*)data,0 );
        break;
       }
       case STR_PATH_R:{
        ActivateBrowser(1);
        GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_R],&data );
//        strcpy( CurrentTab[1]->Path,(char*)data );
        ListDir( 1,(char*)data,0 );
        break;
       }
      }
      break;
     }
//-(UN-)ICONIFY----------------------------------------
     case WMHI_ICONIFY: IconifyWindow(); break;
/* wird jetzt direkt am Port abgefragt (HandleAppMessage())
     case WMHI_UNICONIFY:{
      DoMethod( Main->Win, WM_OPEN );
      GetAttr( WINDOW_Window, Main->Win,(ULONG *) &Main->IntuiWin );
      Main->IsIcon=FALSE;
      break;
     }
*/
    }
   } // while HANDLEINPUT
   switch( Main->IDCMPEvent.Msg.Class ){
    case IDCMP_RAWKEY:      HandleRawKey( &Main->IDCMPEvent.Msg ); break;
    case IDCMP_DISKINSERTED:
    case IDCMP_DISKREMOVED:{
#ifdef BOBERG_TEST
 CA_D(("diskremove erkannt\n"));
#endif
      if( Main->PathNotify ) HandleDiskNotify( Main->IDCMPEvent.Msg.Class );
      break;
    }
    case IDCMP_MOUSEBUTTONS:{
     if( Main->IDCMPEvent.Msg.Code == SELECTDOWN ){
       if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[FUELGAUGE] ) ){
         RearrangeBrowserSize();
       }else Drag( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY );
     }else if( Main->IDCMPEvent.Msg.Code == MENUDOWN ){
      if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)GetListBrowser(0) ) ){
       ActivateBrowser(0);PopupOnCurrentFiles();
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)GetListBrowser(1) ) ){
       ActivateBrowser(1);PopupOnCurrentFiles();
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[STR_PATH_L] ) ){
       ActivateBrowser(0);OpenPath( 0 );
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[STR_PATH_R] ) ){
       ActivateBrowser(1);OpenPath( 1 );
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[BUTTON_PARENT_L] ) ){
       ActivateBrowser(0);OpenOld( 0 );
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[BUTTON_PARENT_R] ) ){
       ActivateBrowser(1);OpenOld( 1 );
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[BUTTON_REFRESH_L] ) ){
       ActivateBrowser(0);OpenVolumeMenu(0);
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[BUTTON_REFRESH_R] ) ){
       ActivateBrowser(1);OpenVolumeMenu(1);
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[BUTTON_ACTIVE_L] ) ){
       ActivateBrowser(0);OpenDirMenu();
      }else if( CoordsInGadget( Main->IDCMPEvent.Msg.MouseX,Main->IDCMPEvent.Msg.MouseY,
                          (Gadget*)Main->Gadgets[BUTTON_ACTIVE_R] ) ){
       ActivateBrowser(1);OpenDirMenu();
      }else OpenMenu(); // standard menu
     }
    }
   }
   Main->IDCMPEvent.Msg.Class=0;
 }}
}
//<

//>"BOOL OpenLibs()"
BOOL OpenLibs(){
 CA_D(("OpenLibs: start\n"));
 if( !(DosBase=OpenLibrary( "dos.library", 39L ))) { CA_D(("OpenLibs: fail dos.library\n")); return(FALSE); }
 if( !(DiskfontBase=OpenLibrary( "diskfont.library", 39L ))) { CA_D(("OpenLibs: fail diskfont.library\n")); return(FALSE); }
 if( !(IFFParseBase=OpenLibrary( "iffparse.library", 39L ))) { CA_D(("OpenLibs: fail iffparse.library\n")); return(FALSE); }
 if( !(IconBase=OpenLibrary( "icon.library", 39L ))) { CA_D(("OpenLibs: fail icon.library\n")); return(FALSE); }
 if( !(GadToolsBase=OpenLibrary( "gadtools.library", 39L ))) { CA_D(("OpenLibs: fail gadtools.library\n")); return(FALSE); }
 if( !(ResourceBase=OpenLibrary( "resource.library", 39L ))) { CA_D(("OpenLibs: fail resource.library\n")); return(FALSE); }
 if( !(WorkbenchBase=OpenLibrary( "workbench.library", 39L ))) { CA_D(("OpenLibs: fail workbench.library\n")); return(FALSE); }
 if( !(AslBase=OpenLibrary( "asl.library", 39L ))) { CA_D(("OpenLibs: fail asl.library\n")); return(FALSE); }
 if( !(AmigaguideBase=OpenLibrary( "amigaguide.library", 39L ))) { CA_D(("OpenLibs: fail amigaguide.library\n")); return(FALSE); }
 if( !(LayoutBase = OpenLibrary("gadgets/layout.gadget", 44))) { CA_D(("OpenLibs: fail layout.gadget\n")); return(FALSE); }
 if( !(ListBrowserBase = OpenLibrary("gadgets/listbrowser.gadget", 44))) { CA_D(("OpenLibs: fail listbrowser.gadget\n")); return(FALSE); }
 if( !(WindowBase = OpenLibrary("window.class", 44))) { CA_D(("OpenLibs: fail window.class\n")); return(FALSE); }
 if( !(ButtonBase = OpenLibrary("gadgets/button.gadget", 44))) { CA_D(("OpenLibs: fail button.gadget\n")); return(FALSE); }
 if( !(SpaceBase = OpenLibrary("gadgets/space.gadget", 44))) { CA_D(("OpenLibs: fail space.gadget\n")); return(FALSE); }
 if( !(StringBase = OpenLibrary("gadgets/string.gadget", 44))) { CA_D(("OpenLibs: fail string.gadget\n")); return(FALSE); }
 if( !(LabelBase = OpenLibrary("images/label.image", 44))) { CA_D(("OpenLibs: fail label.image\n")); return(FALSE); }
 if( !(BitMapBase = OpenLibrary("images/bitmap.image", 44))) { CA_D(("OpenLibs: fail bitmap.image\n")); return(FALSE); }
 if( !(CheckBoxBase = OpenLibrary("gadgets/checkbox.gadget", 44))) { CA_D(("OpenLibs: fail checkbox.gadget\n")); return(FALSE); }
 if( !(DataBase=OpenLibrary("datatypes.library",0)) ) { CA_D(("OpenLibs: fail datatypes.library\n")); return(FALSE); }
 if( !(RexxSysBase=OpenLibrary("rexxsyslib.library",0)) ) { CA_D(("OpenLibs: fail rexxsyslib.library\n")); return(FALSE); }
 if( !(KeymapBase=OpenLibrary("keymap.library",0)) ) { CA_D(("OpenLibs: fail keymap.library\n")); return(FALSE); }
 if( !(CxBase=OpenLibrary("commodities.library",0)) ) { CA_D(("OpenLibs: fail commodities.library\n")); return(FALSE); }
 if( !(PopupMenuBase=(struct PopupMenuBase*)OpenLibrary(POPUPMENU_NAME,0)) ) { CA_D(("OpenLibs: fail popupmenu.library\n")); return(FALSE); }
 CA_D(("OpenLibs: ok\n"));
 return( TRUE );
}
//<

//>"void CloseLibs()"
void CloseLibs(){
 CloseLibrary( (Library*)PopupMenuBase );
 CloseLibrary( CxBase );
 CloseLibrary( KeymapBase );
 CloseLibrary( RexxSysBase );
 CloseLibrary( DataBase );
 CloseLibrary( CheckBoxBase );
 CloseLibrary( StringBase );
 CloseLibrary( BitMapBase );
 CloseLibrary( LabelBase );
 CloseLibrary( SpaceBase );
 CloseLibrary( ButtonBase );
 CloseLibrary( WindowBase );
 CloseLibrary( ListBrowserBase );
 CloseLibrary( LayoutBase );
 CloseLibrary( AmigaguideBase );
 CloseLibrary( AslBase );
 CloseLibrary( WorkbenchBase );
 CloseLibrary( ResourceBase );
 CloseLibrary( GadToolsBase );
 CloseLibrary( IconBase );
 CloseLibrary( IFFParseBase );
 CloseLibrary( DiskfontBase );
 CloseLibrary( DosBase );
}
//<

//>"void CreateActionList()"
void CreateActionList(){
  caActions->Add( "ABOUT","ABOUT",RUN_INTERNAL,"","" );
  caActions->Add( "ALL","ALL",RUN_INTERNAL,"","" );
  caActions->Add( "ASSIGN","ASSIGN",RUN_INTERNAL,"","" );
  caActions->Add( "CLIPCOPY","CLIPCOPY",RUN_INTERNAL,"","" );
  caActions->Add( "CLIPPASTE","CLIPPASTE",RUN_INTERNAL,"","" );
  caActions->Add( "COPY","COPY",RUN_INTERNAL,"","" );
  caActions->Add( "COPYAS","COPYAS",RUN_INTERNAL,"","" );
  caActions->Add( "DUPLICATE","DUPLICATE",RUN_INTERNAL,"","" );
  caActions->Add( "DELETE","DELETE",RUN_INTERNAL,"","" );
  caActions->Add( "EDIT","c:ed [f]",RUN_NOCLI,"8000","0",TRUE );
  caActions->Add( "HELP","HELP",RUN_INTERNAL,"","" );
  caActions->Add( "ICONIFY","ICONIFY",RUN_INTERNAL,"","" );
  caActions->Add( "LIST","LIST",RUN_INTERNAL,"","" );
  caActions->Add( "MAKEDIR","MAKEDIR",RUN_INTERNAL,"","" );
  caActions->Add( "MOVE","MOVE",RUN_INTERNAL,"","" );
  caActions->Add( "MOVEAS","MOVEAS",RUN_INTERNAL,"","" );
  caActions->Add( "NONE","NONE",RUN_INTERNAL,"","" );
  caActions->Add( "PATTERN","PATTERN",RUN_INTERNAL,"","" );
  caActions->Add( "PREFS","PREFS",RUN_INTERNAL,"","" );
  caActions->Add( "QUIT","QUIT",RUN_INTERNAL,"","" );
  caActions->Add( "RENAME","RENAME",RUN_INTERNAL,"","" );
  caActions->Add( "ROOT","ROOT",RUN_INTERNAL,"","" );
  caActions->Add( "SEARCH","SEARCH",RUN_INTERNAL,"","" );
  caActions->Add( "SNAPSHOT","SNAPSHOT",RUN_INTERNAL,"","" );
  caActions->Add( "TOGGLE","TOGGLE",RUN_INTERNAL,"","" );
  caActions->Add( "TRASH","TRASH",RUN_INTERNAL,"","" );
  caActions->Add( "TYPE","TYPE",RUN_INTERNAL,"","" );
  caActions->Add( "VIEW","multiview [f]",RUN_NOCLI,"8000","0",TRUE );
  caActions->Add( "WINDOW","WINDOW",RUN_INTERNAL,"","" );
}
//<

//>"void BuildMenu( Class *SelectedClass=NULL )"
/*
void BuildMenu( Class *SelectedClass ){
 Main->Menu.Clear();
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_DIR,TXT_DIR_STR),
                        NULL,NM_TITLE,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_INFORMATION,TXT_INFORMATION_STR),
                        NULL,NM_ITEM,WbInfoDir );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_CREATESUBDIR,TXT_CREATESUBDIR_STR),
                        NULL,NM_ITEM,MkDir );

 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_FILE,TXT_FILE_STR),
                        NULL,NM_TITLE,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_INFORMATION,TXT_INFORMATION_STR),
                        NULL,NM_ITEM,WbInfo );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_RENAME,TXT_RENAME_STR),
                        NULL,NM_ITEM,RenameFile );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_PACK,TXT_PACK_STR),
                        NULL,NM_ITEM,Pack );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_COPY,TXT_COPY_STR),
                        NULL,NM_ITEM,Copy );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_COPYAS,TXT_COPYAS_STR),
                        NULL,NM_ITEM,Clone );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_MOVE,TXT_MOVE_STR),
                        NULL,NM_ITEM,Move );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_MOVEAS,TXT_MOVEAS_STR),
                        NULL,NM_ITEM,MoveAs );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_DELETE,TXT_DELETE_STR),
                        NULL,NM_ITEM,Delete );
 // Actions
 if( SelectedClass != NULL ){
  BOOL AtLeastOneAction = FALSE;
  Main->Menu.AddItem( SelectedClass->name,NULL,NM_TITLE,NULL );
  for( Action *act=SelectedClass->action;act;act=act->next ){
   AtLeastOneAction = TRUE;
   Main->Menu.AddItem( act->name,NULL,NM_ITEM,act );
  }
  if( (SelectedClass != &Main->Generic) && (SelectedClass != &Main->Directory) ){
   if( AtLeastOneAction ) Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
   for( Action *act=Main->Generic.action;act;act=act->next ){
    Main->Menu.AddItem( act->name,NULL,NM_ITEM,act );
   }
  }
 }
 // Edit
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_EDIT,TXT_EDIT_STR),
                        NULL,NM_TITLE,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_COPY,TXT_COPY_STR),
                        "C",NM_ITEM,Main->ActionList.Item("CUT") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_PASTE,TXT_PASTE_STR),
                        "V",NM_ITEM,Main->ActionList.Item("PASTE") );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SELALL,TXT_SELALL_STR),
                        "+",NM_ITEM,Main->ActionList.Item("ALL") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SELNONE,TXT_SELNONE_STR),
                        "-",NM_ITEM,Main->ActionList.Item("NONE") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SELPATTERN,TXT_SELPATTERN_STR),
                        "?",NM_ITEM,Main->ActionList.Item("PATTERN") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SELTOGGLE,TXT_SELTOGGLE_STR),
                        "%",NM_ITEM,Main->ActionList.Item("TOGGLE") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SELTYPE,TXT_SELTYPE_STR),
                        "=",NM_ITEM,Main->ActionList.Item("TYPE") );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SEARCH,TXT_SEARCH_STR),
                        "F",NM_ITEM,Main->ActionList.Item("SEARCH") );
 // Settings
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SETTINGS,TXT_SETTINGS_STR),
                        NULL,NM_TITLE,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SNAPSHOTWINDOW,TXT_SNAPSHOTWINDOW_STR),
                        "S",NM_ITEM,Main->ActionList.Item("SNAPSHOT") );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_GENERAL,TXT_GENERAL_STR),
                        NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_CONFIRMREPLACE,TXT_CONFIRMREPLACE_STR),
                        NULL,NM_SUB,&Main->ConfirmReplace );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_CHECKFIT,TXT_CHECKFIT_STR),
                        NULL,NM_SUB,&Main->CheckFit );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SHOWHELP,TXT_SHOWHELP_STR),
                        NULL,NM_SUB,&Main->ShowHelp );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_LISTERS,TXT_LISTERS_STR),
                        NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SHOWIMAGES,TXT_SHOWIMAGES_STR),
                        NULL,NM_SUB,&Main->ShowImages );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_HIGHDIRS,TXT_HIGHDIRS_STR),
                        NULL,NM_SUB,&Main->HighlightDirs );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_SHOWDRAWERSFIRST,TXT_SHOWDRAWERSFIRST_STR),
                        NULL,NM_SUB,&Main->ShowDrawersFirst );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_DATESTRING,TXT_DATESTRING_STR),
                        NULL,NM_SUB,&Main->DateString );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_DISPLAYTIME,TXT_DISPLAYTIME_STR),
                        NULL,NM_SUB,&Main->DisplayTime );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_PATHNOTIFY,TXT_PATHNOTIFY_STR),
                        NULL,NM_SUB,&Main->PathNotify );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_ICONS,TXT_ICONS_STR),
                        NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_DISPLAYINFO,TXT_DISPLAYINFO_STR),
                        NULL,NM_SUB,&Main->ShowIcons );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_CREATEICONS,TXT_CREATEICONS_STR),
                        NULL,NM_SUB,&Main->CreateIcons );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_COPYICONS,TXT_COPYICONS_STR),
                        NULL,NM_SUB,&Main->CopyIcons );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_DELETEICONS,TXT_DELETEICONS_STR),
                        NULL,NM_SUB,&Main->DeleteIcons );
 // Project
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_PROJECT,TXT_PROJECT_STR),
                        NULL,NM_TITLE,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_HELP,TXT_HELP_STR),
                        "H",NM_ITEM,Main->ActionList.Item("HELP") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_PREFERENCES,TXT_PREFERENCES_STR),
                        "P",NM_ITEM,Main->ActionList.Item("PREFS") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_ABOUT,TXT_ABOUT_STR),
                        "A",NM_ITEM,Main->ActionList.Item("ABOUT") );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_ICONIFY,TXT_ICONIFY_STR),
                        "I",NM_ITEM,Main->ActionList.Item("ICONIFY") );
 Main->Menu.AddItem( NM_BARLABEL,NULL,NM_ITEM,NULL );
 Main->Menu.AddItem( GetCatalogStr(Main->Catalog,TXT_QUIT,TXT_QUIT_STR),
                        "Q",NM_ITEM,Main->ActionList.Item("QUIT") );

 Main->IMenu=Main->Menu.Render( Main->Visualinfo );
}
*/
//<

//>"void ARX_Quit( char* arg )"
void ARX_Quit( char* arg ){
 Main->Leave=TRUE;
}
//<

//>"void ARX_Status( char* arg )"
void ARX_Status( char* arg ){
 if(Main->IsIcon) Main->ARexx.SetResult( "0" ); else Main->ARexx.SetResult( "1" );
}
//<

//>"void ARX_AppIconify( char* arg )"
void ARX_AppIconify( char* arg ){
 if( !Main->IsIcon ){
  DoMethod( Main->Win, WM_ICONIFY );
  Main->IntuiWin=NULL;
  Main->IsIcon=TRUE;
 }
}
//<

//>"void ARX_Use( char* arg )"
void ARX_Use( char* arg ){
 Main->Leave = Main->Restart = TRUE;
}
//<

//>"void ARX_Load( char* arg )"
void ARX_Load( char* arg ){
 long lock;
 TFileType *ft;
 char *full=arg+5;
 if(full[0]=='"') {full++;full[strlen(full)-1]=0;}
 if( lock=Lock( full,ACCESS_READ ) ){
  UnLock( lock );
  ft = caFileTypes->GetFileType( full,(char *)FilePart(full),FALSE );
  OpenActionMenu( ft,full );
 }else{
  Main->ARexx.SetResult( GetCatalogStr(Main->Catalog,ERR_FILENOTFOUND,ERR_FILENOTFOUND_STR) );
 }
}
//<

//>"void ARX_Show( char* arg )"
void ARX_Show( char* arg ){
 if( Main->IsIcon ){
  DoMethod( Main->Win, WM_OPEN );
  GetAttr( WINDOW_Window, Main->Win,(ULONG *) &Main->IntuiWin );
  Main->IsIcon=FALSE;
 }
}
//<

//>"void ARX_Ver( char* arg )"
void ARX_Ver( char* arg ){
 Main->ARexx.SetResult( VERS );
}
//<

//>"void ARX_Getclass( char* arg )"
void ARX_Getclass( char* arg ){
 char *full=arg+9;
 if( full[0]=='"' ) {full++;full[strlen(full)-1]=0;}
 long lock=Lock( full,ACCESS_READ );
 if( !lock ) Main->ARexx.SetError( GetCatalogStr(Main->Catalog,ERR_FILENOTFOUND,ERR_FILENOTFOUND_STR) );
 else{
  UnLock( lock );
  TFileType *ft = caFileTypes->GetFileType( full,(char *)FilePart(full),FALSE );
  Main->ARexx.SetResult( ft->Name );
 }
}
//<

//>"void ARX_Action( char* arg )"
void ARX_Action( char* arg ){
 char toto[256],*file,*pattern,*ptr;
 TAction *run;
 strcpy(toto,arg+7);
 file=toto;
 if(file[0]=='"'){
  file++;
  ptr=(char *)strchr(file,'"');  // looking for the second "
  if(ptr==NULL){
   Main->ARexx.SetError( GetCatalogStr(Main->Catalog,ERR_FILENOTFOUND,ERR_FILENOTFOUND_STR) );
   return;
  }else{
   *ptr=0;
   if( *(++ptr)==' ' ) pattern=ptr+1;
  }
 }else{
  ptr=(char *)strpbrk(file," ");
  if(ptr==NULL){
   Main->ARexx.SetError( GetCatalogStr(Main->Catalog,ERR_FILENOTFOUND,ERR_FILENOTFOUND_STR) );
   return;
  }else{ *ptr=0; pattern=ptr+1; }
 }
 long lock=Lock( file,ACCESS_READ );
 if( !lock ){
  Main->ARexx.SetError( GetCatalogStr(Main->Catalog,ERR_FILENOTFOUND,ERR_FILENOTFOUND_STR) );
  return;
 }else{
  UnLock( lock );
  TFileType *ft = caFileTypes->GetFileType( file,(char *)FilePart(file),FALSE );
  run=NULL;
  TAction *act = (TAction*)ft->Actions.lh_Head;
  while( act->ln_Succ ){
   if( (run==NULL)&&(strstr(act->Name,pattern)!=NULL) ) run=act;
   act = (TAction*)act->ln_Succ;
  }
  if(run!=NULL) Runner(run,file);
 }
}
//<

//>"void MC_Quit()"
void MC_Quit(){
 Main->Leave=TRUE;
}
//<

//>"void MC_Help()"
void MC_Help(){
 ShowHelp( "LINK using" );
}
//<

//>"void MC_Copy()"
void MC_Copy(){
 char *Result,Path[512];
 char *path;
 Node *node;
 List *lblist;
 ULONG selected;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 TTabNode *tnd=CurrentTab[1-listn];
 Object *lb=GetCurrentListBrowser();
 FileNode *fn;

 // quit if in archive
 if( tn->LhaActive ) return;

 Result=new char[5000];
 Result[0]=0;

 path=GetSrc();
 strcpy(Path,path);
 if( (Path[0]!=0)&&(Path[strlen(Path)-1]!=':')&&(Path[strlen(Path)-1]!='/') ){
  strcat(Path,"/");
 }
 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );

 node=lblist->lh_Head;
 while( node->ln_Succ!=NULL ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ){
   fn=(FileNode*)((TLBNode*)node)->Fn;
   strcat( Result,QuoteStr );
   strcat( Result,Path );
   strcat( Result,fn->Filename );
   strcat( Result,QuoteStr );
   strcat( Result," " );
   SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
  }
  node=node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,LISTBROWSER_Labels, lblist, TAG_DONE );
 ChoixDirList( listn );
 if( Result[0] ) Result[strlen(Result)-1]=0; // cut the last blank
 ClipBoard->Copy( Result );
 delete [] Result;
}
//<

//>"BOOL GetNextFile( char *Files,char *NextFile,BOOL start=FALSE )"
char *Store;
BOOL GetNextFile( char *Files,char *NextFile,BOOL start=FALSE ){
 NextFile[0]=0;
 // Start: set the <Store>:
 if( start ) Store=Files;
 // Run through blanks:
 while( *Store==' ' ) Store++;
 if( *Store==0 ) return( FALSE );
 // filename beginning with quotes?
 char tosearch=' ';
 if( *Store==Quote ){
  Store++;
  tosearch=Quote;
  if( *Store==0 ) return( FALSE );
 }
 // look for the end of the name:
 char *ptr=strchr( Store,tosearch );
 if( ptr ){
  int size = (int)(ptr-Store);
  strncpy( NextFile,Store,size );
  NextFile[size]=0;
  Store = ptr + 1;
  return( TRUE );
 }else{
  strcpy( NextFile,Store );
  Store += strlen( Store ); // so it == on the ending 0
  return( TRUE );
 }
}
//<

//>"void MC_Paste()"
void MC_Paste(){
 char dest[512],good[1024];
 char *fuelstr,*fuelstring,NextFile[256],*ClipFiles;
 BPTR lock;
 BOOL ok,success=FALSE,Abort=FALSE,found;
 int n=0;
 TArchive Archive;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 strcpy(dest,GetSrc()); // CAUTION !! Here the files are inserted in SOURCE !!!
 if( dest[0]==0 ) return;
 if( (dest[strlen(dest)-1]!=':')&&(dest[strlen(dest)-1]!='/') ) strcat(dest,"/");

 EnableAbort( TRUE );

 fuelstring=new char[512];
 fuelstr=GetCatalogStr(Main->Catalog,TXT_COPYING,TXT_COPYING_STR);

 // now count the files:
 ClipFiles = ClipBoard->Paste();
 while( GetNextFile( ClipFiles,NextFile,(n==0) ? TRUE : FALSE ) ){
  n++;
 }
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Max,n,TAG_END );

 if( n>1 ){
   Main->Transfer.ReplaceText=GetCatalogStr( Main->Catalog,TXT_YESALLNONENO,TXT_YESALLNONENO_STR );
 }else{
   Main->Transfer.ReplaceText=GetCatalogStr( Main->Catalog,TXT_YESNO,TXT_YESNO_STR );
 } 
 Main->Transfer.Count = 0;
 Main->Transfer.RequesterResult = 0;
 Main->Transfer.Buffer = AllocMem( Main->BufferSize,MEMF_CLEAR );
 Main->Transfer.ProgressText = (char*)malloc( 512 );
 char *lhafile=new char[512];

 found = GetNextFile( ClipFiles,NextFile,TRUE );
 while( (! Abort) && found && (! CheckAbort())  ){
  sprintf( fuelstring,"%s %s ...",fuelstr,NextFile );
  SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                  FUELGAUGE_Level,Main->Transfer.Count,
                  GA_Text,fuelstring,TAG_END );
  sprintf( good,"%s%s",dest,FilePart(NextFile) );
  // test, if we will replace a file!
  ok=TRUE;
  if( tn->LhaActive ){
   // as files are only added to the root, we must test archive/filename
   strcpy( lhafile,dest );
   Archive.ArchiveName( lhafile );
   strcat( lhafile,"/" );
   strcat( lhafile,FilePart(NextFile) );
   lock=Archive.Lock( lhafile );
  }else lock=Lock(good,ACCESS_READ);
  if( lock ){
   if( tn->LhaActive ) Archive.UnLock();
   else UnLock( lock );
   if( Main->ConfirmReplace ){
    if( (Main->Transfer.RequesterResult == 0) || (Main->Transfer.RequesterResult == 1) ){    // we must ask the user
      Main->Transfer.RequesterResult=AskReplace( NextFile,good,Main->Transfer.ReplaceText );
    }
    if( (Main->Transfer.RequesterResult == 0) || (Main->Transfer.RequesterResult == 3) ) ok=FALSE;
   }
  }
  if( ok ){
   if( tn->LhaActive || caFileTypes->ContainsArchive(dest,lhafile) ){
     success = CopyArchiveFile( NextFile,good );
   }else{
     n       = MRECopy( NextFile,good,Main->Gadgets[FUELGAUGE] );
     Abort   = n == -1;
     success = n == 0;
   }
   if( success ){
    /* copy ok */
   }else{
    sprintf( fuelstring,GetCatalogStr(Main->Catalog,ERR_NOCOPY,ERR_NOCOPY_STR),NextFile );
    Info( fuelstring );
   }
  }
  Main->Transfer.Count++;
  found = GetNextFile( ClipFiles,NextFile,FALSE );
 }
 delete [] lhafile;
 delete [] fuelstring;
 FreeMem( Main->Transfer.Buffer,Main->BufferSize );
 free( Main->Transfer.ProgressText );
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Level,0,GA_Text," ",TAG_DONE );
 if( success ){
  if( tn->LhaActive ){
   Archive.ArchiveName( dest );
   ListDir( listn,dest,0,0 );
   Refresh( listn );
  }else Refresh( listn );
 }
 EnableAbort( FALSE );
}
//<

//>"BOOL PreInit()"
BOOL PreInit(){
 CA_D(("PreInit: start\n"));
  Main->SrcStartPath=(char*)malloc( 256 );
  Main->SrcStartPath[0]=0;
  Main->DstStartPath=(char*)malloc( 256 );
  Main->DstStartPath[0]=0;
  Main->WindowTitle=(char*)malloc( 256 );
  Main->OwnScreen=(char*)malloc( 100 );
  Main->PublicScreen=(char*)malloc( 256 );
  Main->BackgroundPattern=(char*)malloc( 256 );
  Main->sMenuFont=(char*)malloc( 100 );
  Main->sListerFont=(char*)malloc( 100 );
  Main->PopupKey=(char*)malloc( 100 );
  Main->HideKey=(char*)malloc( 100 );
  NewList( &Main->Tabs[0] );
  NewList( &Main->Tabs[1] );
  NewList( &Main->Paths );
  NewList( &Main->History );
  CA_D(("PreInit: ok\n"));
  return( TRUE );
}
//<

//>"void PostClear()"
void PostClear(){
  ClearHistory();
  free( Main->HideKey );
  free( Main->PopupKey );
  free( Main->sListerFont );
  free( Main->sMenuFont );
  free( Main->BackgroundPattern );
  free( Main->PublicScreen );
  free( Main->OwnScreen );
  free( Main->WindowTitle );
  free( Main->SrcStartPath );
  free( Main->DstStartPath );
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
 TIniFile IniFile;
 char *ptr;
 char *prefsused;

 CA_D(("InitAll: start\n"));

 Main->LhaPossible=TRUE;

 Main->Scr=NULL; Main->NotifyPort=Main->AppPort=Main->MsgPort=NULL; Main->Resource=NULL;
 Main->Win=Main->ReqWin=NULL;
 Main->IntuiWin=NULL;
 Main->Dobj=NULL;
 Main->FileReq=NULL;
 Main->GuideHandle=NULL;
 Main->HelpOpen=FALSE;
 Main->MainPopup=NULL;
 Main->DirMenu=NULL;
 Main->VolumeMenu=NULL;
 Main->MenuFont=NULL;
 Main->ListerFont=NULL;
 NewList( &Main->UserPaths );

 Main->ShowDrawersFirst = TRUE;
 Main->IsIcon      = FALSE;
 Main->OwnScreenOpen = FALSE;

 prefsused = NULL;
 if( IniFile.LoadFromFile(TPREFSFILE) ) prefsused = TPREFSFILE;
 else if( IniFile.LoadFromFile(PREFSFILE) ) prefsused = PREFSFILE;
 else if( IniFile.LoadFromFile(DPREFSFILE) ) prefsused = DPREFSFILE;

 if( prefsused == NULL ){
   CA_D(("InitAll: no prefs (tried T:/PROGDIR Config); using built-in defaults\n"));
   Info( GetCatalogStr(Main->Catalog,TXT_NOCONFIG,TXT_NOCONFIG_STR) );
   Main->PrefsVersion = PREFSVERS;
 }else{
   CA_D(("InitAll: loaded prefs '%s'\n", prefsused));
   Main->PrefsVersion = IniFile.GetInt( "INFO","VERSION",0 );
   if( Main->PrefsVersion!=PREFSVERS ){
     CA_D(("InitAll: prefs version %ld != %ld\n", (long)Main->PrefsVersion, (long)PREFSVERS));
     Info( GetCatalogStr(Main->Catalog,ERR_WRONGPREFS,ERR_WRONGPREFS_STR) );
     return( FALSE );
   }
 }
 LoadOptions( &IniFile );
 caFileTypes->LittleImages = Main->LittleImages;
 LoadPaths( &IniFile );
 LoadClasses( &IniFile );
 CA_D(("InitAll: options/paths/classes loaded\n"));

 // we get the appicon
 if( !(Main->Dobj=GetIconTags( CA_ICONFILE, ICONGETA_FailIfUnavailable, FALSE, TAG_END)) ){
  CA_D(("InitAll: no icon at %s\n", CA_ICONFILE));
  Info( GetCatalogStr(Main->Catalog,ERR_NOICON,ERR_NOICON_STR) );
 }else{
  if( Main->AppX!=-1 ) Main->Dobj->do_CurrentX=Main->AppX;
  if( Main->AppY!=-1 ) Main->Dobj->do_CurrentY=Main->AppY;
 }

 if( Main->OwnScreen[0] ){
   ULONG id,width,height,depth,overscan,scroll,PenDummy=~0;
   sscanf( Main->OwnScreen,"%d %d %d %d %d %d",&id,&width,&height,&depth,&overscan,&scroll );
   Main->Scr=OpenScreenTags( NULL,
     SA_Title,"ClassAction screen",
     SA_PubName,"ClassActionScreen",
     SA_Type,PUBLICSCREEN,
     SA_Width,width,
     SA_Height,height,
     SA_Depth,depth,
     SA_DisplayID,id,
//     SA_Pens,Pens_3D_Struct,
//     SA_FullPalette,TRUE,
     SA_LikeWorkbench,TRUE,
//     SA_Overscan,overscan,
//     SA_AutoScroll,scroll,
     TAG_END );
   Main->OwnScreenOpen = Main->Scr != NULL;
   if( Main->OwnScreenOpen ) PubScreenStatus( Main->Scr,0 );
 }
 if( (!Main->OwnScreenOpen) || (!Main->OwnScreen[0]) ){
   ptr=Main->PublicScreen;
   if( ptr[0]==0 ) ptr=NULL;
   CA_D(("InitAll: LockPubScreen '%s'\n", ptr ? ptr : (char *)"(default)"));
   if( !(Main->Scr=LockPubScreen(ptr)) ){
    if( !(Main->Scr=LockPubScreen(NULL)) ){
      CA_D(("InitAll: LockPubScreen failed\n"));
      return( FALSE );
    }
   }
 }

 Main->ZoomCoords[0]=0;
 Main->ZoomCoords[1]=0;
 Main->ZoomCoords[2]=Main->Scr->Width;
 Main->ZoomCoords[3]=Main->Scr->Height;

 if( !((Main->AppPort=CreateMsgPort())&&
     (Main->NotifyPort=CreateMsgPort())&&
     (Main->MsgPort=CreateMsgPort())) ){
   CA_D(("InitAll: CreateMsgPort failed\n"));
   return( FALSE );
 }
 CA_D(("InitAll: CA_FixupRCTResource\n"));
 CA_FixupRCTResource();
 CA_D(("InitAll: RL_OpenResource\n"));
 if( !(Main->Resource=RL_OpenResource(RCTResource,Main->Scr,Main->Catalog)) ){
   CA_D(("InitAll: RL_OpenResource failed\n"));
   return( FALSE );
 }

// Main->Visualinfo=GetVisualInfoA( Main->Scr,TAG_DONE );

 CreateActionList();

 ULONG wpos=WPOS_CENTERSCREEN;
 if( (Main->WinX!=-1)||(Main->WinY!=-1) ) wpos=0;
 InitHook( &Main->IDCMPHook,(ULONG (*)())IDCMPHandler,&Main->IDCMPEvent );

 ptr = Main->ARexx.Name();
 strcpy( Main->WindowTitle,GetCatalogStr(Main->Catalog,TXT_PROG,TXT_PROG_STR) );
 if( ptr[strlen(ptr)-1] != '1' )
   sprintf( Main->WindowTitle+strlen(Main->WindowTitle)," (%c)",ptr[strlen(ptr)-1] );

 CA_D(("InitAll: RL_NewObject WIN_1\n"));
 if( !(Main->Win=RL_NewObject( Main->Resource,WIN_1_ID,
   WA_Title,Main->WindowTitle,
   WA_PubScreen,Main->Scr,
   WA_Top,Main->WinY,WA_Left,Main->WinX,
   WA_Zoom,Main->ZoomCoords,
   WA_InnerWidth,Main->WinWidth,WA_InnerHeight,Main->WinHeight,
   WA_RMBTrap,TRUE,
   WINDOW_AppPort,Main->AppPort,WINDOW_AppWindow,(ULONG)TRUE,
   WINDOW_SharedPort,Main->MsgPort,
   WINDOW_Position,wpos,
   WA_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEBUTTONS|
            IDCMP_RAWKEY|IDCMP_DISKINSERTED|IDCMP_DISKREMOVED,
   WINDOW_IDCMPHook,&Main->IDCMPHook,
   WINDOW_IDCMPHookBits,IDCMP_DISKREMOVED|IDCMP_DISKINSERTED|IDCMP_MOUSEBUTTONS|IDCMP_GADGETDOWN|IDCMP_RAWKEY,
   WINDOW_Icon,Main->Dobj,
   Main->BackgroundPattern[0] ? WINDOW_BackFillName : TAG_IGNORE,Main->BackgroundPattern,
   TAG_END)) )
 {
   CA_D(("InitAll: WIN_1 failed\n"));
   return( FALSE );
 }
 if( !(Main->ReqWin=RL_NewObject( Main->Resource,WIN_3_ID,
                                 WINDOW_SharedPort,Main->MsgPort,
                                 TAG_END)) ){
   CA_D(("InitAll: WIN_3 failed\n"));
   return( FALSE );
 }


 Main->Gadgets=RL_GetObjectArray( Main->Resource, Main->Win, GROUP_ROOT );
 Main->Group5=RL_GetObjectArray( Main->Resource, Main->ReqWin, GROUP_5_ID );

 if( Main->VerticalLists ){
   SetGadgetAttrs(
     (Gadget*)Main->Gadgets[LAYOUT_MAIN],NULL,NULL,
     LAYOUT_Orientation,LAYOUT_VERTICAL,TAG_END
   );
 }

 // adjust gadgets:
 // parent buttons:
 if( !Main->ShowParent ){
  SetGadgetAttrs( (Gadget*)Main->Gadgets[LAYOUT_PATH_L],NULL,NULL,
                        LAYOUT_RemoveChild,Main->Gadgets[BUTTON_PARENT_L],TAG_END );
  SetGadgetAttrs( (Gadget*)Main->Gadgets[LAYOUT_PATH_R],NULL,NULL,
                        LAYOUT_RemoveChild,Main->Gadgets[BUTTON_PARENT_R],TAG_END );
 }
 // refresh buttons:
 if( !Main->ShowRefresh ){
  SetGadgetAttrs( (Gadget*)Main->Gadgets[LAYOUT_PATH_L],NULL,NULL,
                        LAYOUT_RemoveChild,Main->Gadgets[BUTTON_REFRESH_L],TAG_END );
  SetGadgetAttrs( (Gadget*)Main->Gadgets[LAYOUT_PATH_R],NULL,NULL,
                        LAYOUT_RemoveChild,Main->Gadgets[BUTTON_REFRESH_R],TAG_END );
 }
 // history:
 Main->HistorySize=0;

 // icons:
 caIcons->LoadCommands( &IniFile,Main->Scr,Main->Gadgets[FUELGAUGE],
   Main->Gadgets[LAYOUT_ICON1],Main->Gadgets[LAYOUT_ICON2] );
// Make menus:
 BuildMenu();
 BuildDirMenu();

 if( Main->LittleImages ){
  Main->AssignImage.LoadFromFile( CA_IMG_LISTER11 "Assign10x11",Main->Scr );
  Main->DirImage.LoadFromFile( CA_IMG_LISTER11 "Drawer10x11",Main->Scr );
  Main->FileImage.LoadFromFile( CA_IMG_LISTER11 "File10x11",Main->Scr );
  Main->FileLinkImage.LoadFromFile( CA_IMG_LISTER11 "FileLink10x11",Main->Scr );
  Main->VolumeImage.LoadFromFile( CA_IMG_LISTER11 "Volume10x11",Main->Scr );
  Main->DeviceImage.LoadFromFile( CA_IMG_LISTER11 "HardDrive10x11",Main->Scr );
 }else{
  Main->AssignImage.LoadFromFile( CA_IMG_LISTER16 "Assign16x16",Main->Scr );
  Main->DirImage.LoadFromFile( CA_IMG_LISTER16 "Drawer16x16",Main->Scr );
  Main->FileImage.LoadFromFile( CA_IMG_LISTER16 "File16x16",Main->Scr );
  Main->FileLinkImage.LoadFromFile( CA_IMG_LISTER16 "FileLink16x16",Main->Scr );
  Main->VolumeImage.LoadFromFile( CA_IMG_LISTER16 "Volume16x16",Main->Scr );
  Main->DeviceImage.LoadFromFile( CA_IMG_LISTER16 "HardDrive16x16",Main->Scr );
 }
 CA_D(("InitAll: lister images loaded (little=%ld)\n", (long)Main->LittleImages));

 // menu font:
 char font[40];
 if( Main->sMenuFont[0] ){
  strcpy( font,Main->sMenuFont );
  ptr = strchr( font,',' );
  if( ptr ){
   *ptr = 0; // cut comma && size
   Main->MenuAttr.ta_YSize = atoi( ptr+1 ); // compute size
  }else Main->MenuAttr.ta_YSize = 8;
  Main->MenuAttr.ta_Name = font;
  Main->MenuFont = OpenDiskFont( &Main->MenuAttr );
 }else Main->MenuFont = NULL;
 // lister font:
 if( Main->sListerFont[0] ){
  ptr = strchr( Main->sListerFont,',' );
  if( ptr ){
   *ptr = 0; // cut comma && size
   Main->ListerAttr.ta_YSize = atoi( ptr+1 ); // compute size
  }else Main->ListerAttr.ta_YSize = 8;
  Main->ListerAttr.ta_Name  = Main->sListerFont;
  Main->ListerAttr.ta_Style = 0;
  Main->ListerAttr.ta_Flags = 0;
  Main->ListerFont = OpenDiskFont( &Main->ListerAttr );
  SetGadgetAttrs( (Gadget*)Main->Gadgets[LST_DIR_L],NULL,NULL,GA_TextAttr,&Main->ListerAttr,TAG_END );
  SetGadgetAttrs( (Gadget*)Main->Gadgets[LST_DIR_R],NULL,NULL,GA_TextAttr,&Main->ListerAttr,TAG_END );
 }else Main->ListerFont = NULL;

 // Tabs && Paths:
 InitPaths();
 if( Main->IconStart ){
  DoMethod( Main->Win, WM_ICONIFY );
  Main->IsIcon=TRUE;
 }else{
  DoMethod( Main->Win, WM_OPEN );
  GetAttr( WINDOW_Window, Main->Win, (ULONG *)&Main->IntuiWin );
  Main->IsIcon=FALSE;
 }

 ReqTags[0].ti_Data=(ULONG)Main->Scr;
 Main->FileReq=(FileRequester *)AllocAslRequest( ASL_FileRequest, ReqTags );

 /* AmigaGuide opened on demand in ShowHelp() — see CA_OpenHelp(). */
 Main->HelpOpen = FALSE;
 Main->GuideHandle = NULL;

 // Commodityport
 Main->CX.Init();
 Main->CX.Add( CXCMD_SHOW,Main->PopupKey );
 Main->CX.Add( CXCMD_HIDE,Main->HideKey );

 strcpy( ((TTabNode*)Main->Tabs[0].lh_Head)->Path,Main->SrcStartPath );
 strcpy( ((TTabNode*)Main->Tabs[1].lh_Head)->Path,Main->DstStartPath );
 ListDir( 0,NULL,0 );
 ListDir( 1,NULL,0 );
 ActivateBrowser( 0 );
 if( Main->MaxTabs > 1 ){
  SetGadgetAttrs( CurrentTab[0]->Gadget,Main->IntuiWin,NULL,GA_Selected,TRUE,TAG_DONE );
  SetGadgetAttrs( CurrentTab[1]->Gadget,Main->IntuiWin,NULL,GA_Selected,TRUE,TAG_DONE );
 }
 DragDrop.Pressed = FALSE;
 CA_D(("InitAll: ok\n"));
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
  CA_D(("FreeAll: start\n"));
  /* Only touch path strings if tabs were initialised (InitPaths ran). */
  if( Main->Tabs[0].lh_Head && Main->Tabs[0].lh_Head->ln_Succ )
    strcpy( Main->SrcStartPath,GetPath(0) );
  if( Main->Tabs[1].lh_Head && Main->Tabs[1].lh_Head->ln_Succ )
    strcpy( Main->DstStartPath,GetPath(1) );
  Main->CX.Free();
  FreeMenu();
  FreeDirMenu();
  if( Main->FileReq ) FreeAslRequest( Main->FileReq );
  Main->FileReq = NULL;
  ClearPaths();
  ClearUserPaths();
  caFileTypes->Clear();
  if( Main->HelpOpen && Main->GuideHandle ){
    CA_D(("FreeAll: CloseAmigaGuide\n"));
    CloseAmigaGuide( Main->GuideHandle );
  }
  Main->GuideHandle = NULL;
  Main->HelpOpen = FALSE;
  memset( &Main->NewAG, 0, sizeof(Main->NewAG) );
  if( Main->Win ) DoMethod( Main->Win, WM_CLOSE );
  Main->Win = NULL;
  if( Main->Resource ) RL_CloseResource( Main->Resource );
  Main->Resource = NULL;
  Main->ReqWin = NULL;
  Main->AssignImage.Free();
  Main->DirImage.Free();
  Main->FileImage.Free();
  Main->FileLinkImage.Free();
  Main->VolumeImage.Free();
  Main->DeviceImage.Free();
  caIcons->FreeIcons();
  if( Main->MsgPort )  DeleteMsgPort( Main->MsgPort );
  if( Main->NotifyPort ) DeleteMsgPort( Main->NotifyPort );
  if( Main->AppPort )  DeleteMsgPort( Main->AppPort );
  Main->MsgPort = Main->NotifyPort = Main->AppPort = NULL;
  if( Main->OwnScreenOpen ){
    while( ! CloseScreen(Main->Scr) ){
      Delay(150);
      Info(GetCatalogStr(Main->Catalog,MSG_CLOSEWINDOW,MSG_CLOSEWINDOW_STR));
    }
  }else if( Main->Scr )  UnlockPubScreen( NULL, Main->Scr );
  Main->Scr = NULL;
  Main->OwnScreenOpen = FALSE;
  if( Main->MenuFont )   CloseFont( Main->MenuFont );
  if( Main->ListerFont ) CloseFont( Main->ListerFont );
  Main->MenuFont = Main->ListerFont = NULL;
  CA_D(("FreeAll: done\n"));
}
//<

//>"int main()"
static int CA_Startup( void ){
  int Result = 0;
  BOOL ret;
  BPTR progdir;

  CA_D(("CA_Startup: begin\n"));
  Main = new TMain;

  if( !OpenLibs() ) Main->Restart = FALSE;
  else Main->Restart = TRUE;

  progdir = GetProgramDir();
  CA_D(("CA_Startup: GetProgramDir=%ld\n", (long)progdir));
  if( progdir ) Olddir = CurrentDir( progdir );
  else Olddir = CurrentDir( NULL );

  ArchiveList = new TArchiveList;
  ClipBoard   = new TClipBoard;
  caIcons     = new TcaIcons;
  caActions   = new TcaActions;
  caFileTypes = new TcaFileTypes;


  Main->Catalog=OpenCatalog( NULL,"ClassAction.catalog",OC_BuiltInLanguage,"english",
    OC_Version,CATVERS,TAG_DONE
  );
  Main->Locale=OpenLocale( NULL );
  CA_D(("CA_Startup: catalog=%ld locale=%ld\n", (long)Main->Catalog, (long)Main->Locale));

  if( !(Main->ARexx.Init("CLASSACTION")) ){
    CA_D(("CA_Startup: ARexx.Init failed\n"));
    Info( GetCatalogStr(Main->Catalog,ERR_NOAREXX,ERR_NOAREXX_STR) );
    return( FALSE );
  }
  Main->ARexx.AddCmd( "Quit",ARX_Quit );
  Main->ARexx.AddCmd( "Status",ARX_Status );
  Main->ARexx.AddCmd( "AppIconify",ARX_AppIconify );
  Main->ARexx.AddCmd( "Use",ARX_Use );
  Main->ARexx.AddCmd( "Show",ARX_Show );
  Main->ARexx.AddCmd( "Load",ARX_Load );
  Main->ARexx.AddCmd( "Ver",ARX_Ver );
  Main->ARexx.AddCmd( "Getclass",ARX_Getclass );
  Main->ARexx.AddCmd( "Action",ARX_Action );

  ClipBoard->Init( PRIMARY_CLIP );

  OpenDevice( "console.device", -1L, (IORequest*)&(Main->IOExtSer), 0L );
  Main->ConsoleDevice=(Library *)(&(Main->IOExtSer.IOSer.io_Device->dd_Library));
  caIcons->Init();

  if( PreInit() ){
    while( Main->Restart ){
      Main->Restart = FALSE;
      CA_D(("CA_Startup: calling InitAll\n"));
      ret = InitAll();
      CA_D(("CA_Startup: InitAll -> %ld\n", (long)ret));

      if( ret ) MessageLoop();
      else Result = 20;

      FreeAll();
    }
    PostClear();
  }

  caIcons->Free();
  CloseDevice( (IORequest*)&(Main->IOExtSer) );

  CloseLocale( Main->Locale );
  CloseCatalog( Main->Catalog );
  Main->ARexx.Free();

  delete caFileTypes;
  delete caActions;
  delete caIcons;
  delete ClipBoard;
  delete ArchiveList;

  CurrentDir( Olddir );
  CloseLibs();

  delete Main;
  CA_D(("CA_Startup: end result=%ld\n", (long)Result));

  return( Result );
}

int main(){
  return( CA_Startup() );
}
//<

/* SAS/C C++ forbids calling main(); WB startup shares CA_Startup(). */
void wbmain( WBStartup *wbmsg ){
 (void)wbmsg;
 CA_Startup();
}
//<
