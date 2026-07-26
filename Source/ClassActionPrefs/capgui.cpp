/****************************************

        Class Action Source

        © 2003 by Martin R. Elsner
               & Salim Gasmi

        File : capgui.cpp

        Aim  : main and GUI interface

****************************************/

#include "capgui.h"

#include <classes/window.h>
#include <clib/alib_protos.h>
#include <gadgets/button.h>
#include <gadgets/chooser.h>
#include <gadgets/clicktab.h>
#include <gadgets/getfile.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <proto/button.h>
#include <proto/chooser.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/layout.h>
#include <proto/listbrowser.h>
#include <proto/locale.h>
#include <proto/space.h>
#include <proto/window.h>
#include <proto/wb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <workbench/icon.h>

#include "caplist.h"
#include "caparexx.h"
#include "capdisk.h"
#include "capopt.h"
#include "capreq.h"
#include "caplist.h"
#define  CATCOMP_NUMBERS
#define  CATCOMP_STRINGS
#include "classactionprefscd.h"
#include "classactionprefs.h"
#include "classactionprefs_rev.h"

TcapGui capGui;

//>"TcapGui::TcapGui()"
TcapGui::TcapGui(){
  ActionCopied = FALSE;
  DateFormats.Append("dd-mm-yy");
  DateFormats.Append("dd-mmm-yy");
  DateFormats.Append("mm-dd-yy");
  DateFormats.Append("yy-mmm-dd");
}
//<

//>"TcapGui::~TcapGui()"
TcapGui::~TcapGui(){
}
//<

//>"void AdjustPath( char *Path )"
void AdjustPath( char *Path ){
 if( !Path[0] ) return;
 if( (Path[0]!=0)&&(Path[strlen(Path)-1]!=':')&&(Path[strlen(Path)-1]!='/') ){
  strcat(Path,"/");
 }
}
//<

//>"struct Node *GetPreNode( List *List,char *string )"
/****************************************/
/*                                      */
/* Function:  GetPreNode                */
/*                                      */
/* Aim:       Get the address of the    */
/*            node after which the      */
/*            given string must be      */
/*            inserted                  */
/*                                      */
/* Input:     List=list to search       */
/*             through                  */
/*            string=text which has to  */
/*             be inserted              */
/*                                      */
/* Output:    the address of the        */
/*             wanted node              */
/*                                      */
/****************************************/
struct Node *GetPreNode( List *List, char *string ){

 struct Node *f=(Node*)List->lh_Head;
//search (   till end    )&&( while the names have lower letters )
 while( (f->ln_Succ!=NULL)&&(stricmp(((NamedNode*)f)->Name,string)<0) ){
  f=f->ln_Succ;
 }
 return(f->ln_Pred);    // of course: we wanted the PREDATOR!
}
//<

//>"void CopyNBytes( char *src, char *dst, int n )"
void CopyNBytes( char *src, char *dst, int n ){
 for( int i=0;i<n;i++ ) src[i]=dst[i];
 src[n] = 0;
}
//<

//>"void AdjustCommandLine( char *CommandLine, char *Buffer )"
void AdjustCommandLine( char *CommandLine, char *Buffer ){
  char *found;
  do{
    if( (found=strstr(CommandLine,"REQD[")) ||
        (found=strstr(CommandLine,"REQF[")) ||
        (found=strstr(CommandLine,"REQT[")) ||
        (found=strstr(CommandLine,"REQV[")) ||
        (found=strstr(CommandLine,"SURE[")) ){
      CopyNBytes( Buffer,CommandLine,found-CommandLine );
      strcat( Buffer,"[" );
      CopyNBytes( Buffer+strlen(Buffer),found,4 );
      strcat( Buffer,":" );
      strcat( Buffer,found+5 );
      strcpy( CommandLine,Buffer );
    }
  }while( found );
}
//<


// Options:

//>"void LoadAndDeleteTooltypes()"
/*
void LoadAndDeleteTooltypes(){
 DiskObject *dob;
 char *tool,*dir,str[50],str2[50];

 strcpy( Main.CAPath, "wb:tools/classaction" );
 if( !Freq( Main.CAPath,GetCatalogStr(Main.Catalog,TXT_WHEREISCA,TXT_WHEREISCA_STR) ) ) return;
 dob=GetDiskObject( Main.CAPath );
 if( !dob) return;
 tool=new char[1024];
 dir=new char[256];

 OptionList.Reset();

//>"General:"
 OptionList.AddGroup( "General",GetCatalogStr(Main.Catalog,TXT_GENERAL,TXT_GENERAL_STR)  );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_ICONSTART,TXT_ICONSTART_STR),
                'b',"ICONSTART",(ULONG)GetToolType(dob,"ICONSTART",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_MUIQUIT,TXT_MUIQUIT_STR),
                'b',"MUIQUIT",(ULONG)GetToolType(dob,"MUIQUIT",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CONFIRMREPLACE,TXT_CONFIRMREPLACE_STR),
                'b',"CONFIRMREPLACE",(ULONG)GetToolType(dob,"CONFIRMREPLACE",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CHECKFIT,TXT_CHECKFIT_STR),
                'b',"CHECKFIT",(ULONG)GetToolType(dob,"CHECKFIT",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SHOWHELP,TXT_SHOWHELP_STR),
                        'b',"SHOWHELP",(ULONG)GetToolType(dob,"SHOWHELP",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_PUBLICSCREEN,TXT_PUBLICSCREEN_STR),
                's',"PUBLICSCREEN",(ULONG)"" );
 if( GetToolType(dob,"CLISIZE",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CLISIZE,TXT_CLISIZE_STR),
                        's',"CLISIZE",(ULONG)tool );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CLISIZE,TXT_CLISIZE_STR),
                        's',"CLISIZE",(ULONG)"CON:0/0/640/100/ClassAction_Output_Window" );
 }//<
//>"Window:"
 OptionList.AddGroup( "Window",GetCatalogStr(Main.Catalog,TXT_WINDOW,TXT_WINDOW_STR)  );
 if( GetToolType(dob,"X",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_LEFTEDGE,TXT_LEFTEDGE_STR),
                        'i',"X",(ULONG)atoi(tool) );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_LEFTEDGE,TXT_LEFTEDGE_STR),
                        'i',"X",(ULONG)-1 );
 }
 if( GetToolType(dob,"Y",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_TOPEDGE,TXT_TOPEDGE_STR),
                        'i',"Y",(ULONG)atoi(tool) );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_TOPEDGE,TXT_TOPEDGE_STR),
                        'i',"Y",(ULONG)-1 );
 }
 if( GetToolType(dob,"WIDTH",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_WIDTH,TXT_WIDTH_STR),
                        'i',"WIDTH",(ULONG)atoi(tool) );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_WIDTH,TXT_WIDTH_STR),
                        'i',"WIDTH",(ULONG)320 );
 }
 if( GetToolType(dob,"HEIGHT",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HEIGHT,TXT_HEIGHT_STR),
                        'i',"HEIGHT",(ULONG)atoi(tool) );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HEIGHT,TXT_HEIGHT_STR),
                        'i',"HEIGHT",(ULONG)256 );
 }
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_APPX,TXT_APPX_STR),
                        'i',"APPX",-1 );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_APPY,TXT_APPY_STR),
                        'i',"APPY",-1 );
 //<
//>"Buttons:"
 OptionList.AddGroup( "Buttons",GetCatalogStr(Main.Catalog,TXT_BUTTONS,TXT_BUTTONS_STR)  );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_PARENT,TXT_PARENT_STR),
                'b',"PARENT",(ULONG)GetToolType(dob,"PARENT",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_PATHMENU,TXT_PATHMENU_STR),
                        'b',"PATHMENU",(ULONG)TRUE );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HISTORY,TXT_HISTORY_STR),
                'b',"HISTORY",(ULONG)GetToolType(dob,"HISTORY",tool) );
//<
//>"Listers:"
 OptionList.AddGroup( "Listers",GetCatalogStr(Main.Catalog,TXT_LISTERS,TXT_LISTERS_STR)  );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SHOWIM,TXT_SHOWIM_STR),
                        'b',"SHOWIM",(ULONG)GetToolType(dob,"SHOWIM",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SMALLIM,TXT_SMALLIM_STR),
                        'b',"SMALLIM",(ULONG)GetToolType(dob,"SMALLIM",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SHOWDEVS,TXT_SHOWDEVS_STR),
                        'b',"SHOWDEVS",(ULONG)FALSE );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HIGHDIRS,TXT_HIGHDIRS_STR),
                        'b',"HIGHDIRS",(ULONG)GetToolType(dob,"HIGHDIRS",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DRAWERSFIRST,TXT_DRAWERSFIRST_STR),
                        'b',"DRAWERSFIRST",(ULONG)GetToolType(dob,"DRAWERSFIRST",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DATESTRING,TXT_DATESTRING_STR),
                        'b',"DATESTRING",(ULONG)GetToolType(dob,"DATESTRING",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DISPTIME,TXT_DISPTIME_STR),
                        'b',"DISPTIME",(ULONG)GetToolType(dob,"DISPTIME",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SRCSTARTPATH,TXT_SRCSTARTPATH_STR),
                        'f',"SRCSTARTPATH",(ULONG)"" );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DSTSTARTPATH,TXT_DSTSTARTPATH_STR),
                        'f',"DSTSTARTPATH",(ULONG)"" );
//<
//>"Icons:"
 OptionList.AddGroup( "Icons",GetCatalogStr(Main.Catalog,TXT_ICONS,TXT_ICONS_STR)  );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DISPINFO,TXT_DISPINFO_STR),
                        'b',"DISPINFO",(ULONG)GetToolType(dob,"DISPINFO",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CREATEICONS,TXT_CREATEICONS_STR),
                        'b',"CREATEICONS",(ULONG)GetToolType(dob,"CREATEICONS",tool) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_COPYICONS,TXT_COPYICONS_STR),
                        'b',"COPYICONS",(ULONG)GetToolType(dob,"COPYICONS",tool) );
//<
//>"Paths:"
 OptionList.AddGroup( "Paths",GetCatalogStr(Main.Catalog,TXT_PATHS,TXT_PATHS_STR)  );
 if( GetToolType(dob,"ICONFILE",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_ICONFILE,TXT_ICONFILE_STR),
                        'f',"ICONFILE",(ULONG)tool );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_ICONFILE,TXT_ICONFILE_STR),
                        'f',"ICONFILE",(ULONG)Main.CAPath );
 }
 if( GetToolType(dob,"HELPFILE",tool) ){
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HELPFILE,TXT_HELPFILE_STR),
                        'f',"HELPFILE",(ULONG)tool );
 }else{
  OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HELPFILE,TXT_HELPFILE_STR),
                        'f',"HELPFILE",(ULONG)"HELP:ClassAction.guide" );
 }
 BPTR lock=GetProgramDir();
 NameFromLock( lock,dir,256 );
 strcpy( tool,dir );
 AddPart( tool,"ClassActionPrefs",400 );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CAPFILE,TXT_CAPFILE_STR),
        'f',"CAPREFS",(ULONG)tool );
 //<
//>"Pathbuttons:"
 OptionList.AddGroup( "Pathmenu",GetCatalogStr(Main.Catalog,TXT_PATHMENU,TXT_PATHMENU_STR)  );
 for( int i=1;i<21;i++ ){
  // str holds the tooltype, str2 the title for the entry:
  sprintf( str,"DRIVE%d",i );
  sprintf( str2,"%s %d",GetCatalogStr(Main.Catalog,TXT_PATH,TXT_PATH_STR),i );
  if( GetToolType(dob,str,tool) ) OptionList.Add( str2,'p',str,(ULONG)tool );
  else OptionList.Add( str2,'p',str,(ULONG)"" );
 }//<


 // delete all except DONOTWAIT
 char *tooltype[3];
 char **lasttools;
 int current;

 current=0;
 tooltype[current]=(char*)AllocVec( 256,MEMF_CLEAR );
 strcpy( tooltype[current],"DONOTWAIT" );
 current++;
 tooltype[current]=(char*)AllocVec( 256,MEMF_CLEAR );
 strcpy( tooltype[current],"Change preferences with ClassActionPrefs!" );
 current++;
 tooltype[current]=NULL;

 lasttools=dob->do_ToolTypes;
 dob->do_ToolTypes=&(tooltype[0]);
 PutDiskObject( Main.CAPath,dob );
 dob->do_ToolTypes=lasttools;
 FreeDiskObject( dob );
 FreeVec( tooltype[0] );
 FreeVec( tooltype[1] );

 delete dir;
 delete tool;
}
*/
//<

//>"BOOL LoadOptions( TIniFile *IniFile )"
BOOL LoadOptions( TIniFile *IniFile ){
 char *svalue=new char[512];
 char *dir=new char[512];
 char *drv=new char[512];
 char *g;

 OptionList.Init( Main.Win,Main.Gadgets[LIST_OPTIONS] );
// General
 g="General";
 OptionList.AddGroup( g,GetCatalogStr(Main.Catalog,TXT_GENERAL,TXT_GENERAL_STR)  );

 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_ICONSTART,TXT_ICONSTART_STR),
                'b',"ICONSTART",(ULONG)IniFile->GetBool(g,"ICONSTART",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_MUIQUIT,TXT_MUIQUIT_STR),
                'b',"MUIQUIT",(ULONG)IniFile->GetBool(g,"MUIQUIT",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_USEREACTIONREQUEST,MSG_USEREACTIONREQUEST_STR),
                'b',"USEREACTIONREQUEST",(ULONG)IniFile->GetBool(g,"USEREACTIONREQUEST",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_PULLDOWNMENU,MSG_PULLDOWNMENU_STR),
                'b',"PULLDOWNMENU",(ULONG)IniFile->GetBool(g,"PULLDOWNMENU",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_MENUFONT,TXT_MENUFONT_STR),
                'z',"MENUFONT",(ULONG)IniFile->GetString( g,"MENUFONT","",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CLISIZE,TXT_CLISIZE_STR),'s',"CLISIZE",
                (ULONG)IniFile->GetString(g,"CLISIZE","CON:0/0/640/100/ClassAction_Output_Window",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_OWNSCREEN,MSG_OWNSCREEN_STR),
                'm',"OWNSCREEN",(ULONG)IniFile->GetString( g,"OWNSCREEN","",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_PUBLICSCREEN,TXT_PUBLICSCREEN_STR),
                's',"PUBLICSCREEN",(ULONG)IniFile->GetString( g,"PUBLICSCREEN","",Main.PublicScreen) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_POPUPKEY,MSG_POPUPKEY_STR),
                's',"POPUPKEY",(ULONG)IniFile->GetString( g,"POPUPKEY","CTRL ALT F1",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_HIDEKEY,MSG_HIDEKEY_STR),
                's',"HIDEKEY",(ULONG)IniFile->GetString( g,"HIDEKEY","CTRL ALT F2",svalue) );
