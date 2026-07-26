#ifndef CAP_OPT_H
#define CAP_OPT_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
               & Salim Gasmi

        File: capopt.h

        Aim:  defines for capopt.cpp

****************************************/


#include "IniFile.h"
#include "StringList.h"
#include <intuition/classusr.h>
#include <intuition/intuition.h>

class TChooser : public TStringList{
public:
  int Selected;
};

struct lbn{
 struct Node *ln_Succ;
 struct Node *ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 /* extended: */
 char   tooltype[256],title[256],value[256];
 char   type;
 // can be:
 //  b = boolean
 //  i = integer
 //  s = string
 //  f = filename
 //  d = directory
 //  c = colour
 //  g = group
 //  m = screen mode
 //  o = chooser (ptr points to TStringList)
 //  z = font
 void   *ptr;
 int    number;

};

class TOptionList{
public:
 void Init( Object *window,Object *listbrowser );
 void Add( char *title,char type, char *tooltype, ULONG value );
 void AddGroup( char *name, char *title );
 void HandleInput();
 void WriteAll( TIniFile *IniFile );
 void Reset();
private:
 BOOL ChooserPopUp( TChooser *Chooser );
 Object *win;
 Object *lb;
};

extern TOptionList OptionList;

#endif
