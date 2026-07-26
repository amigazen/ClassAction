#ifndef INIFILE_H
#define INIFILE_H
/****************************************
 * Reconstructed from ClassAction 4.6
 * StormC Global/IniFile (missing from
 * the source drop; only .o left).
 * TIniFile subclasses TStringList:
 * prefs are a line list; groups are
 * lines of the form [Name].
 ****************************************/

#include "StringList.h"

class TIniFile : public TStringList {
public:
  TIniFile();
  ~TIniFile();

  BOOL  SeekLine( int line );
  BOOL  SeekGroup( char *group, BOOL create = FALSE );
  BOOL  SeekEntry( char *group, char *id, BOOL create = FALSE );
  char *SeekValue( char *group, char *id );
  BOOL  GetBool( char *group, char *id, BOOL def );
  int   GetInt( char *group, char *id, int def );
  char *GetString( char *group, char *id, char *def, char *buffer );
  char *GetNextGroupEntry( void );
  BOOL  AddToGroup( char *line );
  BOOL  AppendGroup( char *group );
  BOOL  WriteString( char *group, char *id, char *value );
  BOOL  WriteBool( char *group, char *id, BOOL value );
  BOOL  WriteInt( char *group, char *id, int value );

private:
  Node *Current;          /* cursor for GetNextGroupEntry */
  Node *FindGroupNode( char *group );
  Node *FindEntryNode( char *group, char *id );
  void  MakeGroupLine( char *dest, char *group );
  BOOL  IsGroupLine( char *line );
  BOOL  GroupNameMatch( char *line, char *group );
};

#endif /* INIFILE_H */