// CopyAndDelete
 g="CopyAndDelete";
 OptionList.AddGroup( g,GetCatalogStr(Main.Catalog,MSG_COPYANDDELETE,MSG_COPYANDDELETE_STR)  );

 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_BUFFERSIZE,MSG_BUFFERSIZE_STR),
                'i',"BUFFERSIZE",(ULONG)IniFile->GetInt(g,"BUFFERSIZE",100) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_COUNTFILES,MSG_COUNTFILES_STR),
                'b',"COUNTFILES",(ULONG)IniFile->GetBool(g,"COUNTFILES",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CHECKFIT,TXT_CHECKFIT_STR),
                'b',"CHECKFIT",(ULONG)IniFile->GetBool(g,"CHECKFIT",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CONFIRMREPLACE,TXT_CONFIRMREPLACE_STR),
                'b',"CONFIRMREPLACE",(ULONG)IniFile->GetBool(g,"CONFIRMREPLACE",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_SHOWVERSION,MSG_SHOWVERSION_STR),
                'b',"SHOWVERSION",(ULONG)IniFile->GetBool(g,"SHOWVERSION",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_REPLACEVERSION,MSG_REPLACEVERSION_STR),
                'b',"REPLACEVERSION",(ULONG)IniFile->GetBool(g,"REPLACEVERSION",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_CONFIRMDELETE,MSG_CONFIRMDELETE_STR),
                'b',"CONFIRMDELETE",(ULONG)IniFile->GetBool(g,"CONFIRMDELETE",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_DELETEPROTECTED,MSG_DELETEPROTECTED_STR),
                'b',"DELETEPROTECTED",(ULONG)IniFile->GetBool(g,"DELETEPROTECTED",FALSE) );
// Window
 g="Window";
 OptionList.AddGroup( g,GetCatalogStr(Main.Catalog,TXT_WINDOW,TXT_WINDOW_STR)  );
  
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_LEFTEDGE,TXT_LEFTEDGE_STR),
                'i',"X",(ULONG)IniFile->GetInt(g,"X",-1) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_TOPEDGE,TXT_TOPEDGE_STR),
                'i',"Y",(ULONG)IniFile->GetInt(g,"Y",-1) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_WIDTH,TXT_WIDTH_STR),
                'i',"WIDTH",(ULONG)IniFile->GetInt(g,"WIDTH",320) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HEIGHT,TXT_HEIGHT_STR),
                'i',"HEIGHT",(ULONG)IniFile->GetInt(g,"HEIGHT",200) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_APPX,TXT_APPX_STR),
                'i',"APPX",(ULONG)IniFile->GetInt(g,"APPX",-1) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_APPY,TXT_APPY_STR),
                'i',"APPY",(ULONG)IniFile->GetInt(g,"APPY",-1) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_BACKGROUND,TXT_BACKGROUND_STR),
                'f',"BACKGROUND",(ULONG)IniFile->GetString( g,"BACKGROUND","",svalue) );
// Buttons
 g="Buttons";
 OptionList.AddGroup( g,GetCatalogStr(Main.Catalog,TXT_BUTTONS,TXT_BUTTONS_STR)  );

 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_PARENT,TXT_PARENT_STR),
                        'b',"PARENT",(ULONG)IniFile->GetBool(g,"PARENT",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_REFRESH,TXT_REFRESH_STR),
                        'b',"REFRESH",(ULONG)IniFile->GetBool(g,"REFRESH",TRUE) );
// Listers
 g="Listers";
 OptionList.AddGroup( g,GetCatalogStr(Main.Catalog,TXT_LISTERS,TXT_LISTERS_STR)  );

 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_VERTICAL,MSG_VERTICAL_STR),
                        'b',"VERTICAL",(ULONG)IniFile->GetBool(g,"VERTICAL",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SHOWIM,TXT_SHOWIM_STR),
                        'b',"SHOWIM",(ULONG)IniFile->GetBool(g,"SHOWIM",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_CLASSIMAGES,MSG_CLASSIMAGES_STR),
                        'b',"CLASSIMAGES",(ULONG)IniFile->GetBool(g,"CLASSIMAGES",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_ONLYPATTERNS,MSG_ONLYPATTERNS_STR),
                        'b',"ONLYPATTERNS",(ULONG)IniFile->GetBool(g,"ONLYPATTERNS",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SMALLIM,TXT_SMALLIM_STR),
                        'b',"SMALLIM",(ULONG)IniFile->GetBool(g,"SMALLIM",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SHOWDEVS,TXT_SHOWDEVS_STR),
                        'b',"SHOWDEVS",(ULONG)IniFile->GetBool(g,"SHOWDEVS",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_HIGHDIRS,TXT_HIGHDIRS_STR),
                        'b',"HIGHDIRS",(ULONG)IniFile->GetBool(g,"HIGHDIRS",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DRAWERSFIRST,TXT_DRAWERSFIRST_STR),
                        'b',"DRAWERSFIRST",(ULONG)IniFile->GetBool(g,"DRAWERSFIRST",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DATESTRING,TXT_DATESTRING_STR),
                        'b',"DATESTRING",(ULONG)IniFile->GetBool(g,"DATESTRING",FALSE) );
 capGui.DateFormats.Selected = capGui.DateFormats.IndexOf(IniFile->GetString(g,"DATEFORMAT","dd-mm-yy",svalue));
 if( capGui.DateFormats.Selected==-1 ) capGui.DateFormats.Selected=0;
 //IniFile->GetString(g,"DATEFORMAT","",svalue)
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_DATEFORMAT,MSG_DATEFORMAT_STR),
                        'o',"DATEFORMAT",(ULONG)&capGui.DateFormats );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DISPTIME,TXT_DISPTIME_STR),
                        'b',"DISPTIME",(ULONG)IniFile->GetBool(g,"DISPTIME",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_PATHNOTIFY,TXT_PATHNOTIFY_STR),
                        'b',"PATHNOTIFY",(ULONG)IniFile->GetBool(g,"PATHNOTIFY",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_MAXPATHS,TXT_MAXPATHS_STR),
                        'i',"MAXPATHS",(ULONG)IniFile->GetInt(g,"MAXPATHS",10) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_MAXTABS,TXT_MAXTABS_STR),
                        'i',"MAXTABS",(ULONG)IniFile->GetInt(g,"MAXTABS",5) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_TABBUTTONSTRANSPARENT,MSG_TABBUTTONSTRANSPARENT_STR),
                        'b',"TABBUTTONSTRANSPARENT",(ULONG)IniFile->GetBool(g,"TABBUTTONSTRANSPARENT",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_SRCSTARTPATH,TXT_SRCSTARTPATH_STR),
                        'd',"SRCSTARTPATH",(ULONG)IniFile->GetString(g,"SRCSTARTPATH","",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DSTSTARTPATH,TXT_DSTSTARTPATH_STR),
                        'd',"DSTSTARTPATH",(ULONG)IniFile->GetString(g,"DSTSTARTPATH","",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_LISTERFONT,TXT_LISTERFONT_STR),
                        'z',"LISTERFONT",(ULONG)IniFile->GetString( g,"LISTERFONT","",svalue) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_USEEXALL,MSG_USEEXALL_STR),
                        'b',"USEEXALL",(ULONG)IniFile->GetBool(g,"USEEXALL",FALSE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,MSG_EXALLFILES,MSG_EXALLFILES_STR),
                        'i',"EXALLFILES",(ULONG)IniFile->GetInt(g,"EXALLFILES",100) );
// Icons
 g="Icons";
 OptionList.AddGroup( g,GetCatalogStr(Main.Catalog,TXT_ICONS,TXT_ICONS_STR)  );

 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DISPINFO,TXT_DISPINFO_STR),
                        'b',"DISPINFO",(ULONG)IniFile->GetBool(g,"DISPINFO",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_CREATEICONS,TXT_CREATEICONS_STR),
                        'b',"CREATEICONS",(ULONG)IniFile->GetBool(g,"CREATEICONS",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_COPYICONS,TXT_COPYICONS_STR),
                        'b',"COPYICONS",(ULONG)IniFile->GetBool(g,"COPYICONS",TRUE) );
 OptionList.Add( GetCatalogStr(Main.Catalog,TXT_DELETEICONS,TXT_DELETEICONS_STR),
                        'b',"DELETEICONS",(ULONG)IniFile->GetBool(g,"DELETEICONS",TRUE) );
 delete [] drv;
 delete [] dir;
 delete [] svalue;
 return( TRUE );
}
//<

//>"void SaveOptions( TIniFile *IniFile )"
void SaveOptions( TIniFile *IniFile ){
 OptionList.WriteAll( IniFile );
}
//<


// Commands:

//>"void LoadCommands( TIniFile *IniFile )"
/****************************************/
/*                                      */
/* Function:  ListCommands              */
/*                                      */
/* Aim:       List all commands         */
/*                                      */
/* Input:     The inifile where the     */
/*            commands are stored in    */
/*            [Commands]                */
/*                                      */
/* Output:    TRUE: everything ok       */
/*            FALSE: error occured      */
/*                                      */
/****************************************/
void LoadCommands( TIniFile *IniFile ){
 CommandNode *newnode,*pred;
 List *LBL;
 BOOL end=FALSE;
 char *ptr,*entryline;
 int mode=-1,i;
 char buffer[256];

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
                        LISTBROWSER_Labels,NULL,TAG_END );
 pred=(CommandNode*)LBL->lh_Head;

 end=!IniFile->SeekGroup( "Commands" );
 
 while (!end ){
  entryline=IniFile->GetNextGroupEntry();
  if( entryline && entryline[0] ){
   i=atoi( entryline );
   if( (i<0) || (i>2) ) break;
   while( mode<i ){
    // insert titles:
    mode++;
    switch( mode ){
     case 0: ptr=GetCatalogStr( Main.Catalog,TXT_TOPICONS,TXT_TOPICONS_STR ); break;
     case 1: ptr=GetCatalogStr( Main.Catalog,TXT_BOTTOMICONS,TXT_BOTTOMICONS_STR ); break;
     case 2: ptr=GetCatalogStr( Main.Catalog,TXT_KEYCOM,TXT_KEYCOM_STR ); break;
    }
    newnode=(CommandNode *)AllocListBrowserNode( 1,
                 LBNA_NodeSize,sizeof(CommandNode),TAG_END );
    SetListBrowserNodeAttrs( (Node *)newnode,
                 LBNCA_Text,ptr,
                 LBNA_Generation,1L,
                 LBNA_Flags,LBFLG_HASCHILDREN,
                 TAG_DONE );
    AddTail( LBL, (Node*)newnode );
   }
   //      make node:
   newnode=(CommandNode *)AllocListBrowserNode( 1,
                LBNA_NodeSize,sizeof(CommandNode),TAG_END );
   if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->name, entryline );
   else newnode->name[0]=0;
   if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->help, entryline );
   else newnode->help[0]=0;
   if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->exec, entryline );
   else newnode->exec[0]=0;
   AdjustCommandLine( newnode->exec,buffer );
   newnode->async   = FALSE;
   newnode->CD      = 0;
   newnode->readsrc = FALSE;
   newnode->readdst = FALSE;
   if( entryline=IniFile->GetNextGroupEntry() ){
     if( entryline[0] != 'M' ){
       newnode->wbrun = atoi( entryline );
       if( newnode->wbrun>9 ){
         newnode->wbrun-=10;
         newnode->readsrc = TRUE;
         newnode->readdst = TRUE;
       }else newnode->async=TRUE;
     }else{
       newnode->wbrun = entryline[1]-48;
       newnode->CD = entryline[2]-48;
       if( (newnode->CD<0) || (newnode->CD>2) ) newnode->CD=0;
       if( strchr(entryline,'A') ) newnode->async=TRUE;
       if( strchr(entryline,'C') ) newnode->CD=1;
       if( strchr(entryline,'S') ) newnode->readsrc=TRUE;
       if( strchr(entryline,'D') ) newnode->readdst=TRUE;
     }
   }else{
     newnode->wbrun=RUN_NOCLI;
     newnode->async=TRUE;
   }
   if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->stack, entryline );
   else strcpy( newnode->stack,"4096" );
   if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->delay, entryline );
   else strcpy( newnode->delay,"0" );
   if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->icon, entryline );
   else newnode->icon[0]=0;
   if( entryline=IniFile->GetNextGroupEntry() ){
    if( entryline[0]=='"' ){
     newnode->key=entryline[1];
     newnode->ascii=TRUE;
    }else{
     newnode->key=atoi( entryline );
     newnode->ascii=FALSE;
    }
   }else{ newnode->key=0; newnode->ascii=FALSE; }
   // assign name to node:
   SetListBrowserNodeAttrs( (Node *)newnode,
                LBNCA_Text,newnode->name,
                LBNA_Generation,2L,TAG_DONE );
   //      insert in List:
   AddTail( LBL, (Node*)newnode );
  }else end=TRUE;
 }

 while( mode<2 ){
  // insert last titles:
  mode++;
  switch( mode ){
   case 0: ptr=GetCatalogStr( Main.Catalog,TXT_TOPICONS,TXT_TOPICONS_STR ); break;
   case 1: ptr=GetCatalogStr( Main.Catalog,TXT_BOTTOMICONS,TXT_BOTTOMICONS_STR ); break;
   case 2: ptr=GetCatalogStr( Main.Catalog,TXT_KEYCOM,TXT_KEYCOM_STR ); break;
  }
  newnode=(CommandNode *)AllocListBrowserNode( 1,
               LBNA_NodeSize,sizeof(CommandNode),TAG_END );
  SetListBrowserNodeAttrs( (Node *)newnode,
               LBNCA_Text,ptr,
               LBNA_Generation,1L,
               LBNA_Flags,LBFLG_HASCHILDREN,
               TAG_DONE );
  AddTail( LBL, (Node*)newnode );
 }

 HideAllListBrowserChildren( LBL );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
                        LISTBROWSER_Labels,LBL,TAG_END );
}
//<

