#include <libraries/commodities.h>
#include <clib/exec_protos.h>
#include <clib/commodities_protos.h>
#include <clib/alib_protos.h>
#include "unCommodities.h"

int TCommodities::TCommodities(){
  Broker = NULL;
  Port   = NULL;
  NodeCount = 0;
}

void TCommodities::Init(){
  LONG result;
  int counter;

  if( !(Port = CreateMsgPort()) ) return;
  PortSignal    = 1 << Port->mp_SigBit;
  nb.nb_Version = NB_VERSION;
  nb.nb_Title   = "MRE ClassAction";
  nb.nb_Descr   = "© Martin Elsner & Salim Gasmi 2002";
  nb.nb_Unique  = NBU_UNIQUE | NBU_NOTIFY;
  nb.nb_Flags   = COF_SHOW_HIDE;
  nb.nb_Port    = Port;
  nb.nb_Pri     = 127;
  nb.nb_Name    = "ClassAction.1";
  counter = 1;
  while( !(Broker=CxBroker(&nb,&result)) && (counter<10) ){
    counter++;
    nb.nb_Name[12] = counter+48;
  }
  ActivateCxObj( Broker, TRUE );
}

void TCommodities::Add( int Id,char *Filter ){
  CxObj *filter;
  AttachCxObj( Broker, filter=CxFilter(Filter) );
  AttachCxObj( filter, CxSender(Port,Id) );
}

void TCommodities::Free(){
  if( Broker ) DeleteCxObjAll( Broker );
  if( Port ){
    Message *msg;
    while( msg=GetMsg(Port) ) ReplyMsg( msg );
    DeleteMsgPort( Port );
  }
}

int  TCommodities::GetCXId(){
  CxMsg *msg;
  if( msg=(CxMsg*)GetMsg(Port) ){
    LONG id=CxMsgID(msg);
    ULONG type=CxMsgType(msg);
    ReplyMsg( (Message*)msg );
    if( (id==CXCMD_DISABLE) || (id==CXCMD_ENABLE) ) ActivateCxObj( Broker,id==CXCMD_ENABLE );
    return( id );
  }else return( 0 );
}


