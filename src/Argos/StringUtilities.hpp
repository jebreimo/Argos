//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string_view>

namespace Argos
{
    bool areEqualCI(std::string_view str1, std::string_view str2);

    bool areEqual(std::string_view str1, std::string_view str2,
                  bool caseInsensitive);

    bool startsWith(std::string_view str, std::string_view prefix);

    bool startsWithCI(std::string_view str, std::string_view prefix);

    bool startsWith(std::string_view str, std::string_view prefix,
                    bool caseInsensitive);

    bool isLessCI(std::string_view str1, std::string_view str2);

    bool isLess(std::string_view str1, std::string_view str2,
                bool caseInsensitive);

    bool removeUnderscoresFromNumber(std::string& num);
}