//>"void ResetCommands()"
void ResetCommands(){
 List *list;
 if( Main.IntuiWin ){
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&list );
  if( list ){
   FreeListBrowserList( list );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
                 LISTBROWSER_Labels,list,TAG_END );
 }}
}
//<

//>"void AddCommand()"
void AddCommand(){
  List *LBL;
  CommandNode *newcom;
  Node *selnode;
  ULONG nodes,newsel;

  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_COMMANDS],&newsel );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&selnode );
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
  GetAttr( LISTBROWSER_TotalNodes,Main.Gadgets[LIST_COMMANDS],(ULONG*)&nodes );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
    LISTBROWSER_Labels,NULL,TAG_END );
  newcom=(CommandNode *)AllocListBrowserNode( 1,
    LBNA_NodeSize,sizeof(CommandNode), LBNCA_Text,"?",LBNA_Generation,2, TAG_END );
  strcpy( newcom->name,"?" );
  newcom->help[0]=0;
  newcom->exec[0]=0;
  strcpy( newcom->stack,"30000" );
  strcpy( newcom->delay,"0" );
  newcom->key=0;
  newcom->ascii=TRUE;
  newcom->wbrun=RUN_NOCLI;
  newcom->async=TRUE;
  newcom->CD=0;
  newcom->readsrc=FALSE;
  newcom->readdst=FALSE;
  newcom->icon[0]=0;

  if( selnode ){
    Insert( LBL,(Node*)newcom,selnode );
    newsel++;
  }else{
    AddTail( LBL,(Node*)newcom );
    newsel=nodes+1;
  }
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
    LISTBROWSER_Labels,LBL, LISTBROWSER_SelectedNode,newcom, TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
    LISTBROWSER_MakeVisible,newsel, TAG_END );
  AdjustComGadgets();
  ActivateLayoutGadget( (Gadget*)Main.Gadgets[LAYOUT_COMDEF],Main.IntuiWin,NULL,
    (ULONG)Main.Gadgets[STRING_CNAME] );
}
//<

//>"void RemoveCommand()"
void RemoveCommand(){
 List *LBL;
 CommandNode *com;
 ULONG gen;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 if( com==NULL ) return;
 GetListBrowserNodeAttrs( (Node*)com,LBNA_Generation,(ULONG*)&gen,TAG_END );
 if( gen==1 ) return;
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
                        LISTBROWSER_Labels,NULL,
                        TAG_END );
 Remove( (Node*)com );
 FreeListBrowserNode( (Node*)com );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_Labels,LBL,
                        LISTBROWSER_Selected,-1L,
                        TAG_END );
 AdjustComGadgets();
}
//<

//>"void MoveComUp()"
void MoveComUp(){
 List *LBL;
 CommandNode *com;
 Node *pre;
 ULONG gen,sel;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_COMMANDS],(ULONG*)&sel );
 if( com==NULL ) return;        // no node selected!
 GetListBrowserNodeAttrs( (Node*)com,LBNA_Generation,(ULONG*)&gen,TAG_END );
 if( gen==1 ) return;           // group can't be moved!

 pre=com->ln_Pred->ln_Pred;
 if( pre->ln_Pred==NULL ) return;       // we are on the top!

 // now deattach, move node and attach again:
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_Labels,NULL,
                        TAG_END );
 ShowAllListBrowserChildren( LBL );

 Remove( (Node*)com );
 Insert( LBL,(Node*)com,pre );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_Labels,LBL,
                        LISTBROWSER_Selected,sel-1,
                        TAG_END );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_MakeVisible,sel-1,
                        TAG_END );
}
//<

//>"void MoveComDown()"
void MoveComDown(){
 List *LBL;
 CommandNode *com;
 Node *succ;
 ULONG gen,sel;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_COMMANDS],(ULONG*)&sel );
 if( com==NULL ) return;        // no node selected!
 GetListBrowserNodeAttrs( (Node*)com,LBNA_Generation,(ULONG*)&gen,TAG_END );
 if( gen==1 ) return;           // group can't be moved!

 succ=com->ln_Succ;
 if( succ->ln_Succ==NULL ) return;       // we are on the bottom!

 // now deattach, move node and attach again:
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_Labels,NULL,
                        TAG_END );
 ShowAllListBrowserChildren( LBL );

 Remove( (Node*)com );
 Insert( LBL,(Node*)com,succ );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_Labels,LBL,
                        LISTBROWSER_Selected,sel+1,
                        TAG_END );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_MakeVisible,sel+1,
                        TAG_END );
}
//<

//>"void MoveCommandLister( ULONG key )"
void MoveCommandLister( ULONG key ){
 CommandNode *com;
 Node *node;
 List *LBL;
 ULONG flags;
 BOOL error=FALSE;

 switch( key ){
  case CURSORUP: case CURSORDOWN: case CURSORLEFT: case CURSORRIGHT: break;
  default: return;
 }

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 if( com==NULL ) return;
 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
 node=(Node*)com;
 switch( key ){
  case CURSORUP:{
   do{
    node=node->ln_Pred;
    if( node->ln_Pred ) GetListBrowserNodeAttrs( node,LBNA_Flags,&flags,TAG_END );
    else error=TRUE;
   }while( !error && (flags & LBFLG_HIDDEN) );
   break;
  }
  case CURSORDOWN:{
   do{
    node=node->ln_Succ;
    if( node->ln_Succ ) GetListBrowserNodeAttrs( node,LBNA_Flags,&flags,TAG_END );
    else error=TRUE;
   }while( !error && (flags & LBFLG_HIDDEN) );
   break;
  }
  case CURSORRIGHT:{
   GetListBrowserNodeAttrs( node,LBNA_Flags,&flags,TAG_END );
   if( flags & LBFLG_HASCHILDREN ){
    SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                          LISTBROWSER_Labels,NULL,TAG_END );
    ShowListBrowserNodeChildren( node,1 );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                          LISTBROWSER_Labels,LBL,TAG_END );
   }
   return;
  }
  case CURSORLEFT:{
   GetListBrowserNodeAttrs( node,LBNA_Flags,&flags,TAG_END );
   if( flags & LBFLG_HASCHILDREN ){
    SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                          LISTBROWSER_Labels,NULL,TAG_END );
    HideListBrowserNodeChildren( node );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                          LISTBROWSER_Labels,LBL,TAG_END );
   }
   return;
  }
 }
 if( !error ){
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                         LISTBROWSER_SelectedNode,node,
                         TAG_END );
  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_COMMANDS],(ULONG*)&flags );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                        LISTBROWSER_MakeVisible,flags,TAG_END );
  AdjustComGadgets();
 }
}
//<

//>"void AdjustCName()"
void AdjustCName(){
 CommandNode *com;
 char *str;
 List *LBL;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&LBL );
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_CNAME],(ULONG*)&str );
 strncpy( com->name,str,20 );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],NULL,NULL,
                LISTBROWSER_Labels,NULL,TAG_END );
 SetListBrowserNodeAttrs( (Node*)com, LBNCA_Text,com->name,TAG_END );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_COMMANDS],Main.IntuiWin,NULL,
                LISTBROWSER_Labels,LBL,TAG_END );
}
//<

//>"void AdjustCHelp()"
void AdjustCHelp(){
 CommandNode *com;
 char *str;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_CHELP],(ULONG*)&str );
 strncpy( com->help,str,256 );
}
//<

//>"void AdjustCommand()"
void AdjustCommand(){
 CommandNode *com;
 char *str;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_COMMAND],(ULONG*)&str );
 strncpy( com->exec,str,256 );
}
//<

//>"void AdjustCMode()"
void AdjustCMode(){
 ULONG code;
 BOOL nocli;
 CommandNode *com;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_CMODE],&code );
 com->wbrun=code;
 nocli = (code!=RUN_CLI);
 SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CSTACK],Main.IntuiWin,NULL,
                       GA_Disabled,nocli,TAG_END );
 RefreshGList( (Gadget*)Main.Gadgets[STRING_CSTACK],Main.IntuiWin,NULL,1 );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CDELAY],Main.IntuiWin,NULL,
                       GA_Disabled,nocli,TAG_END );
 RefreshGList( (Gadget*)Main.Gadgets[STRING_CDELAY],Main.IntuiWin,NULL,1 );
 code=(com->wbrun != RUN_INTERNAL);
 SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_INT],Main.IntuiWin,NULL,
                       GA_Disabled,code,TAG_END );
}
//<

//>"void  AdjustCCD()"
void  AdjustCCD(){
  ULONG code;
  CommandNode *com;
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
  GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_CDIR],&code );
  com->CD=code;
}
//<

//>"void AdjustCDir()"
void AdjustCDir(){
 ULONG code;
 CommandNode *com;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( GA_Selected,Main.Gadgets[CHECK_CASYNC],&code );
 com->async   = code;
 GetAttr( GA_Selected,Main.Gadgets[CHECK_CRESOURCE],&code );
 com->readsrc = code;
 GetAttr( GA_Selected,Main.Gadgets[CHECK_CREDEST],&code );
 com->readdst = code;
}
//<

//>"void AdjustCStack()"
void AdjustCStack(){
 CommandNode *com;
 int correction;
 char *str;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_CSTACK],(ULONG*)&str );
 correction=atoi(str);
 if( correction<4096 ) correction=4096;
 sprintf( com->stack,"%d",correction );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CSTACK],Main.IntuiWin,NULL,
                        STRINGA_TextVal,com->stack,TAG_END );
}
//<

//>"void AdjustCDelay()"
void AdjustCDelay(){
 CommandNode *com;
 int correction;
 char *str;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_CDELAY],(ULONG*)&str );
 correction=atoi(str);
 if( correction<-1 ) correction=-1;
 else if( correction>99 ) correction=99;
 sprintf( com->delay,"%d",correction );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CDELAY],Main.IntuiWin,NULL,
                        STRINGA_TextVal,com->delay,TAG_END );
}
//<

//>"void EnterKey()"
void EnterKey(){
 CommandNode *node;
 BOOL end;
 Object *win,*layout,*gadg;
 ULONG windowsignals,mask,code,result;
 char str[10];

 str[0]=0;
 SetAttrs( Main.Win,WA_BusyPointer,TRUE,TAG_END );

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );

 // alloc window
 layout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                        LAYOUT_Orientation,LAYOUT_HORIZONTAL,
                        LAYOUT_DeferLayout,TRUE,
                        TAG_END );
 win=(Object*)NewObject( WINDOW_GetClass(),NULL,
                        WINDOW_Position,WPOS_CENTERMOUSE,
                        WINDOW_Layout,layout,
                        WINDOW_LockHeight,TRUE,
                        WA_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_RAWKEY,
                        WA_Activate,TRUE,
                        WA_DragBar,TRUE,
                        WA_SizeGadget,TRUE,
                        WA_RMBTrap,TRUE,
                        WA_CloseGadget,TRUE,
                        TAG_END );
 gadg=(Object*)NewObject( BUTTON_GetClass(),NULL,
                        GA_Text,GetCatalogStr(Main.Catalog,TXT_PRESSKEY,TXT_PRESSKEY_STR),
                        BUTTON_Transparent,TRUE,
                        BUTTON_BevelStyle,BVS_NONE,
                        TAG_END );
 SetGadgetAttrs( (Gadget*)layout,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
 // open window and wait till closing
 DoMethod( win,WM_OPEN );
 end=FALSE;
 GetAttr( WINDOW_SigMask, win, &windowsignals );
 do{
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) end=TRUE;
  while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
   switch( result & WMHI_CLASSMASK ){
    case WMHI_CLOSEWINDOW: end=TRUE; break;
    case WMHI_RAWKEY:{
     node->key=(result & WMHI_KEYMASK);
     sprintf( str,"%d",node->key );
     end=TRUE;
     break;
    }
   }
  }
 }while( !end );
 DoMethod( win,WM_CLOSE );
 DisposeObject( win );
 SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
 if( str[0]!=0 ){
  node->ascii=FALSE;
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_KEY],Main.IntuiWin,NULL,
                        STRINGA_TextVal,str,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_KEY],Main.IntuiWin,NULL,
                        CHOOSER_Selected,0,TAG_END );
 }
}
//<

//>"void AdjustKeyMode()"
void AdjustKeyMode(){
 CommandNode *com;
 char *str;
 ULONG n;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_KEY],&n );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_KEY],(ULONG*)&str );
 if( (com->ascii&&(n==1))||((!com->ascii)&&(n==0)) ) return; // nothing changed
 if( n==0 ){
  com->key=0;
  com->ascii=FALSE;
 }else{
  com->key=0;
  com->ascii=TRUE;
 }
 SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_KEY],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",TAG_END );
}
//<

//>"void AdjustKey()"
void AdjustKey(){
 CommandNode *com;
 int correction;
 char *str;
 char s[10];
 ULONG n;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_KEY],&n );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_KEY],(ULONG*)&str );
 if( n==0 ){    //
  correction=atoi(str);
  if( correction!=0 ) sprintf( s,"%d",correction );
  else s[0]=0;
  com->key=correction;
 }else{
  com->key=str[0];
  sprintf( s,"%c",com->key );
 }
 SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_KEY],Main.IntuiWin,NULL,
                        STRINGA_TextVal,s,TAG_END );
}
//<

