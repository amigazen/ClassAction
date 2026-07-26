#ifndef CA_STORMCOMPAT_H
#define CA_STORMCOMPAT_H
/****************************************
 * StormC used bare Amiga type names
 * (Library *, Window *, ...). SAS/C +
 * NDK headers need struct tags unless
 * we typedef them here for the port.
 ****************************************/

#include "ca_begin.h"

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif
#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif
#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif
#ifndef EXEC_PORTS_H
#include <exec/ports.h>
#endif
#ifndef EXEC_INTERRUPTS_H
#include <exec/interrupts.h>
#endif
#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif
#ifndef LIBRARIES_LOCALE_H
#include <libraries/locale.h>
#endif
#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif
#ifndef LIBRARIES_ASL_H
#include <libraries/asl.h>
#endif
#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif
#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif
#ifndef DOS_DATETIME_H
#include <dos/datetime.h>
#endif
#ifndef DOS_EXALL_H
#include <dos/exall.h>
#endif
#ifndef DEVICES_INPUTEVENT_H
#include <devices/inputevent.h>
#endif
#ifndef DOS_NOTIFY_H
#include <dos/notify.h>
#endif
#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif
#ifndef GRAPHICS_CLIP_H
#include <graphics/clip.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
/* Class comes from intuition/classes.h via classusr / MakeClass users */
typedef struct Library       Library;
typedef struct Catalog       Catalog;
typedef struct Locale        Locale;
typedef struct Screen        Screen;
typedef struct Window        Window;
typedef struct Gadget        Gadget;
typedef struct Menu          Menu;
typedef struct Image         Image;
typedef struct RastPort      RastPort;
typedef struct BitMap        BitMap;
typedef struct TextFont      TextFont;
typedef struct DiskObject    DiskObject;
typedef struct FileRequester FileRequester;
typedef struct FontRequester FontRequester;
typedef struct ScreenModeRequester ScreenModeRequester;
typedef struct MsgPort       MsgPort;
typedef struct Hook          Hook;
typedef struct List          List;
typedef struct Node          Node;
typedef struct Process       Process;
typedef struct Task          Task;
typedef struct IORequest     IORequest;
typedef struct IntuiMessage  IntuiMessage;
typedef struct DateTime      DateTime;
typedef struct InputEvent    InputEvent;
typedef struct ExAllData     ExAllData;
typedef struct ExAllControl  ExAllControl;
typedef struct EasyStruct    EasyStruct;
typedef struct NotifyRequest NotifyRequest;
typedef struct FileInfoBlock FileInfoBlock;
typedef struct TagItem       TagItem;
typedef struct Layer         Layer;
typedef struct Rectangle     Rectangle;
/* Class is already typedef'd to struct IClass in <intuition/classes.h>.
 * DateStamp is also a dos.library function — use "struct DateStamp". */

#endif /* CA_STORMCOMPAT_H */
