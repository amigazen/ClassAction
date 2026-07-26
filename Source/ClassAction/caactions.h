#ifndef CA_ACTIONS_H
#define CA_ACTIONS_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner & Salim Gasmi

        File: caactions.h

****************************************/

struct TAction{
 Node   *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;

 char   Name[33];       // Action Name
 char   Exec[250];      // Execute string
 char   Dest[250];      // non standard destination
 char   Type;           // Cli/WB run Flag
 BOOL   Async,ReadSrc,ReadDst;
 UBYTE  CD;             // 0=cd source, 1=dest, 2=progdir
 char   Stack[8];       // ASCII Value of the CLI Stack
 char   Delay[3];       // Delay of Cli
};

#define TA_MAX_EXEC     249

// flags for Type:
#define RUN_CLI         0      // SystemTags
#define RUN_WB          1      // OpenWorkbenchObject
#define RUN_NOCLI       2      // SystemTags, do ! open a console
#define RUN_AREXX       3      // rx "..."
#define RUN_INTERNAL    4      // CA command
#define RUN_SPACE       5      // only space (icon)

class TcaActions{
public:
  TcaActions();
  ~TcaActions();

  TAction *CreateAction( char *name );
  void DeleteAction( TAction *act,BOOL RemoveFromList=FALSE );

  TAction *Add( char *Name, char *Command, char WBRun, char *Stack, char *sDelay,
    BOOL Async=FALSE, BOOL ReadSource=FALSE, BOOL ReadDestination=FALSE );
  TAction *Item( char *Name );
private:
  List Actions;
};

extern TcaActions *caActions;

#endif