//>"void AdjustComGadgets()"
void AdjustComGadgets(){
 CommandNode *node;
 ULONG code;
 char str[10];

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );
 if( node!=NULL ){
  GetListBrowserNodeAttrs( (Node*)node,LBNA_Generation,(ULONG*)&code,TAG_END );
  if( code==2 ){        // command selected
   SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CNAME],Main.IntuiWin,NULL,
                         STRINGA_TextVal,node->name,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CHELP],Main.IntuiWin,NULL,
                         STRINGA_TextVal,node->help,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CSTACK],Main.IntuiWin,NULL,
     STRINGA_TextVal,node->stack,
     GA_Disabled,(node->wbrun isnot RUN_CLI) and (node->wbrun isnot RUN_NOCLI),TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CDELAY],Main.IntuiWin,NULL,
     STRINGA_TextVal,node->delay,GA_Disabled,node->wbrun isnot RUN_CLI,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_COMMAND],Main.IntuiWin,NULL,
                         STRINGA_TextVal,node->exec,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[GETFILE_ICON],Main.IntuiWin,NULL,
                         GETFILE_FullFile,node->icon,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CMODE],Main.IntuiWin,NULL,
                         CHOOSER_Selected,node->wbrun,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CDIR],Main.IntuiWin,NULL,
                         CHOOSER_Selected,node->CD,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CASYNC],Main.IntuiWin,NULL,
                         GA_Selected,node->async,GA_Disabled,FALSE,TAG_END );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_CASYNC],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CRESOURCE],Main.IntuiWin,NULL,
                         GA_Selected,node->readsrc,GA_Disabled,FALSE,TAG_END );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_CRESOURCE],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CREDEST],Main.IntuiWin,NULL,
                         GA_Selected,node->readdst,GA_Disabled,FALSE,TAG_END );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_CREDEST],Main.IntuiWin,NULL,1 );
   if( node->ascii ){
    str[0]=node->key;
    str[1]=0;
    code=1;
   }else{
    sprintf( str,"%d",node->key );
    code=0;
   }
   SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_KEY],Main.IntuiWin,NULL,
                         STRINGA_TextVal,str,GA_Disabled,FALSE,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_KEY],Main.IntuiWin,NULL,
                         CHOOSER_Selected,code,GA_Disabled,FALSE,TAG_END );
   code=(node->wbrun != RUN_INTERNAL);
   SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_INT],Main.IntuiWin,NULL,
                         GA_Disabled,code,TAG_END );
   SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_PREVIEW],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
   if( !Main.CommandEnabled ){  // buttons that have to be activated
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CLOAD],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CVARIABLES],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_KEY],Main.IntuiWin,NULL,
                         GA_Disabled,FALSE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CUP],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CDOWN],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CREM],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
   }
   Main.CommandEnabled=TRUE;
  }else{        // group selected
   if( Main.CommandEnabled ){   // else we have nothing to do
    Main.CommandEnabled=FALSE;
    SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CNAME],Main.IntuiWin,NULL,
                          STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CHELP],Main.IntuiWin,NULL,
                          STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CSTACK],Main.IntuiWin,NULL,
                          STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CDELAY],Main.IntuiWin,NULL,
                          STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_COMMAND],Main.IntuiWin,NULL,
                          STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[GETFILE_ICON],Main.IntuiWin,NULL,
                          GETFILE_FullFile,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CMODE],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CDIR],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CASYNC],Main.IntuiWin,NULL,
                          GA_Selected,FALSE,GA_Disabled,TRUE,TAG_END );
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CASYNC],Main.IntuiWin,NULL,1 );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CRESOURCE],Main.IntuiWin,NULL,
                          GA_Selected,FALSE,GA_Disabled,TRUE,TAG_END );
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CRESOURCE],Main.IntuiWin,NULL,1 );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CREDEST],Main.IntuiWin,NULL,
                          GA_Selected,FALSE,GA_Disabled,TRUE,TAG_END );
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CREDEST],Main.IntuiWin,NULL,1 );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CLOAD],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_INT],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CVARIABLES],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_KEY],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_KEY],Main.IntuiWin,NULL,
                          STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_KEY],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CUP],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CDOWN],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
    SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CREM],Main.IntuiWin,NULL,
                          GA_Disabled,TRUE,TAG_END );
   }
   SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_PREVIEW],Main.IntuiWin,NULL,
                          GA_Disabled,FALSE,TAG_END );
  }
 }else{ // no node selected
  Main.CommandEnabled=FALSE;
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CNAME],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CHELP],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CSTACK],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_CDELAY],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_COMMAND],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[GETFILE_ICON],Main.IntuiWin,NULL,
                        GETFILE_FullFile,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CMODE],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CDIR],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CASYNC],Main.IntuiWin,NULL,
                        GA_Selected,FALSE,GA_Disabled,TRUE,TAG_END );
  RefreshGList( (Gadget*)Main.Gadgets[CHECK_CASYNC],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CRESOURCE],Main.IntuiWin,NULL,
                        GA_Selected,FALSE,GA_Disabled,TRUE,TAG_END );
  RefreshGList( (Gadget*)Main.Gadgets[CHECK_CRESOURCE],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHECK_CREDEST],Main.IntuiWin,NULL,
                        GA_Selected,FALSE,GA_Disabled,TRUE,TAG_END );
  RefreshGList( (Gadget*)Main.Gadgets[CHECK_CREDEST],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CLOAD],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_INT],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_CVARIABLES],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_KEY],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[STRING_KEY],Main.IntuiWin,NULL,
                        STRINGA_TextVal,"",GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_KEY],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CUP],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_CDOWN],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_PREVIEW],Main.IntuiWin,NULL,
                        GA_Disabled,TRUE,TAG_END );
 }
}
//<

//>"void InsertVar()"
void InsertVar(){
// long a;
 long r;
 char tmp[300];
 char *x;
 CommandNode *com;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_CVARIABLES],(ULONG*)&r );
 if( r == 12 ){
   MenuShowHelp( "LINK vars" );
   return;
 }
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_COMMAND],(ULONG*)&x );
 strcpy(tmp,x);
 strcat(tmp,Main.Ret[r]);
 strncpy( com->exec,tmp,256 );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_COMMAND],Main.IntuiWin, NULL,
                                STRINGA_TextVal, tmp, TAG_DONE );
}
//<

//>"void InsertCom()"
void InsertCom(){
 // long a;
  ULONG r;
  char tmp[300];
  char *comstring,*text;
  List *cl;
  CommandNode *com;
  Node *chnode;

  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&com );
  GetAttr( CHOOSER_Labels,Main.Gadgets[CHOOSER_INT],(ULONG*)&cl );
  GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_INT],(ULONG*)&r );
  GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_COMMAND],(ULONG*)&comstring );

  strcpy(tmp,comstring);
  // find the chooser node:
  chnode=cl->lh_Head;
  for( int i=0;i<r;i++ ) chnode=chnode->ln_Succ;
  // and get the text:
  GetChooserNodeAttrs( chnode,CNA_Text,&text,TAG_END );
  if( text[0] is '.' ) MenuShowHelp( "LINK internalcom" );
  else{
    strcat( tmp,text );
    strncpy( com->exec,tmp,256 );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_COMMAND],Main.IntuiWin, NULL,
      STRINGA_TextVal, tmp, TAG_DONE );
  }
}
//<

//>"void GetIcon()"
void GetIcon(){
 char purefile[256];
 char *str;
 CommandNode *node;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );
 if( !node->icon[0] ){
   SetGadgetAttrs(
     (Gadget*)Main.Gadgets[GETFILE_ICON],Main.IntuiWin,NULL,
     GETFILE_FullFile,"MRE:Icons/",TAG_END
   );
 }
 DoMethod( Main.Gadgets[GETFILE_ICON],GFILE_REQUEST,Main.IntuiWin );
 GetAttr( GETFILE_FullFile,Main.Gadgets[GETFILE_ICON],(ULONG*)&str );
 strcpy( purefile,str );
 if( stricmp(purefile,"MRE:Icons/")==0 ) purefile[0]=0;
 if( strlen(purefile)>5 ){
   str=&purefile[strlen(purefile)-5];
   if( stricmp(str,".info")==0 ) *str=0;
 }
 SetGadgetAttrs( (Gadget*)Main.Gadgets[GETFILE_ICON],Main.IntuiWin,NULL,
                GETFILE_FullFile,purefile,TAG_END );
 strncpy( node->icon,purefile,256 );
}
//<

//>"void Preview()"
void Preview(){
 CommandNode *node;
 Node *ptr;
 DiskObject *dob;
 ULONG gen;
 BOOL end;
 Object *win,*layout,*gadg;
 ULONG windowsignals,mask,code=0,result;
 char path[256],*cp;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );
 if( node==NULL ) return;                       // no icon chosen
 ptr=node->ln_Succ;
 while( (code==0)&&(ptr->ln_Succ!=NULL) ){
  GetListBrowserNodeAttrs( ptr,LBNA_Generation,&gen,TAG_END );
  if( gen==1 ) code++;
  ptr=ptr->ln_Succ;
 }
 if( code==0 ) return;  // we are in the last group, we need no preview
 GetListBrowserNodeAttrs( (Node*)node,LBNA_Generation,&gen,TAG_END );


 if( gen==1 ){
  if( node->ln_Succ->ln_Succ==NULL ) return;    // group without icons
  end=FALSE;
  node=(CommandNode*)node->ln_Succ;   // proceed to first icon in group
 }else end=TRUE;
 SetAttrs( Main.Win,WA_BusyPointer,TRUE,TAG_END );
 // alloc layout and window
 layout=(Object*)NewObject( LAYOUT_GetClass(),NULL,
                        LAYOUT_Orientation,LAYOUT_HORIZONTAL,
                        LAYOUT_DeferLayout,TRUE,
                        TAG_END );
 win=(Object*)NewObject( WINDOW_GetClass(),NULL,
                        WINDOW_Position,WPOS_CENTERMOUSE,
                        WINDOW_Layout,layout,
                        WINDOW_LockHeight,TRUE,
                        WA_IDCMP,IDCMP_CLOSEWINDOW,
                        WA_Activate,TRUE,
                        WA_DragBar,TRUE,
                        WA_SizeGadget,TRUE,
                        WA_RMBTrap,TRUE,
                        WA_CloseGadget,TRUE,
                        TAG_END );
 strcpy( path,Main.CAPath );
 cp=PathPart( path );
 *cp=0;
 BPTR oldlock,lock=Lock (path,ACCESS_READ );
 if( lock ) oldlock=CurrentDir( lock );
 // insert icon(s)
 do{
  GetListBrowserNodeAttrs( (Node*)node,LBNA_Generation,&gen,TAG_END );
  if( gen==1 ) end=TRUE;
  else{
   dob=GetIconTags( node->icon,ICONGETA_Screen,(ULONG)Main.Scr,TAG_END );
   if( node->wbrun==RUN_SPACE ){
    gadg=(Object*)NewObject( SPACE_GetClass(),NULL,TAG_END );
   }else{
    gadg=(Object*)NewObject( Main.IconClass,NULL,GA_UserData,dob,TAG_END );
   }
   SetGadgetAttrs( (Gadget*)layout,NULL,NULL,LAYOUT_AddChild,gadg,TAG_END );
   node=(CommandNode*)node->ln_Succ;
  }
 }while( (node->ln_Succ!=NULL)&&!end );
 if( lock ){ CurrentDir( oldlock );  UnLock( lock ); }

 // open window and wait till closing
 DoMethod( win,WM_OPEN );
 end=FALSE;
 GetAttr( WINDOW_SigMask, win, &windowsignals );
 do{
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) end=TRUE;
  while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
   if( (result & WMHI_CLASSMASK) == WMHI_CLOSEWINDOW ) end=TRUE;
  }
 }while( !end );
 DoMethod( win,WM_CLOSE );
 DisposeObject( win );
 SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
}
//<

//>"void OpenIconWindow()"
void OpenIconWindow(){
  OpenWorkbenchObject( "MRE:Icons",TAG_END );
}
//<

//>"void LoadCom()"
void LoadCom(){
 char file[256];
 char *x;
 char tmp[300];
 CommandNode *node;

 file[0]=0;
 if( !Freq( file,0,GetCatalogStr(Main.Catalog,TXT_CHOOSEFILE,TXT_CHOOSEFILE_STR) ) ) return;

 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_COMMAND],(ULONG*)&x );
 strcpy( tmp,x );
 strcat(tmp,file);
 strncpy( node->exec,tmp,256 );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_COMMAND],Main.IntuiWin, NULL,
                                STRINGA_TextVal, tmp, TAG_DONE );
}
//<

//>"void SaveCommands( TIniFile *IniFile )"
/****************************************/
/*                                      */
/* Function:  SaveCommands              */
/*                                      */
/* Aim:       Save all commands         */
/*            in prefs file             */
/*                                      */
/* Input:     The global inifile        */
/*                                      */
/****************************************/
void SaveCommands( TIniFile *IniFile ){
 CommandNode *node;
 UBYTE mode=0;
 ULONG gen;
 char string[10];

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_COMMANDS],(ULONG*)&node );
 node=(CommandNode*)node->ln_Succ->ln_Succ;    // head of the list == first group

 IniFile->AppendGroup( "Commands" );

 while( node->ln_Succ!=NULL ){
  do{
   GetListBrowserNodeAttrs( (Node*)node,LBNA_Generation,&gen,TAG_END );
   if( gen==1 ){        // group found, so proceed
    node=(CommandNode*)node->ln_Succ;
    mode++;
   }
  }while( node->ln_Succ && (gen==1) );
  if( node->ln_Succ ){
   sprintf( string,"%d",mode );
   IniFile->Append( string );
   IniFile->Append( node->name );
   IniFile->Append( node->help );
   IniFile->Append( node->exec );
   sprintf( string,"M%d%d",node->wbrun,node->CD );
   if( node->async   ) strcat( string,"A" );
   if( node->readsrc ) strcat( string,"S" );
   if( node->readdst ) strcat( string,"D" );
   IniFile->Append( string );
   IniFile->Append( node->stack );
   IniFile->Append( node->delay );
   IniFile->Append( node->icon );
   if( !node->ascii ) sprintf( string,"%d",node->key );
   else sprintf( string,"%c%c%c",'"',node->key,'"' );
   IniFile->Append( string );
   node=(CommandNode*)node->ln_Succ;
 }}
}
//<


// Classes:

