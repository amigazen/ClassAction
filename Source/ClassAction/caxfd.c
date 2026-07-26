
/****************************************

        Class Action Source

        (c) 2001 by Martin R. Elsner
        (c) 1994 by Salim Gasmi

        File : xfd.c 

        Goal : Handle decrunching 

        Ver  : 3.7

****************************************/

#include <clib/exec_protos.h>
#include <exec/memory.h>
#include <clib/locale_protos.h>
#include <proto/xfdmaster.h>

#include "caextern.h"
#include "caglobal.h"
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#include "classactioncd.h"

#include "caxfd.h"

XFDMasterBase *xfdMasterBase;
// I have renamed this structure because the same name
// for struct and var can make problems


//>"int OpenXfdLib()"
/****************************************/
/*                                      */
/* Function: OpenXfdLib                 */
/*                                      */
/* Aim:      Try to open the            */
/*           xfdmaster.library          */
/*                                      */
/* Return Values:                       */
/*                                      */
/*       0= OKAY                        */
/*      -1= Failed                      */
/*                                      */
/****************************************/
int OpenXfdLib(){
 xfdMasterBase=(struct XFDMasterBase *)OpenLibrary("xfdmaster.library",0);
 if( xfdMasterBase==NULL ) return(-1); else return(0);
}
//<

//>"void CloseXfdLib()"
/****************************************/
/*                                      */
/* Function: CloseXfdLib()              */
/*                                      */
/* Aim:      Close the xfdmaster        */
/*           Library                    */
/*                                      */
/****************************************/
void CloseXfdLib(){
 if( xfdMasterBase!=NULL ) CloseLibrary( (struct Library*)xfdMasterBase );
}
//<

//>"APTR Decrunch(char *buff,int size,char *cruncher,int *newlen)"
/****************************************/
/*                                      */
/* Function: Decrunch                   */
/*                                      */
/* Aim:      Decrunch the buffer 'buff' */
/*           of len 'buflen'            */
/*           and return the pointer     */
/*           of the decrunched buffer   */
/*           Initialize 'cruncher'      */
/*           to the used decruncher     */
/*           and 'newlen' with the len  */
/*           of the decrunched buffer   */
/*                                      */
/* Return Values:                       */
/*                                      */
/*      the decrunched buffer pointer   */
/*      or NULL if decrunching failed   */
/*                                      */
/****************************************/
APTR Decrunch( char *buff,int size,char *cruncher,int *newlen ){
 struct xfdBufferInfo *xfd;
 int ret;
 APTR dec;

 if( xfdMasterBase==NULL ) return(NULL);

 xfd=xfdAllocBufferInfo();
 xfd->xfdbi_SourceBuffer=buff;
 xfd->xfdbi_SourceBufLen=size;
 xfd->xfdbi_Special=NULL;

 ret=xfdRecogBuffer(xfd);
 if(ret==FALSE){
  strcpy( cruncher,GetCatalogStr(Main->Catalog,ERR_NOTCRUNCHED,ERR_NOTCRUNCHED_STR) );
  xfdFreeBufferInfo(xfd);
  return(NULL);
 }

 if(xfd->xfdbi_PackerFlags<2){
  strcpy( cruncher,GetCatalogStr(Main->Catalog,ERR_NOTDATA,ERR_NOTDATA_STR) );
  xfdFreeBufferInfo(xfd);
  return(NULL);
 }
 strcpy(cruncher,xfd->xfdbi_PackerName);
 xfd->xfdbi_TargetBufMemType=MEMF_PUBLIC;

 ret=xfdDecrunchBuffer(xfd);
 if(ret==FALSE){
  strcpy( cruncher,GetCatalogStr(Main->Catalog,ERR_NOTDECR,ERR_NOTDECR_STR) );
  xfdFreeBufferInfo(xfd);
  return(NULL);
 }
 dec=xfd->xfdbi_TargetBuffer;    

 *newlen=xfd->xfdbi_TargetBufLen;
 xfdFreeBufferInfo(xfd);
 return(dec);
}
//<
