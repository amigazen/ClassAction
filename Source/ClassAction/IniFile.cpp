#include "ca_begin.h"
/****************************************
 * TIniFile - INI-style prefs on a
 * TStringList. Groups are [Name] lines.
 ****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "IniFile.h"

TIniFile::TIniFile()
{
  Current = NULL;
}

TIniFile::~TIniFile()
{
  Current = NULL;
}

void TIniFile::MakeGroupLine( char *dest, char *group )
{
  sprintf( dest, "[%s]", group ? group : "" );
}

BOOL TIniFile::IsGroupLine( char *line )
{
  if( line == NULL || line[0] != '[' ) return FALSE;
  return (strchr( line, ']' ) != NULL) ? TRUE : FALSE;
}

BOOL TIniFile::GroupNameMatch( char *line, char *group )
{
  char tmp[256];
  char *end;
  ULONG len;

  if( !IsGroupLine( line ) || group == NULL ) return FALSE;
  end = strchr( line, ']' );
  if( end == NULL ) return FALSE;
  len = (ULONG)(end - (line + 1));
  if( len >= sizeof(tmp) ) len = sizeof(tmp) - 1;
  memcpy( tmp, line + 1, len );
  tmp[len] = 0;
  return (stricmp( tmp, group ) == 0) ? TRUE : FALSE;
}

Node *TIniFile::FindGroupNode( char *group )
{
  Node *n;

  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( GroupNameMatch( n->ln_Name, group ) )
      return n;
  }
  return NULL;
}

BOOL TIniFile::SeekLine( int line )
{
  Node *n;
  int i;

  i = 0;
  Current = NULL;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( i == line )
    {
      Current = n;
      return TRUE;
    }
    i++;
  }
  return FALSE;
}

BOOL TIniFile::SeekGroup( char *group, BOOL create )
{
  Node *n;
  char buf[256];

  n = FindGroupNode( group );
  if( n == NULL )
  {
    if( !create ) { Current = NULL; return FALSE; }
    MakeGroupLine( buf, group );
    Append( buf );
    n = FindGroupNode( group );
    if( n == NULL ) { Current = NULL; return FALSE; }
  }
  Current = n;
  return TRUE;
}

char *TIniFile::GetNextGroupEntry( void )
{
  Node *n;

  if( Current == NULL ) return NULL;
  n = Current->ln_Succ;
  if( n == NULL || n->ln_Succ == NULL )
  {
    Current = NULL;
    return NULL;
  }
  if( IsGroupLine( n->ln_Name ) )
  {
    Current = NULL;
    return NULL;
  }
  Current = n;
  return n->ln_Name;
}

Node *TIniFile::FindEntryNode( char *group, char *id )
{
  Node *g;
  Node *n;
  ULONG idlen;

  if( id == NULL ) return NULL;
  g = FindGroupNode( group );
  if( g == NULL ) return NULL;
  idlen = strlen( id );
  for( n = g->ln_Succ; n->ln_Succ; n = n->ln_Succ )
  {
    if( IsGroupLine( n->ln_Name ) ) break;
    if( n->ln_Name
        && (strncmp( n->ln_Name, id, idlen ) == 0)
        && (n->ln_Name[idlen] == '=') )
      return n;
  }
  return NULL;
}

char *TIniFile::SeekValue( char *group, char *id )
{
  Node *n;
  ULONG idlen;

  n = FindEntryNode( group, id );
  if( n == NULL || n->ln_Name == NULL ) return NULL;
  idlen = strlen( id );
  return n->ln_Name + idlen + 1;
}

BOOL TIniFile::SeekEntry( char *group, char *id, BOOL create )
{
  Node *n;
  char buf[512];

  n = FindEntryNode( group, id );
  if( n )
  {
    Current = n;
    return TRUE;
  }
  if( !create ) return FALSE;
  if( !SeekGroup( group, TRUE ) ) return FALSE;
  sprintf( buf, "%s=", id ? id : "" );
  AddToGroup( buf );
  n = FindEntryNode( group, id );
  if( n == NULL ) return FALSE;
  Current = n;
  return TRUE;
}

BOOL TIniFile::GetBool( char *group, char *id, BOOL def )
{
  char *v;

  v = SeekValue( group, id );
  if( v == NULL || v[0] == 0 ) return def;
  if( v[0] == '0' || v[0] == 'n' || v[0] == 'N' || v[0] == 'f' || v[0] == 'F' )
    return FALSE;
  if( v[0] == '1' || v[0] == 'y' || v[0] == 'Y' || v[0] == 't' || v[0] == 'T' )
    return TRUE;
  return def;
}

int TIniFile::GetInt( char *group, char *id, int def )
{
  char *v;

  v = SeekValue( group, id );
  if( v == NULL || v[0] == 0 ) return def;
  return atoi( v );
}

char *TIniFile::GetString( char *group, char *id, char *def, char *buffer )
{
  char *v;

  if( buffer == NULL ) return NULL;
  v = SeekValue( group, id );
  if( v )
    strcpy( buffer, v );
  else
    strcpy( buffer, def ? def : "" );
  return buffer;
}

BOOL TIniFile::AddToGroup( char *line )
{
  /* Append a line after Current (group header || last entry). */
  if( Current == NULL ) return FALSE;
  Insert( line, Current );
  /* Insert places after pred; Current still group || previous.
     Move Current to the new node (successor of old Current if we
     inserted after it - Insert(list,n,pred) inserts AFTER pred). */
  Current = Current->ln_Succ;
  return TRUE;
}

BOOL TIniFile::AppendGroup( char *group )
{
  char buf[256];

  MakeGroupLine( buf, group );
  Append( buf );
  Current = FindGroupNode( group );
  return (Current != NULL) ? TRUE : FALSE;
}

BOOL TIniFile::WriteString( char *group, char *id, char *value )
{
  Node *n;
  char buf[512];

  if( id == NULL ) return FALSE;
  sprintf( buf, "%s=%s", id, value ? value : "" );
  n = FindEntryNode( group, id );
  if( n )
  {
    Change( n, buf );
    return TRUE;
  }
  if( !SeekGroup( group, TRUE ) ) return FALSE;
  AddToGroup( buf );
  return TRUE;
}

BOOL TIniFile::WriteBool( char *group, char *id, BOOL value )
{
  return WriteString( group, id, value ? (char *)"1" : (char *)"0" );
}

BOOL TIniFile::WriteInt( char *group, char *id, int value )
{
  char buf[32];

  sprintf( buf, "%d", value );
  return WriteString( group, id, buf );
}