//>"void LoadClass( TIniFile *IniFile,char *title,TClass *cla )"
void LoadClass( TIniFile *IniFile,char *title,TClass *cla ){
  Action *act;
  char *entryline,*ppp;
  char pat[102],buffer[512];

  if( title[0] ) IniFile->SeekGroup( title );
  while( (entryline=IniFile->GetNextGroupEntry()) && (strcmp(entryline,EOC)!=0) ){
    act = CreateAction( cla,entryline );
    if( entryline=IniFile->GetNextGroupEntry() ) strncpy( act->Exec,entryline,250 );
    else act->Exec[0]=0;
    AdjustCommandLine( act->Exec,buffer );
    act->Async   = FALSE;
    act->CD      = 0;
    act->ReadSrc = FALSE;
    act->ReadDst = FALSE;
    if( entryline=IniFile->GetNextGroupEntry() ){
      if( entryline[0] != 'M' ){
        act->Type = atoi( entryline );
        if( act->Type>9 ){
          act->Type-=10;
          act->ReadSrc = TRUE;
          act->ReadDst = TRUE;
        }else act->Async=TRUE;
      }else{
        act->Type = entryline[1]-48;
        act->CD = entryline[2]-48;
        if( (act->CD<0) || (act->CD>2) ) act->CD=0;
        if( strchr(entryline,'A') ) act->Async=TRUE;
        if( strchr(entryline,'C') ) act->CD=1;
        if( strchr(entryline,'S') ) act->ReadSrc=TRUE;
        if( strchr(entryline,'D') ) act->ReadDst=TRUE;
      }
    }else{
      act->Type  = RUN_NOCLI;
      act->Async = TRUE;
    }
    if( entryline=IniFile->GetNextGroupEntry() ){
     strncpy( pat,entryline,100 );
     ppp=strchr( pat,',' );
     if( ppp==NULL ){
      strncpy( act->Stack,entryline,8 );
      strcpy( act->Delay,"-1" );
     }else{
      *ppp=0;
      strncpy( act->Stack,pat,8 );
      strncpy( act->Delay,ppp+1,3 );
     }
    }else{ strcpy( act->Stack,"4096" ); strcpy( act->Delay,"-1" ); }
  }
}
//<

//>"void LoadClasses( TIniFile *IniFile )"
/****************************************/
/*                                      */
/* Function: LoadClasses()              */
/*                                      */
/* Aim:      Load the classes from the  */
/*           config file and build up   */
/*           all the needed lists       */
/*                                      */
/* Input:    IniFile=pointer to an      */
/*            inifile, prepared for     */
/*            reading                   */
/*                                      */
/* Output:   TRUE:  all ok              */
/*           FALSE: not all classes     */
/*            found                     */
/*                                      */
/****************************************/
void LoadClasses( TIniFile *IniFile ){
 struct TClass *cla;
 int i;
// int out;
 char *entryline;

 // Read "Open with"
 Main.OpenWith = CreateClass( &Main.Classes,GetCatalogStr(Main.Catalog,MSG_OPENWITH,MSG_OPENWITH_STR) );
 LoadClass( IniFile,"Classopenwith",Main.OpenWith );

 // Read Unknown Class
 cla = CreateClass( &Main.Classes,UNKNOWN );
 LoadClass( IniFile,"Classunknown",cla );

 // Read generic class
 Main.Generic = CreateClass( &Main.Classes,GetCatalogStr(Main.Catalog,TXT_GENERIC,TXT_GENERIC_STR) );
 LoadClass( IniFile,"Classgen",Main.Generic );

 // Reading volume class
 Main.Volume = CreateClass( &Main.Classes,GetCatalogStr(Main.Catalog,MSG_VOLUME,MSG_VOLUME_STR) );
 LoadClass( IniFile,"Classvol",Main.Volume );

 // Reading directoy class
 Main.Directory = CreateClass( &Main.Classes,GetCatalogStr(Main.Catalog,TXT_DIRECTORY,TXT_DIRECTORY_STR) );
 LoadClass( IniFile,"Classdir",Main.Directory );

 // Read classes
 IniFile->SeekGroup( "Classes" );
 while( (entryline=IniFile->GetNextGroupEntry()) and entryline[0] ){
  cla = CreateClass( &Main.Classes,entryline );
  if( Main.PrefsVersion>=46 ){
    if( entryline=IniFile->GetNextGroupEntry() ){
      if( strchr(entryline,'A') ) cla->IsArchive=TRUE;
    }
  }
  if( Main.PrefsVersion>=45 ){
    if( entryline=IniFile->GetNextGroupEntry() ) strncpy( cla->ImageFile,entryline,108 );
  }
  if( entryline=IniFile->GetNextGroupEntry() ){
   if( entryline[0]!=0 ){
    strncpy( cla->Suffix,entryline,208 );
  }}

  for( i=0;i<5;i++ ) if( entryline=IniFile->GetNextGroupEntry() ) strncpy( cla->Value[i],entryline,50 );
//  out=0;
  LoadClass( IniFile,"",cla );

/*
  while( out==0 ){
   if( (entryline=IniFile->GetNextGroupEntry())
       &&(strcmp(entryline,EOC)!=0) ){
    act=(Action *)AllocVec( sizeof(Action),MEMF_CLEAR );
    act->next=NULL;

    if( cla->action==NULL ) cla->action=cla->actiontail=act;
    else {cla->actiontail->next=act;cla->actiontail=act;}
    cla->nbaction++;
    strncpy( act->name,entryline,20 );
    if( entryline=IniFile->GetNextGroupEntry() ) strncpy( act->exec,entryline,250 );
    else act->exec[0]=0;
    AdjustCommandLine( act->exec,buffer );
    act->async   = FALSE;
    act->cddest  = FALSE;
    act->readsrc = FALSE;
    act->readdst = FALSE;
    if( entryline=IniFile->GetNextGroupEntry() ){
      if( entryline[0] != 'M' ){
        act->wbrun = atoi( entryline );
        if( act->wbrun>9 ){
          act->wbrun-=10;
          act->readsrc = TRUE;
          act->readdst = TRUE;
        }else act->async=TRUE;
      }else{
        act->wbrun = entryline[1]-48;
       if( strchr(entryline,'A') ) act->async=TRUE;
       if( strchr(entryline,'C') ) act->cddest=TRUE;
       if( strchr(entryline,'S') ) act->readsrc=TRUE;
       if( strchr(entryline,'D') ) act->readdst=TRUE;
      }
    }else{
      act->wbrun=RUN_NOCLI;
      act->async=TRUE;
    }
    if( entryline=IniFile->GetNextGroupEntry() ){
     strncpy( pat,entryline,100 );
     ppp=strchr( pat,',' );
     if(ppp==NULL){
      strncpy( act->stack,entryline,8 );
      strcpy( act->delay,"-1" );
     }else{
      *ppp=0;
      strncpy( act->stack,pat,8 );
      strncpy( act->delay,ppp+1,3 );
     }
    }else{ strcpy( act->stack,"4096" ); strcpy( act->delay,"-1" ); }
   }else out=1;
  }
*/
 }
 SortList();
}
//<

//>"void InsertNode( List *list,char *text,int mode,int color )"
/****************************************/
/*                                      */
/* Function:  InsertNode                */
/*                                      */
/* Aim:       Create a listbrowser node */
/*            and insert in the list    */
/*                                      */
/* Input:     list=list where to insert */
/*            text=string to show       */
/*            mode=OFFSET_BEGINNING,    */
/*             OFFSET_CURRENT           */
/*             (insert sorted!) or      */
/*             OFFSET_END:              */
/*             where to insert          */
/*            color=color of the text   */
/*             color<3: white,          */
/*             else black               */
/*                                      */
/****************************************/
void InsertNode( struct List *list,char *text,int mode,int color ){
 NamedNode *newnode;
//      make node:
 newnode=(struct NamedNode *)AllocListBrowserNode( 1,LBNA_NodeSize,sizeof(NamedNode),TAG_END );
 strncpy( newnode->Name, text,256 );
 if( color<5 ){
  SetListBrowserNodeAttrs( (struct Node *)newnode,
                           LBNA_Column,0,LBNCA_Text,newnode->Name,
                           LBNA_Flags,LBFLG_CUSTOMPENS,LBNCA_FGPen,2L,TAG_DONE );
 }else{
  SetListBrowserNodeAttrs( (struct Node *)newnode,
                           LBNA_Column,0,LBNCA_Text,newnode->Name,
                           TAG_DONE );
 }
//      insert in List:
 if( mode==OFFSET_CURRENT ) Insert( list, (struct Node*)newnode, GetPreNode(list,text) );
 else if( mode==OFFSET_END ) AddTail( list, (struct Node*)newnode );
 else AddHead( list, (struct Node*)newnode );
}
//<

//>"void ClearActionList()"
/****************************************/
/*                                      */
/* Function: ClearActionList            */
/*                                      */
/* Goal    : Clear the list of actions  */
/*                                      */
/****************************************/

void ClearActionList(){
  List *lblist;
  ULONG code;

  GetAttr( CLICKTAB_Current,Main.Root[CLICKTAB], &code );
  Window *win=(code==2)?Main.IntuiWin:NULL;

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
    LISTBROWSER_Labels, NULL, TAG_DONE );
  FreeListBrowserList( lblist );

  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],win, NULL,
    LISTBROWSER_Labels, lblist, LISTBROWSER_Selected,-1L,TAG_DONE );
  if( Main.ActionEnabled ){
    SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ANAME],win, NULL,
      GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],win, NULL,
      GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_STACK],win, NULL,
      GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_DELAY],win, NULL,
      GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_ASYNC],win, NULL,
      GA_Disabled,TRUE,GA_Selected,FALSE,TAG_DONE );
    if( win ) RefreshGList( (Gadget*)Main.Gadgets[CHECK_ASYNC],win,NULL,1 );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_RESOURCE],win, NULL,
      GA_Disabled,TRUE,GA_Selected,FALSE,TAG_DONE );
    if( win ) RefreshGList( (Gadget*)Main.Gadgets[CHECK_RESOURCE],win,NULL,1 );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_REDEST],win, NULL,
      GA_Disabled,TRUE,GA_Selected,FALSE,TAG_DONE );
    if( win ) RefreshGList( (Gadget*)Main.Gadgets[CHECK_REDEST],win,NULL,1 );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_MODE],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_DIR],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_UP],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_DOWN],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_REMOVE2],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_LOAD],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_AINT],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_AVARIABLES],win, NULL,
      GA_Disabled,TRUE,TAG_DONE );
    Main.ActionEnabled=FALSE;
  }
}
//<

//>"void AppendClass()"
void AppendClass(){
  TClass *cla;
  char str[512];
  List *lblist;

  sprintf(str,GetCatalogStr( Main.Catalog,TXT_NEWCLASS,TXT_NEWCLASS_STR),Main.ClassCount+1);
  cla = CreateClass( &Main.Classes,str );

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                   LISTBROWSER_Labels, NULL, TAG_DONE );
  InsertNode( lblist,cla->Name,OFFSET_END,10 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],Main.IntuiWin, NULL,
                   LISTBROWSER_Labels, lblist,
                   LISTBROWSER_Selected,Main.ClassCount-1,
                   LISTBROWSER_MakeVisible,Main.ClassCount-1,TAG_DONE );
  Main.SelectedClass = cla;
  Lwc();
  ActivateLayoutGadget( (Gadget*)Main.Gadgets[LAYOUT_CLASS],Main.IntuiWin,NULL,
    (ULONG)Main.Gadgets[STRING_CLASS] );
}
//<

//>"ULONG DeleteClass()"
ULONG DeleteClass(){

 Action *act;
 List *lblist;
 Node *node;
 ULONG sel;

 if( Main.SelectedClass==NULL ) return(0);
 GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_CLASSES], (ULONG*)&sel );
 if( sel<5 ){
   Info(GetCatalogStr(Main.Catalog,ERR_DELCLASS,ERR_DELCLASS_STR));
   return(0);
 }
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_CLASSES], (ULONG*)&node );
 if( (ULONG)node==(ULONG)0L ) return(0);

 if( !Req(GetCatalogStr(Main.Catalog,TXT_DELCLASS,TXT_DELCLASS_STR)) ) return(0);

 while( act=(Action*)RemHead(&Main.SelectedClass->Actions) ) FreeVec( act );
 Remove( (Node*)Main.SelectedClass );
 FreeVec( Main.SelectedClass );
 Main.ClassCount --;
 Main.SelectedClass = NULL;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                 LISTBROWSER_Labels, NULL, TAG_DONE );
 Remove( node );
 FreeListBrowserNode( node );

 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],Main.IntuiWin, NULL,
                  LISTBROWSER_Labels,lblist,
                  LISTBROWSER_Selected, -1L,TAG_DONE );
 Lwc();
}
//<

//>"void TcapGui::DuplicateClass()"
void TcapGui::DuplicateClass(){
  if( Main.SelectedClass==NULL ) return;
  TClass *cla;
  Action *act;
  char str[512];
  List *lblist;
  sprintf(str,GetCatalogStr( Main.Catalog,TXT_NEWCLASS,TXT_NEWCLASS_STR),Main.ClassCount+1);
  cla = CreateClass( &Main.Classes,str );
  cla->IsArchive = Main.SelectedClass->IsArchive;
  strcpy( cla->ImageFile,Main.SelectedClass->ImageFile );
  strcpy( cla->Suffix,Main.SelectedClass->Suffix );
  for( int i=0;i<5;i++ ) strcpy( cla->Value[i],Main.SelectedClass->Value[i] );
  for( Action *oldact=(Action*)Main.SelectedClass->Actions.lh_Head;oldact->ln_Succ;oldact=(Action*)oldact->ln_Succ ){
    strcpy( str,oldact->Name );
    act = CreateAction( cla,str );
    strcpy( act->Exec,oldact->Exec );
    strcpy( act->Stack,oldact->Stack );
    strcpy( act->Delay,oldact->Delay );
    act->Type    = oldact->Type;
    act->Async   = oldact->Async;
    act->CD      = oldact->CD;
    act->ReadSrc = oldact->ReadSrc;
    act->ReadDst = oldact->ReadDst;
  }

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                   LISTBROWSER_Labels, NULL, TAG_DONE );
  InsertNode( lblist,cla->Name,OFFSET_END,10 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],Main.IntuiWin, NULL,
                   LISTBROWSER_Labels, lblist,
                   LISTBROWSER_Selected,Main.ClassCount-1,
                   LISTBROWSER_MakeVisible,Main.ClassCount-1,TAG_DONE );
  Main.SelectedClass = cla;
  Lwc();
  ActivateLayoutGadget( (Gadget*)Main.Gadgets[LAYOUT_CLASS],Main.IntuiWin,NULL,
    (ULONG)Main.Gadgets[STRING_CLASS] );
}
//<

