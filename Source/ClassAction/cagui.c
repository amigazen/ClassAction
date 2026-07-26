/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File : gui.h

        Goal : GUI interface code

        Ver  : 3.00 

****************************************/

#include <ctype.h>
#include <stdlib.h>

// Libraries:
#include <clib/dos_protos.h>
#include <dos/datetime.h>
#include <dos/dostags.h>
#include <clib/exec_protos.h>
#include <exec/alerts.h>
#include <clib/graphics_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/locale_protos.h>
#include <clib/wb_protos.h>
#include <workbench/icon.h>

// Objects:
#include <proto/button.h>
#include <proto/checkbox.h>
#include <gadgets/fuelgauge.h>
#include <proto/label.h>
#include <images/label.h>
#include <proto/layout.h>
#include <gadgets/layout.h>
#include <proto/listbrowser.h>
#include <gadgets/listbrowser.h>
#include <gadgets/space.h>
#include <proto/space.h>
#include <proto/string.h>
#include <proto/window.h>
#include <classes/window.h>


#include "cadisk.h"
#include "caexec.h"
#include "caextern.h"
#include "caFileTypes.h"
#include "caglobal.h"
#include "cagui.h"
#include "calha.h"
#include "capopup.h"
#include "careq.h"
#include "cautils.h"
#include "cavers.h"
#include "newmouse.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classaction.h"
#include "classactioncd.h"

#ifndef NDEBUG
extern ofstream DebOut;
#endif

extern char Quote,*QuoteStr;

//#define DEBUG_VERSION

//>"void HandleNotify()"
void HandleNotify(){
 NotifyMessage *msg;

 while( msg=(NotifyMessage*)GetMsg(Main->NotifyPort) ){
  if( Main->PathNotify ){
   TPathNode *pn=(TPathNode*)msg->nm_NReq->nr_UserData;
   // first test, if the path node has not yet been removed:
   if( IsPathNodeValid(pn) ){
    // if the path is shown, we can refresh the lister:
    if( strcmp(msg->nm_NReq->nr_Name,CurrentTab[0]->Path)==0 ) Refresh(0);
    else if( strcmp(msg->nm_NReq->nr_Name,CurrentTab[1]->Path)==0 ) Refresh(1);
    // else we just remove the path from the buffered paths, it can be read when necessary:
    else RemovePathNode( pn );
  }}
  ReplyMsg( (Message*)msg );
 }
}
//<

//>"void HandleDiskNotify( ULONG Class )"
void HandleDiskNotify( ULONG Class ){

/*
 if( Class is IDCMP_DISKREMOVED ){
  // first we have to check the buffered paths to end notification:
  TPathNode *nextpn,*pn=(TPathNode*)Main->Paths.lh_Head;
  BPTR PathLock;
  while( pn->ln_Succ ){
   nextpn = (TPathNode*)pn->ln_Succ;
   if( pn->NotifyOn ){
    if( PathLock=Lock(pn->Path,ACCESS_READ) ) UnLock( PathLock );
    else{ EndNotify( &pn->NotiReq ); pn->NotifyOn = FALSE; }
   }
   pn = nextpn;
 }}
*/
 // now we can reread devices:
 if( CurrentTab[0]->Path[0] is 0 ){
   Refresh(0);
 }else if( CurrentTab[1]->Path[0] is 0 ){
  Refresh(1);
 }else{ // else we just remove the path from the buffered paths, it can be read when necessary:
  TPathNode *pn=GetPathNode( "",FALSE );
  if( pn ) RemovePathNode( pn );
 }
}
//<

//>"void FormatSize( char *buffer,float size )"
void FormatSize( char *buffer,float size ){
 int count=0,n=0;
 char temp[50];

 sprintf( temp,"%.0f",size );
 while( count<strlen(temp) ){
  buffer[n]=temp[count];
  n++; count++;
  // check if we are on a place before a . but not on last pos
  if( ((strlen(temp)-count)%3==0)&&(strlen(temp)!=count) ){
   buffer[n]='.';
   n++;
  }
 }
 buffer[n]=0;
}
//<

//>"Object *GetCurrentListBrowser()"
Object *GetCurrentListBrowser(){
 if( Main->CurrentList==0 ) return( Main->Gadgets[LST_DIR_L] );
 else return( Main->Gadgets[LST_DIR_R] );
}
//<

//>"Object *GetListBrowser( UBYTE which )"
Object *GetListBrowser( UBYTE which ){
 if( which==0 ) return( Main->Gadgets[LST_DIR_L] );
 else return( Main->Gadgets[LST_DIR_R] );
}
//<

//>"char *GetPath( UBYTE which )"
char *GetPath( UBYTE which ){
 if( CurrentTab[which] ) return( CurrentTab[which]->Path );
 else return( "" );
}
//<

//>"char *GetSrc()"
char *GetSrc(){
 return( CurrentTab[Main->CurrentList]->Path )
}
//<

//>"char *GetDst()"
char *GetDst(){
 return( CurrentTab[1-Main->CurrentList]->Path )
}
//<

//>"void ActivateBrowser( UBYTE which )"
void ActivateBrowser( UBYTE which ){
 ULONG Pressed;
 if( which is 0 ){
  GetAttr( GA_Selected,Main->Gadgets[BUTTON_ACTIVE_L],&Pressed );
  if( !Pressed ){
   SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_ACTIVE_L],Main->IntuiWin,NULL,GA_Selected,TRUE,TAG_DONE );
  }
  if( which is Main->CurrentList ) return;
  SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_ACTIVE_R],Main->IntuiWin,NULL,GA_Selected,FALSE,TAG_DONE );
 }else{
  GetAttr( GA_Selected,Main->Gadgets[BUTTON_ACTIVE_R],&Pressed );
  if( !Pressed ){
   SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_ACTIVE_R],Main->IntuiWin,NULL,GA_Selected,TRUE,TAG_DONE );
  }
  if( which is Main->CurrentList ) return;
  SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_ACTIVE_L],Main->IntuiWin,NULL,GA_Selected,FALSE,TAG_DONE );
 }
 // first we will save the position of the old browser/path because it could be listed in
 // the other browser:
 ULONG top;
 GetAttr( LISTBROWSER_Top,GetListBrowser(1-which), &top );
 TPathNode *LastPath=GetPathNode( CurrentTab[1-which]->Path,FALSE );
 if( LastPath ) LastPath->FirstOnList = top;
 Main->CurrentList = which;
}
//<

//>"void OpenPath( UBYTE lister )"
void OpenPath( UBYTE which ){
 char *newpath = OpenPathMenu();
 if( newpath ) ListDir( which,newpath,0 );
}
//<

//>"void AdjustHistory( char *path )"
void AdjustHistory( char *path ){
 Node *newnode,*node;
 BOOL found=FALSE;

 // look for duplicates:
 node=Main->History.lh_Head;
 while( (!found) && node->ln_Succ ){
  if( strcmp(((TNameNode*)node)->Name,path)==0 ){
   Remove( node );
   FreeVec( node );
   found=TRUE;
   Main->HistorySize--;
  }
  node=node->ln_Succ;
 }
 // cut list if too long:
 while( Main->HistorySize>19 ){
  node=Main->History.lh_TailPred;
  Remove( node );
  FreeVec( node );
  Main->HistorySize--;
 }
 // insert new node:
 newnode=(Node*)AllocVec( sizeof(TNameNode),MEMF_CLEAR );
 if( newnode ){
  strcpy( ((TNameNode*)newnode)->Name,path );
  AddHead( &Main->History,newnode );
  Main->HistorySize++;
 }
}
//<

//>"void ClearHistory()"
void ClearHistory(){
 Node *node;
 while( !IsListEmpty( &Main->History ) ){
  node=Main->History.lh_Head;
  Remove( node );
  FreeVec( node );
 }
}
//<

//>"void OpenOld( UBYTE which )"
void OpenOld( UBYTE which ){
 char *newpath = OpenHistoryMenu();
 if( newpath ) ListDir( which,newpath,0 );
}
//<

//>"void PopupOnCurrentFiles()"
void PopupOnCurrentFiles(){
 char fullname[512],*FilePointer;
 Node *node;
 ULONG selected;
 TFileType *ft;
 BOOL  AllTheSame=TRUE;
 TTabNode *tn=CurrentTab[Main->CurrentList];
 Object *lb=GetCurrentListBrowser();

 GetAttr( LISTBROWSER_NumSelected,lb,&selected );
 if( selected is 0 ) return;    // nothing chosen, go away!

 GetAttr( LISTBROWSER_SelectedNode,lb,(ULONG*)&node );
 strcpy( fullname,GetSrc() );
 AdjustPath( fullname );
 FilePointer = fullname + strlen(fullname);
 strcpy( FilePointer,((TLBNode*)node)->Fn->Filename );

 if( tn->LhaActive ){
  if( (selected is 1) and (((TLBNode*)node)->Fn->Type is TYPE_FILE) ){
  // for an archived file, we provide a menu on the extracted file.
  // Handling multiple files would be a problem as we must copy all files to t:
   LhaPopup( fullname );
  }
 }else{
   if( selected is 1 ){           // only one entry selected, just a popup
     if( ((TLBNode*)node)->Fn->Type is TYPE_FILE )
       OpenActionMenu( caFileTypes->GetFileType(fullname,((TLBNode*)node)->Fn->Filename,FALSE),fullname );
     else if( ((TLBNode*)node)->Fn->Type is TYPE_DIR )
       OpenActionMenu( caFileTypes->Directory );
     else if( ((TLBNode*)node)->Fn->Type is TYPE_ASSIGN )
       OpenActionMenu( caFileTypes->Assign );
     else
       OpenActionMenu( caFileTypes->Volume );
   }else if( ((TLBNode*)node)->Fn->Type is TYPE_FILE ){
     // the first is:
     ft = caFileTypes->GetFileType( fullname,((TLBNode*)node)->Fn->Filename,FALSE );
     // now see what the others are:
     GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&node );
     node = node->ln_Succ;
     while( node->ln_Succ ){
       GetListBrowserNodeAttrs( node,LBNA_Selected,&selected,TAG_END );
       if( selected ){
         strcpy( FilePointer,((TLBNode*)node)->Fn->Filename );
         if( not caFileTypes->BelongsToFileType( fullname,ft ) ) AllTheSame=FALSE;
       }
       node = node->ln_Succ;
     }
     if( AllTheSame ) OpenActionMenu( ft );
     else OpenActionMenu( caFileTypes->Generic );
   }else if( (((TLBNode*)node)->Fn->Type is TYPE_VOLUME)
          or (((TLBNode*)node)->Fn->Type is TYPE_DEVICE) ){
     GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&node );
     node = node->ln_Succ;
     while( node->ln_Succ ){
       GetListBrowserNodeAttrs( node,LBNA_Selected,&selected,TAG_END );
       if( selected ){
         if( ((TLBNode*)node)->Fn->Type is TYPE_ASSIGN ) AllTheSame=FALSE;
       }
       node = node->ln_Succ;
     }
     if( AllTheSame ) OpenActionMenu( caFileTypes->Volume );
     else OpenActionMenu( caFileTypes->Generic );
   }else if( ((TLBNode*)node)->Fn->Type is TYPE_ASSIGN ){
     GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&node );
     node = node->ln_Succ;
     while( node->ln_Succ ){
       GetListBrowserNodeAttrs( node,LBNA_Selected,&selected,TAG_END );
       if( selected ){
         if( ((TLBNode*)node)->Fn->Type isnot TYPE_ASSIGN ) AllTheSame=FALSE;
       }
       node = node->ln_Succ;
     }
     if( AllTheSame ) OpenActionMenu( caFileTypes->Assign );
     else OpenActionMenu( caFileTypes->Generic );
   }else{
     // now see if the others are also dirs:
     GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&node );
     node = node->ln_Succ;
     while( node->ln_Succ ){
       GetListBrowserNodeAttrs( node,LBNA_Selected,&selected,TAG_END );
       if( selected ){
         if( ((TLBNode*)node)->Fn->Type is TYPE_FILE )  AllTheSame=FALSE;
       }
       node = node->ln_Succ;
     }
     if( AllTheSame ) OpenActionMenu( caFileTypes->Directory );
     else OpenActionMenu( caFileTypes->Generic );
   }
 }
}
//<

