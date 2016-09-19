/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_thread
 *  @brief   Wrapper around Valgrind's `memcheck.h`.
 *
 *  This header file is a simple wrapper around Valgrind's `memcheck.h`
 *  header file.  It ensures that either Valgrind's `memcheck.h` is
 *  included or the user instrumentation macros used by PEARL are defined
 *  as no-ops.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_WRAP_MEMCHECK_H
#define PEARL_WRAP_MEMCHECK_H


#if HAVE(VALGRIND_MEMCHECK_H)

#include <valgrind/memcheck.h>

#else   // !HAVE(VALGRIND_MEMCHECK_H)

/// Defined to non-zero when running under Valgrind, zero otherwise
#define RUNNING_ON_VALGRIND    0

/// Mark @a len bytes starting at @a addr as non-addressable
#define VALGRIND_MAKE_MEM_NOACCESS(addr, len) \
    do {} while (0)

/// Mark @a len bytes starting at @a addr as addressable and undefined
#define VALGRIND_MAKE_MEM_UNDEFINED(addr, len) \
    do {} while (0)

/// Mark @a len bytes starting at @a addr as addressable and defined
#define VALGRIND_MAKE_MEM_DEFINED(addr, len) \
    do {} while (0)

/// @brief Mark @a len bytes starting at @a addr as having been allocated
/// by a malloc()-like function, with @a rzB redzone bytes before and
/// after the block.  The block is marked as addressable, and defined if
/// @a is_zeroed is non-zero or undefined otherwise.
#define VALGRIND_MALLOCLIKE_BLOCK(addr, len, rzB, is_zeroed) \
    do {} while (0)

/// @brief Mark block at @a addr with @a rzB redzone bytes as deallocated
/// by a free()-like function.  The block is marked as non-addressable.
#define VALGRIND_FREELIKE_BLOCK(addr, rzB) \
    do {} while (0)

#endif   // !HAVE(VALGRIND_MEMCHECK_H)


#endif   // !PEARL_WRAP_MEMCHECK_H
