#ifndef CA_POPUP_H
#define CA_POPUP_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                & Salim Gasmi

        File : capopup.h

****************************************/

#include "caFileTypes.h"

void BuildMenu();

void BuildDirMenu();

void FreeDirMenu();

void FreeMenu();

BOOL HandlePopupMessage( IntuiMessage *msg );

void OpenActionMenu( TFileType *SelectedClass,char *filename=NULL );

void OpenDirMenu();

char *OpenHistoryMenu();

void OpenMenu();

char *OpenPathMenu();

void OpenVolumeMenu( UBYTE which );

#endif
