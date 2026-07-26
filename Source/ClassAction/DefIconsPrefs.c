
#include "DefIconsPrefs.h"

APTR	mempool = NULL;

BOOL load_prefs( STRPTR name, struct List *list )
{
	BPTR	seglist;
	
	mempool = CreatePool( MEMF_PUBLIC, 8192, 8192 );
	if( ! mempool ) return FALSE;
	
	if( seglist = LoadSeg( name ) )
	{								
		struct descr_header	 descr_h;
		UBYTE				*data = (UBYTE *) BADDR( seglist ) + 4;
		
		descr_h.data	= 
		descr_h.start	= data;
		descr_h.end		= data + (*(ULONG *)(data-8));
		descr_h.counter	= 0L;
		descr_h.list	= list;

		if( read_description( &descr_h, 0 ) )
		{				
			if( descr_h.counter )
			{
				EasyReq( NULL, getString( MSG_PARSE_ERROR_LEVELS, MSG_PARSE_ERROR_LEVELS_STR ), name );
				free_prefs( list );
				list = NULL;
			}
		}
		else
		{
			free_prefs( list );
			list = NULL;
		}
		
		UnLoadSeg( seglist );
	}
	else
	{
		ShowDosError( NULL, name );
		list = NULL;
	}
	
	return list != NULL;
}

/****************************************************************************/