//>"BOOL DeSelect( TLBNode *fn, char *filename )"
BOOL DeSelect( TLBNode *fn, char *filename ){
 TLBNode *node=fn;
 BOOL found=FALSE;
 ULONG selected;

 while( (!found)&&(node->ln_Succ!=NULL) ){
  if( (stricmp(((FileNode*)node->Fn)->Filename,filename))==0 ){
   found=TRUE;
   GetListBrowserNodeAttrs( (Node*)node,LBNA_Selected,&selected,TAG_END );
   if( selected ){
    SetListBrowserNodeAttrs( (Node*)node,LBNA_Selected,FALSE,TAG_END );
   }
  }
  node=(TLBNode*)(node->ln_Succ);
 }
 return( found );
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
 TFileType *ft;
 char *File=new char[512];

 while( appmsg=(AppMessage*)GetMsg( Main->AppPort ) ){
  ft = NULL;
  /*
  switch( appmsg->am_Type ){
    case AMTYPE_APPICON:{
     if( appmsg->am_Class==AMCLASSICON_Open ){
       if( appmsg->am_NumArgs==0 ){
         UnIconifyWindow();
       }else{
         argptr = &(appmsg->am_ArgList[0]);
         NameFromLock( argptr->wa_Lock,File,511 );
//         UnLock( argptr->wa_Lock );
         if( strlen(argptr->wa_Name)!=0 ){
          AddPart( File,argptr->wa_Name,511 );
          ft = caFileTypes->GetFileType( File,argptr->wa_Name,FALSE );
         }else ft = caFileTypes->Directory;
       }
     }
     ReplyMsg( (Message*)appmsg );
     if( ft ) OpenActionMenu( ft,File );
     break;
    }
    case AMTYPE_APPWINDOW:{
      argptr = appmsg->am_ArgList;
      NameFromLock( argptr->wa_Lock,File,511 );
      if( strlen(argptr->wa_Name)!=0 ){
        AddPart( File,argptr->wa_Name,511 );
        ft = caFileTypes->GetFileType( File,argptr->wa_Name,FALSE );
        OpenActionMenu( ft,File );
      }else{
        if( (!Main->VerticalLists and (appmsg->am_MouseX < ((Gadget*)Main->Gadgets[LST_DIR_L])->LeftEdge+
              ((Gadget*)Main->Gadgets[LST_DIR_L])->Width))
            or (Main->VerticalLists and (appmsg->am_MouseY < ((Gadget*)Main->Gadgets[LST_DIR_L])->TopEdge+
                           ((Gadget*)Main->Gadgets[LST_DIR_L])->Height)) )
        {
          ListDir( 0,File,0 );
        }
        else ListDir( 1,File,0 );
      }
      UnLock( argptr->wa_Lock );
      ReplyMsg( (Message*)appmsg );
      break;
    }
    default: ReplyMsg( (Message*)appmsg );
  }
  */
  switch( appmsg->am_Type ){
   case AMTYPE_APPICON:{
    if( appmsg->am_Class==AMCLASSICON_Open ){
     if( appmsg->am_NumArgs==0 ){
      UnIconifyWindow();
     }else{
       argptr = &(appmsg->am_ArgList[0]);
       NameFromLock( argptr->wa_Lock,File,511 );
//       UnLock( argptr->wa_Lock );
       if( strlen(argptr->wa_Name)!=0 ){
        AddPart( File,argptr->wa_Name,511 );
        ft = caFileTypes->GetFileType( File,argptr->wa_Name,FALSE );
       }else ft = caFileTypes->Directory;
       OpenActionMenu( ft,File );
    }}
    break;
   }
   case AMTYPE_APPWINDOW:{
     argptr = appmsg->am_ArgList;
     NameFromLock( argptr->wa_Lock,File,511 );
     if( strlen(argptr->wa_Name)!=0 ){
       AddPart( File,argptr->wa_Name,511 );
       ft = caFileTypes->GetFileType( File,argptr->wa_Name,FALSE );
       OpenActionMenu( ft,File );
     }else{
       if( (!Main->VerticalLists and (appmsg->am_MouseX < ((Gadget*)Main->Gadgets[LST_DIR_L])->LeftEdge+
             ((Gadget*)Main->Gadgets[LST_DIR_L])->Width))
           or (Main->VerticalLists and (appmsg->am_MouseY < ((Gadget*)Main->Gadgets[LST_DIR_L])->TopEdge+
                          ((Gadget*)Main->Gadgets[LST_DIR_L])->Height)) )
       {
         ListDir( 0,File,0 );
       }
       else ListDir( 1,File,0 );
     }
//     UnLock( argptr->wa_Lock );
  }}
  ReplyMsg( (Message*)appmsg );
 }
 delete [] File;
}
//<

//>"char *Suffix( char *string )"
char *Suffix( char *string ){
 char *ptr;
 if( ptr=strchr(string,'.') ) return( ptr+1 );
 else return( "" );
}
//<

