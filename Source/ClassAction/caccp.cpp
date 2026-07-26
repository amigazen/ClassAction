#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <devices/clipboard.h>
#include <libraries/iffparse.h>
#include <utility/hooks.h>
#include <clib/exec_protos.h>
#include <clib/iffparse_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caccp.h"
#include <clib/exec_protos.h>
#include <string.h>


/* IFF parse library base */
extern struct Library *IFFParseBase;
TClipBoard *ClipBoard;

LONG ftxtprops[] = {ID_FTXT, ID_AUTH, ID_FTXT, ID_NAME};

//>"TClipBoard::TClipBoard()"
//////////////////////////////////////////////////////////////////////
// TClipBoard::TClipBoard                                           //
//////////////////////////////////////////////////////////////////////
TClipBoard::TClipBoard(){
 Handle = NULL;
 FTXTBuffer = NULL;
}
//<

//>"void TClipBoard::Init( int unit )"
//////////////////////////////////////////////////////////////////////
// TClipBoard::Init                                                 //
//////////////////////////////////////////////////////////////////////
void TClipBoard::Init( int unit ){
 // Allocate an IFF handle:
 if( Handle = AllocIFF() ){
  // Open the clipboard for access:
  if( Handle->iff_Stream = (ULONG)OpenClipboard( unit ) ){
   // Initialize the handle as clipboard access:
   InitIFFasClip( Handle );
  }else{
   // Free the IFF handle:
   FreeIFF( Handle );
   Handle = NULL;
}}}
//<

//>"TClipBoard::~TClipBoard()"
//////////////////////////////////////////////////////////////////////
// TClipBoard::~TClipBoard                                          //
//////////////////////////////////////////////////////////////////////
TClipBoard::~TClipBoard(){
 FreeBuffer();
 /* Make sure that the handle points to something */
 if( Handle ){
  // Close the clipboard:
  if( Handle->iff_Stream )
   CloseClipboard ((ClipboardHandle*) Handle->iff_Stream);
  // Free the IFF handle:
  FreeIFF( Handle );
}}
//<

//>"char *TClipBoard::Paste()"
//////////////////////////////////////////////////////////////////////
// TClipBoard::Paste                                                //
//////////////////////////////////////////////////////////////////////
char *TClipBoard::Paste(){
 FreeBuffer();

 FTXTBuffer = NULL;
 LONG error = 0L;

 // Make sure that the handle points to something
 if( Handle ){
  struct ClipboardHandle *cbh = (struct ClipboardHandle *) Handle->iff_Stream;
  struct IOClipReq *clipIO = &(cbh->cbh_Req);

  if( ((struct Library *) IFFParseBase)->lib_Version < 36 ){
   // Bug workaround: clear io_Error field
   clipIO->io_Error = 0;
   // Bug workaround: Turn off the write flag
   Handle->iff_Flags &= ~IFFF_WRITE;
  }
  // Open the IFF handle for reading
  if( !(error = OpenIFF (Handle, IFFF_READ)) ){
   // Declare the properties to search for
   PropChunks( Handle,ftxtprops,2 );

   // Register the stop chunk
   if ((error = StopChunk(Handle,ID_FTXT,ID_CHRS)) == 0L){
    // Parse the file, stopping at a character chunk
    error = ParseIFF( Handle, IFFPARSE_SCAN );
   }

   // Check the return value
   if (error == IFFERR_EOF || error == IFFERR_NOTIFF){
    FTXTBuffer = (struct ClipBuff *) RC_EMPTY_CLIP;
   }else if (error != 0L){
    FTXTBuffer = (struct ClipBuff *) RC_BAD_IFF;
   }else{
    struct ContextNode *cn;
    LONG msize;
    // Get information on the current chunk
    if( (cn = CurrentChunk (Handle)) && (cn->cn_Size > 0L) ){
     // Calculate the size of our buffer
     msize = sizeof (struct ClipBuff) + cn->cn_Size + 1;
     // Allocate a buffer to place our text in
     if( FTXTBuffer = (struct ClipBuff *) AllocMem (msize, MEMF_CLEAR) ){
      // Point the contents buffer at the right place
      FTXTBuffer->Buffer = (VOID *) ((FTXTBuffer) + 1);

      // Read in the characters
      if ((error = ReadChunkBytes (Handle, FTXTBuffer->Buffer, cn->cn_Size))
          == cn->cn_Size){
       FTXTBuffer->Size = msize;                      // Remember the buffer size
       FTXTBuffer->Type = ID_FTXT;                    // Remember the type of buffer
       FTXTBuffer->ClipID = clipIO->io_ClipID;        // Get the current clip ID
      }else{
       // Free the buffer
       FreeMem ((APTR) FTXTBuffer, msize);
       FTXTBuffer = (struct ClipBuff *) RC_COULDNT_READ;
      }
     }else{ // End of if allocate buffer
      FTXTBuffer = (struct ClipBuff *) RC_NOT_ENOUGH_MEMORY;
     }
    }else{  // End of get current chunk info
     FTXTBuffer = (struct ClipBuff *) RC_COULDNT_READ;
    }
   } // End of handle parse

   // Say that we're done reading
   CloseIFF (Handle);
  }else{
   FTXTBuffer = (struct ClipBuff *) error;
  }
 } // End of if there is a handle
 if( FTXTBuffer > 0 ) return ( (char*)(FTXTBuffer->Buffer) );
 else return("");
}
//<