BOOL read_description( struct descr_header *descr_h, WORD generation )
{
	struct description	*descr;
	UBYTE				*data = descr_h->data;
	BOOL				 res;

	for( ;; )
	{
		if( data > descr_h->end )
		{
			EasyReq( NULL, getString( MSG_PARSE_ERROR_UEOF, MSG_PARSE_ERROR_UEOF_STR ) ); 
			return FALSE;
		}
		
		switch( *data )
		{ 
			case TYPE_UP_LEVEL:
				if( generation == 0 )
				{					
					EasyReq( NULL, getString( MSG_PARSE_ERROR_UNEXPECTED_TYPE_UP, MSG_PARSE_ERROR_UNEXPECTED_TYPE_UP_STR ), (ULONG) (data-descr_h->start) );
					return FALSE;
				}
			case TYPE_END:
				descr_h->data = data+1;
			return TRUE;
			
			case TYPE_DOWN_LEVEL:
				EasyReq( NULL, getString( MSG_PARSE_ERROR_UNEXPECTED_TYPE_DOWN, MSG_PARSE_ERROR_UNEXPECTED_TYPE_DOWN_STR ), (ULONG) (data-descr_h->start) );
			return FALSE;
		}
		
		res = FALSE;
		
		if( descr = (struct description *) AllocListBrowserNode( 1,
			LBNA_Column, 0,
			//LBNA_MemPool, (ULONG) mempool,
			LBNA_NodeSize, sizeof( *descr ),
			LBNA_Generation, generation,			
			LBNCA_CopyText, TRUE,
			LBNCA_Text, (ULONG) data,
			TAG_DONE ) )
		{
			BOOL			 failure = FALSE;

			NewList( &descr->actions );

			kprintf( "%s\n", data );
			
			data += StrLen( data )+1;
		
			while( *data && ! failure )
			{	
				struct action		*act;
				ULONG			 	 action = (ULONG) *data++;
				
				if( data > descr_h->end )
				{
					EasyReq( NULL, getString( MSG_PARSE_ERROR_UEOF, MSG_PARSE_ERROR_UEOF_STR ) );
					return FALSE;
				}
				
				if( act = (struct action *) AllocListBrowserNode( 2,
					LBNA_Column, 0,
					//LBNA_MemPool, (ULONG) mempool,					
					LBNA_NodeSize, sizeof( *act ),
					LBNA_Column, 1,
					LBNCA_RenderHook, (ULONG) &RenderHook,
					TAG_DONE ) )
				{
					STRPTR	name;
				
					act->action = action
					kprintf( "Action: %ld\n", action );
				
					switch( action )
					{				
						case ACT_MATCH:
						{	
							ULONG	len;
							
							kprintf( "ACT_MATCH\n" );
						
							act->act_match.offs  = *data++ << 8;
							act->act_match.offs |= *data++;
							act->act_match.len   = *data++;
							
							kprintf( "Offs: %ld, Len: %ld\n", act->act_match.offs, act->act_match.len );
							
							len = abs( act->act_match.len );
							
							if( act->act_match.str = AllocPooled( mempool, len ) )
							{
								CopyMem( data, act->act_match.str, len );
								data += len;							
							}
							else 
							{
								EasyReq( NULL, getString( MSG_PARSE_ERROR_OUT_OF_MEMORY, MSG_PARSE_ERROR_OUT_OF_MEMORY_STR ) );
								failure = TRUE;
							}
							
							name = getString( MSG_ACTION_MATCH, MSG_ACTION_MATCH_STR );
						}
						break;
						
						case ACT_SEARCH:
						case ACT_SEARCHSKIPSPACES:
						{
							ULONG	len;
													
							kprintf( "ACT_SEARCH(SKIPSPACES)\n" );
						
							act->act_search.len = *data++;
							
							len = abs( act->act_search.len );
							
							kprintf( "Len: %ld\n", act->act_search.len );
							
							if( act->act_search.str = AllocPooled( mempool, len ) )
							{
								CopyMem( data, act->act_search.str, len );
								data += len;
							}
							else 
							{
								EasyReq( NULL, getString( MSG_PARSE_ERROR_OUT_OF_MEMORY, MSG_PARSE_ERROR_OUT_OF_MEMORY_STR ) );
								failure = TRUE;
							}
							
							name = getString( MSG_ACTION_SEARCH, MSG_ACTION_SEARCH_STR );
						}
						break;
						
						case ACT_FILESIZE:
							kprintf( "ACT_FILESIZE\n" );
							
							act->act_filesize  = *data++ << 24;
							act->act_filesize |= *data++ << 16;
							act->act_filesize |= *data++ << 8;
							act->act_filesize |= *data++;
							
							name = getString( MSG_ACTION_FILESIZE, MSG_ACTION_FILESIZE_STR );
						break;
						
						case ACT_NAMEPATTERN:
						{
							LONG	len;
							
							kprintf( "ACT_NAMEPATTERN\n" );
						
							len = StrLen( data ) + 1;
							
							if( act->act_namepattern = AllocPooled( mempool, len ) )
							{
								kprintf( "Pattern: %s\n", data );
							
								CopyMem( data, act->act_namepattern, len );
								
								data += len;
							}
							else 
							{
								EasyReq( NULL, getString( MSG_PARSE_ERROR_OUT_OF_MEMORY, MSG_PARSE_ERROR_OUT_OF_MEMORY_STR ) );
								failure = TRUE;
							}
							
							name = getString( MSG_ACTION_PATTERN, MSG_ACTION_PATTERN_STR );
						}			
						break;
						
						case ACT_PROTECTION:
							kprintf( "ACT_PROTECTION\n" );
						
							act->act_protection.mask  = *data++ << 24;
							act->act_protection.mask |= *data++ << 16;
							act->act_protection.mask |= *data++ << 8;
							act->act_protection.mask |= *data++;
							
							act->act_protection.bits  = *data++ << 24;
							act->act_protection.bits |= *data++ << 16;
							act->act_protection.bits |= *data++ << 8;
							act->act_protection.bits |= *data++;
							
							kprintf( "Mask: %08lx, Bits: %08lx\n", 
								act->act_protection.mask,
								act->act_protection.bits );
								
							name = getString( MSG_ACTION_PROTECTION, MSG_ACTION_PROTECTION_STR );
						break;
						
						case ACT_OR:
							name = getString( MSG_ACTION_OR, MSG_ACTION_OR_STR );
						break;
						
						case ACT_MACROCLASS:
							name = getString( MSG_ACTION_MACROCLASS, MSG_ACTION_MACROCLASS_STR );
						break;
						
						case ACT_ISASCII:
							name = getString( MSG_ACTION_ISASCII, MSG_ACTION_ISASCII_STR );
						break;
						
						default:
							EasyReq( NULL, getString( MSG_PARSE_ERROR_BAD_ACTION, MSG_PARSE_ERROR_BAD_ACTION_STR ), action, (ULONG) (data-descr_h->start) );
							failure = TRUE;
					}
					
					if( ! failure )
					{						
						AddTail( &descr->actions, &act->link );
						
						SetListBrowserNodeAttrs( &act->link,
							LBNCA_Text, (ULONG) name,
							TAG_DONE );
					}
				}
				else 
				{
					EasyReq( NULL, getString( MSG_PARSE_ERROR_OUT_OF_MEMORY, MSG_PARSE_ERROR_OUT_OF_MEMORY_STR ) );
					failure = TRUE;
				}			
			}
			
			data++; // skip ACT_END
			
			if( ! failure )
			{	
				AddTail( descr_h->list, &descr->link );

				switch( *data++ )
				{	
					case TYPE_DOWN_LEVEL:
						SetListBrowserNodeAttrs( &descr->link,
							LBNA_Flags, LBFLG_HASCHILDREN | LBFLG_SHOWCHILDREN,
							TAG_DONE );
					
						descr_h->counter++;
						descr_h->data = data;						
						res = read_description( descr_h, generation+1 );
						descr_h->counter--;
						
						if( res )
						{
							data = descr_h->data;
							
							switch( *(data-1) )
							{
								case TYPE_DOWN_LEVEL:
									data--;
								case TYPE_UP_LEVEL:
								continue;
							}
						}								
					break;
					
					case TYPE_UP_LEVEL:
					case TYPE_END:
						res = TRUE;
					break;
					
					default:
						data--;
					continue;
				}
			}
		}
		else
		{
			EasyReq( NULL, getString( MSG_PARSE_ERROR_OUT_OF_MEMORY, MSG_PARSE_ERROR_OUT_OF_MEMORY_STR ) );
		}
		
		break;
	}

	descr_h->data = data;

	return res;
}

/****************************************************************************/

void free_prefs( struct List *list )
{
	if( list )
	{		
		struct Node	*ln;
		
		while( ln = RemHead( list ) )
		{		
			FreeListBrowserList( &((struct description *)ln)->actions );
			FreeListBrowserNode( ln );
		}
	}
	
	DeletePool( mempool ); mempool = NULL;
}

/****************************************************************************/