//>"Node *GetPreNode( List *List,TLBNode *node,int sortmode,BOOL reverse )"
/****************************************/
/*                                      */
/* Function:  GetPreNode                */
/*                                      */
/* Aim:       Get the address of the    */
/*            node after which the      */
/*            given node must be        */
/*            inserted                  */
/*                                      */
/* Input:     List=list to search       */
/*             through                  */
/*            node=node that shall      */
/*             be inserted              */
/*            sortmode=sort mode of list*/
/*            reverse=reverse order     */
/*                                      */
/* Output:    the address of the        */
/*             wanted node              */
/*                                      */
/****************************************/
Node *GetPreNode( List *List,TLBNode *node,int sortmode,BOOL reverse ){
 int sig=-1;
 if( reverse ) sig=1;
 FileNode *fn=node->Fn;

 if( IsListEmpty(List) ) return( (Node*)List );
 Node *f=(Node*)List->lh_Head;
//search (   till end    )&&( while the names have lower letters )
 if( fn->Type==TYPE_VOLUME ){
  while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig)
         &&(((TLBNode*)f)->Fn->Type==TYPE_VOLUME) ){
   f=f->ln_Succ;
  }
 }else if( fn->Type==TYPE_DEVICE ){
        // first run through all volumes
  while( (f->ln_Succ!=NULL)&&(((TLBNode*)f)->Fn->Type==TYPE_VOLUME) ){
   f=f->ln_Succ;
  }
        // then get the correct position
  while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig) ){
   f=f->ln_Succ;
  }
 }else if( fn->Type==TYPE_ASSIGN ){
        // first run through all volumes
  while( (f->ln_Succ!=NULL)&&
         ((((TLBNode*)f)->Fn->Type==TYPE_VOLUME)||(((TLBNode*)f)->Fn->Type==TYPE_DEVICE)) ){
   f=f->ln_Succ;
  }
        // then get the correct position
  while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig) ){
   f=f->ln_Succ;
  }
 }else{ // file or drawer:
  if( sortmode==SM_NAME ){
   if( Main->ShowDrawersFirst ){          // first the drawers, then the variables
    if( fn->Type==TYPE_DIR ){
     while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig)
            &&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
    }else if( fn->Type==TYPE_FILE ){
           // first run through all dirs
     while( (f->ln_Succ!=NULL)&&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
           // then get the correct position
     while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig) ){
      f=f->ln_Succ;
     }
    }
   }else{        // insert in normal order
    while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig) ){
     f=f->ln_Succ;
    }
   }
  }else if( sortmode==SM_SUFF ){
   if( Main->ShowDrawersFirst ){          // first the drawers, then the variables
    if( fn->Type==TYPE_DIR ){
     while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig)
            &&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
    }else if( fn->Type==TYPE_FILE ){
           // first run through all dirs
     while( (f->ln_Succ!=NULL)&&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
           // then get the correct position
     while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig) ){
      f=f->ln_Succ;
     }
    }
   }else{        // insert in normal order
    while( (f->ln_Succ!=NULL)&&(sign(StrnCmp(Main->Locale,((TLBNode*)f)->Fn->Filename,fn->Filename,-1,SC_COLLATE1))==sig) ){
     f=f->ln_Succ;
    }
   }
  }else if(sortmode==SM_SIZE ){
   if( Main->ShowDrawersFirst ){          // first the drawers, then the variables
    if( fn->Type==TYPE_DIR ){
     while( (f->ln_Succ!=NULL)&&(fsign(((TLBNode*)f)->Fn->Bytes-fn->Bytes)!=sig)
            &&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
    }else if( fn->Type==TYPE_FILE ){
           // first run through all dirs
     while( (f->ln_Succ!=NULL)&&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
           // then get the correct position
     while( (f->ln_Succ!=NULL)&&(fsign(((TLBNode*)f)->Fn->Bytes-fn->Bytes)!=sig) ){
      f=f->ln_Succ;
     }
    }
   }else{        // insert in normal order
    while( (f->ln_Succ!=NULL)&&(fsign(((TLBNode*)f)->Fn->Bytes-fn->Bytes)!=sig) ){
     f=f->ln_Succ;
    }
   }
  }else if(sortmode==SM_DATE ){
   if( Main->ShowDrawersFirst ){          // first the drawers, then the variables
    if( fn->Type==TYPE_DIR ){
     while( (f->ln_Succ!=NULL)&&(sign(CompareDates(&(((TLBNode*)f)->Fn->DateStamp),&(fn->DateStamp)))==sig)
            &&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
    }else if( fn->Type==TYPE_FILE ){
           // first run through all dirs
     while( (f->ln_Succ!=NULL)&&(((TLBNode*)f)->Fn->Type==TYPE_DIR) ){
      f=f->ln_Succ;
     }
           // then get the correct position
     while( (f->ln_Succ!=NULL)&&(sign(CompareDates(&(((TLBNode*)f)->Fn->DateStamp),&(fn->DateStamp)))==sig) ){
      f=f->ln_Succ;
     }
    }
   }else{        // insert in normal order
    while( (f->ln_Succ!=NULL)&&(sign(CompareDates(&(((TLBNode*)f)->Fn->DateStamp),&(fn->DateStamp)))==sig) ){
     f=f->ln_Succ;
    }
 }}}
 return(f->ln_Pred);    // of course: we wanted the PREDATOR!
}
//<

//>"void MakeDateString( char *dest,DateStamp *ds )"
void MakeDateString( char *dest,DateStamp *ds ){
 DateTime dt;
 char timestr[20];
 if( ds ){
  memcpy( &(dt.dat_Stamp),ds,sizeof(struct DateStamp) );
  dt.dat_StrDate = dest;
  dt.dat_Format  = Main->DateFormat;
  if( Main->DateString ) dt.dat_Flags=DTF_SUBST;
  else dt.dat_Flags=0;
  dt.dat_StrDay=NULL;
  if( Main->DisplayTime ) dt.dat_StrTime=timestr;
  else dt.dat_StrTime=NULL;
  DateToStr( &dt );
  if( Main->DisplayTime ){
   char *ptr;
   if( ptr=strchr(dest,' ') ) *ptr=0;   // delete blanks at the end
   strcat(dest,", ");
   strcat(dest,timestr);
  }
 }else dest[0]=0;
}
//<

//>"void MakeProtString( char *dest,ULONG bits )"
void MakeProtString( char *dest,ULONG bits ){
 if( bits&FIBF_SCRIPT )  dest[0]='s'; else dest[0]='-';
 if( bits&FIBF_ARCHIVE ) dest[1]='a'; else dest[1]='-';
 if( bits&FIBF_READ )    dest[2]='-'; else dest[2]='r';
 if( bits&FIBF_WRITE )   dest[3]='-'; else dest[3]='w';
 if( bits&FIBF_EXECUTE ) dest[4]='-'; else dest[4]='e';
 if( bits&FIBF_DELETE )  dest[5]='-'; else dest[5]='d';
 dest[6] = ' ';
 dest[7] = 0;
}
//<

//>"void BuildLBList( UBYTE which,List *lblist,TPathNode *pathnode,TTabNode *tab )"
void BuildLBList( UBYTE which,List *lblist,TPathNode *pathnode,TTabNode *tab ){
  TLBNode *newnode;
  FileNode *realnode;
  int color=sizeof(TLBNode);
//  Object *image;

  tab->CurrentPath = pathnode;
  realnode=(FileNode*)pathnode->FList.lh_Head;

  Main->AvailableDirs[which]=0;
  Main->AvailableFiles[which]=0;

  while( realnode->ln_Succ ){

    if( Main->ShowIcons or (stricmp(&realnode->Filename[strlen(realnode->Filename)-5],".info") isnot 0) ){
     switch( realnode->Type ){
       case TYPE_FILE:{
         color=1L;
//         if( realnode->Link ) image=Main->FileLinkImage;
//         else image=Main->FileImage;
         Main->AvailableFiles[which]++;
         break;
       }
       case TYPE_ASSIGN:{
         color=1L;
//         image=Main->AssignImage;
         Main->AvailableDirs[which]++;
         break;
       }
       case TYPE_DEVICE:{
         if( Main->HighlightDirs ) color=2L; else color=1L;
//       image=Main->DeviceImage;
         Main->AvailableDirs[which]++;
         break;
       }
       case TYPE_DIR:{
         if( Main->HighlightDirs ) color=2L; else color=1L;
//       if( realnode->Link ) image=Main->AssignImage;
//       else image=Main->DirImage;
         Main->AvailableDirs[which]++;
         break;
       }
       case TYPE_VOLUME:{
         if( Main->HighlightDirs ) color=2L; else color=1L;
//         image=Main->VolumeImage;
         Main->AvailableDirs[which]++;
         break;
       }
     }

    //      make node:
     newnode=(TLBNode *)AllocListBrowserNode(6,LBNA_NodeSize,sizeof(TLBNode),TAG_END);
     newnode->Fn=realnode;
     SetListBrowserNodeAttrs( (Node *)newnode,
                               LBNA_Column,0,
                               Main->ShowImages ? LBNCA_Image : TAG_IGNORE,realnode->ListerImage,
                               !Main->ShowImages ? LBNCA_Text : TAG_IGNORE,"",
                               LBNCA_Justification,LCJ_RIGHT,
                               LBNA_Column,1,LBNCA_Text,realnode->Filename,
                               LBNA_Flags,LBFLG_CUSTOMPENS,LBNCA_FGPen,color,
                               LBNA_Column,2,LBNCA_Text,realnode->Size,
                               LBNCA_Justification,LCJ_RIGHT,
                               LBNA_Flags,LBFLG_CUSTOMPENS,LBNCA_FGPen,color,
                               LBNA_Column,3,LBNCA_Text,realnode->Date,
                               LBNA_Flags,LBFLG_CUSTOMPENS,LBNCA_FGPen,color,
                               LBNA_Column,4,LBNCA_Text,realnode->Prot,
                               LBNA_Flags,LBFLG_CUSTOMPENS,LBNCA_FGPen,color,
                               LBNA_Column,5,LBNCA_Text,realnode->Comment,
                               LBNA_Flags,LBFLG_CUSTOMPENS,LBNCA_FGPen,color,
                               LBNCA_Justification,LCJ_LEFT,
                               TAG_END
     );

    //      insert in List:
     Insert( lblist, (Node*)newnode,
             GetPreNode(lblist,newnode,tab->SortMode,tab->SortReverse) );
    }
    realnode = (FileNode*)realnode->ln_Succ;
  }
}
//<

//>"FileNode *InsertPathFile( pn,file,size,type,bits,ds,bytes,link,comment=NULL )"
FileNode *InsertPathFile( TPathNode *pn,char *file,char *size,UBYTE type,ULONG bits,DateStamp *ds,ULONG bytes,BOOL link, char *comment=NULL ){
 FileNode *newnode;

 newnode=(FileNode *)AllocVec( sizeof(FileNode),MEMF_CLEAR );
 strncpy( newnode->Filename,file,107 );
 strncpy( newnode->Size,size,39 );
 newnode->Bytes=bytes;
 if( ds ){
  memcpy( &(newnode->DateStamp),ds,sizeof(struct DateStamp) );
  MakeDateString( newnode->Date,ds );
 }else newnode->Date[0]=0;
 newnode->Bits=bits;
 if( comment ) strcpy( newnode->Comment,comment );
 else newnode->Comment[0] = 0;
 newnode->Link=link;
 MakeProtString( newnode->Prot,bits );
 newnode->Type=type;

 AddTail( &pn->FList, (Node*)newnode );
 return( newnode );
}
//<

//>"void SetVolumeSizeStr( char *str, InfoData id )"
/****************************************/
/*                                      */
/* Function: SetVolumeSizeStr           */
/*                                      */
/* Aim:      Store size of the volume   */
/*           indicated by id in the     */
/*           string str                 */
/*           Format:                    */
/*            x KB (y% used)            */
/*             (^ or MB or GB)          */
/* Input:    str = pointer to a buffer  */
/*            for the result            */
/*           id = pointer to the data   */
/*            from the volume, returned */
/*            from Info()               */
/*                                      */
/****************************************/
void SetVolumeSizeStr( char *str, InfoData *id ){
 char s[100];
 // get the used per cent:
 float percent=(100*(float)id->id_NumBlocksUsed)/id->id_NumBlocks;
 // round it:
 if( percent-int(percent)<0.5 ) percent=int(percent);
 else percent=int(percent)+1;
 // and the size:
 float size=(float)id->id_NumBlocks*id->id_BytesPerBlock;
 if( size>10000000000 ) sprintf( s,"%1.1f GB",size/(1<<30) );
 else if( size>10000000 ) sprintf( s,"%1.1f MB",size/(1<<20) );
 else sprintf( s,"%1.1f KB",size/(1<<10) );

 // and now all together:
 sprintf( str,"%s (%1.0f%% %s)",s,percent,
   GetCatalogStr(Main->Catalog,TXT_USED,TXT_USED_STR));
}
//<

//>"TPathNode *ReadDirIntoList( char *path )"
TPathNode *ReadDirIntoList( char *path ){
 FileInfoBlock *fib;
 char *catstr,Size[512];
 BOOL end=FALSE, lha=FALSE, Link;
 BPTR lock;
 TPathNode *pn;
 TArchive Archive;
 FileNode *fn;
 char FileName[512],*fileptr;

 catstr=GetCatalogStr(Main->Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR);
 if( !(fib=(FileInfoBlock *)AllocDosObject( DOS_FIB,NULL )) ) return(NULL);

 if( lock=Lock(path,ACCESS_READ) ){
   Examine( lock, fib );
   if( fib->fib_DirEntryType<0 ){
     // found a file, so we probably shall read an archive:
     UnLock( lock );
     lha=TRUE;
   }
 }else if( path[strlen(path)-1]!=':' ) lha=TRUE;

 if( lha ){
   lock=Archive.Lock(path);
   if( lock ) Archive.Examine( fib );
 }

 if( lock ){
   pn=(TPathNode*)AllocVec( sizeof(TPathNode),MEMF_CLEAR );
   strcpy( pn->Path,path );
   strcpy( FileName,path );
   AdjustPath( FileName );
   fileptr=&FileName[strlen(FileName)];
   NewList( &pn->FList );
   pn->ReadTime = fib->fib_Date;
   if( lha || (!Main->UseExAll) ){
     while( !end ){
      if( lha ) end=!Archive.ExNext( fib );
      else end=!ExNext( lock,fib );
      if( !end ){
       if( fib->fib_DirEntryType>0 ){
        if( fib->fib_EntryType is ST_SOFTLINK ) Link = TRUE;
        else Link = FALSE;
        fn=InsertPathFile( pn,fib->fib_FileName,catstr,TYPE_DIR,
                        fib->fib_Protection,&(fib->fib_Date),fib->fib_Size,Link,fib->fib_Comment ); // insert directory
        fn->ListerImage = Main->DirImage.IntuiImage();
       }else{
        if( fib->fib_EntryType is ST_LINKFILE ) Link = TRUE;
        else Link = FALSE;
        FormatSize( Size,fib->fib_Size );
        pn->Size += fib->fib_Size;
        fn=InsertPathFile( pn,fib->fib_FileName,Size,TYPE_FILE,
                        fib->fib_Protection,&(fib->fib_Date),fib->fib_Size,Link,fib->fib_Comment );
        if( Main->ClassImages ){
          strcpy( fileptr,fib->fib_FileName );
          fn->ListerImage = caFileTypes->GetFileTypeImage( FileName,Main->Scr,Main->CheckOnlyPatterns );
        }
        if( !fn->ListerImage ) fn->ListerImage = Main->FileImage.IntuiImage();
       }
       pn->Count++;
     }}
   }else{
     // The same with ExAll : seems to be not faster, so why do it?
     ExAllControl *eac = (ExAllControl*)AllocDosObject( DOS_EXALLCONTROL,NULL );
     ExAllData *buffer = (ExAllData*)AllocVec( Main->ExAllFiles*sizeof(ExAllData),MEMF_CLEAR );
     BOOL more;
     ExAllData *ead;
     if( eac ){

      eac->eac_LastKey = 0;
      do{
       more = ExAll( lock, buffer, Main->ExAllFiles*sizeof(ExAllData), ED_COMMENT, eac );
       if( (!more) && (IoErr() != ERROR_NO_MORE_ENTRIES) ){
        // ExAll failed abnormally
        break;
       }
       if( eac->eac_Entries == 0 ){
        // ExAll failed normally with no entries
        continue;                   // ("more" is *usually* zero)
       }
       ead = (ExAllData*) buffer;
       do{
        if( ead->ed_Type>0 ){
         if( ead->ed_Type is ST_SOFTLINK ) Link = TRUE;
         else Link = FALSE;
         fn=InsertPathFile( pn,ead->ed_Name,catstr,TYPE_DIR,
                         ead->ed_Prot,(DateStamp*)&(ead->ed_Days),ead->ed_Size,Link,ead->ed_Comment ); // insert directory
         fn->ListerImage = Main->DirImage.IntuiImage();
        }else{
         if( ead->ed_Type is ST_LINKFILE ) Link = TRUE;
         else Link = FALSE;
         FormatSize( Size,ead->ed_Size );
         fn=InsertPathFile( pn,ead->ed_Name,Size,TYPE_FILE,
                         ead->ed_Prot,(DateStamp*)&(ead->ed_Days),ead->ed_Size,Link,ead->ed_Comment );
         if( Main->ClassImages ){
           strcpy( fileptr,ead->ed_Name );
           fn->ListerImage = caFileTypes->GetFileTypeImage( FileName,Main->Scr,Main->CheckOnlyPatterns );
         }
         if( !fn->ListerImage ) fn->ListerImage = Main->FileImage.IntuiImage();
        }
        pn->Count++;

        // get next ead
        ead = ead->ed_Next;
       }while( ead );

      }while(more);
     }
     if( eac ) FreeDosObject(DOS_EXALLCONTROL,eac);
     if( buffer ) FreeVec(buffer);
   }// if UseExAll
   if( lha ) Archive.UnLock();
   else UnLock( lock );
   // now set the NotifyRequest:
   pn->NotiReq.nr_Name  = pn->Path;
   pn->NotiReq.nr_Flags = NRF_SEND_MESSAGE;// | NRF_WAIT_REPLY;
   pn->NotiReq.nr_stuff.nr_Msg.nr_Port = Main->NotifyPort;
   pn->NotiReq.nr_UserData = (ULONG)pn;
   AddTail( &Main->Paths,(Node*)pn );
   Main->PathCount++;
  // pn->NotifyOn = StartNotify( &pn->NotiReq );
   pn->NotifyOn = FALSE;
 }// if lock
 FreeDosObject( DOS_FIB,fib );
 return( pn );
}
//<

//>"TPathNode *ReadVolumesIntoList()"
TPathNode *ReadVolumesIntoList(){
 DosList *dl;
 char    *Filename,*Size,*Name;
 ULONG   code;
 BPTR    lock;
 TPathNode *pn;
 FileNode *fn;
 ULONG   stdbits=FIBF_READ | FIBF_WRITE | FIBF_EXECUTE | FIBF_DELETE ;

#ifdef BOBERG_TEST
 DebOut << "lese volumes" << endl;
#endif

 Filename=(char*)AllocVec( 512,MEMF_CLEAR );
 Size=(char*)AllocVec( 256,MEMF_CLEAR );

 pn=(TPathNode*)AllocVec( sizeof(TPathNode),MEMF_CLEAR );
 strcpy( pn->Path,"" );
 NewList( &pn->FList );

 dl=LockDosList(LDF_VOLUMES | LDF_READ);       // first the volumes
 InfoData *infd=(InfoData*)AllocVec( sizeof(InfoData),MEMF_CLEAR );
 if( !infd ){ Alert( AG_NoMemory ); return( (TPathNode*)Main->Paths.lh_Head ); }
 while( dl=NextDosEntry(dl, LDF_VOLUMES) ){
  Name=(char*)BADDR(dl->dol_Name);
  if( Name[0]<256 ){
   memcpy( &Filename[0], Name + 1, Name[0] );
   Filename[Name[0]]=(char)0;
   strcat( Filename,":" );
   code=0;
   lock=Lock( Filename,ACCESS_READ );
   if( lock ){
    if( Info( lock,infd ) ){
     SetVolumeSizeStr( Size,infd );
     code=infd->id_NumBlocks;
    }else strcpy( Size,GetCatalogStr(Main->Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR) );
    UnLock( lock );
   }else strcpy( Size, GetCatalogStr(Main->Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR) );
   fn = InsertPathFile( pn,Filename,Size,TYPE_VOLUME,stdbits,NULL,code,FALSE );
   fn->ListerImage = Main->VolumeImage.IntuiImage();
   pn->Count++;

 }}
 UnLockDosList( LDF_VOLUMES | LDF_READ );
 FreeVec( infd );

 if( Main->ShowDevices ){
  dl=LockDosList(LDF_DEVICES | LDF_READ);       // and the devices
  while( dl=NextDosEntry(dl, LDF_DEVICES) ){
   Name=(char*)BADDR(dl->dol_Name);
   if( (Name[0]<256)&& dl->dol_Task ){          // I think we should take this as decision
    memcpy( &Filename[0], Name + 1, Name[0] );
    Filename[Name[0]]=(char)0;
    strcat( Filename,":" );
    fn = InsertPathFile( pn,Filename,GetCatalogStr(Main->Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR),
                    TYPE_DEVICE,stdbits,NULL,0,FALSE );
    fn->ListerImage = Main->DeviceImage.IntuiImage();
    pn->Count++;
  }}
  UnLockDosList(LDF_DEVICES | LDF_READ);
 }

 dl=LockDosList(LDF_ASSIGNS | LDF_READ);       // then the assigns
 while( dl=NextDosEntry(dl, LDF_ASSIGNS) ){
  Name=(char*)BADDR(dl->dol_Name);
  if( Name[0]<256 ){
   memcpy( &Filename[0], Name + 1, Name[0] );
   Filename[Name[0]]=(char)0;
   strcat( Filename,":" );
   fn = InsertPathFile( pn,Filename,GetCatalogStr(Main->Catalog,TXT_UNKNOWN,TXT_UNKNOWN_STR),
                   TYPE_ASSIGN,stdbits,NULL,0,FALSE );
   fn->ListerImage = Main->AssignImage.IntuiImage();
   pn->Count++;
 }}
 UnLockDosList( LDF_ASSIGNS | LDF_READ );

 AddTail( &Main->Paths,(Node*)pn );
 Main->PathCount++;
 pn->NotifyOn = FALSE;
 FreeVec( Filename );
 FreeVec( Size );
 return( pn );
}
//<

//>"void SetCurrentTabTitle( UBYTE which,char *newtitle )"
void SetCurrentTabTitle( UBYTE which,char *newtitle ){
  if( CurrentTab[which]->Gadget ){
    char *ptr;
    // we will only show the last directory level:
    ptr = FilePart( newtitle );
    if( *ptr is 0 ) ptr = newtitle; // it's a volume
    SetGadgetAttrs( CurrentTab[which]->Gadget,Main->IntuiWin,NULL,GA_Text,ptr,TAG_END );
  }
}
//<

//>"TPathNode *GetPathNode( char *path, BOOL read=TRUE )"
TPathNode *GetPathNode( char *path, BOOL read ){
 // First look, if the path has already been read:
 TPathNode *pn=(TPathNode*)Main->Paths.lh_Head;
 while( pn->ln_Succ && (stricmp(path,pn->Path)!=0) ) pn=(TPathNode*)pn->ln_Succ;
 if( pn->ln_Succ ) return( pn );
 else{
  if( !read ) return( NULL ); // not found !!
  // else insert because not there:
  if( Main->PathCount==Main->MaxPaths ) RemovePathNode();
  if( path[0]==0 ) return( ReadVolumesIntoList() );
  else return( ReadDirIntoList(path) );
 }
}
//<

//>"BOOL ListDir( UBYTE which,char *path,int check,int pos=0 )"
/****************************************/
/*                                      */
/* Function: ListDir                    */
/*                                      */
/* Aim:      Read and show the given    */
/*           dir                        */
/* Input:    which=0 or 1, number of    */
/*            the browser               */
/*           path=Directory to show     */
/*           path="" for devices        */
/*           path=NULL: get from        */
/*            string-gadget             */
/*           check=0 only this browser  */
/*           check=1 also the other     */
/*            if it has the same path   */
/*           pos=-1 show the last pos   */
/*            at the top                */
/*           pos=0 show the top of list */
/*           pos=n show node n at top   */
/*                                      */
/****************************************/
BOOL ListDir( UBYTE which,char *path,int check,ULONG top ){
 char     *Filename, *Size, *RealPath;
 List     *lblist;
 ULONG    stdbits=FIBF_READ | FIBF_WRITE | FIBF_EXECUTE | FIBF_DELETE;
 ULONG    Oldtop;
 int      PathProp;
 char     *path2;
 BOOL     PathsEqual=FALSE,lha=FALSE;
 TTabNode *tn;
 Object   *lb=GetListBrowser( which );
 TPathNode *LastPath;

 Filename=(char*)AllocVec( 512,MEMF_CLEAR );
 Size=(char*)AllocVec( 256,MEMF_CLEAR );
 RealPath=(char*)AllocVec( 512,MEMF_CLEAR );


 if( !Filename || !Size || !RealPath ){
  Alert( AG_NoMemory );
  return( FALSE );
 }

 tn=CurrentTab[which];

 GetAttr( LISTBROWSER_Top,lb, &Oldtop ); // remember the position

 if( path is NULL ) path = tn->Path; // get the path from the tab

 if( LastPath=GetPathNode(tn->Path) ) LastPath->FirstOnList=Oldtop; // remember the position

 strcpy( RealPath,path ); // store the path because it could be changed


 if( RealPath[0] isnot 0 ){
   PathProp = CheckPath(RealPath,Filename);
   if (PathProp==PATH_PARTEXISTS) lha=TRUE;
   // check if path does exist, else return
   if( (lha&&!Main->LhaPossible) || (PathProp==PATH_NOTFOUND) ){
     if( which is 0 ) SetGadgetAttrs( (Gadget *)Main->Gadgets[STR_PATH_L],Main->IntuiWin, NULL,
                                    STRINGA_TextVal, tn->Path, TAG_DONE );
     else SetGadgetAttrs( (Gadget *)Main->Gadgets[STR_PATH_R],Main->IntuiWin, NULL,
                          STRINGA_TextVal, tn->Path, TAG_DONE );
     Info( GetCatalogStr(Main->Catalog,ERR_WRONGPATH,ERR_WRONGPATH_STR) );
     FreeVec( Filename );
     FreeVec( Size );
     FreeVec( RealPath );
     return( FALSE );
   }
 }
 SetAttrs( Main->Win, WA_BusyPointer, TRUE, TAG_END );

 // update the string gadget and store the path in the current tab:
 if( which is 0 ){
  SetGadgetAttrs( (Gadget *)Main->Gadgets[STR_PATH_L],Main->IntuiWin, NULL,
                                STRINGA_TextVal, RealPath, TAG_DONE );
  GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_R],(ULONG*)&path2 );
 }else{
  SetGadgetAttrs( (Gadget *)Main->Gadgets[STR_PATH_R],Main->IntuiWin, NULL,
                      STRINGA_TextVal, RealPath, TAG_DONE );
  GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_L],(ULONG*)&path2 );
 }
 strcpy( tn->Path,RealPath );
 tn->LhaActive = lha;   // remember if we are in an archive
 SetCurrentTabTitle( which,RealPath );

 if( stricmp(RealPath,path2) is 0 ) PathsEqual=TRUE; // paths equal, we must update both

 // get the listbrowser list, deattach  and empty it:
 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );
 FreeListBrowserList( lblist );

 // now construct the listbrowser list from the path list:
 BuildLBList( which,lblist,GetPathNode(RealPath),tn );

 // and get the position if wanted:
 if( top is -1){
  if( LastPath=GetPathNode(tn->Path) ) top=LastPath->FirstOnList;
  else top=0;
 }

 // adapt the parent gadget:
 if( Main->ShowParent ){
  if( RealPath[0]==0 ){ // devices == top level, disable parent gadget
   if( which==0 ){
    SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_PARENT_L],Main->IntuiWin,NULL,
                        GA_Image,Main->Gadgets[PENMAP_PAR2],TAG_END );
    if( Main->IntuiWin ) RefreshGList( (Gadget*)Main->Gadgets[BUTTON_PARENT_L],Main->IntuiWin,NULL,1 );
   }else{
    SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_PARENT_R],Main->IntuiWin,NULL,
                        GA_Image,Main->Gadgets[PENMAP_PAR4],TAG_END );
    if( Main->IntuiWin ) RefreshGList( (Gadget*)Main->Gadgets[BUTTON_PARENT_R],Main->IntuiWin,NULL,1 );
   }
  }else{                // normal dir
   if( which==0 ){
    SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_PARENT_L],Main->IntuiWin,NULL,
                         GA_Image,Main->Gadgets[PENMAP_PAR1],TAG_END );
    if( Main->IntuiWin ) RefreshGList( (Gadget*)Main->Gadgets[BUTTON_PARENT_L],Main->IntuiWin,NULL,1 );
   }else{
    SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_PARENT_R],Main->IntuiWin,NULL,
                         GA_Image,Main->Gadgets[PENMAP_PAR3],TAG_END );
    if( Main->IntuiWin ) RefreshGList( (Gadget*)Main->Gadgets[BUTTON_PARENT_R],Main->IntuiWin,NULL,1 );
  }}
 }
 // set the correct position, if wanted, and attach the list again:
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Top,top,TAG_DONE );
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels,lblist,
                  IsListEmpty(lblist) ? TAG_IGNORE : LISTBROWSER_AutoFit,TRUE,
                  LISTBROWSER_Selected,-1L,
                  TAG_DONE );
 if( PathsEqual && (check is 1) ) ListDir( 1-which,NULL,0 );
 Main->SelectedFileSize[which] = 0;
 ListHelp( which );
 if( RealPath[0] ) AdjustHistory( RealPath );
 SetAttrs( Main->Win,WA_BusyPointer,FALSE,TAG_END );
 FreeVec( Filename );
 FreeVec( Size );
 FreeVec( RealPath );
}
//<