//>"LONG TClipBoard::WriteChunk( struct IFFHandle * iff,LONG id,STRPTR buffer )"
//////////////////////////////////////////////////////////////////////
// TClipBoard::WriteChunk                                           //
// This function is used to write out a text chunk, such as ANNO,   //
// AUTH or CHRS.                                                    //
// in:                                                              //
//  handle    - A valid IFF handle, returned by OpenClip()          //
//  chunk    - A chunk type that supports text, such as ID_ANNO, ID_AUTH,
//        ID_CHRS, ID_(c) or ID_NAME.                               //
//  text    - A pointer to a NULL terminated buffer that contains the
//         text to write.                                           //
// out:                                                             //
//  error    - A value of zero indicates that no errors occurred. A non-
//        zero value indicates an error.  Values are defined in     //
//         <libraries/iffparse.h>.                                  //
//////////////////////////////////////////////////////////////////////
LONG TClipBoard::WriteChunk( struct IFFHandle * iff,LONG id,STRPTR buffer ){
 LONG error = 0L;
 LONG clen = (LONG) strlen (buffer) + 1;

 if (buffer && (clen > 0L)){
  if ((error = PushChunk (iff, 0, id, -1L)) == 0L){
   if ((error = WriteChunkBytes (iff, buffer, clen)) > 0L){
    error = PopChunk (iff);
 }}}
 return (error);
}
//<

//>"int TClipBoard::Copy ( STRPTR buffer )"
//////////////////////////////////////////////////////////////////////
// TClipBoard::Paste                                                //
//////////////////////////////////////////////////////////////////////
int TClipBoard::Copy ( STRPTR buffer ){
 LONG retval = 0L;

 if( Handle ){
  struct ClipboardHandle *cbh = (struct ClipboardHandle *) Handle->iff_Stream;
  struct IOClipReq *clipIO = &(cbh->cbh_Req);

  if( ((struct Library *) IFFParseBase)->lib_Version < 36 ){
   // Bug workaround: clear io_Error field
   clipIO->io_Error = 0;
  }

  // Open the IFF handle for writing
  if ((retval = OpenIFF (Handle, IFFF_WRITE)) == 0L){
   // Start writing the text ...
   if ((retval = PushChunk (Handle, ID_FTXT, ID_FORM, -1L)) == 0){
    // Write out the buffer as characters
    WriteChunk (Handle, ID_CHRS, buffer);

    // Finish out the entire write
    PopChunk (Handle);

    // Assign the clip ID
    retval = clipIO->io_ClipID;
   }
   // Say that we're done writing
   CloseIFF (Handle);
  }
 }
 return (retval);
}
//<

//>"void TClipBoard::FreeBuffer()"
//////////////////////////////////////////////////////////////////////
// TClipBoard::FreeBuffer                                           //
//////////////////////////////////////////////////////////////////////
void TClipBoard::FreeBuffer(){
 // Make sure that buffer is a pointer, not an error
 if( (LONG)FTXTBuffer > 0 ){
  // Free the entire buffer
  FreeMem( (APTR)FTXTBuffer, FTXTBuffer->Size );
 }
}
//<
