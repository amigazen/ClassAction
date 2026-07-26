#ifndef CA_CAMEN_H
#define CA_CAMEN_H
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner

        File: camen.h

        Aim:  Header for camen.cpp

        Ver:  4.2

****************************************/
#include <libraries/gadtools.h>
#include "caextern.h"

#define MAX_MENUS 70

class TMenu{
public:
 TMenu();
 ~TMenu();
 void AddItem( char *title,char *key,UBYTE type,APTR ptr );
 Menu *Render( APTR visinfo );
 // type: NM_ITEM,NM_TITLE,NM_SUB
 void Handle( ULONG message,Menu *menu );
 void Update();
 void Clear();
private:
 NewMenu *MenuData;
 ULONG   *MenuIds;
 Menu    *IMenu;
 int     Current;
 int     CurMenu,CurItem,CurSub;
};

#define POP_ITEM 0
#define POP_MENU 1

class TPopMenu;

struct TPopNode{
 Node   *ln_Succ,ln_Pred;
 UBYTE  ln_Type;
 BYTE   ln_Pri;
 char   *ln_Name;
 char   Title[100];
 void   (*Action)();
 UBYTE  Type;
 TPopMenu *SubMenu;
};

class TPopMenu{

public:
 TPopMenu();
 ~TPopMenu();
 void     AddItem( char *title,UBYTE type,APTR ptr );
 // type: POP_ITEM,POP_MENU,POP_SEPARATOR
 // ptr:  pointer to the void action() or to the submenu
 int      Show();
 int      SelectedItem;
private:
 TPopMenu *OpenSubMenu;
 List     Items;
 int      ItemCount;
 Window   *MenuWindow;
 Object   *WindowObject;
};

#endif
