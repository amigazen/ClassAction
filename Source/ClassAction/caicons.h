#ifndef CA_ICONS_H
#define CA_ICONS_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                & Salim Gasmi

        File : caicons.h

****************************************/

#include "IniFile.h"

class TcaIcons{
public:
  void Init();
  void Free();
  BOOL DoIconCommand( Window *win,ULONG msg );
  void FreeIcons();
  List *GetCommandList( int Index );
  void LoadCommands( TIniFile *IniFile,Screen *scr,Object *Fuelgauge,Object *UpperLayout,Object *LowerLayout );
  void ShowIconHelp( Window *win,ULONG id );
private:
  IClass *IconClass;             // pointer to the icon class
  List   Icons[3];               // lists for icons and commands. consists of IconNodes
  Object *oFuelgauge,*oUpperLayout,*oLowerLayout;
  void FreeIconList( List *list );
};

struct IconNode{
 Node   *ln_Succ,*ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 /* extended: */
 TAction *Action;       // action
 char   *Help;          // help string
 Object *Gadg;          // gadget
 UBYTE  Id;             // Gadget id
 int    LHlp;           // Length of help
 UBYTE  Key;
 BOOL   Ascii;
};

extern TcaIcons *caIcons;

#endif
