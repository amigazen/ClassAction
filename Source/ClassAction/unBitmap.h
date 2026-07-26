#ifndef UN_BITMAP_H
#define UN_BITMAP_H

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>

class MREBitmap{
  public:
    MREBitmap( char *filename=NULL,Screen *scr=NULL  );
    ~MREBitmap();
    BOOL   LoadFromFile( char *filename,Screen *scr );
    Object *Clone( Screen *scr );
    Image  *IntuiImage();
    void   Free();

    BitMap *Bitmap;
    APTR   MaskPlane;
    ULONG  Width,Height;

  private:
    Object *BoopsiImage;
};

#endif