//>"void TcapGui::ImportClass()"
void TcapGui::ImportClass(){
  char File[512];
  strcpy( File,"MRE:Config/FileTypes/NewClass.txt" );
  if( Freq(File,0,GetCatalogStr(Main.Catalog,MSG_IMPORTCLASS,MSG_IMPORTCLASS_STR)) ){
    TIniFile IniFile;
    if( IniFile.LoadFromFile(File) && (IniFile.ItemCount()>9) ){
      List *lblist;
      TClass *cla = CreateClass( &Main.Classes,IniFile.Items(0) );
      if( strchr(IniFile.Items(1),'A') ) cla->IsArchive=TRUE;
      strncpy( cla->ImageFile,IniFile.Items(2),108 );
      strncpy( cla->Suffix,IniFile.Items(3),208 );
      for( int i=0;i<5;i++ ) strncpy( cla->Value[i],IniFile.Items(i+4),50 );
      IniFile.SeekLine(8);
      LoadClass( &IniFile,"",cla );

      GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lblist );
      SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                       LISTBROWSER_Labels, NULL, TAG_DONE );
      InsertNode( lblist,cla->Name,OFFSET_END,10 );
      SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],Main.IntuiWin, NULL,
                       LISTBROWSER_Labels, lblist,
                       LISTBROWSER_Selected,Main.ClassCount-1,
                       LISTBROWSER_MakeVisible,Main.ClassCount-1,TAG_DONE );
      Main.SelectedClass = cla;
      Lwc();
    }
  }

}
//<

//>"void TcapGui::ExportClass()"
void TcapGui::ExportClass(){
  if( Main.SelectedClass==NULL ) return;
  char File[512];
  strcpy( File,"MRE:Config/FileTypes/" );
  strcat( File,Main.SelectedClass->Name );
  strcat( File,".txt" );
  if( Freq(File,0,GetCatalogStr(Main.Catalog,MSG_EXPORTCLASS,MSG_EXPORTCLASS_STR)) ){
    TIniFile IniFile;
    WriteClass( &IniFile,"",Main.SelectedClass );
    IniFile.SaveToFile( File );
  }
}
//<

//>"void ListClasses()"
void ListClasses(){
 List *lblist;
 TClass *cla = (TClass*)Main.Classes.lh_Head;
 char str[512];
 int n=0;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                 LISTBROWSER_Labels, NULL, TAG_DONE );
 while( cla->ln_Succ ){
  strcpy(str,cla->Name);
  InsertNode( lblist,cla->Name,OFFSET_END,n );     // list must already be sorted!
  n++;
  cla = (TClass*)cla->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                 LISTBROWSER_Labels, lblist,TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASS],NULL, NULL,
                 GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASSIMAGE],NULL, NULL,
                 GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_LOADIMAGE],NULL, NULL,
                 GA_Disabled,TRUE,TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_MATCH],NULL, NULL,
                 GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],NULL, NULL,
                 GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_OFFSET],NULL, NULL,
                 GA_Disabled,TRUE,TAG_DONE );
 Main.ClassEnabled=FALSE;
 Main.ActionEnabled=TRUE;       // because ClearActionList must change this now
 ClearActionList();
}
//<

//>"void Lwc()"
void Lwc(){
 long a;
 TClass *cla;
 Action *act;
 List *lblist;

 GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_CLASSES], (ULONG*)&a );
 if( a>-1 ){
  cla = (TClass*)Main.Classes.lh_Head;
  for( int i=0;i<a;i++ ) cla = (TClass*)cla->ln_Succ;
  Main.SelectedClass = cla;
 }else Main.SelectedClass = NULL;
 Main.SelectedAction = NULL;

 if( (a<5)&&(Main.ClassEnabled) ){
  SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_REMOVE],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_OFFSET],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  if( SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin, NULL,
      GA_Disabled, TRUE, TAG_DONE ) )
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASSIMAGE],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_LOADIMAGE],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_MATCH],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASS],Main.IntuiWin, NULL,
                  GA_Disabled, TRUE, TAG_DONE );
  Main.ClassEnabled=FALSE;
 }else if( (a>4)&&(Main.ClassEnabled==FALSE) ){
  SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_REMOVE],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_OFFSET],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  if( SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin, NULL,
      GA_Disabled, FALSE, TAG_DONE ) )
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASSIMAGE],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_LOADIMAGE],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_MATCH],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASS],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, TAG_DONE );
  Main.ClassEnabled=TRUE;
 }

 SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_OFFSET],Main.IntuiWin, NULL,
                 CHOOSER_Selected,0, TAG_DONE );
 if( Main.SelectedClass ){
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASS],Main.IntuiWin, NULL,
                  STRINGA_TextVal,cla->Name, TAG_DONE );
  if( SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin, NULL,
      GA_Selected,cla->IsArchive,TAG_DONE ) )
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASSIMAGE],Main.IntuiWin, NULL,
                  STRINGA_TextVal,cla->ImageFile, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],Main.IntuiWin, NULL,
                  STRINGA_TextVal,cla->Value[0], TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_MATCH],Main.IntuiWin, NULL,
                  STRINGA_TextVal,cla->Suffix, TAG_DONE );
 }else{
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASS],Main.IntuiWin, NULL,
                  STRINGA_TextVal,"", TAG_DONE );
  if( SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin, NULL,
      GA_Selected,FALSE,TAG_DONE ) )
    RefreshGList( (Gadget*)Main.Gadgets[CHECK_CARCHIVE],Main.IntuiWin,NULL,1 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASSIMAGE],Main.IntuiWin, NULL,
                  STRINGA_TextVal,"", TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],Main.IntuiWin, NULL,
                  STRINGA_TextVal,"", TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_MATCH],Main.IntuiWin, NULL,
                  STRINGA_TextVal,"", TAG_DONE );
 }

 ClearActionList();

 if( Main.SelectedClass ){
  act = (Action*)Main.SelectedClass->Actions.lh_Head;

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                   LISTBROWSER_Labels, NULL, TAG_DONE );
  while( act->ln_Succ ){
   InsertNode( lblist,act->Name,OFFSET_END,10 );
   act = (Action*)act->ln_Succ;
  }
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist,TAG_DONE );
 }
 Main.ActionsActive = FALSE;
}
//<


//>"ULONG AppendAction()"
ULONG AppendAction(){
 struct Action *act;
 List *lblist;
 char str[512];

 if( Main.SelectedClass==NULL ) return(0);

 sprintf(str,GetCatalogStr( Main.Catalog,TXT_NEWACTION,TXT_NEWACTION_STR),Main.SelectedClass->ActionCount+1);
 act = CreateAction( Main.SelectedClass,str );

 strcpy( act->Exec,NOACT );
 strcpy( act->Stack,"30000" );
 strcpy( act->Delay,"-1" );
 act->Type    = RUN_NOCLI;
 act->Async   = FALSE;
 act->CD      = 0;
 act->ReadSrc = FALSE;
 act->ReadDst = FALSE;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                  LISTBROWSER_Labels, NULL, TAG_DONE );
 InsertNode( lblist,act->Name,OFFSET_END,10 );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist,
                  LISTBROWSER_Selected,Main.SelectedClass->ActionCount-1,
                  LISTBROWSER_MakeVisible,Main.SelectedClass->ActionCount-1,TAG_DONE );
 Main.SelectedAction=act;
 Lwa();
 ActivateLayoutGadget( (Gadget*)Main.Gadgets[LAYOUT_ACTION],Main.IntuiWin,NULL,
   (ULONG)Main.Gadgets[STRING_ANAME] );
}
//<

//>"ULONG DeleteAction()"
ULONG DeleteAction(){
  struct Action *next;
  List *lblist;
  Node *node;
  ULONG sel;

  if( Main.SelectedAction==NULL ) return(0);
  if( !Req(GetCatalogStr(Main.Catalog,TXT_DELACTION,TXT_DELACTION_STR)) ) return(0);

  next = (Action*)Main.SelectedAction->ln_Succ;
  Remove( (Node*)Main.SelectedAction );
  FreeVec( Main.SelectedAction );
  Main.SelectedClass->ActionCount--;

  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_ACTIONS], &sel );

  if( not next->ln_Succ ){
    if( Main.SelectedClass->ActionCount>0 ){
      next = (Action*)Main.SelectedClass->Actions.lh_Tail->ln_Pred;
      sel = sel-1;
    }else{
      Main.ActionEnabled = FALSE;
      sel = -1;
    }
  }
  Main.SelectedAction=next;
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_ACTIONS], (ULONG*)&node );
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                   LISTBROWSER_Labels, NULL, TAG_DONE );
  Remove( node );
  FreeListBrowserNode( node );

  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
    LISTBROWSER_Labels, lblist,
    LISTBROWSER_Selected, sel,TAG_DONE );
  Lwa();
}
//<

//>"void TcapGui::CopyAction()"
void TcapGui::CopyAction(){
  if( Main.SelectedAction==NULL ) return;
  memcpy( &ActionBuffer,Main.SelectedAction,sizeof(Action) );
  ActionCopied = TRUE;
}
//<

//>"void TcapGui::PasteAction()"
void TcapGui::PasteAction(){
  if( !ActionCopied ) return;
  struct Action *act;
  List *lblist;
  char str[512];

  if( Main.SelectedClass==NULL ) return;

  strcpy( str,ActionBuffer.Name );
  act = CreateAction( Main.SelectedClass,str );
  strcpy( act->Exec,ActionBuffer.Exec );
  strcpy( act->Stack,ActionBuffer.Stack );
  strcpy( act->Delay,ActionBuffer.Delay );
  act->Type    = ActionBuffer.Type;
  act->Async   = ActionBuffer.Async;
  act->CD      = ActionBuffer.CD;
  act->ReadSrc = ActionBuffer.ReadSrc;
  act->ReadDst = ActionBuffer.ReadDst;

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                   LISTBROWSER_Labels, NULL, TAG_DONE );
  InsertNode( lblist,act->Name,OFFSET_END,10 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
                   LISTBROWSER_Labels, lblist,
                   LISTBROWSER_Selected,Main.SelectedClass->ActionCount-1,
                   LISTBROWSER_MakeVisible,Main.SelectedClass->ActionCount-1,TAG_DONE );
  Main.SelectedAction=act;
  Lwa();
}
//<

//>"ULONG Sel()"
ULONG Sel(){
 char file[200];
 char *x;
 char tmp[300];

 file[0]=0;
 if( (Main.SelectedAction==NULL)
     || !Freq( file,0,GetCatalogStr(Main.Catalog,TXT_CHOOSEFILE,TXT_CHOOSEFILE_STR) ) ) return(0);

 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_ACOMMAND],(ULONG*)&x );
 strcpy( tmp,x );
 strcat(tmp,file);
 strncpy( Main.SelectedAction->Exec,tmp,250 );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],Main.IntuiWin, NULL,
                                STRINGA_TextVal, tmp, TAG_DONE );
}
//<

//>"ULONG Lwa()"
ULONG Lwa(){
  long a;
  Action *act;
  ULONG i;

  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_ACTIONS], (ULONG*)&a );
  if( a is -1 ){
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_MODE],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_DIR],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ANAME],Main.IntuiWin, NULL,
                   STRINGA_TextVal,"",GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],Main.IntuiWin, NULL,
                   STRINGA_TextVal,"",GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_STACK],Main.IntuiWin, NULL,
                   STRINGA_TextVal,"",GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_DELAY],Main.IntuiWin, NULL,
                   STRINGA_TextVal,"",GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_ASYNC],Main.IntuiWin, NULL,
                   GA_Selected,FALSE,GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_RESOURCE],Main.IntuiWin, NULL,
                   GA_Selected,FALSE,GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_REDEST],Main.IntuiWin, NULL,
                   GA_Selected,FALSE,GA_Disabled,TRUE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_UP],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_DOWN],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_REMOVE2],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_LOAD],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_AINT],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_AVARIABLES],Main.IntuiWin, NULL,
                   GA_Disabled,TRUE,TAG_DONE );
   return(0);
  }
  act = (Action*)Main.SelectedClass->Actions.lh_Head;
  for( i=0;i<a;i++ ) act = (Action*)act->ln_Succ;
  Main.SelectedAction = act;

  if( Main.ActionEnabled ){
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_ASYNC],Main.IntuiWin, NULL,
                   GA_Selected,act->Async, TAG_DONE );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_ASYNC],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_RESOURCE],Main.IntuiWin, NULL,
                   GA_Selected,act->ReadSrc, TAG_DONE );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_RESOURCE],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_REDEST],Main.IntuiWin, NULL,
                   GA_Selected,act->ReadDst, TAG_DONE );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_REDEST],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_MODE],Main.IntuiWin, NULL,
                   CHOOSER_Selected,act->Type, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_DIR],Main.IntuiWin, NULL,
                   CHOOSER_Selected,act->CD,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ANAME],Main.IntuiWin, NULL,
                   STRINGA_TextVal,act->Name, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],Main.IntuiWin, NULL,
                   STRINGA_TextVal,act->Exec, TAG_DONE );
  }else{
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_ASYNC],Main.IntuiWin, NULL,
                   GA_Selected,act->Async,GA_Disabled,FALSE, TAG_DONE );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_ASYNC],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_RESOURCE],Main.IntuiWin, NULL,
                   GA_Selected,act->ReadSrc,GA_Disabled,FALSE, TAG_DONE );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_RESOURCE],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHECK_REDEST],Main.IntuiWin, NULL,
                   GA_Selected,act->ReadDst,GA_Disabled,FALSE, TAG_DONE );
   RefreshGList( (Gadget*)Main.Gadgets[CHECK_REDEST],Main.IntuiWin,NULL,1 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_MODE],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,CHOOSER_Selected,act->Type, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_DIR],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,CHOOSER_Selected,act->CD,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ANAME],Main.IntuiWin, NULL,
                   STRINGA_TextVal,act->Name,GA_Disabled,FALSE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],Main.IntuiWin, NULL,
                   STRINGA_TextVal,act->Exec,GA_Disabled,FALSE, TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_UP],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_DOWN],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_REMOVE2],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[BUTTON_LOAD],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,TAG_DONE );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_AVARIABLES],Main.IntuiWin, NULL,
                   GA_Disabled,FALSE,TAG_DONE );
  }
  SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_AINT],Main.IntuiWin,NULL,
    GA_Disabled,act->Type isnot RUN_INTERNAL,TAG_END );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_DELAY],Main.IntuiWin, NULL,
    GA_Disabled,act->Type isnot RUN_CLI,STRINGA_TextVal,act->Delay, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_STACK],Main.IntuiWin, NULL,
    GA_Disabled,(act->Type isnot RUN_CLI) and (act->Type isnot RUN_NOCLI),
    STRINGA_TextVal,act->Stack, TAG_DONE );
  Main.ActionEnabled = TRUE;
  Main.ActionsActive = TRUE;
}
//<