//>"void Refresh( UBYTE which )"
void Refresh( UBYTE which ){
 char  *path=GetPath(which);
 ULONG Position;
 char  *Archive=new char[512];
 strcpy( Archive,path );

 // Inside archive -> remove from buffer
 if( CheckPath(Archive,Archive)==PATH_PARTEXISTS ) ArchiveList->RemoveList( Archive );
 delete [] Archive;

 // First look, if the path has already been read:
 TPathNode *pn=(TPathNode*)Main->Paths.lh_Head;
 while( pn->ln_Succ && (stricmp(path,pn->Path)!=0) ) pn=(TPathNode*)pn->ln_Succ;
 if( !pn->ln_Succ ) return;

 RemovePathNode( pn );
 GetAttr( LISTBROWSER_Top,GetListBrowser(which), &Position ); // remember the position
 ListDir( which,NULL,1,Position ); // with other-browser-check
}
//<

//>"void ShowOtherTab( UBYTE which,UBYTE num )"
void ShowOtherTab( UBYTE which,UBYTE num ){
 int i;
 char *OldPath;
 TTabNode *tn = (TTabNode*)Main->Tabs[which].lh_Head;
 for( i=0;i<num;i++ ) tn = (TTabNode*)tn->ln_Succ;
 if( CurrentTab[which] isnot tn ){
  if( CurrentTab[which]->Gadget )
   SetGadgetAttrs( CurrentTab[which]->Gadget,Main->IntuiWin,NULL,GA_Selected,FALSE,TAG_DONE );
  CurrentTab[which]=tn;
  i=-1;         // to be ...
 }
 if( Main->CurrentList isnot which ) ActivateBrowser( which );
 if( i=-1 ){    // ... continued
  if( CurrentTab[which]->Gadget )
   SetGadgetAttrs( CurrentTab[which]->Gadget,Main->IntuiWin,NULL,GA_Selected,TRUE,TAG_DONE );
  if( which is 0 ) GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_L],(ULONG*)&OldPath );
  else GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_R],(ULONG*)&OldPath );
  if( strcmp(OldPath,tn->Path) isnot 0 ) ListDir( which,NULL,0 );
 }
}
//<

//>"BOOL ChoixDirList( UBYTE which )"
/****************************************/
/*                                      */
/* Function: ChoixDirList               */
/*                                      */
/* Aim:      Changes in the lister      */
/*           so we must update gadgets  */
/*                                      */
/****************************************/
BOOL ChoixDirList( UBYTE which ){
  ULONG selected;
  ActivateBrowser( which );
  Object *lb = GetListBrowser( which );
  List *labels;
  Node *nd;
  Main->SelectedFileSize[which]  = 0;
  Main->SelectedFiles[which]     = 0;
  Main->SelectedDirs[which]      = 0;
  GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&labels );
  nd = labels->lh_Head;
  while( nd->ln_Succ ){
    GetListBrowserNodeAttrs( nd,LBNA_Selected, &selected,TAG_END );
    if( selected ){
      Main->SelectedFileSize[which] += ((TLBNode*)nd)->Fn->Bytes;
      if( ((TLBNode*)nd)->Fn->Type is TYPE_FILE ) Main->SelectedFiles[which]++;
      else Main->SelectedDirs[which]++;
    }
    nd = nd->ln_Succ;
  }
  ListHelp(which);
  return(TRUE);
}
//<

//>"void LhaPopup( char *file )"
void LhaPopup( char *file ){
 char tmpf[300];
 BOOL bConfirmReplace=Main->ConfirmReplace;
 SetAttrs( Main->Win,WA_BusyPointer,TRUE,TAG_END );
 sprintf( tmpf,"t:%s",FilePart(file) );
 Main->ConfirmReplace = FALSE;           // don't ask for replace ...
 CopyArchiveFile( file,tmpf );
 Main->ConfirmReplace = bConfirmReplace; // ... and reset the state
 SetAttrs( Main->Win,WA_BusyPointer,FALSE,TAG_END );
 OpenActionMenu( caFileTypes->GetFileType(tmpf,&tmpf[2],FALSE),tmpf );
}
//<

//>"void LhaFileDoubleClick( char *file )"
void LhaFileDoubleClick( char *file ){
 char tmpf[300];
 BOOL bConfirmReplace=Main->ConfirmReplace;
 SetAttrs( Main->Win,WA_BusyPointer,TRUE,TAG_END );
 sprintf( tmpf,"t:%s",FilePart(file) );
 Main->ConfirmReplace = FALSE;           // don't ask for replace ...
 CopyArchiveFile( file,tmpf );
 Main->ConfirmReplace = bConfirmReplace; // ... and reset the state
 SetAttrs( Main->Win,WA_BusyPointer,FALSE,TAG_END );
 TFileType *ft = caFileTypes->GetFileType( tmpf,&tmpf[2],FALSE );
 if( ft->ActionCount>0 ) Runner( (TAction*)ft->Actions.lh_Head,tmpf );
}
//<

//>"void DoubleClickDirList()"
/****************************************/
/*                                      */
/* Function: DoubleClickDirList         */
/*                                      */
/* Aim:      We have dbl clicked on a   */
/*           file/dir on files LV       */
/*                                      */
/* Input:    which: clicked list (0/1)  */
/*                                      */
/****************************************/
void DoubleClickDirList( UBYTE which ){
 char newpath[512];
 char *lhafile;
 TLBNode *node;
 ULONG column;
 Object *list;
 TTabNode *tn;
 lhafile=new char[512];
 list=GetListBrowser(which);
 tn=CurrentTab[which];

 GetAttr( LISTBROWSER_SelectedNode,list,(ULONG*)&node );
 GetAttr( LISTBROWSER_RelColumn,list,&column );

 if( node ){
  // get the name of the chosen entry:
  strcpy( newpath,GetPath(which) );
  AddPart( newpath,node->Fn->Filename,500 );

  if( column < 2 ){ // perform standard action
   if( node->Fn->Type is TYPE_FILE ){

    // if it is a file in an archive, decrunch and open popup:
    if( tn->LhaActive ){
     LhaFileDoubleClick( newpath );
    // if it is not an archive and we are not in arch mode, we can do the standard action
    }else{
     TFileType *cla = caFileTypes->GetFileType( newpath,node->Fn->Filename,FALSE );
     // if it is an archive, we can list it:
     if( cla->IsArchive ){
       ListDir( which,newpath,0 );
     }else{
       // if we haven't got the class, but XAD says it's an archive, we open it:
       if( (cla is caFileTypes->Unknown) and (XAD_IsArchive(newpath)) ){
         ListDir( which,newpath,0 );
       }else{
         // now we have only one chance: execute the first action, if there is one:
         if( cla->ActionCount>0 ) Runner( (TAction*)cla->Actions.lh_Head,newpath );
       }
     }
    }
   }else{       // Simply list the dir:
    ListDir( which,newpath,0 );
   }
  }else if( column is 2 ){ // count size
   if( node->Fn->Type is TYPE_DIR ) GetSize( FALSE );
  }else{
    // for real files open own requester:
    if( newpath[strlen(newpath)-1]!=':' ){
      if( SetFileAttributes(newpath) ) Refresh( which );
    }else WbInfo();
  }
  // WbInfo(); // show the wbinfo requester to get more info or change attributes
 }
 delete lhafile;
}
//<

//>"void TitleClickDirList( int list)"
/****************************************/
/*                                      */
/* Function: TitleClickDirList          */
/*                                      */
/* Aim:      Sort the list according    */
/*           to the clicked column      */
/*                                      */
/* Input:    list=number of clicked list*/
/*            =0 or 1                   */
/*                                      */
/****************************************/

void TitleClickDirList( int list ){
 ULONG code;
 int newmode;
 Object *lst_dir;
 TTabNode *tn=CurrentTab[list];
 char *path=tn->Path;

 if( list==0 ) lst_dir=Main->Gadgets[LST_DIR_L];
 else lst_dir=Main->Gadgets[LST_DIR_R];

 GetAttr( LISTBROWSER_RelColumn,lst_dir, &code );
 switch( code ){
  case 0: newmode = SM_SUFF; break;
  case 1: newmode = SM_NAME; break;
  case 2: newmode = SM_SIZE; break;
  case 3: newmode = SM_DATE; break;
  default: newmode=-1;
 }
 if( (path[0]==0) && (newmode!=SM_NAME) ) return;           // no resort possible!
 if( newmode!=-1 ){
  if( (newmode is tn->SortMode) ){
   tn->SortReverse=!tn->SortReverse;      // toggle direction
  }
  ULONG top;
  List *lblist;
  TLBNode *nd,*nnd;

  tn->SortMode=newmode;
  // what is on top?
  GetAttr( LISTBROWSER_Top,lst_dir, &top );
  // get the list and deattach
  GetAttr( LISTBROWSER_Labels,lst_dir, (ULONG*)&lblist );
  if( IsListEmpty(lblist) ) return;
  SetGadgetAttrs( (Gadget *)lst_dir,Main->IntuiWin, NULL,
                   LISTBROWSER_Labels,NULL,TAG_DONE );
  // now we will get the nodes in reverse order, clear the list and insert the nodes in the new order
  lblist->lh_Head->ln_Pred = NULL; // deattach head from the list
  nd  = (TLBNode*)(lblist->lh_TailPred);
  NewList( lblist );
  while( nd ){
   nnd=(TLBNode*)(nd->ln_Pred);
   Insert( lblist, (Node*)nd,
           GetPreNode(lblist,nd,tn->SortMode,tn->SortReverse) );
   nd=nnd;
  }
  SetGadgetAttrs( (Gadget *)lst_dir,NULL, NULL,LISTBROWSER_Top,top,TAG_DONE );
  SetGadgetAttrs( (Gadget *)lst_dir,Main->IntuiWin, NULL,
                   LISTBROWSER_Labels,lblist,
                   LISTBROWSER_AutoFit,TRUE,TAG_DONE );
 }
}
//<

