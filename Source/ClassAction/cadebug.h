#ifndef CA_DEBUG_H
#define CA_DEBUG_H
/****************************************
 * Init / startup Printf tracing.
 * Enable with DEF=CA_DEBUG in the smakefile.
 ****************************************/

#ifdef CA_DEBUG
#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif
#define CA_D(args) Printf args
#else
#define CA_D(args) /* no debug */
#endif

#endif /* CA_DEBUG_H */