//>"ULONG StNoma()"
ULONG StNoma(){
 char *str;
 List *lblist;
 Node *node;

 if( Main.SelectedAction==NULL ) return(0);

 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_ANAME], (ULONG*)&str );

 strncpy( Main.SelectedAction->Name,str,32 );

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_ACTIONS], (ULONG*)&node );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                  LISTBROWSER_Labels, NULL, TAG_DONE );
 strncpy( ((NamedNode*)node)->Name,str,20 );
 SetListBrowserNodeAttrs( node,LBNA_Column,0,LBNCA_Text,((NamedNode*)node)->Name,TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist,TAG_DONE );
}
//<

//>"ULONG StNomc()"
ULONG StNomc(){
 char *x;
 Node *node;
 List *lblist;

 if( Main.SelectedClass==NULL ) return(0);
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_CLASS], (ULONG*)&x );
 strncpy( Main.SelectedClass->Name,x,32 );
 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_CLASSES], (ULONG*)&lblist );
 GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_CLASSES], (ULONG*)&node );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],NULL, NULL,
                  LISTBROWSER_Labels, NULL, TAG_DONE );
 strncpy( ((NamedNode*)node)->Name,x,32 );
 SetListBrowserNodeAttrs( node,LBNA_Column,0,LBNCA_Text,((NamedNode*)node)->Name,TAG_DONE );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_CLASSES],Main.IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist,TAG_DONE );
}
//<

//>"void ImageFileOnChange()"
void ImageFileOnChange(){
  char *sTemp;
  if( Main.SelectedClass==NULL ) return;
  GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_CLASSIMAGE], (ULONG*)&sTemp );
  strncpy( Main.SelectedClass->ImageFile,sTemp,108 );
}
//<

//>"void ImageFileRequest()"
void ImageFileRequest(){
  if( Main.SelectedClass==NULL ) return;
  char sTemp[512];
  strcpy( sTemp,"MRE:Images/FileTypes/Size16/ext/" );
  strcat( sTemp,Main.SelectedClass->ImageFile );
  if( !Freq(sTemp,0,GetCatalogStr(Main.Catalog,TXT_CHOOSEFILE,TXT_CHOOSEFILE_STR)) ) return;
  strncpy( Main.SelectedClass->ImageFile,FilePart(sTemp),108 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_CLASSIMAGE],Main.IntuiWin, NULL,
    STRINGA_TextVal, FilePart(sTemp), TAG_DONE );
}
//<

//>"void CheckArchiveOnClick()"
void CheckArchiveOnClick(){
  ULONG sel;
  if( Main.SelectedClass==NULL ) return;
  GetAttr( GA_Selected,Main.Gadgets[CHECK_CARCHIVE],&sel );
  Main.SelectedClass->IsArchive = sel;
}
//<

//>"ULONG StFiltre()"
ULONG StFiltre(){
  char *x;
  if( Main.SelectedClass==NULL ) return(0);
  GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_MATCH], (ULONG*)&x );
  strncpy( Main.SelectedClass->Suffix,x,100 );
}
//<

//>"ULONG StOffset()"
ULONG StOffset(){
 char *x;
 long a;

 if( Main.SelectedClass==NULL ) return(0);
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_OFFSET], (ULONG*)&a );
 if( a>-1 ){
  GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_OFFSET], (ULONG*)&x );
  strncpy( Main.SelectedClass->Value[a],x,50 );
}}
//<

//>"ULONG StPile()"
ULONG StPile(){
 char *x;

 if( Main.SelectedAction==NULL ) return(0);
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_STACK], (ULONG*)&x );
 strncpy( Main.SelectedAction->Stack,x,8 );
}
//<

//>"ULONG StDelay()"
ULONG StDelay(){
 char *x;

 if( Main.SelectedAction==NULL ) return(0);
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_DELAY], (ULONG*)&x );
 strncpy( Main.SelectedAction->Delay,x,3 );
}
//<

//>"ULONG StExec()"
ULONG StExec(){
 char *x;

 if(  Main.SelectedAction==NULL ) return(0);
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_ACOMMAND], (ULONG*)&x );
 strncpy( Main.SelectedAction->Exec,x,256 );
}
//<

//>"ULONG CyMode()"
ULONG CyMode(){
 long a;
 struct Action *act;
 ULONG ninternal;

 if( Main.SelectedAction==NULL ) return(0);

 act=Main.SelectedAction;

 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_MODE], (ULONG*)&a );
 if( a != RUN_CLI ){
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_DELAY],Main.IntuiWin, NULL,
                  GA_Disabled,TRUE,STRINGA_TextVal,"",TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_STACK],Main.IntuiWin, NULL,
                  GA_Disabled,TRUE,STRINGA_TextVal,"", TAG_DONE );
 }else{
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_DELAY],Main.IntuiWin, NULL,
                  GA_Disabled,FALSE,STRINGA_TextVal,act->Delay, TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_STACK],Main.IntuiWin, NULL,
                  GA_Disabled,FALSE,STRINGA_TextVal,act->Stack, TAG_DONE );
 }
 ninternal= ( a != RUN_INTERNAL );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[CHOOSER_AINT],Main.IntuiWin,NULL,
                       GA_Disabled,ninternal,TAG_END );

 Main.SelectedAction->Type = (char)a;
}
//<

//>"void  AdjustCD()"
void  AdjustCD(){
  if( !Main.SelectedAction ) return;

  struct Action *act=Main.SelectedAction;
  ULONG code;

  GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_DIR],&code );
  act->CD=code;
}
//<


//>"ULONG CyOffset()"
ULONG CyOffset(){
 long a;

 if(  Main.SelectedClass==NULL ) return(0);
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_OFFSET], (ULONG*)&a );
 if( a>-1 ){
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],Main.IntuiWin, NULL,
                 STRINGA_TextVal,Main.SelectedClass->Value[a],TAG_DONE );
  if( Main.IntuiWin ) RefreshGList( (Gadget*)Main.Gadgets[STRING_OFFSET],Main.IntuiWin,NULL,1 );
}}
//<

//>"ULONG ChRescan()"
ULONG ChRescan(){
  ULONG a;
  if( Main.SelectedAction==NULL ) return(0);

  GetAttr( GA_Selected,Main.Gadgets[CHECK_ASYNC],&a );
  Main.SelectedAction->Async   = a;
  GetAttr( GA_Selected,Main.Gadgets[CHECK_RESOURCE], (ULONG*)&a );
  Main.SelectedAction->ReadSrc = a;
  GetAttr( GA_Selected,Main.Gadgets[CHECK_REDEST], (ULONG*)&a );
  Main.SelectedAction->ReadDst = a;
}
//<

//>"ULONG Up()"
/****************************************/
/*                                      */
/* Function: Up                         */
/*                                      */
/* Aim:      Move an action up          */
/*                                      */
/****************************************/
ULONG Up(){
  List *lblist;
  Node *node,*prenode;
  ULONG a;

  if( Main.SelectedAction==NULL ) return(0);
  if( Main.SelectedAction==(Action*)Main.SelectedClass->Actions.lh_Head ) return(0);

  // first move the action in our list:
  prenode = Main.SelectedAction->ln_Pred->ln_Pred;
  Remove( (Node*)Main.SelectedAction );
  Insert( &Main.SelectedClass->Actions,(Node*)Main.SelectedAction,prenode );

  // now move the node in the listbrowser:
  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_ACTIONS], &a );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_ACTIONS], (ULONG*)&node );
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                  LISTBROWSER_Labels,NULL,TAG_DONE );
  prenode=node->ln_Pred;    // prenode!=NULL because this is not the head ...
  if( node->ln_Pred!=NULL ){
    Remove( node );
    Insert( lblist, node, prenode->ln_Pred );
  }
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
    LISTBROWSER_Labels,lblist,
    LISTBROWSER_Selected,a-1,TAG_DONE );
}
//<

//>"ULONG Down()"
/****************************************/
/*                                      */
/* Function: Down                       */
/*                                      */
/* Aim:      Move an action down        */
/*                                      */
/****************************************/
ULONG Down(){
  List *lblist;
  Node *node,*succnode;
  ULONG a;

  if( Main.SelectedAction==NULL ) return(0);
  if( Main.SelectedAction==(Action*)Main.SelectedClass->Actions.lh_Tail->ln_Pred ) return(0);

  // first move the action in our list:
  succnode = Main.SelectedAction->ln_Succ;
  Remove( (Node*)Main.SelectedAction );
  Insert( &Main.SelectedClass->Actions,(Node*)Main.SelectedAction,succnode );

  // now move the node in the listbrowser:
  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_ACTIONS], &a );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_ACTIONS], (ULONG*)&node );
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_ACTIONS], (ULONG*)&lblist );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],NULL, NULL,
                  LISTBROWSER_Labels,NULL,TAG_DONE );
  succnode=node->ln_Succ;    // prenode!=NULL because this is not the tail ...
  if( succnode->ln_Succ!=NULL ){
    Remove( node );
    Insert( lblist, node, succnode );
  }
  SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_ACTIONS],Main.IntuiWin, NULL,
    LISTBROWSER_Labels,lblist,
    LISTBROWSER_Selected,a+1,TAG_DONE );
}
//<

//>"void InsertComA()"
void InsertComA(){
// long a;
 ULONG r;
 char tmp[300];
 char *comstring,*text;
 List *cl;
 Node *chnode;

 if( Main.SelectedAction==NULL ) return;

 GetAttr( CHOOSER_Labels,Main.Gadgets[CHOOSER_AINT],(ULONG*)&cl );
 GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_AINT],(ULONG*)&r );
 GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_ACOMMAND],(ULONG*)&comstring );

 strcpy(tmp,comstring);
 // find the chooser node:
 chnode=cl->lh_Head;
 for( int i=0;i<r;i++ ){
   chnode=chnode->ln_Succ;
 }
 // and get the text:
 GetChooserNodeAttrs( chnode,CNA_Text,&text,TAG_END );
 if( text[0] is '.' ) MenuShowHelp( "LINK internalcom" );
 else{
   strcat( tmp,text );
   strncpy( Main.SelectedAction->Exec,tmp,256 );
   SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],Main.IntuiWin, NULL,
     STRINGA_TextVal, tmp, TAG_DONE );
 }
}
//<

//>"ULONG LwCom()"
ULONG LwCom(){
  long r;
  char tmp[300];
  char *x;

  if( Main.SelectedAction==NULL ) return(0L);

  GetAttr( CHOOSER_Selected,Main.Gadgets[CHOOSER_AVARIABLES],(ULONG*)&r );
  if( r == 12 ){
    MenuShowHelp( "LINK vars" );
    return(0);
  }
  GetAttr( STRINGA_TextVal,Main.Gadgets[STRING_ACOMMAND],(ULONG*)&x );
  strcpy(tmp,x);
  strcat(tmp,Main.Ret[r]);
  strncpy( Main.SelectedAction->Exec,tmp,256 );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_ACOMMAND],Main.IntuiWin, NULL,
    STRINGA_TextVal, tmp, TAG_DONE );
}
//<

//>"void MoveClassLister( ULONG key )"
void MoveClassLister( ULONG key ){
 CommandNode *com;
 Node *node;
 List *LBL;
 ULONG flags;
 BOOL error=FALSE;
 Object *lb;

 if( (key!=CURSORUP)&&(key!=CURSORDOWN) ) return;

 if( Main.ActionsActive ) lb = Main.Gadgets[LIST_ACTIONS];
 else lb = Main.Gadgets[LIST_CLASSES];

 GetAttr( LISTBROWSER_SelectedNode,lb,(ULONG*)&com );
 if( com==NULL ) return;
 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&LBL );
 node=(Node*)com;
 switch( key ){
  case CURSORUP:{
   do{
    node=node->ln_Pred;
    if( node->ln_Pred ) GetListBrowserNodeAttrs( node,LBNA_Flags,&flags,TAG_END );
    else error=TRUE;
   }while( !error && (flags & LBFLG_HIDDEN) );
   break;
  }
  case CURSORDOWN:{
   do{
    node=node->ln_Succ;
    if( node->ln_Succ ) GetListBrowserNodeAttrs( node,LBNA_Flags,&flags,TAG_END );
    else error=TRUE;
   }while( !error && (flags & LBFLG_HIDDEN) );
   break;
  }
 }
 if( !error ){
  SetGadgetAttrs( (Gadget*)lb,Main.IntuiWin,NULL,
                         LISTBROWSER_SelectedNode,node,
                         TAG_END );
  GetAttr( LISTBROWSER_Selected,lb,(ULONG*)&flags );
  SetGadgetAttrs( (Gadget*)lb,Main.IntuiWin,NULL,
                        LISTBROWSER_MakeVisible,flags,TAG_END );
  if( Main.ActionsActive ) Lwa();
  else Lwc();
 }
}
//<


// Path list:

