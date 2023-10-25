// Copyright (c) 2016-2023 Antony Polukhin
// Copyright (c) 2022 Denis Mikhailov
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BHO_PFR_CONFIG_HPP
#define BHO_PFR_CONFIG_HPP
#pragma once

#if __cplusplus >= 201402L || (defined(_MSC_VER) && defined(_MSVC_LANG) && _MSC_VER > 1900)
#include <type_traits> // to get non standard platform macro definitions (__GLIBCXX__ for example)
#endif

/// \file bho/pfr/config.hpp
/// Contains all the macros that describe BHO.PFR configuration, like BHO_PFR_ENABLED
///
/// \note This header file doesn't require C++14 Standard and supports all C++ compilers, even pre C++14 compilers (C++11, C++03...).

// Reminder:
//  * MSVC++ 14.2 _MSC_VER == 1927 <- Loophole is known to work (Visual Studio ????)
//  * MSVC++ 14.1 _MSC_VER == 1916 <- Loophole is known to NOT work (Visual Studio 2017)
//  * MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
//  * MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)

#ifdef BHO_PFR_NOT_SUPPORTED
#   error Please, do not set BHO_PFR_NOT_SUPPORTED value manually, use '-DBHO_PFR_ENABLED=0' instead of it
#endif

#if defined(_MSC_VER)
#   if !defined(_MSVC_LANG) || _MSC_VER <= 1900
#       define BHO_PFR_NOT_SUPPORTED 1
#   endif
#elif __cplusplus < 201402L
#   define BHO_PFR_NOT_SUPPORTED 1
#endif

#ifndef BHO_PFR_USE_LOOPHOLE
#   if defined(_MSC_VER)
#       if _MSC_VER >= 1927
#           define BHO_PFR_USE_LOOPHOLE 1
#       else
#           define BHO_PFR_USE_LOOPHOLE 0
#       endif
#   elif defined(__clang_major__) && __clang_major__ >= 8
#       define BHO_PFR_USE_LOOPHOLE 0
#   else
#       define BHO_PFR_USE_LOOPHOLE 1
#   endif
#endif

#ifndef BHO_PFR_USE_CPP17
#   ifdef __cpp_structured_bindings
#       define BHO_PFR_USE_CPP17 1
#   elif defined(_MSVC_LANG)
#       if _MSVC_LANG >= 201703L
#           define BHO_PFR_USE_CPP17 1
#       else
#           define BHO_PFR_USE_CPP17 0
#       endif
#   else
#       define BHO_PFR_USE_CPP17 0
#   endif
#endif

#if (!BHO_PFR_USE_CPP17 && !BHO_PFR_USE_LOOPHOLE)
#   if (defined(_MSC_VER) && _MSC_VER < 1916) ///< in Visual Studio 2017 v15.9 PFR library with classic engine normally works
#      define BHO_PFR_NOT_SUPPORTED 1
#   endif
#endif

#ifndef BHO_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE
// Assume that libstdc++ since GCC-7.3 does not have linear instantiation depth in std::make_integral_sequence
#   if defined( __GLIBCXX__) && __GLIBCXX__ >= 20180101
#       define BHO_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE 1
#   elif defined(_MSC_VER)
#       define BHO_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE 1
//# elif other known working lib
#   else
#       define BHO_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE 0
#   endif
#endif

#ifndef BHO_PFR_HAS_GUARANTEED_COPY_ELISION
#   if  defined(__cpp_guaranteed_copy_elision) && (!defined(_MSC_VER) || _MSC_VER > 1928)
#       define BHO_PFR_HAS_GUARANTEED_COPY_ELISION 1
#   else
#       define BHO_PFR_HAS_GUARANTEED_COPY_ELISION 0
#   endif
#endif

#ifndef BHO_PFR_ENABLE_IMPLICIT_REFLECTION
#   if  defined(__cpp_lib_is_aggregate)
#       define BHO_PFR_ENABLE_IMPLICIT_REFLECTION 1
#   else
// There is no way to detect potential ability to be reflectable without std::is_aggregare
#       define BHO_PFR_ENABLE_IMPLICIT_REFLECTION 0
#   endif
#endif

#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(maybe_unused)
#       define BHO_PFR_MAYBE_UNUSED [[maybe_unused]]
#   endif
#endif

#ifndef BHO_PFR_MAYBE_UNUSED
#   define BHO_PFR_MAYBE_UNUSED
#endif

#ifndef BHO_PFR_ENABLED
#   ifdef BHO_PFR_NOT_SUPPORTED
#       define BHO_PFR_ENABLED 0
#   else
#       define BHO_PFR_ENABLED 1
#   endif
#endif

#undef BHO_PFR_NOT_SUPPORTED

#endif // BHO_PFR_CONFIG_HPP
