/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
          & Salim Gasmi

        File : caplearn.c

        Aim : learn new classes

****************************************/

#include <classes/window.h>
#include <clib/alib_protos.h>
#include <gadgets/chooser.h>
#include <gadgets/fuelgauge.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <stdio.h>
#include <string.h>
#include "captypes.h"
#include "capreq.h"
#define  CATCOMP_NUMBERS
#define  CATCOMP_STRINGS
#include "classactionprefscd.h"
#include "classactionprefs.h"

#define SENS                                    3
#define MAXI                                    16

//>"void CopyOffsets()"
void CopyOffsets(){
 char *str;

 GetAttr( STRINGA_TextVal,Main.LGadgets[STRING_O1],(ULONG*)&str );
 strcpy( Main.SelectedClass->Value[0],str);
 GetAttr( STRINGA_TextVal,Main.LGadgets[STRING_O2],(ULONG*)&str );
 strcpy( Main.SelectedClass->Value[1],str);
 GetAttr( STRINGA_TextVal,Main.LGadgets[STRING_O3],(ULONG*)&str );
 strcpy( Main.SelectedClass->Value[2],str);
 GetAttr( STRINGA_TextVal,Main.LGadgets[STRING_O4],(ULONG*)&str );
 strcpy( Main.SelectedClass->Value[3],str);
 GetAttr( STRINGA_TextVal,Main.LGadgets[STRING_O5],(ULONG*)&str );
 strcpy( Main.SelectedClass->Value[4],str);

 strcpy(Main.SelectedClass->Suffix,"");
}
//<

//>"int Analyse()"
int Analyse(){
 FileInfoBlock *info;
 BPTR lock,h;
 int i,j,l,k,s,nbz,size;
 LONG min=9999999999;
 char *buff=NULL,*tmp=NULL;
 unsigned int *same=NULL;
 char isasc;
 char cc[5],offset[60],filename[512],shortestfile[512],dir[512];
 Window *win;

 GetAttr( WINDOW_Window,Main.LWin,(ULONG*)&win );

 WBArg *ArgList=Main.FileReq->fr_ArgList;
 int NArg=Main.FileReq->fr_NumArgs;

 strcpy( dir,Main.FileReq->fr_Drawer );
 l=strlen( dir );
 if( (dir[0]!=0)&&(dir[l-1]!=':')&&(dir[l-1]!='/') ) strcat( dir,"/" );
 SetGadgetAttrs( (Gadget *)Main.LGadgets[GAUGE],win, NULL,
                  GA_Text,GetCatalogStr(Main.Catalog,TXT_LPREP,TXT_LPREP_STR),
                  FUELGAUGE_Level,0, TAG_DONE );

 for(i=0;i<NArg;i++){
  sprintf( filename,"%s%s",dir,ArgList[i].wa_Name );
  lock=Lock( filename,ACCESS_READ );
  info=(FileInfoBlock*)AllocDosObject( DOS_FIB,NULL );
  Examine(lock,info);
  l=(info->fib_Size);
  FreeDosObject( DOS_FIB,info );
  if(l<min) {min=l;strcpy(shortestfile,filename);}
  UnLock(lock);
 }
 if(min<8){
  return(0);
 }

 buff=(char *)AllocMem( min,MEMF_CLEAR );
 tmp=(char *)AllocMem( min,MEMF_CLEAR );
 same=(unsigned int *)AllocMem( min*sizeof(int),MEMF_CLEAR );

 if(buff==NULL || tmp==NULL || same==NULL){
  if(buff!=NULL) FreeMem( buff,min );
  if(tmp!=NULL) FreeMem( tmp,min );
  if(same!=NULL) FreeMem( same,min*sizeof(int) );
  return(-10);
 }

 h=Open( shortestfile,MODE_OLDFILE );
 Read( h,buff,min );
 Close( h );

 for(i=0;i<NArg;i++){
  sprintf(offset,"%s (%d/%d)",GetCatalogStr(Main.Catalog,TXT_LPROG,TXT_LPROG_STR),i+1,NArg);

  SetGadgetAttrs( (Gadget *)Main.LGadgets[GAUGE],win, NULL,
                  GA_Text,offset,FUELGAUGE_Level,(i*100)/NArg, TAG_DONE );

  sprintf( filename,"%s%s",dir,ArgList[i].wa_Name );
  if(strcmp(filename,shortestfile)!=0){
   h=Open( filename,MODE_OLDFILE );
   Read( h,tmp,min );
   Close( h );
   for(j=0;j<min;j++){
    if(tmp[j]==buff[j]) same[j]++;
   }
  }
 }

 i=-1;
 int nf=0;
 int precis=NArg-1;

 // nf counts the offsets,
 while((i<min-SENS) && (nf<5)){
  i++;
  if(same[i]>=precis){
   k=i+1;
   s=i;
   while(k<min && same[k]>=precis && (k-i<=MAXI)) k++;
   while((buff[s]==0) && (s<=k)) s++;
   if(k-s>=SENS){
    size=k-s;
    isasc=1;
    nbz=0;
    for(j=0;j<size;j++){
     if(buff[s+j]==0) nbz++;
     if(buff[s+j]<32 || buff[s+j]>126){
      if(j>=SENS && isasc==1){
       size=j;
       isasc=1;
       j=size+1;
      }else isasc=0;
    }}
    nf++;
    sprintf(offset,"%d,",s);
    if(isasc==1) strcat(offset,"'");
    for(j=0;j<size;j++){
     if(isasc==0) sprintf(cc,"%02x",(unsigned char)buff[s+j]);
     else sprintf(cc,"%c",buff[s+j]);
     strcat(offset,cc);
    }
    if(isasc==1) strcat(offset,"'");
    switch( nf ){
     case 1: SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O1],win, NULL,
             STRINGA_TextVal,offset,TAG_END );break;
     case 2: SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O2],win, NULL,
             STRINGA_TextVal,offset,TAG_END );break;
     case 3: SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O3],win, NULL,
             STRINGA_TextVal,offset,TAG_END );break;
     case 4: SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O4],win, NULL,
             STRINGA_TextVal,offset,TAG_END );break;
     case 5: SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O5],win, NULL,
             STRINGA_TextVal,offset,TAG_END );break;
    }
    i=k-1;
   } 
  }
 }
 SetGadgetAttrs( (Gadget *)Main.LGadgets[GAUGE],win, NULL,
                 GA_Text,GetCatalogStr(Main.Catalog,TXT_LCOMP,TXT_LCOMP_STR),
                 FUELGAUGE_Level,100, TAG_DONE );

 FreeMem( buff,min );
 FreeMem( tmp,min );
 FreeMem( same,min*sizeof(int) );
 return(nf);
}
//<

