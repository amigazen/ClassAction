#include "ca_begin.h"
/****************************************

        ClassActionPrefs Source

        (c) 2002 by Martin R. Elsner
          & Salim Gasmi

        File : capdisk.cpp

        Aim  : Load/Save Functions

****************************************/

#include <stdio.h>
#include <string.h>
#include <proto/icon.h>
#include "captypes.h"
#include "IniFile.h"
#include "capreq.h"
#define  CATCOMP_NUMBERS
#define  CATCOMP_STRINGS
#include "classactionprefscd.h"

//>"void Re( FILE *f,char *s )"
/*
void Re( FILE *f,char *s ){
  static dummy=0;

  if( fgets(s,256,f)==NULL ){
    strcpy(s,"???");
    if(dummy==0){
      dummy=1;
      Info( GetCatalogStr(Main.Catalog,ERR_CONFIGCORRUPT,ERR_CONFIGCORRUPT_STR) );
    }
  }
  else s[strlen(s)-1]=0;
}
*/
//<

//>"void WriteClass( TIniFile *IniFile,char *name, TClass *cla )"
void WriteClass( TIniFile *IniFile,char *name, TClass *cla ){
  char tmp[20];
  Action *act = (Action*)cla->Actions.lh_Head;

  if( name[0] ) IniFile->AppendGroup( name );
  else{
    IniFile->Append( cla->Name );
    if( cla->IsArchive ) IniFile->Append( "M0A" );
    else IniFile->Append( "M0" );
    IniFile->Append( cla->ImageFile );
    IniFile->Append( cla->Suffix );
    for(int i=0;i<5;i++) IniFile->Append( cla->Value[i] );
  }
  while( act->ln_Succ ){
   IniFile->Append( act->Name );
   IniFile->Append( act->Exec );
   sprintf(tmp,"M%d%d",act->Type % 10,act->CD);
    if( act->Async   ) strcat( tmp,"A" );
    if( act->ReadSrc ) strcat( tmp,"S" );
    if( act->ReadDst ) strcat( tmp,"D" );
   IniFile->Append( tmp );
   sprintf(tmp,"%s,%s",act->Stack,act->Delay);
   IniFile->Append( tmp );
   act = (Action*)act->ln_Succ;
  }
  if( ! name[0] ) IniFile->Append( EOC );
}
//<

//>"void SaveClasses( TIniFile *IniFile )"
void SaveClasses( TIniFile *IniFile ){
  struct TClass *cla;

  // Writing Open With class
  cla = (TClass*)Main.Classes.lh_Head;
  WriteClass( IniFile,"Classopenwith",cla );

  // Writing Unknown Class
  cla = (TClass*)cla->ln_Succ;
  WriteClass( IniFile,"Classunknown",cla );

  // Saving Generic file
  cla = (TClass*)cla->ln_Succ;
  WriteClass( IniFile,"Classgen",cla );

  // Saving Volume file
  cla = (TClass*)cla->ln_Succ;
  WriteClass( IniFile,"Classvol",cla );

  // Saving Directory file
  cla = (TClass*)cla->ln_Succ;
  WriteClass( IniFile,"Classdir",cla );

  // Writing other classes
  cla = (TClass*)cla->ln_Succ;
  IniFile->AppendGroup( "Classes" );
  while( cla->ln_Succ ){
    WriteClass( IniFile,"",cla );
    cla = (TClass*)cla->ln_Succ;
  }
}
//<

//>"int GetToolType(struct DiskObject *dobj,char *tool,char *result)"
/****************************************/
/*                                      */
/* Function : GetToolType               */
/*                                      */
/* Goal     : Get the Value of the      */
/*            Tooltype 'tool'           */
/*            in string 'result'        */
/*                                      */
/*                                      */
/* Return Values :                      */
/*                                      */
/*  TRUE  = Tooltype found              */
/*  FALSE = Tooltype not found          */
/*                                      */
/****************************************/
int GetToolType(struct DiskObject *dobj,char *tool,char *result){
 char *ptr;

 ptr=(char *)FindToolType((STRPTR*)(dobj->do_ToolTypes),(STRPTR)tool);
 if(ptr==NULL) return( FALSE );

 strcpy(result,ptr);
 return( TRUE );
}
//<

//>"BOOL ReadToolTypes()"
/****************************************/
/*                                      */
/* Function:  ReadToolTypes             */
/*                                      */
/* Aim:       Get the ToolTypes         */
/*            and setup global flags    */
/*                                      */
/* Output:    TRUE: all ok              */
/*            FALSE: important          */
/*             tooltype not found       */
/*                                      */
/****************************************/
BOOL ReadToolTypes(){
 char tool[100];

 if( GetToolType(Main.Dobj,"X",tool) ) sscanf( tool,"%d",&Main.WinX );
 else Main.WinX=-1;
 if( GetToolType(Main.Dobj,"Y",tool) ) sscanf( tool,"%d",&Main.WinY );
 else Main.WinY=-1;
 if( GetToolType(Main.Dobj,"WIDTH",tool) ) sscanf( tool,"%d",&Main.WinWidth );
 else Main.WinWidth=400;
 if( GetToolType(Main.Dobj,"HEIGHT",tool) ) sscanf( tool,"%d",&Main.WinHeight );
 else Main.WinHeight=250;
/*
 if( GetToolType(Main.Dobj,"SHOWHELP",tool) ) Main.ShowHelp=TRUE;
 else Main.ShowHelp=FALSE;
*/
 Main.ShowHelp=FALSE;
/*
 if( GetToolType(Main.Dobj,"CAPATH",tool) ){
  strcpy( Main.CAPath,tool );
  BPTR lock=Lock( Main.CAPath,ACCESS_READ );
  if( !lock ) return( FALSE );          // CA not found!
  else UnLock( lock );
 }else return( FALSE );*/

 return( TRUE );
}
//<

