#include <libraries/iffparse.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <devices/clipboard.h>

#define    EXAMPLE    TRUE
#define    MAX_UNITS    256

// Our errors
#define    RC_EMPTY_CLIP        -1L    // Clipboard == empty
#define    RC_NOT_ENOUGH_MEMORY    -4L    // ! enough memory
#define    RC_COULDNT_READ        -5L    // Couldn't read a chunk
#define    RC_BAD_IFF        -8L    // Clipboard contains a bad IFF
#define    RC_INVALID_TYPE        -9L    // don't understand FORM type

// text stuff
#define    ID_FTXT        MAKE_ID('F','T','X','T')
#define    ID_CHRS        MAKE_ID('C','H','R','S')
#define    ID_AUTH        MAKE_ID('A','U','T','H')
#define    ID_NAME        MAKE_ID('N','A','M','E')

// ! really a valid chunk type
#define    ID_SPACE    MAKE_ID(' ',' ',' ',' ')

// Return the contents of the clipboard
struct ClipBuff{
 LONG ClipID;      // Clip ID of assigned to contents
 LONG Type;        // Content type of buffer
 VOID *Buffer;     // Pointer to the buffer area
 LONG Size;        // Size of buffer (plus structure)
};

class TClipBoard{
public:
 TClipBoard();
 ~TClipBoard();
 void Init( int unit=0 );
 int  Copy( char *string );
 char *Paste();
private:
 IFFHandle *Handle;
 ClipBuff *FTXTBuffer;
 LONG WriteChunk( struct IFFHandle * iff,LONG id,STRPTR buffer );
 void FreeBuffer();
};

extern TClipBoard *ClipBoard;

