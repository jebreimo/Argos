//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Argos/ArgosException.hpp"

#define ARGOS_THROW(msg) \
        throw ::Argos::ArgosException((msg), __FILE__, __LINE__, __func__)