//>"void Prefs()"
/****************************************/
/*                                      */
/* Function : Prefs                     */
/*                                      */
/* Goal     : Click on Prefs button     */
/*            Run ClassActionPrefs      */
/*                                      */
/****************************************/
void Prefs(){
 BPTR Handle=Open("NIL:",MODE_OLDFILE);
 SystemTags("MRE:Executables/ClassActionPrefs",SYS_Input,Handle,SYS_Output,NULL,SYS_Asynch,TRUE,NP_StackSize,30000,TAG_DONE);
}
//<

//>"void RequesterVers()"
/****************************************/
/*                                      */
/* Function : RequesterVers             */
/*                                      */
/* Goal     : Click on Version button   */
/*            Display it                */
/*                                      */
/****************************************/
void RequesterVers(){
 char st[1024];
 sprintf( st,GetCatalogStr( Main->Catalog,TXT_COPYRIGHT,TXT_COPYRIGHT_STR ),VERS );
 Info( st,TRUE );
}
//<

//>"BOOL CheckAbort()"
BOOL CheckAbort(){
 ULONG result,code;
 while( (result = DoMethod( Main->Win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
  if( (result & WMHI_CLASSMASK)==WMHI_GADGETUP  ){
   if( (result & RL_GADGETMASK)==BUTTON_ABORT  ) return( TRUE );
  }
 }
 return( FALSE );
}
//<

//>"void EnableAbort( BOOL enable )"
void EnableAbort( BOOL enable ){
 if( enable ){
  SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_ABORT], Main->IntuiWin,NULL,
                        GA_Image,Main->Gadgets[PENMAP_ABORT1], TAG_END );
 }else{
  SetGadgetAttrs( (Gadget*)Main->Gadgets[BUTTON_ABORT], Main->IntuiWin,NULL,
                        GA_Image,Main->Gadgets[PENMAP_ABORT2], TAG_END );
  RefreshGList( (Gadget*)Main->Gadgets[BUTTON_ABORT],Main->IntuiWin,NULL,1 );
 }
}
//<

//>"void Parent( UBYTE which )"
/****************************************/
/*                                      */
/* Function : Parent                    */
/*                                      */
/* Goal     : Click on Parent button    */
/*            get parent dir && rescan  */
/*                                      */
/****************************************/
void Parent( UBYTE which ){
 char *path,ParentPath[512],*ptr;
 char *filename;

 path=GetPath( which );
 if( path[0]!=0 ){
  strncpy( ParentPath,path,512);
  ptr=&ParentPath[strlen(ParentPath)-1];
  if( (*ptr==':')||(*ptr=='/') ) *ptr=0;
  filename=PathPart( ParentPath );
  *filename=0;
  ListDir( which,ParentPath,0 );
 }
}
//<

//>"void WbInfo()"
void WbInfo(){
 char pa[512],file[256],buffer[256],*ptr;
 Node *node;
 List *lblist;
 ULONG selected;
 BPTR lock;
 TTabNode *tn=CurrentTab[Main->CurrentList];
 Object *lb=GetCurrentListBrowser();

 if( tn->LhaActive ) return;

 strcpy( pa,GetSrc() );


 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );
 EnableAbort( TRUE );

 node=lblist->lh_Head;
 if( pa[0]==0 ) lock=NULL;
 // it's a device, so we must do a lock on each entry
 else lock=Lock( pa,ACCESS_READ );


 while( (node->ln_Succ!=NULL) && !CheckAbort() ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ){
   if( lock ){
    strcpy( file,((FileNode*)((TLBNode*)node)->Fn)->Filename );
    ptr = &(file[strlen(file)-5]);
    if( stricmp(ptr,".info")==0 ) *ptr=0;
    WBInfo( lock,file,Main->Scr );
   }else{
    if( lock=Lock( ((FileNode*)((TLBNode*)node)->Fn)->Filename,ACCESS_READ ) ){
     NameFromLock( lock,file,256 );
     if( file[strlen(file)-1]==':' ){
      WBInfo( lock,"disk",Main->Scr );
      UnLock( lock );
     }else{
      strcpy(buffer,file);
      ptr=PathPart(buffer);*ptr=0;
      UnLock( lock );
      if( lock=Lock( buffer, ACCESS_READ ) ){
       WBInfo( lock,FilePart(file),Main->Scr );
       UnLock( lock );
     }}
     lock=NULL;
    }
   }
   SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
  }
  node=node->ln_Succ;
 }
 if( lock ) UnLock( lock );
 EnableAbort( FALSE );
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,LISTBROWSER_Labels, lblist, TAG_DONE );
}
//<

//>"void WbInfoDir()"
void WbInfoDir(){
 char Path[512],File[256],*ptr;
 strcpy( Path,GetSrc() );
 if( !Path[0] ) return;
 if( caFileTypes->ContainsArchive(Path,File) ) return;

 BPTR lock;
 if( Path[strlen(Path)-1] isnot ':' ){
  ptr = FilePart( Path );
  strcpy( File,ptr );
  *ptr = 0;     // get the parent dir !
  lock = Lock( Path,ACCESS_READ );
 }else{
  strcpy( File,"Disk" );
  lock = Lock( Path,ACCESS_READ );
 }
 if( lock ){
  WBInfo( lock,File,Main->Scr );
  UnLock( lock );
 }
}
//<

//>"void AddIcon()"
void AddIcon(){
 char pa[512],fullname[1024],reqmsg[256];
 ULONG selected,deficon;
 BOOL success=FALSE;
 int n,count=0;
 Node *node;
 List *lblist;
 DiskObject *dobj;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 if( tn->LhaActive ) return;

 strcpy( pa,GetSrc() );
 AdjustPath( pa );

 GetAttr( LISTBROWSER_NumSelected,lb, (ULONG*)&n );
 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );
 EnableAbort( TRUE );

 node=lblist->lh_Head;
 if( n>1 ){
  SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Level,0,FUELGAUGE_Max,n,
                   GA_Text,(ULONG)GetCatalogStr(Main->Catalog,TXT_COPYING,TXT_COPYING_STR),TAG_DONE );
 }
 while( (node->ln_Succ!=NULL) && !CheckAbort() ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ){
   sprintf(fullname,"%s%s",pa,((FileNode*)((TLBNode*)node)->Fn)->Filename);
   // test, if we will replace a file!
   dobj=GetIconTags( fullname, ICONGETA_FailIfUnavailable, FALSE, ICONGETA_IsDefaultIcon,&deficon,TAG_END );
   if( !deficon ){
    sprintf( reqmsg,"%s\n%s",((FileNode*)((TLBNode*)node)->Fn)->Filename,
             GetCatalogStr(Main->Catalog,TXT_HASICON,TXT_HASICON_STR) );
    Info( reqmsg );
   }else{
    PutIconTags( fullname,dobj,TAG_END );
    success=TRUE;
   }
   count++;
   if( n>1 ) SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                             FUELGAUGE_Level,count, TAG_DONE );
  }
  node=node->ln_Succ;
 }
 if( n>1 ){
  SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Level,0,GA_Text," ",TAG_DONE );
 }
 EnableAbort( FALSE );
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist, TAG_DONE );
 if( success&&Main->ShowIcons ) Refresh( listn );
}
//<

//>"void RenameFile( TFileList *list )"
void RenameFile( TFileList *list ){
 char pa[512],real[109],src[512],dest[512];
 Node *node;
 List *lblist;
 ULONG selected;
 BOOL  device=FALSE,success=FALSE,go=FALSE;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 if( tn->LhaActive ) return;

 list->ReadSource = TRUE;

 strcpy( pa,GetSrc() );
 if( pa[0] is 0 ){
   device=TRUE;
   if( list->Command is IC_ADDSUFFIX ) return; // can't add a suffix to a volume/device etc.!
 }
 if( !device ) AdjustPath( pa );

 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );
 EnableAbort( TRUE );

 node=lblist->lh_Head;

 while( (node->ln_Succ!=NULL) && !CheckAbort() ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ){
   strncpy( real,((TLBNode*)node)->Fn->Filename,108 );
   sprintf( src,"%s%s",pa,real );
   // when a device, then only real volumes
   if( device ){
    if( (((TLBNode*)node)->Fn->Type isnot TYPE_ASSIGN) and
        (((TLBNode*)node)->Fn->Type isnot TYPE_DEVICE) ){
     real[strlen(real)-1]=0; // exclude ':'
     if( GetText(real,108,GetCatalogStr(Main->Catalog,TXT_RENAME,TXT_RENAME_STR)) ){
      // we strip ':' from src and look if the name has changed:
      strcpy( dest,src );
      dest[strlen(dest)-1]=0;
      if( strcmp(dest,real)!=0 ){
       success=Relabel( src, real );
      }
     }
    }
   }else{
     go=FALSE;
     if( list->Command is IC_RENAME ){
       go = GetText(real,108,GetCatalogStr(Main->Catalog,TXT_RENAME,TXT_RENAME_STR));
       sprintf(dest,"%s%s",pa,real);
     }else{
       strcpy(dest,src);
       go = caFileTypes->AddSuffix( dest,caFileTypes->GetFileType(dest,real,FALSE) )
          and (stricmp(dest,src)!=0);
     }
     if( go ){
       if( strcmp(src,dest)!=0 ){
        success=Rename( src, dest );
        SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );

        if( success and (!device) and (!strstr(src,".info")) ){
          // Do for the Icon as well
          strcat(dest,".info");
          strcat(src,".info");
          Rename( src, dest );
         }
       }
     }
   }
   if( go and not success ){
     char *sTemp=new char[256];
     sprintf( sTemp,GetCatalogStr(Main->Catalog,MSG_NORENAME,MSG_NORENAME_STR),src);
     Info( sTemp );
     delete [] sTemp;
   }
  }
  node=node->ln_Succ;
 }
 EnableAbort( FALSE );
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,LISTBROWSER_Labels, lblist, TAG_DONE );
}
//<

//>"int AskReplace( char *f1, char *f2, char *reqtext )"
/****************************************/
/*                                      */
/* Function: AskReplace                 */
/*                                      */
/* Aim:      Show a requester with      */
/*           information about two      */
/*           files and with buttons     */
/*           given by reqtext, and      */
/*           the pressed button         */
/*                                      */
/* Input:    f1: file that replaces     */
/*           f2: file that would be     */
/*               replaced               */
/*                                      */
/* Output:   number of the pressed      */
/*            button, 1,2,...,n-1,0     */
/*                                      */
/****************************************/
int AskReplace( char *f1, char *f2, char *reqtext ){
 char *vers1=new char[100],*vers2=new char[100];
 char *fi1=new char[100];
 char *fi2=new char[100];
 char *reqmsg=new char[1000];
 int ret;

 vers1[0]=0; vers2[0]=0;
 if( Main->ShowVersion or Main->ReplaceVersion ){
   if( GetVersion(f1,vers1) and GetVersion(f2,vers2) ){
     if( Main->ReplaceVersion ){
       float fVers1=atof(vers1);
       float fVers2=atof(vers2);
       if( fVers1>fVers2 ) ret=REPLACE_ONE; else ret=REPLACE_NOT;
     }
   }
 }
 if( not Main->ReplaceVersion ){
   ret=Main->CurrentList;
   StoreFib( f1, fi1, CurrentTab[ret]->LhaActive ? TRUE : FALSE );
   StoreFib( f2, fi2, CurrentTab[1-ret]->LhaActive ? TRUE : FALSE );
   if( Main->ShowVersion and vers1[0] and vers2[0] ){
     sprintf( reqmsg,"%s\n\n%s    ?\n\n%s %s %s, %s\n%s %s %s, %s",
            GetCatalogStr(Main->Catalog,TXT_FILEEXISTS,TXT_FILEEXISTS_STR),f2,
            GetCatalogStr(Main->Catalog,TXT_OLDFILE,TXT_OLDFILE_STR),
            GetCatalogStr(Main->Catalog,MSG_VERSION,MSG_VERSION_STR),vers2,fi2,
            GetCatalogStr(Main->Catalog,TXT_NEWFILE,TXT_NEWFILE_STR),
            GetCatalogStr(Main->Catalog,MSG_VERSION,MSG_VERSION_STR),vers1,fi1 );
   }else{
     sprintf( reqmsg,"%s\n\n%s    ?\n\n%s %s\n%s %s",
            GetCatalogStr(Main->Catalog,TXT_FILEEXISTS,TXT_FILEEXISTS_STR),f2,
            GetCatalogStr(Main->Catalog,TXT_OLDFILE,TXT_OLDFILE_STR),fi2,
            GetCatalogStr(Main->Catalog,TXT_NEWFILE,TXT_NEWFILE_STR),fi1 );
   }
   ret=SureReq( reqmsg,reqtext );
 }
 delete [] reqmsg;
 delete [] fi2;
 delete [] fi1;
 delete [] fi1;
 delete [] vers1;
 delete [] vers2;
 return( ret );
}
//<

//>"BOOL EnoughSpaceForCopy()"
BOOL EnoughSpaceForCopy(){
 if( !Main->CheckFit or CurrentTab[1-Main->CurrentList]->LhaActive ) return( TRUE );
 // we don't know how big ^^^^^^^^ the packed files will be
 BOOL result = TRUE;
 float available,needed = GetSize(FALSE,FALSE);
 InfoData *infd=(InfoData*)AllocVec( sizeof(InfoData),MEMF_CLEAR );
 if( !infd ) return( FALSE );
 BPTR lock=Lock( GetDst(),ACCESS_READ );
 if( lock ){
  if( Info( lock,infd ) ){
   available = (infd->id_NumBlocks-infd->id_NumBlocksUsed)*(float)infd->id_BytesPerBlock;
   if( available is 0 ){
     if( (stricmp("Ram Disk:",GetDst()) is 0) or (stricmp("Ram:",GetDst()) is 0) ) result = TRUE;
     else{
       DosList *dl=(DosList*)BADDR(infd->id_VolumeNode);
       Info( (char*)BADDR(dl->dol_Name) );
       result = Req(GetCatalogStr(Main->Catalog,MSG_CONFIRMCHECKSIZE,MSG_CONFIRMCHECKSIZE_STR));
     }
   }else if( available < needed ) result = FALSE;
  }
  UnLock( lock );
 }
 FreeVec( infd );
 if( result==FALSE ) Info( GetCatalogStr(Main->Catalog,TXT_NOTENOUGHSPACE,TXT_NOTENOUGHSPACE_STR) );
 return( result );
}
//<

