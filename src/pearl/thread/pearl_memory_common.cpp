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
 *  @brief   Implementation of custom memory management functions common
 *           to single- and multi-threaded codes.
 *
 *  This header file provides the implementation of the PEARL-internal
 *  custom memory management functions that are common for both single-
 *  and multi-threaded codes.  See pearl_memory.h for a detailed description
 *  of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "pearl_memory.h"

#include <new>

#include <pearl/Error.h>

using namespace std;
using namespace pearl;
using namespace pearl::detail;


//--- Internal declarations -------------------------------------------------

namespace
{
    // *INDENT-OFF*
    void newHandler();
    // *INDENT-ON*
}   // unnamed namespace


//--- Memory management: Environmental management ---------------------------

void
pearl::detail::memoryInit()
{
    set_new_handler(newHandler);
}


//--- Helper functions ------------------------------------------------------

namespace
{
    /// @brief Custom new handler for the PEARL library.
    ///
    /// Custom new handler for the PEARL library which throws a pearl::MemoryError
    /// exception.
    ///
    /// @throws pearl::MemoryError
    ///     when encountering an out-of-memory situation
    ///
    void
    newHandler()
    {
        throw MemoryError();
    }
}   // unnamed namespace
