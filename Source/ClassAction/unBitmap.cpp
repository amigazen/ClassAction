#include "unBitmap.h"

#include <clib/intuition_protos.h>
#include <clib/bitmap_protos.h>
#include <images/bitmap.h>

MREBitmap::MREBitmap( char *filename,Screen *scr ){
  BoopsiImage = NULL;
  Bitmap = NULL;
  if( filename && scr ) LoadFromFile( filename,scr );
}

MREBitmap::~MREBitmap(){
  if( BoopsiImage ) DisposeObject( BoopsiImage );
}

BOOL MREBitmap::LoadFromFile( char *filename,Screen *scr ){
  if( BoopsiImage ) DisposeObject( BoopsiImage );
  BoopsiImage = (Object*)NewObject(
    BITMAP_GetClass(),NULL,
    BITMAP_Masking,TRUE,BITMAP_Screen,scr,BITMAP_SourceFile,filename,
    TAG_END
  );
  if( BoopsiImage ){
    GetAttr( BITMAP_BitMap,BoopsiImage,(ULONG*)&Bitmap );
    GetAttr( BITMAP_Width,BoopsiImage,(ULONG*)&Width );
    GetAttr( BITMAP_Height,BoopsiImage,(ULONG*)&Height );
    GetAttr( BITMAP_MaskPlane,BoopsiImage,(ULONG*)&MaskPlane );
    return( TRUE );
  }else{
    Bitmap    = NULL;
    Width     = NULL;
    Height    = NULL;
    MaskPlane = NULL;
    return( FALSE );
  }
}

Object *MREBitmap::Clone( Screen *scr ){
  if( Bitmap ){
    return(
      (Object*)NewObject(
        BITMAP_GetClass(),NULL,
        BITMAP_Masking,TRUE,
        BITMAP_Screen,scr,
        BITMAP_Width,Width,
        BITMAP_Height,Height,
        BITMAP_BitMap,Bitmap,
        BITMAP_MaskPlane,MaskPlane,
        TAG_END
      )
    );
  }else return( NULL );
}

Image *MREBitmap::IntuiImage(){
  return( (Image*)BoopsiImage );
}

void MREBitmap::Free(){
  if( BoopsiImage ) DisposeObject( BoopsiImage );
  BoopsiImage = NULL;
}