//>"void Learn()"
/****************************************/
/*                                      */
/* Function:  Learn                     */
/*                                      */
/* Aim:       Let the user choose some  */
/*            files and try to learn    */
/*            from these                */
/*                                      */
/****************************************/
void Learn(){
 int x;
 Window *win;

 if( Main.SelectedClass==NULL ) return;

 if( !AslRequestTags( Main.FileReq,ASLFR_DrawersOnly,FALSE,
                      ASLFR_DoMultiSelect,TRUE,
                      ASLFR_TitleText,GetCatalogStr(Main.Catalog,TXT_LCHOOSE,TXT_LCHOOSE_STR),TAG_END ) ){
  return;
 }
 if( Main.FileReq->fr_NumArgs<2 ){
  Info(GetCatalogStr( Main.Catalog,TXT_LNEED,TXT_LNEED_STR));
  return;
 }

 SetAttrs( Main.Win,WA_BusyPointer,TRUE,TAG_END );
 DoMethod( Main.LWin,WM_OPEN );
 GetAttr( WINDOW_Window,Main.LWin,(ULONG*)&win );

 x=Analyse();

 if(x==0) Info(GetCatalogStr( Main.Catalog,TXT_LNOTHING,TXT_LNOTHING_STR));
 else if(x==-10) Info(GetCatalogStr( Main.Catalog,TXT_LMEMORY,TXT_LMEMORY_STR));

 ULONG windowsignals,mask, result,code;
 BOOL Leave=FALSE,Changed=FALSE;
 while( !Leave ){
  GetAttr( WINDOW_SigMask, Main.LWin, &windowsignals );
  mask = Wait( windowsignals | SIGBREAKF_CTRL_C );
  if( mask & SIGBREAKF_CTRL_C ) Leave=TRUE;
  if( (!Leave)&&(mask & windowsignals) ){
   while( (!Leave)&&((result = DoMethod( Main.LWin, WM_HANDLEINPUT, &code)) != WMHI_LASTMSG) ){
    switch( result & WMHI_CLASSMASK ){
     case WMHI_CLOSEWINDOW:{
      Leave=TRUE;
      break;
     }
     case WMHI_GADGETHELP:{
      if( Main.ShowHelp ){
       switch( result & RL_GADGETMASK ){
        case BUTTON_LACCEPT:{
         SetAttrs( Main.LWin,WA_Title,
                   (ULONG)GetCatalogStr(Main.Catalog,TXT_BUTTLACCEPT,TXT_BUTTLACCEPT_STR),TAG_END );
         break;
        }
        case BUTTON_LCANCEL:{
         SetAttrs( Main.LWin,WA_Title,
                   (ULONG)GetCatalogStr(Main.Catalog,TXT_BUTTLCANCEL,TXT_BUTTLCANCEL_STR),TAG_END );
         break;
        }
        default:{
         SetAttrs( Main.LWin,WA_Title,(ULONG)GetCatalogStr(Main.Catalog,TXT_LCOMP,TXT_LCOMP_STR),TAG_END );
         break;
        }
      }}
      break;
     }
     case WMHI_GADGETUP:{
      switch( result & RL_GADGETMASK ){
       case BUTTON_LACCEPT:{
        CopyOffsets();
        Changed=Leave=TRUE;
        break;
       }
       case BUTTON_LCANCEL:{
        Leave=TRUE;
        break;
       }
      }
      break;
     }
 }}}}

 SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O1],NULL, NULL,
                 STRINGA_TextVal,"",TAG_END );
 SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O2],NULL, NULL,
                 STRINGA_TextVal,"",TAG_END );
 SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O3],NULL, NULL,
                 STRINGA_TextVal,"",TAG_END );
 SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O4],NULL, NULL,
                 STRINGA_TextVal,"",TAG_END );
 SetGadgetAttrs( (Gadget *)Main.LGadgets[STRING_O5],NULL, NULL,
                 STRINGA_TextVal,"",TAG_END );
 DoMethod( Main.LWin,WM_CLOSE );
 SetAttrs( Main.Win,WA_BusyPointer,FALSE,TAG_END );
 if( Changed ){
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_MATCH],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, STRINGA_TextVal,"",TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[STRING_OFFSET],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE, STRINGA_TextVal,Main.SelectedClass->Value[0],TAG_DONE );
  SetGadgetAttrs( (Gadget *)Main.Gadgets[CHOOSER_OFFSET],Main.IntuiWin, NULL,
                  GA_Disabled, FALSE,CHOOSER_Selected,0, TAG_DONE );
 }
 return;
}
//<

