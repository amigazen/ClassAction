#include "ca_begin.h"
/****************************************
 * TStringList - line-oriented string list
 ****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include "StringList.h"

TStringList::TStringList()
{
  NewList( &Strings );
}

TStringList::~TStringList()
{
  Clear();
}

Node *TStringList::AllocStringNode( char *s )
{
  Node *n;
  char *copy;
  ULONG len;

  if( s == NULL ) s = "";
  len = strlen( s ) + 1;
  n = (Node *)AllocVec( sizeof(Node), MEMF_CLEAR );
  if( n == NULL ) return NULL;
  copy = (char *)AllocVec( len, MEMF_CLEAR );
  if( copy == NULL )
  {
    FreeVec( n );
    return NULL;
  }
  strcpy( copy, s );
  n->ln_Name = copy;
  return n;
}

void TStringList::FreeStringNode( Node *n )
{
  if( n == NULL ) return;
  if( n->ln_Name ) FreeVec( n->ln_Name );
  FreeVec( n );
}

int TStringList::ItemCount( void )
{
  Node *n;
  int count;

  count = 0;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
    count++;
  return count;
}

void TStringList::Clear( void )
{
  Node *n;

  while( (n = RemHead( &Strings )) != NULL )
    FreeStringNode( n );
}

void TStringList::Append( char *s )
{
  Node *n;

  n = AllocStringNode( s );
  if( n ) AddTail( &Strings, n );
}

char *TStringList::Items( int index )
{
  Node *n;
  int i;

  i = 0;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( i == index ) return n->ln_Name;
    i++;
  }
  return NULL;
}

void TStringList::Insert( char *s, int index )
{
  Node *n;
  Node *at;
  int i;

  n = AllocStringNode( s );
  if( n == NULL ) return;
  if( index <= 0 )
  {
    AddHead( &Strings, n );
    return;
  }
  i = 0;
  for( at = Strings.lh_Head; at->ln_Succ; at = at->ln_Succ )
  {
    if( i == index )
    {
      ::Insert( &Strings, n, at->ln_Pred );
      return;
    }
    i++;
  }
  AddTail( &Strings, n );
}

void TStringList::Insert( char *s, Node *pred )
{
  Node *n;

  n = AllocStringNode( s );
  if( n == NULL ) return;
  if( pred == NULL )
    AddHead( &Strings, n );
  else
    ::Insert( &Strings, n, pred );
}

Node *TStringList::Find( char *s )
{
  Node *n;

  if( s == NULL ) return NULL;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( n->ln_Name && (strcmp( n->ln_Name, s ) == 0) )
      return n;
  }
  return NULL;
}

int TStringList::IndexOf( char *s )
{
  Node *n;
  int i;

  if( s == NULL ) return -1;
  i = 0;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( n->ln_Name && (strcmp( n->ln_Name, s ) == 0) )
      return i;
    i++;
  }
  return -1;
}

void TStringList::Change( Node *n, char *s )
{
  char *copy;
  ULONG len;

  if( n == NULL || s == NULL ) return;
  len = strlen( s ) + 1;
  copy = (char *)AllocVec( len, MEMF_CLEAR );
  if( copy == NULL ) return;
  strcpy( copy, s );
  if( n->ln_Name ) FreeVec( n->ln_Name );
  n->ln_Name = copy;
}

void TStringList::Change( int index, char *s )
{
  Node *n;
  int i;

  i = 0;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( i == index )
    {
      Change( n, s );
      return;
    }
    i++;
  }
}

void TStringList::Delete( char *s )
{
  Node *n;

  n = Find( s );
  if( n ) Delete( n );
}

void TStringList::Delete( Node *n )
{
  if( n == NULL ) return;
  Remove( n );
  FreeStringNode( n );
}

BOOL TStringList::LoadFromFile( char *filename )
{
  FILE *f;
  char buf[1024];
  char *p;
  int c;
  int len;

  if( filename == NULL || filename[0] == 0 ) return FALSE;
  f = fopen( filename, "r" );
  if( f == NULL ) return FALSE;
  Clear();
  len = 0;
  while( (c = fgetc( f )) != EOF )
  {
    if( c == '\r' ) continue;
    if( c == '\n' || len >= (int)sizeof(buf) - 1 )
    {
      buf[len] = 0;
      /* strip trailing spaces */
      p = buf + len;
      while( p > buf && (p[-1] == ' ' || p[-1] == '\t') ) { --p; *p = 0; }
      Append( buf );
      len = 0;
      if( c != '\n' && c != EOF )
      {
        buf[len++] = (char)c;
      }
    }
    else
      buf[len++] = (char)c;
  }
  if( len > 0 )
  {
    buf[len] = 0;
    Append( buf );
  }
  fclose( f );
  return TRUE;
}

BOOL TStringList::SaveToFile( char *filename )
{
  FILE *f;
  Node *n;

  if( filename == NULL || filename[0] == 0 ) return FALSE;
  f = fopen( filename, "w" );
  if( f == NULL ) return FALSE;
  for( n = Strings.lh_Head; n->ln_Succ; n = n->ln_Succ )
  {
    if( n->ln_Name )
    {
      fputs( n->ln_Name, f );
      fputc( '\n', f );
    }
  }
  fclose( f );
  return TRUE;
}
