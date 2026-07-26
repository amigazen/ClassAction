#ifndef CA_PATHS_H
#define CA_PATHS_H
/****************************************

        Class Action Source

        � 2002 by Martin R. Elsner & Salim Gasmi

        File: capths.h

****************************************/

#include <dos/notify.h>

struct TPathNode;
struct TTabNode;
struct TUserPathNode;
struct FileNode;
struct TLBNode;
typedef struct TPathNode     TPathNode;
typedef struct TTabNode      TTabNode;
typedef struct TUserPathNode TUserPathNode;
typedef struct FileNode      FileNode;
typedef struct TLBNode       TLBNode;

struct TPathNode{
   Node   *ln_Succ;
   Node   *ln_Pred;
   UBYTE  ln_Type;
   BYTE   ln_Pri;
   char   *ln_Name;
   // extended:
   List FList;    // list of FileNodes
   int  Count;     // number of FileNodes
   float Size;    // size of all files in KBytes
   int  FirstOnList; // last position in the browser
   char Path[256]; // complete path of the dir
   BOOL NotifyOn;
   struct DateStamp ReadTime;
   NotifyRequest NotiReq; // struct to handle notification
};

struct TTabNode{
  Node   *ln_Succ;
  Node   *ln_Pred;
  UBYTE  ln_Type;
  BYTE   ln_Pri;
  char   *ln_Name;
  // extended:
  char     Path[256]; // complete path of the dir
  int      SortMode;
  BOOL     SortReverse;
  BOOL     LhaActive;
  TPathNode *CurrentPath;
  Gadget   *Gadget;
};

struct TUserPathNode{
  Node *ln_Succ,*ln_Pred;
  UBYTE  ln_Type;
  BYTE   ln_Pri;
  char   *ln_Name;
  char   Name[100];
  char   Path[256];
};

struct FileNode{
  Node   *ln_Succ,*ln_Pred;
  UBYTE  ln_Type;
  BYTE   ln_Pri;
  char   *ln_Name;
  char   Filename[108];  // same length as in FIB
  char   Size[40];
  float  Bytes;
  UBYTE  Type;
  char   Date[40];
  char   Prot[10];
  char   Comment[80];
  ULONG  Bits;
  BOOL   Link;
  struct DateStamp DateStamp;
  Image  *ListerImage;
};

// Node for the Listbrowsers:
struct TLBNode{
  Node   *ln_Succ,*ln_Pred;
  UBYTE  ln_Type;
  BYTE   ln_Pri;
  char   *ln_Name;
  struct FileNode *Fn;          // pointer to the connected FileNode
};


extern TTabNode *CurrentTab[2];         // the selected tabs

void ClearPaths();
void ClearUserPaths();
void InitPaths();
BOOL IsPathNodeValid( TPathNode *pn0 );
void RemovePathNode( TPathNode *pn=NULL );

#endif

