
#ifndef DEFICONSPREFS_H
#define DEFICONSPREFS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

/****************************************************************************/

enum {
	ACT_END,
	ACT_MATCH,
	ACT_SEARCH,
	ACT_SEARCHSKIPSPACES,
	ACT_FILESIZE,
	ACT_NAMEPATTERN,
	ACT_PROTECTION,
	ACT_OR,
	ACT_ISASCII,
	ACT_MACROCLASS = 20
};

enum {
	TYPE_END,
	TYPE_DOWN_LEVEL,
	TYPE_UP_LEVEL
};

/****************************************************************************/

struct action {
	struct Node		link;
	ULONG			action;
	
	union {
		struct {
			STRPTR	str;
			UWORD	offs;
			BYTE	len;
		} act_match;
	
		struct {
			STRPTR	str;
			BYTE	len;
		} act_search;
		
		ULONG	act_filesize;
		
		STRPTR	act_namepattern;
		
		struct {
			ULONG	mask;
			ULONG	bits;
		} act_protection;
	} dummy;
};

struct description {
	struct Node		link;
	struct List		actions;
};

struct descr_header {
	UBYTE			 *data;
	UBYTE			 *start;
	UBYTE			 *end;
	struct List		 *list;
	LONG			  counter;
};

#define act_match		dummy.act_match
#define act_search		dummy.act_search
#define act_filesize	dummy.act_filesize
#define act_namepattern	dummy.act_namepattern
#define act_protection	dummy.act_protection

/****************************************************************************/

#endif