//>"void FileTransfer( TFileList *list )"
void FileTransfer( TFileList *list ){
 char src[512],*filename,dest[512],*destfilename;
 // with the following meanings:
 // src takes the source path and the source filename,
 // filename is a pointer to the first char of the filpart;
 // dest the complete destination, destfilename the pointer ...

 char *fuelstr,*fuelstring,temptext[110],*ptr;
 TFileEntry *node;
 BPTR lock;
 BOOL ok,success=FALSE,Abort=FALSE,ProcessIcons;
 int n=0,reqres=0;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 TTabNode *tnd=CurrentTab[1-listn];
 TArchive Archive;
 FileInfoBlock *fib;

 // quit if no files selected:
 if( list->Count is 0 ) return;

 if( tn->LhaActive ){
   if( tnd->LhaActive ) return; // quit if both lists are archives
   if( list->Command isnot FT_COPY ) return; // or if no copy from archive
 }else if( tnd->LhaActive ){
   if( (list->Command isnot FT_COPY) and (list->Command isnot FT_DELETE) and
       (list->Command isnot FT_TRASHCAN) ) return;
 }

 strcpy( src,list->Source );
 // quit if no source:
 if( src[0]==0 ) return;
 AdjustPath( src );
 filename = src + strlen(src);
 if( list->Command is FT_DUPLICATE ) strcpy( dest,list->Source );
 else if( list->Command is FT_TRASHCAN ){
   strcpy( dest,list->Source );
   if( ptr=strchr(dest,':') ) *(ptr+1)=0;
   // else ?
   strcat( dest,"trashcan" );
   if( !FileExists(dest) ){
     Info( GetCatalogStr(Main->Catalog,MSG_NOTRASHCANAVAILABLE,MSG_NOTRASHCANAVAILABLE_STR) );
     list->Command=FT_DELETE;
   }
 }else strcpy( dest,list->Destination );
 // quit if copy without dest:
 if( (dest[0]==0) and (list->Command isnot FT_DELETE) and (list->Command isnot FT_TRASHCAN) ) return;
 AdjustPath( dest );
 destfilename = dest + strlen(dest);

 // check if it fits:
 if( ((list->Command is FT_COPY)
     or (list->Command is FT_COPYAS)
     or (list->Command is FT_MOVE)
     or (list->Command is FT_MOVEAS)) ){
   // leave copies to ftp:, FTPMount or similar dirs:
   if( (not (ptr=stristr(list->Destination,"ftp"))) or (ptr isnot list->Destination) ){
     if( not EnoughSpaceForCopy() ) return;
   }
 }

 fuelstring=new char[512];
 node = (TFileEntry*)list->Files.lh_Head;

 // ask for delete:
 if( (list->Command is FT_DELETE) or (list->Command is FT_TRASHCAN) ){
   if( list->Command is FT_DELETE ){
     if( list->Count is 1 ) sprintf( fuelstring,GetCatalogStr(Main->Catalog,TXT_DELOKSINGLE,TXT_DELOKSINGLE_STR),node->Name );
     else sprintf( fuelstring,GetCatalogStr(Main->Catalog,TXT_DELOK,TXT_DELOK_STR),node->Name );
   }else{
     if( list->Count is 1 ) sprintf( fuelstring,GetCatalogStr(Main->Catalog,TXT_DELOKSINGLETRASH,TXT_DELOKSINGLETRASH_STR),node->Name );
     else sprintf( fuelstring,GetCatalogStr(Main->Catalog,TXT_DELOKTRASH,TXT_DELOKTRASH_STR),node->Name );
     list->Command = FT_COPY;
   }
   if( Main->ConfirmDelete ){
     if( SureReq(fuelstring,GetCatalogStr(Main->Catalog,TXT_YESNO,TXT_YESNO_STR)) is 0 ){
       delete [] fuelstring;
       return;
     }
   }
   list->ReadSource = TRUE;
   ProcessIcons = Main->DeleteIcons;
 }else{
   if( list->Command is FT_DUPLICATE ) list->ReadSource = TRUE;
   else{
     list->ReadDestination = TRUE;
     if( (list->Command is FT_MOVE) or (list->Command is FT_MOVEAS) ) list->ReadSource = TRUE;
   }
   ProcessIcons = Main->CopyIcons;
 }

 // count the files:
 if( Main->CountFiles ){
   SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
     GA_Text,GetCatalogStr(Main->Catalog,MSG_COUNTINGFILES,MSG_COUNTINGFILES_STR),TAG_END );
   fib = (FileInfoBlock*)AllocDosObject( DOS_FIB,TAG_END );
   while( node->ln_Succ ){
     strcpy( filename,node->Name );
     lock = Lock( src,ACCESS_READ );
     if( lock ){
       Examine( lock,fib );
       UnLock( lock );
       n += FileCount( src,fib );
     }else n++;
     node = (TFileEntry*)(node->ln_Succ);
   }
   FreeDosObject( DOS_FIB,fib );
//   if( n is 0 ) return;
   SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
     FUELGAUGE_Max,n,TAG_END );
 }
 EnableAbort( TRUE );

 node = (TFileEntry*)list->Files.lh_Head;

 list->DeselectMode = DESELECT_LIST;

 fuelstr=GetCatalogStr(Main->Catalog,TXT_COPYING,TXT_COPYING_STR);
 if( n>1 ){
   Main->Transfer.ReplaceText=GetCatalogStr( Main->Catalog,TXT_YESALLNONENO,TXT_YESALLNONENO_STR );
 }else{
   Main->Transfer.ReplaceText=GetCatalogStr( Main->Catalog,TXT_YESNO,TXT_YESNO_STR );
 }
 Main->Transfer.Buffer = AllocMem( Main->BufferSize,MEMF_CLEAR );
 Main->Transfer.ProgressText = (char*)malloc( 512 );
 Main->Transfer.Count  = 0;
 Main->Transfer.RequesterResult = 0;
 while( (not Abort) and node->ln_Succ && !CheckAbort()  ){
   if( not node->Selected ){
     node = (TFileEntry*)node->ln_Succ;
     continue;
   }
   sprintf( fuelstring,"%s %s ...",fuelstr,node->Name );
   strcpy( filename,node->Name );
   // don't copy into a subdir:
   if( ((list->Command is FT_COPY)
     or (list->Command is FT_COPYAS)
     or (list->Command is FT_MOVE)
     or (list->Command is FT_MOVEAS)) ){
     if( SubDir( src,dest ) ){
       Info( GetCatalogStr(Main->Catalog,MSG_NOCOPYTOSUBDIR,MSG_NOCOPYTOSUBDIR_STR) );
       node = (TFileEntry*)node->ln_Succ;
       continue;
     }
   }

   ok=TRUE;
   if( (list->Command is FT_DUPLICATE) or (list->Command is FT_COPYAS)
   or (list->Command is FT_MOVEAS) ){
     strcpy( temptext,node->Name );
     if( list->Command is FT_DUPLICATE ) strcat( temptext,".bak" );
     if( GetText( temptext,108,GetCatalogStr(Main->Catalog,TXT_CLONE,TXT_CLONE_STR) ) ){
       strcpy( destfilename,temptext );
     }else ok = FALSE;
   }else strcpy( destfilename,node->Name );
/*
   if( ok ){
     // test, if we will replace a file!
     if( tnd->LhaActive ){
      // as files are only added to the root, we must test archive/filename
      char *lhafile=new char[512];
      strcpy( lhafile,dest );
      Archive.ArchiveName( lhafile );
      strcat( lhafile,"/" );
      strcat( lhafile,node->Name );
      lock=Archive.Lock( lhafile );
      delete lhafile;
     }else lock=Lock(dest,ACCESS_READ);
     if( lock ){
      if( tnd->LhaActive ) Archive.UnLock();
      else UnLock( lock );
      if( Main->ConfirmReplace ){
       if( (reqres==0)||(reqres==1) ){    // we must ask the user
        reqres=AskReplace( src,dest,reqtext );
       }
       if( (reqres==0)||(reqres==3) ) ok=FALSE;
      }
     }
   }
*/
   if( ok ){
     if( (list->Command is FT_COPY) and (tn->LhaActive or tnd->LhaActive) ){
       success = CopyArchiveFile( src,dest );
     }else{
       if( ((list->Command isnot FT_MOVE) and (list->Command isnot FT_MOVEAS)) or not Rename( src,dest ) ){
         n       = MRECopy( src,dest,Main->Gadgets[FUELGAUGE],list->Command );
         Abort   = n is -1;
         success = n is 0;
       }else{
         Abort   = FALSE;
         success = TRUE;
       }
     }
     if( success ){
       node->Success  = TRUE;
       node->Selected = FALSE;
       if( (!strstr(filename,".info")) and ProcessIcons ){
         /* icon as well */
         strcat(filename,".info");
         strcat(destfilename,".info");
         if( (list->Command is FT_COPY) and (tn->LhaActive or tnd->LhaActive) ){
           success = CopyArchiveFile( src,dest );
         }else{
           if( ((list->Command isnot FT_MOVE) and (list->Command isnot FT_MOVEAS)) or not Rename( src,dest ) ){
             n       = MRECopy( src,dest,Main->Gadgets[FUELGAUGE],list->Command );
             Abort   = n is -1;
             success = n is 0;
           }else{
             Abort   = FALSE;
             success = TRUE;
           }
         }
         if( success ){
           // now deselect the .info file is it is selected
           TFileEntry *iconnode=node;
           while( iconnode->ln_Succ ){
             if( stricmp(iconnode->Name,filename)==0 ){
               iconnode->Selected = FALSE;
               Main->Transfer.Count++;
               break;
             }
             iconnode=(TFileEntry*)(iconnode->ln_Succ);
           }
         }
       }
       success=TRUE; // this "success" means at least the file is copied
/*
     }else{
       if( not Abort ){
         sprintf( fuelstring,GetCatalogStr(Main->Catalog,ERR_NOCOPY,ERR_NOCOPY_STR),node->Name );
         Info( fuelstring );
       }
*/
     }
   }
   node = (TFileEntry*)node->ln_Succ;
 }
 FreeMem( Main->Transfer.Buffer,Main->BufferSize );
 free( Main->Transfer.ProgressText );
 delete [] fuelstring;
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Level,0,GA_Text," ",TAG_DONE );
/*
 if( success ){
  if( tnd->LhaActive ){
   Archive.ArchiveName( dest );
   ListDir( 1-listn,dest,0,0 );
   Refresh( 1-listn );
  }else Refresh( 1-listn );
 }
 ChoixDirList( listn );
*/
 EnableAbort( FALSE );
}
//<

