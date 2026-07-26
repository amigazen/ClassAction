#ifndef CLIB_XFDMASTER_PROTOS_H
#define CLIB_XFDMASTER_PROTOS_H

/*
**	$VER: xfdmaster_protos.h 34.1 (19.9.94)
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 1994 by Georg Hörmann.
**	All Rights Reserved.
*/

struct xfdBufferInfo *xfdAllocBufferInfo(void);
struct xfdSegmentInfo *xfdAllocSegmentInfo(void);
BOOL xfdDecrunchBuffer(struct xfdBufferInfo *);
BOOL xfdDecrunchSegment(struct xfdSegmentInfo *);
void xfdFreeBufferInfo(struct xfdBufferInfo *);
void xfdFreeSegmentInfo(struct xfdSegmentInfo *);
STRPTR xfdGetErrorText(UWORD);
BOOL xfdRecogBuffer(struct xfdBufferInfo *);
BOOL xfdRecogSegment(struct xfdSegmentInfo *);
BOOL xfdTestHunkStructure(APTR, ULONG);

/* New for V34 */
UWORD xfdRelocate(APTR, ULONG, ULONG *, UWORD);
UWORD xfdTestHunkStructureNew(APTR, ULONG);

#endif /* CLIB_XFDMASTER_PROTOS_H */
