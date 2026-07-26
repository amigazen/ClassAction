#ifndef CA_INI_H
#define CA_INI_H
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
                 and Salim Gasmi

        File: caiini.h

        Aim:  Header for caini.cpp

        Ver:  4.2

****************************************/
#include <fstream.h>
#include <exec/types.h>

class TIniFile{
public:
 TIniFile();
 ~TIniFile();
 BOOL Prepare( char *filename,BOOL forwrite );
 void Free();
 // Standard functions to get a value
 BOOL GetBool( char *group,char *id, BOOL def );
 int  GetInt( char *group,char *id, int def );
 char *GetString( char *group,char *id, char *def, char *buffer );
 // Get the lines of a group sequentially
 BOOL SeekGroup( char *group );
 char *GetNextGroupEntry();     // returns NULL if no more entry
 // The write functions need to be prepared by Prepare( "...",TRUE );
 // they overwrite the ini file. So they are a fast possibility to write the file.
 BOOL WriteBool( char *id, BOOL value );
 BOOL WriteInt( char *id, int value );
 BOOL WriteString( char *id, char *value );
 BOOL WriteGroup( char *title );
 BOOL WriteLine( char *line );
 // Overloading one function ChangeValue,
 // the standard function to change a value in the ini file.
 // Can be used together with Get... on one (prepared) IniFile object
 BOOL ChangeValue( char *group,char *id, int value );
 BOOL ChangeValue( char *group,char *id, BOOL value );
 BOOL ChangeValue( char *group,char *id, char *value );
private:
 char  *IdValue( char *group,char *id );
 char  *IdLine( char *group,char *id );
 void  ReplaceInBuffer( char what,char by );
 char  *FileName;
 ULONG BufferSize;
 char  *Buffer;
 // the buffer == used to store the file (Get/Change) || temporary strings (Write)
 BOOL  Overwrite;
 FILE  *File;
 char  *Current;
};


#endif
