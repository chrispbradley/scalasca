/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Implements the representation and analysis of library dependencies
 */

#include <config.h>

#include <iostream>
#include <stdlib.h>
#include <cstring>

#include <otf2_config.hpp>

#include <otf2_config_data.hpp>

using namespace std;

otf2_config_data*
otf2_config_get_backend_data( void )
{
    static otf2_config_data data;

    data.m_cc                    = "gcc";
    data.m_cppflags              = "-I/home/geimer/.local/packages/scalasca/2.trunk-gcc-mpich3/include";
    data.m_use_rpath_flag        = 1;
    data.m_rpath_flag_cc         = "$wl-rpath $wl$libdir";
    data.m_wl_flag               = "-Wl,";
    data.m_aix_libpath           = "";
    data.m_sys_lib_dlsearch_path = "/lib /usr/lib /lib/i386-linux-gnu /usr/lib/i386-linux-gnu /lib/i486-linux-gnu /usr/lib/i486-linux-gnu /usr/local/lib /lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu /lib32 /usr/lib32 ";
    data.m_have_sionlib_support  = HAVE_BACKEND_SIONLIB_SUPPORT;

    deque<string> libs;
    deque<string> ldflags;
    deque<string> rpaths;
    deque<string> dependency_las;

    #define la_object  otf2_config_data::la_object
    #define la_objects ( &data.m_la_objects )
    /* otf2_config_library_dependencies_backend_inc.hpp is
       generated by vendor/common/build-config/generate-library-dependency.sh */
    #include <otf2_config_library_dependencies_backend_inc.hpp>

    data.prepare_rpath_flags();

    return &data;
}
