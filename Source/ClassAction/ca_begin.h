#ifndef CA_BEGIN_H
#define CA_BEGIN_H
/*
 * First include for every ClassAction TU under SAS/C C++.
 *
 * NDK 3.2 headers place __ASM__/__STDARGS__/__REG__ on struct function-pointer
 * members. SAS/C C++ Error 1522 rejects __asm/__stdargs there; emptying only
 * __ASM__ leaves __REG__ as register __d0 and yields Error 1615 on protos
 * (e.g. proto/alib.h) and Error 39 on gfxnodes/monitor members.
 *
 * Hyperion <clib/compiler-specific.h> only defines each macro if unset.
 * Pre-define them harmlessly here, then include the system header for
 * __CLIB_PROTOTYPE / __CHIP__ / etc.
 *
 * Real registerized hooks must use raw __asm / __saveds and raw
 * "register __a0 Type name" — not __ASM__/__REG__.
 */
#ifdef __ASM__
#undef __ASM__
#endif
#ifdef __STDARGS__
#undef __STDARGS__
#endif
#ifdef __SAVE_DS__
#undef __SAVE_DS__
#endif
#ifdef __INTERRUPT__
#undef __INTERRUPT__
#endif
#ifdef __REG__
#undef __REG__
#endif

#define __ASM__
#define __STDARGS__
#define __SAVE_DS__
#define __INTERRUPT__
#define __REG__(r, p) p

#include <clib/compiler-specific.h>

#endif /* CA_BEGIN_H */
