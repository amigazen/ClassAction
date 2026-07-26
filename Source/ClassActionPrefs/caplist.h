
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
               & Salim Gasmi

        File : caplist.h

        Aim  : header for caplist.c

****************************************/


#include "captypes.h"

void FreeList(List *li);

void FreeClassList();

int  SortList();

TClass *CreateClass( List *li,char *name );

Action *CreateAction( TClass *cla,char *name );

