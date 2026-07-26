#ifndef STRINGLIST_H
#define STRINGLIST_H
/****************************************
 * Reconstructed from ClassAction 4.6
 * StormC Global/StringList (missing
 * from the source drop; only .o left).
 * API matches ClassAction.map symbols.
 ****************************************/

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/nodes.h>

class TStringList {
public:
  TStringList();
  ~TStringList();

  int   ItemCount( void );
  void  Clear( void );
  void  Append( char *s );
  char *Items( int index );
  void  Insert( char *s, int index );
  void  Insert( char *s, Node *pred );
  Node *Find( char *s );
  int   IndexOf( char *s );
  void  Change( Node *n, char *s );
  void  Change( int index, char *s );
  void  Delete( char *s );
  void  Delete( Node *n );
  BOOL  LoadFromFile( char *filename );
  BOOL  SaveToFile( char *filename );

protected:
  List  Strings;
  Node *AllocStringNode( char *s );
  void  FreeStringNode( Node *n );
};

#endif /* STRINGLIST_H */