//>"BOOL MkDir( TFileList *list,UBYTE Arg1 )"
/****************************************/
/*                                      */
/* Function: MkDir( TFileList *list )   */
/*                                      */
/* Aim:      Open a window and let the  */
/*           user choose a name for a   */
/*           new directory, option for  */
/*           icon                       */
/*                                      */
/* Input:    list: standard file list   */
/*                                      */
/****************************************/
void MkDir( TFileList *list,UBYTE Arg1 ){
  BOOL end,Save=FALSE;
  Object *win,*vlayout,*gName,*gIcon;
  ULONG windowsignals,mask,code=0,result,selected;
  ULONG WithIcon;
  Window *intuiwin;
  char pa[512];
  char fullname[512];
  char command[1024];
  char *name;
  BPTR lock;

  TTabNode *tn=CurrentTab[Arg1];

  if( tn->LhaActive ) return;
  strcpy( pa,list->Source );
  if( pa[0]==0 ) return;
  AdjustPath( pa );

  SetAttrs( Main->Win, WA_BusyPointer, TRUE, TAG_END );

  // alloc vertical layout
  vlayout=(Object*)NewObject(
    LAYOUT_GetClass(),NULL,
    LAYOUT_Orientation,LAYOUT_VERTICAL,LAYOUT_DeferLayout,TRUE,
    LAYOUT_SpaceInner,TRUE,LAYOUT_SpaceOuter,TRUE,
    LAYOUT_AddChild,(Object*)NewObject(
      LAYOUT_GetClass(),NULL,
      LAYOUT_SpaceOuter,TRUE,
      LAYOUT_HorizAlignment,LALIGN_CENTER,
      LAYOUT_VertAlignment,LALIGN_CENTER,
      LAYOUT_AddImage,NewObject(LABEL_GetClass(),NULL,
        LABEL_Text,GetCatalogStr(Main->Catalog,MSG_PLEASEENTERDIRNAME,MSG_PLEASEENTERDIRNAME_STR),
        TAG_END
      ),
      TAG_END
    ),
    CHILD_WeightedHeight,100,
    // Name:
    LAYOUT_AddChild,gName=(Object*)NewObject(
      STRING_GetClass(),NULL,GA_RelVerify,TRUE,GA_ID,9,STRINGA_MaxChars,108,
      STRINGA_TextVal,"",TAG_END
    ),
    CHILD_Label,(Object*)NewObject(
      LABEL_GetClass(),NULL,LABEL_Text,GetCatalogStr(Main->Catalog,MSG_NAMEOFDIR8,MSG_NAMEOFDIR8_STR),TAG_END
    ),
    // WithIcon:
    LAYOUT_AddChild,gIcon=(Object*)NewObject(
      CHECKBOX_GetClass(),NULL,GA_Text,GetCatalogStr(Main->Catalog,MSG_ICON,MSG_ICON_STR),
      GA_Selected,Main->CreateIcons,TAG_END
    ),
    // Ok/Cancel:
    LAYOUT_AddChild,(Object*)NewObject(
      LAYOUT_GetClass(),NULL,LAYOUT_Orientation,LAYOUT_HORIZONTAL,
      LAYOUT_EvenSize,TRUE,LAYOUT_BevelStyle,BVS_SBAR_VERT,
      LAYOUT_AddChild,NewObject(
        BUTTON_GetClass(),NULL,
        GA_ID,10,GA_RelVerify,TRUE,GA_Text,GetCatalogStr(Main->Catalog,MSG_CREATE1,MSG_CREATE1_STR),
        TAG_END
      ),
      CHILD_WeightedWidth,0,
//      LAYOUT_AddChild,NewObject(SPACE_GetClass(),NULL,TAG_END),
      LAYOUT_AddChild,NewObject(
        BUTTON_GetClass(),NULL,
        GA_ID,11,GA_RelVerify,TRUE,GA_Text,GetCatalogStr(Main->Catalog,MSG_CANCEL1,MSG_CANCEL1_STR),
        TAG_END
      ),
      CHILD_WeightedWidth,0,
      TAG_END
    ),
    CHILD_WeightedHeight,0,
    TAG_END
  );

  // open window and wait till closing
  win=(Object*)NewObject( WINDOW_GetClass(),NULL,
    WINDOW_Position,WPOS_CENTERSCREEN,WINDOW_Layout,vlayout,
    WA_PubScreen,Main->Scr,WA_IDCMP,IDCMP_GADGETUP|IDCMP_CLOSEWINDOW,WA_Activate,TRUE,
    WA_Title,GetCatalogStr(Main->Catalog,MSG_MKDIR,MSG_MKDIR_STR),
    WA_CloseGadget,TRUE,WA_DragBar,TRUE,WA_DepthGadget,TRUE,WA_SizeGadget,TRUE,WA_RMBTrap,TRUE,TAG_END
  );

  DoMethod( win,WM_OPEN );
  end=FALSE;
  GetAttr( WINDOW_Window, win, (ULONG*)&intuiwin );
  GetAttr( WINDOW_SigMask, win, &windowsignals );
  ActivateLayoutGadget( (Gadget*)vlayout,intuiwin,NULL,(ULONG)gName );
  do{
   mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
   if( mask & SIGBREAKF_CTRL_C ) end=TRUE;
   while( (result = DoMethod( win, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_CLOSEWINDOW: end = TRUE; break;
     case WMHI_GADGETUP:{
      selected=(result & RL_GADGETMASK);
      switch( selected ){
        case 9: case 10:{
          end = Save = TRUE;
          GetAttr(GA_Selected,gIcon,&WithIcon);
          GetAttr(STRINGA_TextVal,gName,(ULONG*)&name);
          break;
        }
        case 11: end  = TRUE; break;
      }
      break;
     }
    }
   }
  }while( !end );
  if( Save ){
    sprintf(fullname,"%s%s",pa,name);
    if( lock=CreateDir( fullname ) ){
      UnLock( lock );
      list->ReadSource = TRUE;
      if( WithIcon ){ // Create icon as well
        sprintf(command,"C:Copy SYS:Prefs/Env-Archive/Sys/def_drawer.info %c%s.info%c",'"',fullname,'"');
        BPTR handle=Open("NIL:",MODE_OLDFILE);
        SystemTags(command,SYS_Input,handle,SYS_Output,NULL,SYS_Asynch,FALSE,TAG_DONE);
        Close(handle);
      }
    }else{
      sprintf( command,GetCatalogStr(Main->Catalog,ERR_NOMKDIR,ERR_NOMKDIR_STR),fullname );
      Info( command );
    }
  }
  DoMethod( win,WM_CLOSE );
  DisposeObject( win );
  SetAttrs( Main->Win, WA_BusyPointer, FALSE, TAG_END );

  list->DeselectMode=DESELECT_NONE;
}
//<

//>"void Pack()"
void Pack(){
 char *LhaCommand,Path[512],Dest[512],ArchiveName[110];
 Node *node;
 List *lblist;
 ULONG selected;
 UBYTE listn   = Main->CurrentList;
 TTabNode *tn  = CurrentTab[listn];
 TTabNode *tnd = CurrentTab[1-listn];
 Object *lb    = GetCurrentListBrowser();
 BPTR handler,lock,oldlock;
 int retco;

 // quit if in archive
 if( tn->LhaActive ) return;

 LhaCommand    = new char[5000];
 LhaCommand[0] = 0;

 strcpy( Path,GetSrc() );
 if( !Path[0] ) return;
 AdjustPath( Path );
 strcpy( Dest,GetDst() );
 if( !Dest[0] ) return;
 AdjustPath( Dest );

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&lblist );
 GetAttr( LISTBROWSER_NumSelected,lb,(ULONG*)&selected );
 if( selected is 0 ) return;
 ArchiveName[0] = 0;
 if( !GetText(ArchiveName,109,GetCatalogStr(Main->Catalog,TXT_CREATEARCHIVE,TXT_CREATEARCHIVE_STR)) ) return;

 SetAttrs( Main->Win,WA_BusyPointer, TRUE );
 strcat( Dest,ArchiveName );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );

 sprintf( LhaCommand,"lha -r a %c%s%c ",'"',Dest,'"' );

 node=lblist->lh_Head;
 while( node->ln_Succ!=NULL ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ){
   strcat( LhaCommand,QuoteStr );
   strcat( LhaCommand,((TLBNode*)node)->Fn->Filename );
   strcat( LhaCommand,QuoteStr );
   strcat( LhaCommand," " );
   SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
  }
  node=node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,LISTBROWSER_Labels, lblist, TAG_DONE );
 handler=Open("NIL:",MODE_OLDFILE);
 if( lock = Lock(Path,ACCESS_READ) ){
  oldlock = CurrentDir( lock );
  retco = SystemTags( LhaCommand,SYS_Input,handler,SYS_Output,NULL,SYS_Asynch,FALSE,TAG_DONE );
  if( retco isnot 0 ) DisplayBeep( Main->Scr );
  Close( handler );
  CurrentDir( oldlock );
  UnLock( lock );
  if( retco is 0 ){
   Refresh( 1-listn );
   ChoixDirList( listn );
  }
 }else DisplayBeep( Main->Scr );

 SetAttrs( Main->Win,WA_BusyPointer, FALSE );

 delete [] LhaCommand;
}
//<

//>"float GetSize( BOOL req,BOOL refresh )"
float GetSize( BOOL req,BOOL refresh ){
 char   pa[512];
 char   fullname[1024],*fuelstr,*fuelstring;
 Node   *node;
 List   *lblist;
 ULONG  selected;
 float  bytes=0,curbytes;
 BOOL   stop;
 int    n,count=0;
 FileInfoBlock *fib;
 BPTR   lock;
 TArchive Arch;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();
 FileNode *fn;

 strcpy( pa,GetSrc() );
 if( !FileExists(pa) ) return(0);
        // >>>>>>>>>>>>> we must also check if we are in the root of the archive !!!

 AdjustPath( pa );

 GetAttr( LISTBROWSER_NumSelected,lb, (ULONG*)&n );
 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
// if( refresh ) SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );
 EnableAbort( TRUE );

 fib=(FileInfoBlock *)AllocDosObject( DOS_FIB,NULL );
 fuelstring=new char[512];
 node=lblist->lh_Head;
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                 FUELGAUGE_Max,n,TAG_END );
 fuelstr=GetCatalogStr(Main->Catalog,TXT_CHECKING,TXT_CHECKING_STR);
 while( node->ln_Succ && !(stop=CheckAbort()) ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ){
   fn=((TLBNode*)node)->Fn;
   sprintf( fuelstring,"%s %s ...",fuelstr,fn->Filename );
   SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Level,count,
                   GA_Text,fuelstring,TAG_END );
   sprintf( fullname,"%s%s",pa,fn->Filename );
   lock=Lock( fullname,ACCESS_READ );
   if( lock ){
    Examine( lock,fib );
    UnLock( lock );
    curbytes=FileLength( fullname,fib );
    bytes+=curbytes;
    if( fn->Type==TYPE_DIR ){
     FormatSize( fn->Size,curbytes );
     fn->Bytes=curbytes;
     SetListBrowserNodeAttrs( node,
       LBNA_Column,2L,
       LBNCA_Text,fn->Size,TAG_END );
    }
   }
   count++;
  }
  node=node->ln_Succ;
 }
 delete fuelstring;
 FreeDosObject( DOS_FIB,fib );
 SetGadgetAttrs( (Gadget *)Main->Gadgets[FUELGAUGE],Main->IntuiWin, NULL,
                   FUELGAUGE_Level,0,GA_Text," ",TAG_DONE );
 if( refresh ){
   SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
     LISTBROWSER_Labels, lblist,LISTBROWSER_AutoFit, TRUE, // size could have changed
     TAG_DONE );
 }
 if( req && !stop ){
  FormatSize( fullname,bytes );
  sprintf( pa,GetCatalogStr(Main->Catalog,TXT_BYTESSEL,TXT_BYTESSEL_STR),fullname );
  Info( pa );
 }
 EnableAbort( FALSE );
 if( stop ) return(-1L);
 else return( bytes );
}
//<

//>"void Swap( char which=0 )"
void Swap( char which ){
 char *s,*d;
 char src[512],dst[512];
 TTabNode *t1=CurrentTab[0];
 TTabNode *t2=CurrentTab[1];
 TTabNode tdummy;

 // is is necessary to swap?
 GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_L],(ULONG*)&s );
 GetAttr( STRINGA_TextVal,Main->Gadgets[STR_PATH_R],(ULONG*)&d );
 if( stricmp(s,d)==0 ) return;

 // swap the string gadgets:
 strcpy( src,s );
 strcpy( dst,d );
 strcpy(tdummy.Path,t1->Path);
 tdummy.SortMode=t1->SortMode;
 tdummy.SortReverse=t1->SortReverse;
 tdummy.LhaActive=t1->LhaActive;
 if( which!='R' ){
   SetGadgetAttrs( (Gadget *)Main->Gadgets[STR_PATH_L],Main->IntuiWin, NULL,
     STRINGA_TextVal,dst,TAG_DONE );
   strcpy(t1->Path,t2->Path);
   t1->SortMode=t2->SortMode;
   t1->SortReverse=t2->SortReverse;
   t1->LhaActive=t2->LhaActive;
 }
 if( which!='L' ){
   SetGadgetAttrs( (Gadget *)Main->Gadgets[STR_PATH_R],Main->IntuiWin, NULL,
     STRINGA_TextVal,src,TAG_DONE );
   strcpy(t2->Path,tdummy.Path);
   t2->SortMode=tdummy.SortMode;
   t2->SortReverse=tdummy.SortReverse;
   t2->LhaActive=tdummy.LhaActive;
 }

 if( which!='R' ) ListDir( 0,NULL,0 );
 if( which!='L' ) ListDir( 1,NULL,0 );
}
//<

//>"void Key( char p )"
/****************************************/
/*                                      */
/* Function: Key                        */
/*                                      */
/* Aim:      Show the first entry with  */
/*           the given character in the */
/*           left browser               */
/* Input:    p = ASCII keycode          */
/*            should be a character     */
/*                                      */
/****************************************/
void Key( char p ){
 Node *node;
 List *lblist;
 ULONG selected=0L;
 BOOL found=FALSE;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();
 FileNode *fn;

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );

 node=lblist->lh_Head;

 while( (!found)&&(node->ln_Succ!=NULL) ){
  fn=(FileNode*)((TLBNode*)node)->Fn;
  if( (p>'Z')&&
      ((fn->Type==TYPE_FILE)||(fn->Type==TYPE_ASSIGN))
      &&(tolower(p)<=tolower(fn->Filename[0])) ){
   found=TRUE;
  }else if( (p<'a')&&
            ((fn->Type==TYPE_DIR)||(fn->Type==TYPE_VOLUME))
            &&(tolower(p)<=tolower(fn->Filename[0])) ){
   found=TRUE;
  }
  node=node->ln_Succ;
  selected++;
 }
 selected--;
 node=node->ln_Pred;
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                 LISTBROWSER_Labels, lblist,
                 LISTBROWSER_Top,selected, TAG_DONE );
}
//<

//>"void KeyMove( UBYTE key,short Qualifier )"
void KeyMove( UBYTE key,short Qualifier ){
 List *lblist;
 UBYTE listn=Main->CurrentList;
 Object *lb;
 if( key>=NM_WHEEL_UP ){
   if( Main->IDCMPEvent.Msg.MouseX<((Gadget*)Main->Gadgets[LST_DIR_R])->LeftEdge )
        lb=Main->Gadgets[LST_DIR_L];
   else lb=Main->Gadgets[LST_DIR_R];
 }else lb=GetCurrentListBrowser();

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&lblist );

 if( Qualifier & (IEQUALIFIER_LSHIFT + IEQUALIFIER_RSHIFT) ){
  switch( key ){
   case NM_WHEEL_RIGHT:
   case 78: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_RIGHTEDGE, TAG_DONE );   break;
   case NM_WHEEL_LEFT:
   case 79: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_LEFTEDGE, TAG_DONE );    break;
   case NM_WHEEL_UP:
   case 76: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_PAGEUP,TAG_DONE );        break;
   case NM_WHEEL_DOWN:
   case 77: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_PAGEDOWN, TAG_DONE );     break;
  }
 }else if( Qualifier & IEQUALIFIER_CONTROL ){
  switch( key ){
   case NM_WHEEL_RIGHT:
   case 78: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_RIGHTEDGE, TAG_DONE );   break;
   case NM_WHEEL_LEFT:
   case 79: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_LEFTEDGE, TAG_DONE );    break;
   case NM_WHEEL_UP:
   case 76: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_TOP,TAG_DONE );        break;
   case NM_WHEEL_DOWN:
   case 77: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_BOTTOM, TAG_DONE );     break;
  }
 }else if( Qualifier & (IEQUALIFIER_LALT + IEQUALIFIER_RALT) ){
  switch( key ){
   case NM_WHEEL_RIGHT:
   case 78: ListDir( 1,GetPath(0),0 ); break;
   case NM_WHEEL_LEFT:
   case 79: ListDir( 0,GetPath(1),0 ); break;
   case NM_WHEEL_UP:
   case 76: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_LINEUP,TAG_DONE );        break;
   case NM_WHEEL_DOWN:
   case 77: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_LINEDOWN, TAG_DONE );     break;
  }
 }else{
  switch( key ){
   case NM_WHEEL_RIGHT:
   case 78: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_SHIFTRIGHT, TAG_DONE );   break;
   case NM_WHEEL_LEFT:
   case 79: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_SHIFTLEFT, TAG_DONE );    break;
   case NM_WHEEL_UP:
   case 76: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_LINEUP,TAG_DONE );        break;
   case NM_WHEEL_DOWN:
   case 77: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
            LISTBROWSER_Position, LBP_LINEDOWN, TAG_DONE );     break;
  }
 }
}
//<

