#include "ca_begin.h"
#include "stormcompat.h"

#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <intuition/classes.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>
#include <workbench/icon.h>
#include <workbench/workbench.h>
#include <gadgets/layout.h>
#include <libraries/gadtools.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/icon_protos.h>
#include <clib/layers_protos.h>
#include <clib/utility_protos.h>
#include <clib/dos_protos.h>

/*
#include <inline/exec.h>
#include <inline/intuition.h>
#include <inline/utility.h>
#include <inline/icon.h>
#include <inline/layers.h>
#include <inline/graphics.h>
*/
//#include "BoopsiStubs.h"


#define G(x)                    ((struct Gadget *)x)

#ifndef GA_BackFill
#define GA_BackFill     (GA_Dummy+44)
#endif

/****************************************************************************/

extern struct Library   *IconBase,
                                                *DOSBase,
                                                *IntuitionBase,
                                                *UtilityBase,
                                                *LayersBase,
                                                *GfxBase;


struct InstanceData {
        ULONG                    secs;
        ULONG                    micro;
        struct Hook             *backfill;
        Object                  *parent;
};
typedef struct InstanceData InstanceData;

/****************************************************************************/
/*
#ifdef __GNUC__
#undef DrawIconState

void DrawIconState( struct RastPort *rp, struct DiskObject *icon, STRPTR label, ULONG x, ULONG y, ULONG state, ULONG tags, ... )
{
        DrawIconStateA( rp, icon, label, x,y, state, (struct TagItem *) &tags );
}
#endif
*/
/****************************************************************************/

void butclass_redraw( Object *o, struct GadgetInfo *gi ){
 struct RastPort *rp;

 if( rp = ObtainGIRPort( gi ) ){
  DoMethod( o, GM_RENDER, (ULONG) gi, (ULONG) rp, GREDRAW_REDRAW );
  ReleaseGIRPort( rp );
 }
}

/****************************************************************************/

ULONG butclass_HANDLEINPUT( Class *cl, Object *o, struct gpInput *gpi ){
 InputEvent *ie = gpi->gpi_IEvent;
 ULONG retval = GMR_MEACTIVE;

 if( ie ){
  switch( ie->ie_Class ){
   case IECLASS_RAWMOUSE:
    switch( ie->ie_Code ){
     case SELECTUP:
      retval = GMR_NOREUSE;
      break;

     case SELECTDOWN:{
       InstanceData     *id = (InstanceData*)INST_DATA( cl, o );
       G(o)->Flags |= GFLG_SELECTED;
       butclass_redraw( o, gpi->gpi_GInfo );
       retval = GMR_NOREUSE | GMR_VERIFY;
       id->secs =
       id->micro = 0UL;
     }
     break;

     case MENUDOWN: retval = GMR_REUSE; break;
    }
   break;
  }
 }
 return retval;
}
/* ORIGINAL:

ULONG butclass_HANDLEINPUT( Class *cl, Object *o, struct gpInput *gpi ){
 InputEvent *ie = gpi->gpi_IEvent;
 ULONG retval = GMR_MEACTIVE;

 if( ie ){
  switch( ie->ie_Class ){
   case IECLASS_RAWMOUSE:
    switch( ie->ie_Code ){
     case SELECTUP: retval = GMR_NOREUSE; break;

     case SELECTDOWN:{
      InstanceData     *id = (InstanceData*)INST_DATA( cl, o );

      if( DoubleClick( id->secs, id->micro, ie->ie_TimeStamp.tv_secs,
                ie->ie_TimeStamp.tv_micro ) ){
       G(o)->Flags |= GFLG_SELECTED;
       butclass_redraw( o, gpi->gpi_GInfo );
       retval = GMR_NOREUSE | GMR_VERIFY;

       id->secs =
       id->micro = 0UL;
      }else{
       id->secs  = ie->ie_TimeStamp.tv_secs;
       id->micro = ie->ie_TimeStamp.tv_micro;
       G(o)->Flags ^= GFLG_SELECTED;
       butclass_redraw( o, gpi->gpi_GInfo );
       retval = GMR_NOREUSE;
      }
     }
     break;

     case MENUDOWN: retval = GMR_REUSE; break;
    }
   break;
  }
 }
 return retval;
}
*/

/****************************************************************************/

BOOL IsIconFrameless( struct DiskObject *icon ){
        ULONG                            frameless;

        IconControl( icon,
                ICONCTRLA_GetFrameless, (ULONG) &frameless,
                TAG_DONE );

        if( frameless == FALSE )
        {
                ULONG   globalfl;

                if( IconControl( NULL,
                                ICONCTRLA_GetGlobalFrameless, (ULONG) &globalfl,
                                TAG_DONE ) )
                {
                        frameless = globalfl;
                }
        }

        return (BOOL)frameless;
}

/****************************************************************************/

ULONG butclass_RENDER( Class *cl, Object *o, struct gpRender *gpr ){
 RastPort         *rp = gpr->gpr_RPort;
 DiskObject       *icon = (struct DiskObject *) G(o)->UserData;
 Hook                     *hook;
 InstanceData     *id = (InstanceData*)INST_DATA( cl, o );
 Layer            *layer = gpr->gpr_GInfo->gi_Layer;
 WORD                             x = G(o)->LeftEdge, y = G(o)->TopEdge;

 hook = InstallLayerHook( layer, id->backfill );

 DrawIconState(rp, icon, NULL, x,y,
         (G(o)->Flags & GFLG_SELECTED) != 0L,
         ICONDRAWA_DrawInfo, (ULONG) gpr->gpr_GInfo->gi_DrInfo,
         ICONDRAWA_Frameless, TRUE,//IsIconFrameless( icon ),
         TAG_DONE );

 InstallLayerHook( layer, hook );

 return 0L;
}