//>"void LoadPaths( TIniFile *IniFile )"
void LoadPaths( TIniFile *IniFile ){
 TPathNode *newnode,*pred;
 List *LBL;
 BOOL end=FALSE;
 char *ptr,*ptr2,*entryline;
 char Buffer[256];

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&LBL );
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],NULL,NULL,
                        LISTBROWSER_Labels,NULL,TAG_END );
 pred=(TPathNode*)LBL->lh_Head;

 end=!IniFile->SeekGroup( "PathList" );

 if( end ){
   end=!IniFile->SeekGroup( "Pathmenu" );
   while (!end ){
     entryline=IniFile->GetNextGroupEntry();
     if( entryline && entryline[0] ){
       strcpy( Buffer,entryline );
       ptr  = strchr( Buffer,'=' );
       ptr2 = strchr( Buffer,',' );
       if( ptr and ptr2 ){
         newnode = (TPathNode*)AllocListBrowserNode(2,LBNA_NodeSize,sizeof(TPathNode),TAG_END);
         *ptr2 = 0;
         ptr++; ptr2++;
         strcpy( newnode->Name, ptr );
         strcpy( newnode->Path, ptr2 );
         // assign name to node:
         SetListBrowserNodeAttrs( (Node*)newnode,LBNA_Column,0,LBNCA_Text,newnode->Name,
           LBNA_Column,1,LBNCA_Text,newnode->Path, TAG_DONE );
         //      insert in List:
         AddTail( LBL, (Node*)newnode );
       }else end=TRUE;
     }else end=TRUE;
   }
 }else{
   while (!end ){
     entryline=IniFile->GetNextGroupEntry();
     if( entryline && entryline[0] ){
      newnode = (TPathNode*)AllocListBrowserNode(2,LBNA_NodeSize,sizeof(TPathNode),TAG_END);
      strcpy( newnode->Name, entryline );
      if( entryline=IniFile->GetNextGroupEntry() ) strcpy( newnode->Path, entryline );
      else newnode->Path[0]=0;
      // assign name to node:
      SetListBrowserNodeAttrs( (Node*)newnode,LBNA_Column,0,LBNCA_Text,newnode->Name,
        LBNA_Column,1,LBNCA_Text,newnode->Path, TAG_DONE );
      //      insert in List:
      AddTail( LBL, (Node*)newnode );
     }else end=TRUE;
   }
 }
 SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],NULL,NULL,
                        LISTBROWSER_Labels,LBL,TAG_END );
}
//<

//>"void FreePathList()"
void FreePathList(){
 List *lbl;

 GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS], (ULONG*)&lbl );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_PATHS],NULL, NULL,
                 LISTBROWSER_Labels, NULL, TAG_DONE );
 FreeListBrowserList( lbl );
 SetGadgetAttrs( (Gadget *)Main.Gadgets[LIST_PATHS],NULL, NULL,
                 LISTBROWSER_Labels, lbl, TAG_DONE );
}
//<

//>"void AdjustPathGadgets()"
void AdjustPathGadgets(){
  ULONG node;
  BOOL sel;

  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_PATHS],&node );
  sel = (node is NULL);
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_PUP],Main.IntuiWin,NULL,
    GA_Disabled,sel,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_PDOWN],Main.IntuiWin,NULL,
    GA_Disabled,sel,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[BUTTON_PREM],Main.IntuiWin,NULL,
    GA_Disabled,sel,TAG_END );
}
//<

//>"void AddPath( char *Path )"
void AddPath( char *Path ){
  List *LBL;
  TPathNode *newcom;
  Node *selnode;
  ULONG nodes,newsel;
  char Name[256],RealPath[256];
  if( Path ) strcpy( RealPath,Path );
  else strcpy(RealPath,"SYS:");

  if( Path or (Freq(RealPath,1,GetCatalogStr(Main.Catalog,MSG_ENTERPATH,MSG_ENTERPATH_STR))) ){
    if( RealPath[strlen(RealPath)-1] isnot ':' ) strcpy( Name,FilePart(RealPath) );
    else strcpy( Name,RealPath );
    if( Path or GetText(Name,255,GetCatalogStr(Main.Catalog,MSG_ENTERPATHTITLE,MSG_ENTERPATHTITLE_STR)) ){
      GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_PATHS],&newsel );
      GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_PATHS],(ULONG*)&selnode );
      GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&LBL );
      GetAttr( LISTBROWSER_TotalNodes,Main.Gadgets[LIST_PATHS],(ULONG*)&nodes );
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],NULL,NULL,
        LISTBROWSER_Labels,NULL,TAG_END );
      newcom = (TPathNode*)AllocListBrowserNode( 2,LBNA_NodeSize,sizeof(TPathNode),TAG_END );
      strcpy( newcom->Name,Name );
      strcpy( newcom->Path,RealPath );
      SetListBrowserNodeAttrs( (Node*)newcom,
        LBNA_Column,0,LBNCA_Text,newcom->Name,LBNA_Column,1,LBNCA_Text,newcom->Path,TAG_END );
      if( selnode ){
       Insert( LBL,(Node*)newcom,selnode );
       newsel++;
      }else{
       AddTail( LBL,(Node*)newcom );
       newsel=nodes+1;
      }
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
        LISTBROWSER_Labels,LBL,LISTBROWSER_SelectedNode,newcom,TAG_END );
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
        LISTBROWSER_MakeVisible,newsel,TAG_END );
      AdjustPathGadgets();
    }
  }
}
//<

//>"void RemovePath()"
void RemovePath(){
  List *LBL;
  TPathNode *com;

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&LBL );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_PATHS],(ULONG*)&com );
  if( com==NULL ) return;
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],NULL,NULL,
    LISTBROWSER_Labels,NULL,TAG_END );
  Remove( (Node*)com );
  FreeListBrowserNode( (Node*)com );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_Labels,LBL,LISTBROWSER_Selected,-1L,TAG_END );
  AdjustPathGadgets();
}
//<

//>"void PathListDblClick()"
void PathListDblClick(){
  ULONG col;
  TPathNode *nd;
  char Name[256],Path[256];
  List *LBL;

  GetAttr( LISTBROWSER_RelColumn,Main.Gadgets[LIST_PATHS],&col );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_PATHS],(ULONG*)&nd );
  if( nd is NULL ) return;
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&LBL );

  if( col is 0 ){
    strcpy( Name,nd->Name );
    if( GetText(Name,255,GetCatalogStr(Main.Catalog,MSG_ENTERPATHTITLE,MSG_ENTERPATHTITLE_STR)) ){
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
        LISTBROWSER_Labels,NULL,TAG_END
      );
      strcpy( nd->Name,Name );
      SetListBrowserNodeAttrs( (Node*)nd,LBNA_Column,0,LBNCA_Text,nd->Name,TAG_END );
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
        LISTBROWSER_Labels,LBL,TAG_END
      );
    }
  }else{
    strcpy( Path,nd->Path );
    AdjustPath( Path );
    if( Freq(Path,1,GetCatalogStr(Main.Catalog,MSG_ENTERPATH,MSG_ENTERPATH_STR)) ){
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
        LISTBROWSER_Labels,NULL,TAG_END
      );
      strcpy( nd->Path,Path );
      SetListBrowserNodeAttrs( (Node*)nd,LBNA_Column,1,LBNCA_Text,nd->Path,TAG_END );
      SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
        LISTBROWSER_Labels,LBL,TAG_END
      );
    }
  }
}
//<

//>"void MovePathUp()"
void MovePathUp(){
  List *LBL;
  TPathNode *com;
  Node *pre;
  ULONG sel;

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&LBL );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_PATHS],(ULONG*)&com );
  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_PATHS],(ULONG*)&sel );
  if( com is NULL ) return;        // no node selected!

  pre=com->ln_Pred->ln_Pred;
  if( pre==NULL ) return;       // we are on the top!

  // now deattach, move node and attach again:
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_Labels,NULL,TAG_END );

  Remove( (Node*)com );
  Insert( LBL,(Node*)com,pre );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_Labels,LBL,LISTBROWSER_Selected,sel-1,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_MakeVisible,sel-1,TAG_END );
}
//<

//>"void MovePathDown()"
void MovePathDown(){
  List *LBL;
  TPathNode *com;
  Node *succ;
  ULONG sel;

  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&LBL );
  GetAttr( LISTBROWSER_SelectedNode,Main.Gadgets[LIST_PATHS],(ULONG*)&com );
  GetAttr( LISTBROWSER_Selected,Main.Gadgets[LIST_PATHS],(ULONG*)&sel );
  if( com==NULL ) return;        // no node selected!

  succ=com->ln_Succ;
  if( succ->ln_Succ==NULL ) return;       // we are on the bottom!

  // now deattach, move node and attach again:
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_Labels,NULL,TAG_END );

  Remove( (Node*)com );
  Insert( LBL,(Node*)com,succ );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_Labels,LBL,LISTBROWSER_Selected,sel+1,TAG_END );
  SetGadgetAttrs( (Gadget*)Main.Gadgets[LIST_PATHS],Main.IntuiWin,NULL,
    LISTBROWSER_MakeVisible,sel+1,TAG_END );
}
//<

//>"void SavePaths( TIniFile *IniFile )"
/****************************************/
/*                                      */
/* Function:  SavePaths                 */
/*                                      */
/* Aim:       Save all paths            */
/*            in prefs file             */
/*                                      */
/* Input:     The global inifile        */
/*                                      */
/****************************************/
void SavePaths( TIniFile *IniFile ){
  TPathNode *node;
  GetAttr( LISTBROWSER_Labels,Main.Gadgets[LIST_PATHS],(ULONG*)&node );
  node=(TPathNode*)node->ln_Succ;
  IniFile->AppendGroup( "PathList" );
  while( node->ln_Succ!=NULL ){
    IniFile->Append( node->Name );
    IniFile->Append( node->Path );
    node=(TPathNode*)node->ln_Succ;
  }
}
//<


// General:

//>"void Save()"
/****************************************/
/*                                      */
/* Function: Save                       */
/*                                      */
/* Aim:      Save changes and exit      */
/*                                      */
/****************************************/
void Save(){
  SetAttrs( Main.Win,WA_BusyPointer,TRUE,TAG_END );
  if( capGui.SavePrefs( TPREFSFILE ) and capGui.SavePrefs( PREFSFILE ) ){
    Main.Leave=TRUE;
    SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
    NotifyMaster();
  }
}
//<

//>"void Use()"
/****************************************/
/*                                      */
/* Function: Use                        */
/*                                      */
/* Aim:      Use the changes and exit   */
/*                                      */
/****************************************/
void Use(){
  TIniFile IniFile;
  if( capGui.SavePrefs( TPREFSFILE ) ){
    Main.Leave=TRUE;
    NotifyMaster();
  }
}
//<

//>"void Quit()"
/****************************************/
/*                                      */
/* Function: Quit                       */
/*                                      */
/* Aim:      quit ?!                    */
/*                                      */
/****************************************/
void Quit(){
 Main.Leave=TRUE;
}
//<


// Menu:

//>"void LoadPrefs( BOOL refresh )"
void LoadPrefs( BOOL refresh ){
  if( Main.IntuiWin ) SetAttrs( Main.Win,WA_BusyPointer,TRUE,TAG_END );
 // first reset all lists:
  FreeClassList();
  FreePathList();
  OptionList.Reset();
  ResetCommands();
 // then load the file:
  TIniFile IniFile;
  if( !IniFile.LoadFromFile( Main.PrefsFile ) )
    if( !IniFile.LoadFromFile( TPREFSFILE ) )
      if( !IniFile.LoadFromFile( PREFSFILE ) )
        if( !IniFile.LoadFromFile( OLDPREFSFILE ) )
          IniFile.LoadFromFile( DPREFSFILE );
// AddGenDirList();
  Main.PrefsVersion = IniFile.GetInt("INFO","VERSION",0);
  LoadClasses( &IniFile );
  LoadOptions( &IniFile );
  LoadPaths( &IniFile );
  LoadCommands( &IniFile );
  ListClasses();
  if( refresh ){
    // and refresh the window:
    RethinkLayout( (Gadget*)Main.Root[LAYOUT_ROOT],Main.IntuiWin,NULL,TRUE );
  }
  if( Main.IntuiWin ) SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
}
//<

//>"BOOL TcapGui::SavePrefs( char *FileName )"
BOOL TcapGui::SavePrefs( char *FileName ){
  TIniFile IniFile;
  IniFile.WriteInt("INFO","VERSION",PREFSVERS);
  SaveOptions( &IniFile );
  SavePaths( &IniFile );
  SaveCommands( &IniFile );
  SaveClasses( &IniFile );
  if( IniFile.SaveToFile(FileName) ) return TRUE;
  else{
    Info( GetCatalogStr(Main.Catalog,ERR_SAVEPREFS,ERR_SAVEPREFS_STR) );
    return FALSE;
  }
}
//<

//>"void MenuOpen()"
void MenuOpen(){
 char File[512];

 strcpy( File,TPREFSFILE );
 if( Freq( File,0,GetCatalogStr( Main.Catalog,TXT_OPENPREFS,TXT_OPENPREFS_STR ) ) ){
  strcpy( Main.PrefsFile,File );
  LoadPrefs( TRUE );
 }
}
//<

//>"void MenuSaveAs()"
void MenuSaveAs(){
  ULONG code;
  char File[512];

  GetAttr( CLICKTAB_Current,Main.Root[CLICKTAB], &code );
  strcpy( File,PREFSFILE );
  if( Freq( File,0,GetCatalogStr( Main.Catalog,TXT_SAVEPREFS,TXT_SAVEPREFS_STR ) ) ){
    strcpy( Main.PrefsFile,File );
    capGui.SavePrefs( Main.PrefsFile );
  }
}
//<

//>"void MenuReset()"
void MenuReset(){
  strcpy( Main.PrefsFile,DPREFSFILE );
  LoadPrefs( TRUE );
}
//<

//>"void MenuLastSaved()"
void MenuLastSaved(){
  strcpy( Main.PrefsFile,PREFSFILE );
  LoadPrefs( TRUE );
}
//<

//>"void MenuRestore( BOOL refresh )"
void MenuRestore( BOOL refresh ){
  strcpy( Main.PrefsFile,TPREFSFILE );
  LoadPrefs( refresh );
}
//<

//>"void MenuOldPrefs( BOOL refresh )"
/*
void MenuOldPrefs( BOOL refresh ){
 if( Main.IntuiWin ) SetAttrs( Main.Win,WA_BusyPointer,TRUE,TAG_END );
// first reset the lists:
 FreeClassList();
// then load the file:
 if( !LoadMem() ){
  Info( GetCatalogStr(Main.Catalog,ERR_NOCONFIG,ERR_NOCONFIG_STR) );
  if( Main.IntuiWin ) SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
  return;
 }
 SortList();
 if( !LoadGen() ){
  Info( GetCatalogStr(Main.Catalog,ERR_NOCONFIG,ERR_NOCONFIG_STR) );
  if( Main.IntuiWin ) SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
  return;
 }
 AddGenDirList();
 ListClasses();
 LoadAndDeleteTooltypes();
 if( refresh ){
  // and refresh the window:
  RethinkLayout( (Gadget*)Main.Root[LAYOUT_ROOT],Main.IntuiWin,NULL,TRUE );
 }
 if( Main.IntuiWin ) SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
}
*/
//<

