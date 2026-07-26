/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
               & Salim Gasmi

        File : capreq.h

        Aim  : Handle Requesters

****************************************/

#include <intuition/intuition.h>

BOOL ColourReq( char *colour,char *title );

void DInfo(char *mess);

BOOL FontReq( char *font, char *text );

/* mode : 0=files,
          1=dirs,2=volumes */
BOOL Freq(char *file,int mode,char *text);

BOOL GetText(char *buffer,int len,char *msg);

void Info(char *mess);

ULONG ReactionRequest( Window *iwin, EasyStruct *req,APTR a,APTR b );

BOOL Req(char *mess);

BOOL ScreenRequester( char *modestring, char *title );



