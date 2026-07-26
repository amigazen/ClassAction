
/****************************************

        ClassActionPrefs Source

        (c) 2001 by Martin R. Elsner
        (c) by 1995 Salim Gasmi

        File : caparexx.c

        Goal : Handle Arexx stuff 

        Ver  : 3.8

****************************************/

#include <proto/dos.h>

char *RexxHostName="CAP_Slave";
/*
long DoRexxCommand(void *msg,struct MsgPort *port,char *res,char **pres){
 return(0L);
}
*/

void NotifyMaster(){
 BPTR file;
 file=Open( "NIL:",MODE_READWRITE );
 Execute( "sys:rexxc/rx 'address classaction.1 use'",NULL,file );
 Close( file );
// PlaceRexxCommandDirect(NULL,"CLASSACTION.1","USE",NULL,NULL);
}

