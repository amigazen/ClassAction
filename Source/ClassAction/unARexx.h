#ifndef UN_AREXX_H
#define UN_AREXX_H

#define MAX_AREXX_COMMANDS 10

#include <rexx/storage.h>

class TARexx{
 public:
  BOOL    Init( char *portname );
  BOOL    Free();
  BOOL    AddCmd( char*,void (*)(char*));
  ULONG   Signals();
  BOOL    HandleMsg();
  void    SetError( char* );
  void    SetResult( char* );
  char    *Name();
 private:
  RexxMsg *GetMessage();
  BOOL    SetLastError( struct RexxMsg*,char* );
  void    ReplyMessage( struct RexxMsg*,char*,ULONG );
  int     Commands;
  MsgPort *Port;     /* The port messages come in at... */
  long    Outstanding;    /* The count of outstanding ARexx messages... */
  char    PortName[40];   /* The port name goes here... */
  char    ErrorName[40];  /* The name of the <base>.LASTERROR... */
  char    *Error;
  ULONG   ErrorCode;
  struct  Cmd{ char *Name; void (*Function)(char*); } ARexxCmd[MAX_AREXX_COMMANDS];
};

#endif
