//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>

/**
 * @file
 * @brief Defines the ArgosException class.
 */

/**
 * @brief The namespace for all Argos classes and functions.
 */
namespace argos
{
    /**
     * @brief The exception class used throughout Argos.
     */
    class ArgosException final : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}
