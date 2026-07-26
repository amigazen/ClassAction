#ifndef UN_COM_H
#define UN_COM_H
/****************************************

        File: unCommodities.h

        Aim:  Header for unCommodities.cpp

****************************************/

#include <libraries/commodities.h>
#define MAX_COMMODITIES 64

struct CCMsg{
  struct Message ccm_Msg;
  UWORD  ccm_Y;
};
struct Cxm{
  struct Message cxm_Msg;
  LONG           cxm_Reserved1;
  ULONG          cxm_ID;
  WORD           cxm_Reserved2;
  UBYTE          cxm_Type;
  UBYTE          cxm_Pad;
  UBYTE          cxm_Reserved3[202];
};

class TCommodities{
public:
  TCommodities();
  Node  *Nodes[MAX_COMMODITIES];
  ULONG NodeCount;
  void  Init();          // get Broker and do initialisation
  void  Free();          // free all stuff
  void  Add( int Id,char *Filter ); // add a filter; choose 1 ... 14 for these id's
  int   GetCXId();      // get the called id, return 0 if none; returns also CXCMD_KILL etc.
  MsgPort   *Port;
  CxObj     *Broker;
  ULONG     PortSignal; // to use in Wait()
private:
  NewBroker nb;
};

#endif
