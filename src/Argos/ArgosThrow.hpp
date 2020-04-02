//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argos/ArgosException.hpp"

#define _ARGOS_THROW_3(file, line, msg) \
    throw ::Argos::ArgosException(file ":" #line ": " msg)

#define _ARGOS_THROW_2(file, line, msg) \
    _ARGOS_THROW_3(file, line, msg)
    
#define ARGOS_THROW(msg) \
    _ARGOS_THROW_2(__FILE__, __LINE__, msg)