/****************************************************************************/

ULONG butclass_SET( Class *cl, Object *o, struct opSet *ops ){
 InstanceData     *id = (InstanceData*)INST_DATA( cl, o );
 TagItem          *ti, *tlist = ops->ops_AttrList;
 DiskObject       *icon = NULL;

 while( ti = NextTagItem( &tlist ) ){
  switch( ti->ti_Tag ){
   case GA_BackFill:
           id->backfill = (Hook*) ti->ti_Data;
   break;

   case LAYOUT_Parent:
           id->parent = (Object *) ti->ti_Data;
   break;

   case GA_UserData:
           icon = (struct DiskObject *) ti->ti_Data;
   break;
  }
 }

 if( id->parent && icon ){
  Rectangle rect;
  UWORD     w,h;

  GetIconRectangle( NULL, icon, NULL, &rect,
          ICONDRAWA_Borderless, FALSE, //IsIconFrameless( icon ),
          TAG_DONE );

  w = ( rect.MaxX - rect.MinX ) + 1;
  h = ( rect.MaxY - rect.MinY ) + 1;

  SetAttrs( id->parent,
          LAYOUT_ModifyChild, (ULONG) o,
          CHILD_MinWidth, w,
          CHILD_MinHeight, h,
          CHILD_MaxWidth, w,
          CHILD_MaxHeight, h,
          TAG_DONE );
 }

 return (ops->MethodID != OM_NEW) ? DoSuperMethodA( cl, o, (Msg) ops ) : 1L;
}

/****************************************************************************/

ULONG butclass_HITTEST( Class *cl, Object *o, struct gpHitTest *gpht ){
#if 0
 struct DiskObject       *icon = G(o)->UserData;

 if( icon && IsIconFrameless( icon ) ){
  UBYTE   *mask = NULL;
  LONG     w, h;

  IconControl( icon,
          ICONCTRLA_GetWidth, (ULONG) &w,
          ICONCTRLA_GetHeight, (ULONG) &h,
          (G(o)->Flags&GFLG_SELECTED?ICONCTRLA_GetImageMask2:ICONCTRLA_GetImageMask1), (ULONG) &mask,
          TAG_DONE );

  if( mask ){
   if( gpht->gpht_Mouse.X > 0 && gpht->gpht_Mouse.Y > 0 ){
    LONG    i = ((gpht->gpht_Mouse.X-1)/8);

    mask += RASSIZE( w, 1 ) * (gpht->gpht_Mouse.Y-1) + i;

    i = 7-((gpht->gpht_Mouse.X-1)-(i*8));

    if( *mask & (1L<<i) )
            return GMR_GADGETHIT;
   }

   return 0L;
  }
 }
#endif
 return GMR_GADGETHIT;
}

/****************************************************************************/

ULONG butclass_DOMAIN( Class *cl, Object *o, struct gpDomain *gpd ){
 InstanceData     *id = (InstanceData*)INST_DATA( cl, o );
 DiskObject       *icon = (struct DiskObject *) G(o)->UserData;

 if( icon ){
  struct Rectangle         rect;
  UWORD                            w,h;

  GetIconRectangle( NULL, icon, NULL, &rect,
          ICONDRAWA_Borderless, FALSE,//IsIconFrameless( icon ),
          TAG_DONE );

  w = rect.MaxX - rect.MinX + 1;
  h = rect.MaxY - rect.MinY + 1;

  switch( gpd->gpd_Which ){
   case GDOMAIN_MINIMUM:
           gpd->gpd_Domain.Width = w;
           gpd->gpd_Domain.Height = h;
   break;

   case GDOMAIN_NOMINAL:
   case GDOMAIN_MAXIMUM:
           gpd->gpd_Domain.Width = w;
           gpd->gpd_Domain.Height = h;
   break;
  }

  return 1L;
 }

 return 0L;
}

/****************************************************************************/

ULONG __asm butclass_DISPATCH( register __a0 Class *cl , register __a2 Object *o , register __a1 Msg msg ){
 ULONG   ret = 0L;

 switch( msg->MethodID ){
  case GM_HANDLEINPUT:
  case GM_GOACTIVE: ret = butclass_HANDLEINPUT( cl, o, (struct gpInput *) msg ); break;

  case OM_SET:
  case OM_UPDATE: ret = butclass_SET( cl, o, (struct opSet *) msg ); break;

  case GM_HITTEST:
        ret = DoSuperMethodA( cl, o, msg );

      //  ret = butclass_HITTEST( cl, o, (struct gpHitTest *) msg );
        break;

  case GM_RENDER: ret = butclass_RENDER( cl, o, (struct gpRender *) msg ); break;

  case GM_DOMAIN: ret = butclass_DOMAIN( cl, o, (struct gpDomain *) msg ); break;

  case OM_NEW:{
   if( ret = DoSuperMethodA( cl, o, msg ) ){
    struct InstanceData     *id = (InstanceData*)INST_DATA( cl, ret );

    butclass_SET( cl, (Object *) ret, (struct opSet *) msg );
   }
   break;
  }
  default: ret = DoSuperMethodA( cl, o, msg );
 }
 return ret;
}

/****************************************************************************/

Class *MakeIconClass(){
 Class   *cl;

 if( cl = MakeClass( NULL, "gadgetclass", NULL, sizeof( struct InstanceData ), 0L ) ){
  cl->cl_Dispatcher.h_Entry = (HOOKFUNC) butclass_DISPATCH;
 }
 return cl;
}

/****************************************************************************/
