#ifndef CA_DISK_H
#define CA_DISK_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                & Salim Gasmi

        File : cadisk.h

****************************************/

#include "IniFile.h"

//BOOL IsArchive( char *file );

void LoadClasses( TIniFile *IniFile );

void LoadOptions( TIniFile *IniFile );

void LoadPaths( TIniFile *IniFile );

BOOL SetFileAttributes( char *file );

#endif
