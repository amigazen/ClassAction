#ifndef PROTO_XFDMASTER_H
#define PROTO_XFDMASTER_H

#include <exec/types.h>
#ifndef LIBRARIES_XFDMASTER_H
#include <libraries/xfdmaster.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern struct xfdMasterBase *xfdMasterBase;
#include <clib/xfdmaster_protos.h>
#include <pragmas/xfdmaster_pragmas.h>

#ifdef __cplusplus
}
#endif

#endif /* PROTO_XFDMASTER_H */