//>"BOOL ListBrowserKeyMove( IntuiMessage *im )"
BOOL ListBrowserKeyMove( IntuiMessage *im ){
  List  *lblist;
  long  sel,total;
  UBYTE listn=Main->CurrentList;
  BOOL  newsel=TRUE;
  TTabNode *tn=CurrentTab[listn];
  Object *lb=GetCurrentListBrowser();

  GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&lblist );
  GetAttr( LISTBROWSER_Selected,lb,(ULONG*)&sel );
  GetAttr( LISTBROWSER_TotalNodes,lb,(ULONG*)&total );

  switch( im->Code ){
    case 29: sel = total-1; break;
    case 30: if( sel < total-1 ) sel++; else sel = 0;  break;
    case 31: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
             LISTBROWSER_Position, LBP_PAGEDOWN, TAG_DONE );
             GetAttr( LISTBROWSER_Top,lb,(ULONG*)&sel );
             break;
    case 61: sel = 0; break;
    case 62: if( sel < 1 ) sel = total-1; else sel--; break;
    case 63: SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
             LISTBROWSER_Position, LBP_PAGEUP,TAG_DONE );
             GetAttr( LISTBROWSER_Top,lb,(ULONG*)&sel );
             break;
    default: return( FALSE );
  }
  if( newsel ){
    SetGadgetAttrs(
      (Gadget *)lb,Main->IntuiWin, NULL,
      LISTBROWSER_Selected,sel,
      LISTBROWSER_MakeVisible,sel,
      TAG_DONE
    );
  }
  return( TRUE );
}
//<

//>"void Pat()"
/****************************************/
/*                                      */
/* Function: Pat                        */
/*                                      */
/* Aim:      Get a pattern from the     */
/*           user and selected all      */
/*           files that match           */
/*                                      */
/****************************************/
void Pat(){
 char pat[100],Pattern[208];
 Node *node,*newselected;
 List *lblist;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 strcpy(pat,"#?");
 if( !GetText(pat,100,GetCatalogStr(Main->Catalog,TXT_ENTPAT,TXT_ENTPAT_STR)) ) return;

 ParsePatternNoCase(pat,Pattern,208);

 GetAttr( LISTBROWSER_Labels,lb,(ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,LISTBROWSER_Labels, NULL, TAG_DONE );

 node=lblist->lh_Head;

 while( node->ln_Succ!=NULL ){
  if( (((TLBNode*)node)->Fn->Type==TYPE_FILE)
      &&(MatchPatternNoCase(Pattern,((TLBNode*)node)->Fn->Filename)==1) ){
   SetListBrowserNodeAttrs( node,LBNA_Selected,TRUE,TAG_END );
   newselected=node;
  }else{
   SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
  }
  node=node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                 LISTBROWSER_Labels, lblist, TAG_DONE );
 ChoixDirList( listn );
}
//<

//>"void All()"
void All(){
 List *lblist;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();
 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,
                  LISTBROWSER_Labels, NULL,
                  LISTBROWSER_Selected,0,TAG_DONE );
 ListBrowserSelectAll( lblist );
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist,TAG_DONE );
 ChoixDirList( listn );
}
//<

//>"void None()"
void None(){
 List *lblist;
 Node *node;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,
                  LISTBROWSER_Labels, NULL, TAG_DONE );

 node=lblist->lh_Head;

 while( node->ln_Succ!=NULL ){
  SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
  node=node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist, TAG_DONE );
 ChoixDirList( listn );
}
//<

//>"void DeselectList( TFileList *list )"
void DeselectList( TFileList *list ){
 List *lblist;
 Node *node;
 TFileEntry *entry;
 ULONG sel;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL, NULL,
                  LISTBROWSER_Labels, NULL, TAG_DONE );

 node  = lblist->lh_Head;
 entry = (TFileEntry*)list->Files.lh_Head;

 while( node->ln_Succ!=NULL ){
   GetListBrowserNodeAttrs( node,LBNA_Selected,&sel,TAG_END );
   if( sel ){
     if( !entry->Selected ) SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
     entry = (TFileEntry*)entry->ln_Succ;
   }
   node  = node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist, TAG_DONE );
 ChoixDirList( listn );
}
//<

//>"void Toggle()"
// toggle the selected entries
void Toggle(){
 List *lblist;
 Node *node;
 ULONG selected;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();

 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL,NULL,LISTBROWSER_Labels, NULL, TAG_DONE );

 node=lblist->lh_Head;

 while( node->ln_Succ!=NULL ){
  GetListBrowserNodeAttrs( node,LBNA_Selected, &selected,TAG_END );
  if( selected ) SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
  else SetListBrowserNodeAttrs( node,LBNA_Selected,TRUE,TAG_END );
  node=node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist, TAG_DONE );
 ChoixDirList( listn );
}
//<

//>"void Type()"
// select all files of this class
void Type(){
 TFileType *class0,*cla;
 char path[512],full[512],selfile[512];
 Node *node;
 List *lblist;
 UBYTE listn=Main->CurrentList;
 TTabNode *tn=CurrentTab[listn];
 Object *lb=GetCurrentListBrowser();
 FileNode *fn;
 TLBNode *lbn;
 BYTE Type;

 GetAttr( LISTBROWSER_SelectedNode,lb, (ULONG*)&lbn );
 if( !lbn ) return;

 SetAttrs( Main->Win, WA_BusyPointer, TRUE, TAG_END );
 strcpy( path,GetSrc() );

 GetAttr( LISTBROWSER_Labels,lb, (ULONG*)&lblist );
 SetGadgetAttrs( (Gadget *)lb,NULL,NULL,LISTBROWSER_Labels, NULL, TAG_DONE );

 node=lblist->lh_Head;

 strcpy( selfile,path );
 AddPart( selfile,lbn->Fn->Filename,511 );
 Type = lbn->Fn->Type;
 if( Type == TYPE_FILE ) class0 = caFileTypes->GetFileType( selfile,lbn->Fn->Filename,FALSE );

 while( node->ln_Succ!=NULL ){
   fn=((TLBNode*)node)->Fn;
   if( fn->Type==Type ){
     if( fn->Type==TYPE_FILE ){
       strcpy( full,path );
       AddPart( full,fn->Filename,512 );
       cla = caFileTypes->GetFileType( full,fn->Filename,FALSE );
       if( cla==class0 ) SetListBrowserNodeAttrs( node,LBNA_Selected,TRUE,TAG_END );
       else SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
     }else SetListBrowserNodeAttrs( node,LBNA_Selected,TRUE,TAG_END );
   }else SetListBrowserNodeAttrs( node,LBNA_Selected,FALSE,TAG_END );
   node=node->ln_Succ;
 }
 SetGadgetAttrs( (Gadget *)lb,Main->IntuiWin, NULL,
                  LISTBROWSER_Labels, lblist, TAG_DONE );
 SetAttrs( Main->Win, WA_BusyPointer, FALSE, TAG_END );
 ChoixDirList( listn );
}
//<

//>"void Search()"
void Search(){
 TagItem taglist[3];

 // Fill the taglist:
 taglist[0].ti_Tag=WBOPENA_ArgLock;
 taglist[0].ti_Data=Lock( GetSrc(),SHARED_LOCK );
 taglist[1].ti_Tag=WBOPENA_ArgName;
 taglist[1].ti_Data=(ULONG)"";
 taglist[2].ti_Tag=TAG_END;
 // Start FIND :
 OpenWorkbenchObjectA( "sys:system/find",taglist );
 UnLock( (BPTR)taglist[0].ti_Data );
}
//<

//>"void Root()"
void Root(){
  char Path[256],*ptr;
  strcpy( Path,GetSrc() );
  if( ptr = strchr(Path,':') ){
    ptr++;
    if( *ptr!=0 ){
      *ptr = 0;
      ListDir( Main->CurrentList,Path,0 );
    }
  }
}
//<

//>"void Assign( char *arguments )"
void Assign( char *arguments ){
  char title[256];
  BPTR lock;
  DosList *dl;
  char    *Name,Filename[256];
  int ret=0;

  strcpy(title,FilePart(GetSrc()));
  if( GetText(title,255,GetCatalogStr(Main->Catalog,MSG_ASSIGNTITLE,MSG_ASSIGNTITLE_STR)) ){
    while( Name=strchr(title,':') ) *Name=0;
    // check if assign exists:
    dl=LockDosList(LDF_ASSIGNS | LDF_READ);       // then the assigns
    while( dl=NextDosEntry(dl, LDF_ASSIGNS) ){
     Name=(char*)BADDR(dl->dol_Name);
     if( Name[0]<256 ){
      memcpy( &Filename[0], Name + 1, Name[0] );
      Filename[Name[0]]=(char)0;
      if( stricmp(Filename,title) is 0 ) ret=1;
    }}
    UnLockDosList( LDF_ASSIGNS | LDF_READ );
    if(ret)
      ret=SureReq(GetCatalogStr(Main->Catalog,MSG_ASSIGNEXISTS,MSG_ASSIGNEXISTS_STR),GetCatalogStr(Main->Catalog,MSG_ASSIGNREPLACEADD,MSG_ASSIGNREPLACEADD_STR));
    else ret=3;
    // now we have: ret = 0: cancel; = 1: replace; = 2: add; = 3 : simple assign
    if(ret){
      if(lock=Lock(GetSrc(),ACCESS_READ)){
        if( ret is 1 ){ AssignLock(title,NULL); ret=3 } // remove assign, then we have a simple assign
        if( ret is 2 ) ret=AssignAdd(title,lock);       // add assign
        else ret=AssignLock(title,lock);                // simple assign
        if( !ret ){
          UnLock(lock);
          Info(GetCatalogStr(Main->Catalog,MSG_ERR_ASSIGN,MSG_ERR_ASSIGN_STR));
        }
      }else Info(GetCatalogStr(Main->Catalog,MSG_ERR_ASSIGN,MSG_ERR_ASSIGN_STR));
    }
  }
}
//<

//>"void OpenDirWindow()"
void OpenDirWindow(){
  char *path=GetSrc();
  if( !path[0] ) return; // no window for volumes!
  TAction *act = caActions->CreateAction( "" );
  sprintf( act->Exec,"%c%s%c",34,path,34 );
  act->Type  = RUN_WB;
  act->Async = TRUE;
  Runner( act );
  caActions->DeleteAction( act );
}
//<

//>"BOOL PathValid( char *path )"
/****************************************/
/*                                      */
/* Function: PathValid                  */
/*                                      */
/* Aim:      Check if the path exists   */
/*           and get the full name      */
/*                                      */
/* Input:    path = path to be checked, */
/*            will be replaced by the   */
/*            correct name              */
/*                                      */
/* Output:   TRUE = path valid          */
/*           FALSE = path valid         */
/*                                      */
/****************************************/
/*
BOOL PathValid( char *path ){
 char NewPath[512];
 BPTR lock;

 if( !path[0] ) return( FALSE );
 lock=Lock( path,ACCESS_READ );
 if(lock){
  NameFromLock( lock,NewPath,512 );
  strcpy( path, NewPath );
  UnLock(lock);
  return( TRUE );
 }else return( FALSE );
}
*/
//<

//>"void IconifyWindow()"
void IconifyWindow(){
/*
 SetAttrs( Main->Win,WA_Left,Main->IntuiWin->LeftEdge,WA_Top,Main->IntuiWin->TopEdge,
           WA_Width,Main->IntuiWin->Width,WA_Height,Main->IntuiWin->Height,TAG_END );
*/
 DoMethod( Main->Win, WM_ICONIFY );
 if( Main->Scr ) UnlockPubScreen( NULL, Main->Scr );
 Main->Scr      = NULL;
 Main->IntuiWin = NULL;
 Main->IsIcon   = TRUE;
}
//<

//>"void UnIconifyWindow()"
void UnIconifyWindow(){
 char *ptr=Main->PublicScreen;
 if( ptr[0]==0 ) ptr=NULL;
 if( !(Main->Scr=LockPubScreen(ptr)) ){
  Main->Scr=LockPubScreen(NULL);
 }
 SetAttrs( Main->Win, WA_PubScreen,Main->Scr,TAG_END );
 DoMethod( Main->Win, WM_OPEN );
 GetAttr( WINDOW_Window, Main->Win,(ULONG *) &Main->IntuiWin );
 Main->IsIcon = FALSE;
}
//<

//>"void SnapshotWindow()"
void SnapshotWindow(){
 TIniFile IniFile;
 ULONG value;

 Main->WinX=Main->IntuiWin->LeftEdge;
 Main->WinY=Main->IntuiWin->TopEdge;
 GetAttr( WA_InnerWidth, Main->Win,&value );
 Main->WinWidth  = value;
 GetAttr( WA_InnerHeight,Main->Win,&value );
 Main->WinHeight = value;

 char *FileName=PREFSFILE;

 for( int i=0;i<2;i++ ){
  if( !IniFile.LoadFromFile( FileName ) ) break;

  IniFile.WriteInt( "Window","X",Main->WinX );
  IniFile.WriteInt( "Window","Y",Main->WinY );
  IniFile.WriteInt( "Window","WIDTH",Main->WinWidth );
  IniFile.WriteInt( "Window","HEIGHT",Main->WinHeight );
  IniFile.SaveToFile( FileName );
  FileName=TPREFSFILE;
 }
}
//<

//>"void ActivatePathGadget( UBYTE which )"
void ActivatePathGadget( UBYTE which ){
  if( which is 0 ){
    ActivateLayoutGadget(
      (Gadget*)Main->Gadgets[LAYOUT_PATH_L],Main->IntuiWin,NULL,(ULONG)Main->Gadgets[STR_PATH_L]
    );
  }else{
    ActivateLayoutGadget(
      (Gadget*)Main->Gadgets[LAYOUT_PATH_R],Main->IntuiWin,NULL,(ULONG)Main->Gadgets[STR_PATH_R]
    );
  }
}
//<

//>"void RearrangeBrowserSize()"
void RearrangeBrowserSize(){
  ULONG x = (Main->IntuiWin->MouseX*100)/Main->IntuiWin->Width;
  if( Main->VerticalLists ){
    SetGadgetAttrs( (Gadget*)Main->Gadgets[LAYOUT_MAIN],Main->IntuiWin,NULL,
      LAYOUT_ModifyChild,Main->Gadgets[LAYOUT_L],
      CHILD_WeightedHeight,x,
      LAYOUT_ModifyChild,Main->Gadgets[LAYOUT_R],
      CHILD_WeightedHeight,100-x,
      TAG_END
    );
  }else{
    SetGadgetAttrs( (Gadget*)Main->Gadgets[LAYOUT_MAIN],Main->IntuiWin,NULL,
      LAYOUT_ModifyChild,Main->Gadgets[LAYOUT_L],
      CHILD_WeightedWidth,x,
      LAYOUT_ModifyChild,Main->Gadgets[LAYOUT_R],
      CHILD_WeightedWidth,100-x,
      TAG_END
    );
  }
  RethinkLayout( (Gadget*)Main->Gadgets[LAYOUT_MAIN],Main->IntuiWin,NULL,TRUE );
}
//<
