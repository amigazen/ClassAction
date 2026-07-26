#ifndef PRAGMAS_XFDMASTER_PRAGMAS_H
#define PRAGMAS_XFDMASTER_PRAGMAS_H

/*
**	$VER: xfdmaster_pragmas.h 34.1 (20.9.94)
**
**	Copyright © 1994 by Georg Hörmann.
**	All Rights Reserved.
*/

#ifdef AZTEC_C

#pragma amicall(xfdMasterBase,0x1e,xfdAllocBufferInfo())
#pragma amicall(xfdMasterBase,0x24,xfdFreeBufferInfo(a1))
#pragma amicall(xfdMasterBase,0x2a,xfdAllocSegmentInfo())
#pragma amicall(xfdMasterBase,0x30,xfdFreeSegmentInfo(a1))
#pragma amicall(xfdMasterBase,0x36,xfdRecogBuffer(a0))
#pragma amicall(xfdMasterBase,0x3c,xfdDecrunchBuffer(a0))
#pragma amicall(xfdMasterBase,0x42,xfdRecogSegment(a0))
#pragma amicall(xfdMasterBase,0x48,xfdDecrunchSegment(a0))
#pragma amicall(xfdMasterBase,0x4e,xfdGetErrorText(d0))
#pragma amicall(xfdMasterBase,0x54,xfdTestHunkStructure(a0,d0))
#pragma amicall(xfdMasterBase,0x5a,xfdTestHunkStructureNew(a0,d0))
#pragma amicall(xfdMasterBase,0x60,xfdRelocate(a0,d0,a1,d1))

#else   /* AZTEC_C */

#pragma libcall xfdMasterBase xfdAllocBufferInfo 1E 00
#pragma libcall xfdMasterBase xfdFreeBufferInfo 24 901
#pragma libcall xfdMasterBase xfdAllocSegmentInfo 2A 00
#pragma libcall xfdMasterBase xfdFreeSegmentInfo 30 901
#pragma libcall xfdMasterBase xfdRecogBuffer 36 801
#pragma libcall xfdMasterBase xfdDecrunchBuffer 3C 801
#pragma libcall xfdMasterBase xfdRecogSegment 42 801
#pragma libcall xfdMasterBase xfdDecrunchSegment 48 801
#pragma libcall xfdMasterBase xfdGetErrorText 4E 001
#pragma libcall xfdMasterBase xfdTestHunkStructure 54 0802
#pragma libcall xfdMasterBase xfdTestHunkStructureNew 5A 0802
#pragma libcall xfdMasterBase xfdRelocate 60 190804

#endif  /* AZTEC_C */

#endif  /* PRAGMAS_XFDMASTER_PRAGMAS_H */

