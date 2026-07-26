#ifndef LIBRARIES_XFDMASTER_H
#define LIBRARIES_XFDMASTER_H

/*
**	$VER: xfdmaster.h 34.1 (19.9.94)
**
**	Copyright © 1994 by Georg Hörmann.
**	All Rights Reserved.
*/

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

/*********************
*                    *
*    Library Base    *
*                    *
*********************/

struct xfdMasterBase {
	struct Library LibNode;
	ULONG	xfdm_SegList;		/* PRIVATE! */
	APTR	xfdm_DosBase;
	APTR	xfdm_FirstSlave;
	APTR	xfdm_FirstForeman;	/* PRIVATE! */
};

#define XFDM_VERSION	34		/* for OpenLibrary() */
#define XFDM_NAME	"xfdmaster.library"

/********************
*                   *
*    Buffer Info    *
*                   *
********************/

struct xfdBufferInfo {
	APTR	xfdbi_SourceBuffer;	/* Pointer to source buffer */
	ULONG	xfdbi_SourceBufLen;	/* Length of source buffer */
	APTR	xfdbi_Slave;		/* PRIVATE! */
	STRPTR	xfdbi_PackerName;	/* Name of recognized packer */
	UWORD	xfdbi_PackerFlags;	/* Flags for recognized packer */
	UWORD	xfdbi_Error;		/* Error return code */
	APTR	xfdbi_TargetBuffer;	/* Pointer to target buffer */
	ULONG	xfdbi_TargetBufMemType;	/* Memtype of target buffer */
	ULONG	xfdbi_TargetBufLen;	/* Full length of buffer */
	ULONG	xfdbi_TargetBufSaveLen;	/* Used length of buffer */
	ULONG	xfdbi_DecrAddress;	/* Address to load decrunched file */
	ULONG	xfdbi_JmpAddress;	/* Address to jump in file */
	APTR	xfdbi_Special;		/* Special decrunch info (eg. password) */
};

#define xfdbi_MaxSpecialLen xfdbi_Error	/* Max. length of special info */

/*********************
*                    *
*    Segment Info    *
*                    *
*********************/

struct xfdSegmentInfo {
	ULONG	xfdsi_SegList;		/* BPTR to segment list */
	APTR	xfdsi_Slave;		/* PRIVATE! */
	STRPTR	xfdsi_PackerName;	/* Name of recognized packer */
	UWORD	xfdsi_PackerFlags;	/* Flags for recognized packer */
	UWORD	xfdsi_Error;		/* Error return code */
	APTR	xfdsi_Special;		/* Special decrunch info (eg. password) */
	UWORD	xfdsi_RelMode;		/* (V34) Relocation mode */
	UWORD	xfdsi_Reserved0;	/* (V34) Reserved */
};

#define xfdsi_MaxSpecialLen xfdsi_Error	/* Max. length of special info */

/********************
*                   *
*    Error Codes    *
*                   *
********************/

#define XFDERR_OK		$0000	/* No errors */

#define XFDERR_NOMEMORY		$0001	/* Error allocating memory */
#define XFDERR_NOSLAVE		$0002	/* No slave entry in info structure */
#define XFDERR_NOTSUPPORTED	$0003	/* Slave doesn't support called function */
#define XFDERR_UNKNOWN		$0004	/* Unknown file */
#define XFDERR_NOSOURCE		$0005	/* No sourcebuffer/seglist specified */
#define XFDERR_WRONGPASSWORD	$0006	/* Wrong password for decrunching */
#define XFDERR_BADHUNK		$0007	/* Bad hunk structure */
#define XFDERR_CORRUPTEDDATA	$0008	/* Crunched data is corrupted */
#define XFDERR_MISSINGRESOURCE	$0009	/* (V34) Missing resource (eg. library) */

#define XFDERR_UNDEFINEDHUNK	$1000	/* (V34) undefined hunk type */
#define XFDERR_NOHUNKHEADER	$1001	/* (V34) file is not executable */
#define XFDERR_BADEXTTYPE	$1002	/* (V34) bad hunk_ext type */
#define XFDERR_BUFFERTRUNCATED	$1003	/* (V34) unexpected end of file */
#define XFDERR_WRONGHUNKAMOUNT	$1004	/* (V34) wrong amount of hunks */

#define XFDERR_UNSUPPORTEDHUNK	$2000	/* (V34) hunk type not supported */
#define XFDERR_BADRELMODE	$2001	/* (V34) unknown XFDREL_#? mode */

/*******************************
*                              *
*    Relocation Modes (V34)    *
*                              *
*******************************/

#define XFDREL_DEFAULT		$0000	/* Use memory types given by hunk_header */
#define XFDREL_FORCECHIP	$0001	/* Force all hunks to chip ram */
#define XFDREL_FORCEFAST	$0002	/* Force all hunks to fast ram */

/*************************************
*                                    *
*    Values for xfd??_PackerFlags    *
*                                    *
*************************************/

/* Bit numbers */
#define XFDPFB_RELOC	0	/* Relocatible file packer */
#define XFDPFB_ADDR	1	/* Absolute address file packer */
#define XFDPFB_DATA	2	/* Data file packer */

#define XFDPFB_PASSWORD	4	/* Packer requires password */
#define XFDPFB_RELMODE	5	/* (V34) Decruncher supports xfdsi_RelMode */

/* Bit masks */
#define XFDPFF_RELOC	(1<<XFDPFB_RELOC)
#define XFDPFF_ADDR	(1<<XFDPFB_ADDR)
#define XFDPFF_DATA	(1<<XFDPFB_DATA)

#define XFDPFF_PASSWORD	(1<<XFDPFB_PASSWORD)
#define XFDPFF_RELMODE	(1<<XFDPFB_RELMODE)

/****************
*               *
*    Foreman    *
*               *
****************/

struct xfdForeMan {
	ULONG	xfdf_Security;		/* moveq #-1,d0 ; rts */
	ULONG	xfdf_ID;		/* Set to XFDF_ID */
	UWORD	xfdf_Version;		/* Set to XFDF_VERSION */
	UWORD	xfdf_Reserved;		/* Not used by now, set to NULL */
	ULONG	xfdf_Next;		/* PRIVATE! */
	ULONG	xfdf_SegList;		/* PRIVATE! */
	APTR	xfdf_FirstSlave;	/* First slave (see below) */
};

#define XFDF_ID		(('X'<<24)|('F'<<16)|('D'<<8)|('F'))
#define XFDF_VERSION	1

/**************
*             *
*    Slave    *
*             *
**************/

struct xfdSlave {
	APTR	xfds_Next;		/* Next slave (or NULL) */
	UWORD	xfds_Version;		/* Set to XFDS_VERSION */
	UWORD	xfds_MasterVersion;	/* Minimum XFDM_VERSION required */
	STRPTR	xfds_PackerName;	/* Name of packer ('\0' terminated) */
	UWORD	xfds_PackerFlags;	/* Flags for packer */
	UWORD	xfds_MaxSpecialLen;	/* Max. length of special info (eg. password) */
	APTR	xfds_RecogBuffer;	/* APTR to buffer recognition code (or NULL) */
	APTR	xfds_DecrunchBuffer;	/* APTR to buffer decrunch code (or NULL) */
	APTR	xfds_RecogSegment;	/* APTR to segment recognition code (or NULL) */
	APTR	xfds_DecrunchSegment;	/* APTR to segment decrunch code (or NULL) */
};

#define XFDS_VERSION	1

#endif /* LIBRARIES_XFDMASTER_H */
