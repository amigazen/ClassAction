#ifndef CA_REQ_H
#define CA_REQ_H
/****************************************

        Class Action Source

        © 2002 by Martin R. Elsner
                & Salim Gasmi

        File : careq.h

****************************************/

// shall we use our own Requesters?
extern BOOL UseReactionRequest;

// Disk insert regarding info display
void DInfo(char *mess);
// File requester
BOOL Freq(char *file,int mode,char *text);
// Text requester
BOOL GetText(char *buffer,int len,char *msg);
// Information display
void Info( char *mess,BOOL cabackground=FALSE );
// Yes/no requester
BOOL Req(char *);
// Are you sure? requester
int  SureReq(char *st,char *button);

#endif
