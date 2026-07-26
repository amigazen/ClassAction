#include "ca_begin.h"
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                    & Salim Gasmi

        File : cadisk.c

        Goal : Load/Save Functions

        Ver  : 4.2

****************************************/


#include <classes/window.h>
#include <dos/datetime.h>
#include <gadgets/button.h>
#include <gadgets/layout.h>
#include <images/label.h>
#include <proto/button.h>
#include <proto/checkbox.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/locale.h>
#include <proto/space.h>
#include <proto/string.h>
#include <proto/window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caexec.h"
#include "caglobal.h"
#include "capaths.h"
#include "calha.h"
#include "careq.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classactioncd.h"

#include "cadisk.h"

//>"void ReadClassEntry( TIniFile *IniFile, TFileType *ft )"
void ReadClassEntry( TIniFile *IniFile, TFileType *ft ){
 TAction *act;
 char *entryline,*ppp;
 char pat[102];

 while( (entryline=IniFile->GetNextGroupEntry())
        &&(strcmp(entryline,"CLASS_END")!=0) ){
  act = caFileTypes->AddAction( ft,entryline );

  if( entryline=IniFile->GetNextGroupEntry() ) strncpy( act->Exec,entryline,250 );
  else act->Exec[0]=0;
  act->Dest[0]=0;
  if( entryline=IniFile->GetNextGroupEntry() ){
    if( entryline[0] == 'M' ){
      act->Type = entryline[1]-48;
      act->CD   = entryline[2]-48;
      if( (act->CD<0) || (act->CD>2) ) act->CD=0;
      if( strchr(entryline,'A') ) act->Async=TRUE;
      if( strchr(entryline,'C') ) act->CD=1;
      if( strchr(entryline,'S') ) act->ReadSrc=TRUE;
      if( strchr(entryline,'D') ) act->ReadDst=TRUE;
    }else{
      act->Type = atoi( entryline );
      if( act->Type>9 ){
        act->Type-=10;
        act->ReadSrc = TRUE;
        act->ReadDst = TRUE;
      }else act->Async=TRUE;
    }
  }else{
    act->Type  = RUN_NOCLI;
    act->Async = TRUE;
  }
  if( entryline=IniFile->GetNextGroupEntry() ){
   strncpy( pat,entryline,100 );
   ppp=strchr( pat,',' );
   if(ppp==NULL){
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

//>"void LoadPaths( TIniFile *IniFile )"
/****************************************/
/*                                      */
/* Function: LoadPaths()                */
/*                                      */
/* Aim:      Load the paths from the    */
/*           config file                */
/*                                      */
/* Input:    IniFile=pointer to an      */
/*            inifile, prepared for     */
/*            reading                   */
/*                                      */
/****************************************/
void LoadPaths( TIniFile *IniFile ){
 char *entryline;
 TUserPathNode *pn;

 IniFile->SeekGroup( "PathList" );
 while( entryline=IniFile->GetNextGroupEntry() ){
  if( entryline[0] ){
    pn=(TUserPathNode*)AllocVec(sizeof(TUserPathNode),MEMF_CLEAR);
    strcpy( pn->Name,entryline );
    if( entryline=IniFile->GetNextGroupEntry() )
      strcpy( pn->Path,entryline );
    else pn->Path[0]=0;
    AddTail( &Main->UserPaths,(Node*)pn );
  }
 }
}
//<

//>"void LoadClasses( TIniFile *IniFile )"
/****************************************/
/*                                      */
/* Function: LoadClasses()              */
/*                                      */
/* Aim:      Load the classes from the  */
/*           config file && build up   */
/*           all the needed lists       */
/*                                      */
/* Input:    IniFile=pointer to an      */
/*            inifile, prepared for     */
/*            reading                   */
/*                                      */
/****************************************/
void LoadClasses( TIniFile *IniFile ){
 TFileType *ft;
 int i;
 char pat[102];
 char *entryline;
 TAction *act,*curact;

 // Read Open With Class
 caFileTypes->OpenWith = caFileTypes->AddFileType( "" );
 IniFile->SeekGroup( "Classopenwith" );
 ReadClassEntry( IniFile,caFileTypes->OpenWith );

 // Read Unknown Class
 caFileTypes->Unknown = caFileTypes->AddFileType( "" );
 IniFile->SeekGroup( "Classunknown" );
 ReadClassEntry( IniFile,caFileTypes->Unknown );

 // Read generic class
 caFileTypes->Generic = caFileTypes->AddFileType( "" );
 IniFile->SeekGroup( "Classgen" );
 ReadClassEntry( IniFile,caFileTypes->Generic );

 // Reading volume class
 caFileTypes->Volume = caFileTypes->AddFileType( "" );
 IniFile->SeekGroup( "Classvol" );
 ReadClassEntry( IniFile,caFileTypes->Volume );

 // Reading directory class
 caFileTypes->Directory = caFileTypes->AddFileType( "" );
 IniFile->SeekGroup( "Classdir" );
 ReadClassEntry( IniFile,caFileTypes->Directory );
 // && copying to assign class:
 caFileTypes->Assign = caFileTypes->AddFileType( "" );
 for( curact=(TAction*)caFileTypes->Directory->Actions.lh_Head;curact->ln_Succ;curact=(TAction*)curact->ln_Succ ){
   act = caFileTypes->AddAction( caFileTypes->Assign,curact->Name );
   strcpy( act->Exec,curact->Exec );
   strcpy( act->Dest,curact->Dest );
   strcpy( act->Stack,curact->Stack );
   strcpy( act->Delay,curact->Delay );
   act->Type    = curact->Type;
   act->Async   = curact->Async;
   act->CD      = curact->CD;
   act->ReadSrc = curact->ReadSrc;
   act->ReadDst = curact->ReadDst;
 } // but with the action UNASSIGN:
 act = caFileTypes->AddAction( caFileTypes->Assign,GetCatalogStr(Main->Catalog,MSG_ASSIGNREMOVE,MSG_ASSIGNREMOVE_STR) );
 sprintf( act->Exec,"[SURE:%s]assign [f] remove",
   GetCatalogStr(Main->Catalog,MSG_ASSIGNWANTREMOVE,MSG_ASSIGNWANTREMOVE_STR) );
 strcpy( act->Dest,"" );
 strcpy( act->Stack,"8000" );
 act->Type    = RUN_NOCLI;
 act->Async   = FALSE;
 act->CD      = 1;
 act->ReadSrc = TRUE;
 act->ReadDst = FALSE;


 // Read classes
 IniFile->SeekGroup( "Classes" );
 while( entryline=IniFile->GetNextGroupEntry() ){
  ft = caFileTypes->AddFileType( entryline );
  if( entryline=IniFile->GetNextGroupEntry() ){
    // mode line, beginning with 'M':
    if( strchr(entryline,'A') ) ft->IsArchive=TRUE;
  }
//  if( Main->PrefsVersion>=45 ){ CA QUITS FOR EARLIER VERSIONS !!
  if( entryline=IniFile->GetNextGroupEntry() ) strncpy( ft->ImageFile,entryline,108 );
//  }
  if( entryline=IniFile->GetNextGroupEntry() ){
    strcpy( ft->FilePattern,entryline );
    if( entryline[0]!=0 ){
      sprintf( pat,"(%s)",entryline );
      ParsePatternNoCase(pat,(UBYTE *)ft->ParsedPattern,208);
    }
  }else ft->FilePattern[0]=0;
  for( i=0;i<5;i++ ){
   if( entryline=IniFile->GetNextGroupEntry() ) strncpy( ft->Value[i],entryline,50 );
   else ft->Value[i][0]=0;
  }
  ReadClassEntry( IniFile,ft );
 }
}
//<

//>"void LoadOptions( TIniFile *IniFile )"
void LoadOptions( TIniFile *IniFile ){
 char *tmp,*tool;

 tool=new char[512];
 tmp=new char[512];

/*
 if( IniFile->GetBool( "DECRUNCH",FALSE ) ){
  Main->DecrunchFlag=0;
  if(OpenXfdLib()==-1){
   Info( GetCatalogStr(Main->Catalog,ERR_NOXFD,ERR_NOXFD_STR) );
  }else Main->DecrunchFlag=1;
 }
*/

 // General:
 Main->IconStart     = IniFile->GetBool( "General","ICONSTART",FALSE );
 Main->MuiQuit       = IniFile->GetBool( "General","MUIQUIT",FALSE );
 Main->bPulldownMenu = IniFile->GetBool( "General","PULLDOWNMENU",TRUE );
 IniFile->GetString( "General","OWNSCREEN","",Main->OwnScreen );
 IniFile->GetString( "General","PUBLICSCREEN","",Main->PublicScreen );
 IniFile->GetString( "General","CLISIZE","CON:0/0/640/100/ClassAction_Output_Window",Main->DevString );
 IniFile->GetString( "General","MENUFONT","",Main->sMenuFont );
 IniFile->GetString( "General","POPUPKEY","CTRL ALT F1",Main->PopupKey );
 IniFile->GetString( "General","HIDEKEY","CTRL ALT F2",Main->HideKey );
 UseReactionRequest = IniFile->GetBool( "General","USEREACTIONREQUEST",TRUE );

 // CopyAndDelete:
 Main->BufferSize       = IniFile->GetInt(  "CopyAndDelete","BUFFERSIZE",100 ) * 1000;
 Main->CountFiles       = IniFile->GetBool( "CopyAndDelete","COUNTFILES",TRUE );
 Main->CheckFit         = IniFile->GetBool( "CopyAndDelete","CHECKFIT",FALSE );
 Main->ConfirmReplace   = IniFile->GetBool( "CopyAndDelete","CONFIRMREPLACE",TRUE );
 Main->ShowVersion      = IniFile->GetBool( "CopyAndDelete","SHOWVERSION",TRUE );
 Main->ReplaceVersion   = IniFile->GetBool( "CopyAndDelete","REPLACEVERSION",FALSE );
 Main->ConfirmDelete    = IniFile->GetBool( "CopyAndDelete","CONFIRMDELETE",TRUE );
 Main->DeleteProtected  = IniFile->GetBool( "CopyAndDelete","DELETEPROTECTED",FALSE );

 // Buttons:
 Main->ShowParent   = IniFile->GetBool( "Buttons","PARENT",TRUE   );
 Main->ShowRefresh  = IniFile->GetBool( "Buttons","REFRESH",TRUE  );

 // Listers:
 Main->ShowDrawersFirst  = IniFile->GetBool( "Listers","DRAWERSFIRST",TRUE );
 Main->VerticalLists     = IniFile->GetBool( "Listers","VERTICAL",FALSE );
 Main->DateString        = IniFile->GetBool( "Listers","DATESTRING",TRUE );
 Main->DisplayTime       = IniFile->GetBool( "Listers","DISPTIME",FALSE );
 Main->ShowImages        = IniFile->GetBool( "Listers","SHOWIM",TRUE );
 Main->ClassImages       = IniFile->GetBool( "Listers","CLASSIMAGES",TRUE );
 Main->CheckOnlyPatterns = IniFile->GetBool( "Listers","ONLYPATTERNS",TRUE );
 Main->LittleImages      = IniFile->GetBool( "Listers","SMALLIM",FALSE );
 Main->ShowDevices       = IniFile->GetBool( "Listers","SHOWDEVS",FALSE );
 Main->HighlightDirs     = IniFile->GetBool( "Listers","HIGHDIRS",FALSE );
 Main->TabButtonsTransparent = IniFile->GetBool( "Listers","TABBUTTONSTRANSPARENT",TRUE );
 Main->MaxTabs           = IniFile->GetInt(  "Listers","MAXTABS",2 );
 Main->MaxPaths          = IniFile->GetInt(  "Listers","MAXPATHS",10 );
 Main->PathNotify        = IniFile->GetBool( "Listers","PATHNOTIFY",TRUE );
 Main->UseExAll          = IniFile->GetBool( "Listers","USEEXALL",FALSE );
 Main->ExAllFiles        = IniFile->GetInt(  "Listers","EXALLFILES",100 );
 IniFile->GetString( "Listers","DATEFORMAT","dd-mm-yy",tmp );
 if( strcmp(tmp,"dd-mm-yy")==0 ) Main->DateFormat=FORMAT_CDN;
 else if( strcmp(tmp,"dd-mmm-yy")==0 ) Main->DateFormat=FORMAT_DOS;
 else if( strcmp(tmp,"mm-dd-yy")==0 ) Main->DateFormat=FORMAT_USA;
 else if( strcmp(tmp,"yy-mmm-dd")==0 ) Main->DateFormat=FORMAT_INT;
 else Main->DateFormat=FORMAT_DOS;


  // start paths only if no restart:
  if( !Main->SrcStartPath[0] )
    IniFile->GetString( "Listers","SRCSTARTPATH","",Main->SrcStartPath );
  if( !Main->DstStartPath[0] )
    IniFile->GetString( "Listers","DSTSTARTPATH","",Main->DstStartPath );
 IniFile->GetString( "Listers","LISTERFONT","",Main->sListerFont );
 if( Main->MaxTabs<1 )   Main->MaxTabs = 1;
 if( Main->MaxPaths<10 ) Main->MaxPaths = 10;

 // Icons:
 Main->CreateIcons = IniFile->GetBool( "Icons","CREATEICONS",TRUE );
 Main->CopyIcons   = IniFile->GetBool( "Icons","COPYICONS",TRUE );
 Main->DeleteIcons = IniFile->GetBool( "Icons","DELETEICONS",TRUE );
 Main->ShowIcons   = IniFile->GetBool( "Icons","DISPINFO",TRUE );

 // Window:
 Main->WinX          = IniFile->GetInt( "Window","X",-1 );
 Main->WinY          = IniFile->GetInt( "Window","Y",-1 );
 Main->WinWidth      = IniFile->GetInt( "Window","WIDTH",400 );
 Main->WinHeight     = IniFile->GetInt( "Window","HEIGHT",200 );
 Main->AppX          = IniFile->GetInt( "Window","APPX",-1 );
 Main->AppY          = IniFile->GetInt( "Window","APPY",-1 );
 Main->PopupWaitTime = IniFile->GetInt( "Window","POPUPWAITTIME",6 );
 IniFile->GetString( "Window","BACKGROUND","",Main->BackgroundPattern );

 delete [] tmp;
 delete [] tool;
}
//<

//>"BOOL SetFileAttributes( char *file )"
/****************************************/
/*                                      */
/* Function: SetFileAttributes          */
/*                                      */
/* Aim:      Open a window && let the  */
/*           user choose protection     */
/*           bits, comment, date        */
/*           && filename               */
/*           && save the changes       */
/*                                      */
/* Input:    file: file to change       */
/*                                      */
/****************************************/
BOOL SetFileAttributes( char *file ){
  BOOL   end,Save=FALSE,bSaveDate=FALSE,bRename=FALSE,IsDir;
  Object *win,*vlayout,*gDate,*gName,*gComment,*gBits[6],*gVersion;
  ULONG  windowsignals,mask,code=0,result;
  ULONG  selected;
  long   Prot=0;
  ULONG  bRead,bWrite,bDelete,bExecute,bArchive,bScript;
  char   sDate[40],sTime[20],*ptr,buffer[256],version[50];
  BPTR   lock;
  Window *intuiwin;
  DateTime dt;
  FileInfoBlock *fib;

  if( !(lock=Lock(file,ACCESS_READ)) ) return( FALSE );

  fib=(FileInfoBlock *)AllocDosObject( DOS_FIB,NULL );
  Examine(lock,fib);
  UnLock(lock);

  IsDir = (fib->fib_DirEntryType>0);
  memcpy( &(dt.dat_Stamp),&(fib->fib_Date),sizeof(struct DateStamp) );
  dt.dat_StrDate = sDate;
  dt.dat_Format  = Main->DateFormat;
  dt.dat_Flags   = 0;
  dt.dat_StrDay  = NULL;
  dt.dat_StrTime = sTime;
  DateToStr( &dt );
  strcat(sDate," ");
  strcat(sDate,sTime);

  bScript  = fib->fib_Protection & FIBF_SCRIPT;
  bArchive = fib->fib_Protection & FIBF_ARCHIVE;
  bRead    = !(fib->fib_Protection & FIBF_READ);
  bWrite   = !(fib->fib_Protection & FIBF_WRITE);
  bExecute = !(fib->fib_Protection & FIBF_EXECUTE);
  bDelete  = !(fib->fib_Protection & FIBF_DELETE);

  // alloc vertical layout
  vlayout=(Object*)NewObject(
    LAYOUT_GetClass(),NULL,
    LAYOUT_Orientation,LAYOUT_VERTICAL,LAYOUT_DeferLayout,TRUE,
    LAYOUT_SpaceInner,TRUE,LAYOUT_SpaceOuter,TRUE,
    LAYOUT_AddChild,(Object*)NewObject(
      SPACE_GetClass(),NULL,TAG_END
    ),
    // Name:
    LAYOUT_AddChild,gName=(Object*)NewObject(
      STRING_GetClass(),NULL,
      STRINGA_MaxChars,108,
      STRINGA_TextVal,fib->fib_FileName,
      TAG_END
    ),
    CHILD_Label,(Object*)NewObject(
      LABEL_GetClass(),NULL,IA_FGPEN,2,
      LABEL_Text,GetCatalogStr(Main->Catalog,MSG_NAME8,MSG_NAME8_STR),TAG_END
    ),
    // Version:
    IsDir ? TAG_IGNORE:LAYOUT_AddChild,gVersion=(Object*)NewObject(
      BUTTON_GetClass(),NULL,
      GA_RelVerify,TRUE,GA_ID,1,
      BUTTON_Justification,BCJ_LEFT,
      GA_Text,"_?",
      TAG_END
    ),
    IsDir ? TAG_IGNORE:CHILD_Label,(Object*)NewObject(
      LABEL_GetClass(),NULL,IA_FGPEN,2,
      LABEL_Text,GetCatalogStr(Main->Catalog,MSG_VERSION8,MSG_VERSION8_STR),TAG_END
    ),
    // Date:
    LAYOUT_AddChild,gDate=(Object*)NewObject(
      STRING_GetClass(),NULL,
      STRINGA_MaxChars,40,
      STRINGA_TextVal,sDate,
      TAG_END
    ),
    CHILD_Label,(Object*)NewObject(
      LABEL_GetClass(),NULL,IA_FGPEN,2,
      LABEL_Text,GetCatalogStr(Main->Catalog,MSG_DATE8,MSG_DATE8_STR),TAG_END
    ),
    // Protection:
    LAYOUT_AddChild,(Object*)NewObject(
      LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_HORIZONTAL,
      LAYOUT_AddChild,(Object*)NewObject(
        LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_VERTICAL,
        LAYOUT_AddChild,gBits[0]=(Object*)NewObject(
          CHECKBOX_GetClass(),NULL,
          GA_Text,GetCatalogStr(Main->Catalog,MSG_READABLE,MSG_READABLE_STR),
          GA_Selected,bRead,TAG_END
        ),
        LAYOUT_AddChild,gBits[1]=(Object*)NewObject(
          CHECKBOX_GetClass(),NULL,
          GA_Text,GetCatalogStr(Main->Catalog,MSG_DELETABLE,MSG_DELETABLE_STR),
          GA_Selected,bDelete,TAG_END
        ),
        TAG_END
      ),
      LAYOUT_AddChild,(Object*)NewObject(
        LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_VERTICAL,
        LAYOUT_AddChild,gBits[2]=(Object*)NewObject(
          CHECKBOX_GetClass(),NULL,
          GA_Text,GetCatalogStr(Main->Catalog,MSG_WRITABLE,MSG_WRITABLE_STR),
          GA_Selected,bWrite,TAG_END
        ),
        LAYOUT_AddChild,gBits[3]=(Object*)NewObject(
          CHECKBOX_GetClass(),NULL,
          GA_Text,GetCatalogStr(Main->Catalog,MSG_EXECUTABLE,MSG_EXECUTABLE_STR),
          GA_Selected,bExecute,TAG_END
        ),
        TAG_END
      ),
      LAYOUT_AddChild,(Object*)NewObject(
        LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_VERTICAL,
        LAYOUT_AddChild,gBits[4]=(Object*)NewObject(
          CHECKBOX_GetClass(),NULL,
          GA_Text,GetCatalogStr(Main->Catalog,MSG_ARCHIVE,MSG_ARCHIVE_STR),
          GA_Selected,bArchive,TAG_END
        ),
        LAYOUT_AddChild,gBits[5]=(Object*)NewObject(
          CHECKBOX_GetClass(),NULL,
          GA_Text,GetCatalogStr(Main->Catalog,MSG_SCRIPT,MSG_SCRIPT_STR),
          GA_Selected,bScript,TAG_END
        ),
        TAG_END
      ),
      TAG_END
    ),
    CHILD_Label,(Object*)NewObject(
      LABEL_GetClass(),NULL,IA_FGPEN,2,
      LABEL_Text,GetCatalogStr(Main->Catalog,MSG_PROTECTION8,MSG_PROTECTION8_STR),TAG_END
    ),
    // Comment:
    LAYOUT_AddChild,gComment=(Object*)NewObject(
      STRING_GetClass(),NULL,
      STRINGA_MaxChars,80,
      STRINGA_TextVal,fib->fib_Comment,
      TAG_END
    ),
    CHILD_Label,(Object*)NewObject(
      LABEL_GetClass(),NULL,IA_FGPEN,2,
      LABEL_Text,GetCatalogStr(Main->Catalog,MSG_COMMENT8,MSG_COMMENT8_STR),TAG_END
    ),
    LAYOUT_AddChild,(Object*)NewObject(
      SPACE_GetClass(),NULL,TAG_END
    ),
    // Ok/Cancel:
    LAYOUT_AddChild,(Object*)NewObject(
      LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_HORIZONTAL,
      LAYOUT_EvenSize,TRUE,LAYOUT_BevelStyle,BVS_SBAR_VERT,
      LAYOUT_AddChild,NewObject(
        BUTTON_GetClass(),NULL,
        GA_ID,10,GA_RelVerify,TRUE,GA_Text,GetCatalogStr(Main->Catalog,MSG_SAVE1,MSG_SAVE1_STR),
        TAG_END
      ),
      CHILD_WeightedWidth,0,
      LAYOUT_AddChild,NewObject(
        BUTTON_GetClass(),NULL,
        GA_ID,11,GA_RelVerify,TRUE,GA_Text,GetCatalogStr(Main->Catalog,MSG_CANCEL1,MSG_CANCEL1_STR),
        TAG_END
      ),
      CHILD_WeightedWidth,0,
      TAG_END
    ),
    TAG_END
  );

  // open window && wait till closing
  win=(Object*)NewObject( WINDOW_GetClass(),NULL,
    WINDOW_Position,WPOS_CENTERMOUSE,WINDOW_Layout,vlayout,
    WA_PubScreen,Main->Scr,WA_IDCMP,IDCMP_GADGETUP|IDCMP_CLOSEWINDOW,
    WA_Activate,TRUE,WA_Title,file,WA_DragBar,TRUE,WA_DepthGadget,TRUE,
    WA_CloseGadget,TRUE,WA_SizeGadget,TRUE,WA_RMBTrap,TRUE,
    TAG_END
  );

  DoMethod( win,WM_OPEN );
  end=FALSE;
  GetAttr( WINDOW_Window, win, (ULONG*)&intuiwin );
  GetAttr( WINDOW_SigMask, win, &windowsignals );
  do{
   mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
   if( mask & SIGBREAKF_CTRL_C ) end=TRUE;
   while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_CLOSEWINDOW: end = TRUE; break;
     case WMHI_GADGETUP:{
      selected=(result & RL_GADGETMASK);
      switch( selected ){
        case 1:{
          if( ! GetVersion(file,version) ){
            strcpy( version,GetCatalogStr(Main->Catalog,MSG_NOTAVAILABLE,MSG_NOTAVAILABLE_STR) );
          }
          SetGadgetAttrs( (Gadget*)gVersion,intuiwin,NULL,GA_Text,version,TAG_END );
          break;
        }
        case 10: Save = TRUE; break;
        case 11: end  = TRUE; break;
      }
      break;
     }
    }
    if( Save ){
      Save = FALSE;
      // first test if the date == valid:
      GetAttr( STRINGA_TextVal,gDate,(ULONG*)&ptr );
      if( strcmp(ptr,sDate)!=0 ){
        strcpy( sDate,ptr );
        ptr=strchr(sDate,' ');
        if( ptr ){
          *ptr=0;
          strcpy( sTime,ptr+1 );
        }else strcpy( sTime,"00:00:00" );
        dt.dat_StrDate = sDate;
        dt.dat_Format  = Main->DateFormat;
        dt.dat_Flags   = 0;
        dt.dat_StrDay  = NULL;
        dt.dat_StrTime = sTime;
        if( StrToDate(&dt) == 0 ){
          Info( GetCatalogStr(Main->Catalog,MSG_DATENOTVALID,MSG_DATENOTVALID_STR) );
          break;
        }
        bSaveDate = TRUE;
      }
      // now test
      GetAttr( STRINGA_TextVal,gName,(ULONG*)&ptr );
      if( strcmp(fib->fib_FileName,ptr)!=0 ){
        strcpy(buffer,file);
        strcpy(FilePart(buffer),ptr);
        if( lock=Lock(buffer,ACCESS_READ) ){
          UnLock(lock);
          Info( GetCatalogStr(Main->Catalog,MSG_FILEALREADYEXISTS,MSG_FILEALREADYEXISTS_STR) );
          break;
        }
        bRename = TRUE;
      }
      Save = TRUE;
      end  = TRUE;
    }
   }
  }while( !end );
  if( Save ){
    if( bSaveDate ) SetFileDate( file,&dt.dat_Stamp ); // already converted !
    GetAttr( GA_Selected,gBits[0],(ULONG*)&ptr );
    if( !ptr ) Prot |= FIBF_READ;
    GetAttr( GA_Selected,gBits[1],(ULONG*)&ptr );
    if( !ptr ) Prot |= FIBF_DELETE;
    GetAttr( GA_Selected,gBits[2],(ULONG*)&ptr );
    if( !ptr ) Prot |= FIBF_WRITE;
    GetAttr( GA_Selected,gBits[3],(ULONG*)&ptr );
    if( !ptr ) Prot |= FIBF_EXECUTE;
    GetAttr( GA_Selected,gBits[4],(ULONG*)&ptr );
    if( ptr ) Prot |= FIBF_ARCHIVE;
    GetAttr( GA_Selected,gBits[5],(ULONG*)&ptr );
    if( ptr ) Prot |= FIBF_SCRIPT;
    if( Prot != fib->fib_Protection ) SetProtection( file,Prot );
    GetAttr( STRINGA_TextVal,gComment,(ULONG*)&ptr );
    if( strcmp(ptr,fib->fib_Comment)!=0 ) SetComment( file,ptr );
    if( bRename ) Rename( file,buffer );
  }
  DoMethod( win,WM_CLOSE );
  DisposeObject( win );
  FreeDosObject( DOS_FIB,fib );
  return( Save );
}
//<

//>"IsArchive( char *file )"
// check if file is an archive
// -->> CLASS ATTRIBUTES!!
/*
BOOL IsArchive( char *file ){
 int i=strlen(file);
 if( i<4 ) return( FALSE );
 char *p=&(file[i-4]);
 if( Main->SupportedArchives[TYPE_LHA] && (stricmp(p,".lha")==0) ) return( TRUE );
 else if( Main->SupportedArchives[TYPE_LZX] && (stricmp(p,".lzx")==0) ) return( TRUE );
// else if( Main->SupportedArchives[TYPE_LZH] && (stricmp(p,".lzh")==0) ) return( TRUE );
 else if( Main->SupportedArchives[TYPE_ZIP] && (stricmp(p,".zip")==0) ) return( TRUE );
 else if( Main->SupportedArchives[TYPE_HA] && (stricmp(p+1,".ha")==0) ) return( TRUE );
 else return( FALSE );
}
*/
//<



