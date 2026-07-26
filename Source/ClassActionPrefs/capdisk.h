/****************************************

        ClassActionPrefs Source

        © 2002 by Martin R. Elsner
               & Salim Gasmi

        File : capdisk.h

        Aim  : header for capdisk.c

****************************************/

int  GetToolType( DiskObject *dobj,char *tool,char *result );

BOOL ReadToolTypes();

void SaveClasses( TIniFile *IniFile );

void WriteClass( TIniFile *IniFile,char *name, TClass *cla );
