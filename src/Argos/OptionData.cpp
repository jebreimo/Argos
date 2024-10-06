//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-10-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "OptionData.hpp"
#include "ArgosThrow.hpp"

namespace argos
{
    namespace
    {
        bool check_flag_with_equal(const std::string& flag,
                                   const OptionData& od)
        {
            const auto eq_pos = flag.find('=');
            if (eq_pos == std::string::npos)
                return true;
            if (eq_pos != flag.size() - 1)
                ARGOS_THROW("Options can not have a '=' in the middle: " + flag);
            if (od.argument.empty())
                ARGOS_THROW("Options ending with '=' must have a named argument: " + flag);
            return true;
        }

        bool check_standard_flag(const std::string& flag,
                                 const OptionData& od)
        {
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() < 2)
                return false;
            if (flag[0] != '-')
                return false;
            if (flag.size() == 2)
                return true;
            if (flag[1] != '-')
                return false;
            return check_flag_with_equal(flag, od);
        }

        bool check_flag(const std::string& flag, char prefix,
                        const OptionData& od)
        {
            if (flag.size() < 2 || flag[0] != prefix)
                return false;
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() == 2)
                return true;
            return check_flag_with_equal(flag, od);
        }
    }

    void validate_and_update(OptionData& option, OptionStyle style)
    {
        if (option.flags.empty())
            ARGOS_THROW("Option must have one or more flags.");

        for (auto& flag : option.flags)
        {
            bool ok = false;
            switch (style)
            {
            case OptionStyle::STANDARD:
                ok = check_standard_flag(flag, option);
                break;
            case OptionStyle::SLASH:
                ok = check_flag(flag, '/', option);
                break;
            case OptionStyle::DASH:
                ok = check_flag(flag, '-', option);
                break;
            default:
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!option.argument.empty() && !option.constant.empty())
            ARGOS_THROW("Option cannot have both argument and constant.");

        switch (option.operation)
        {
        case OptionOperation::NONE:
            if (!option.constant.empty())
                ARGOS_THROW("NONE-options cannot have a constant.");
            if (!option.alias.empty())
                ARGOS_THROW("NONE-options cannot have an alias.");
            break;
        case OptionOperation::ASSIGN:
            if (option.argument.empty() && option.constant.empty())
                option.constant = "1";
            break;
        case OptionOperation::APPEND:
            if (option.argument.empty() && option.constant.empty())
                ARGOS_THROW("Options that appends must have either constant or argument.");
            break;
        case OptionOperation::CLEAR:
            if (!option.argument.empty() || !option.constant.empty())
                option.constant = "1";
            if (!option.optional)
                ARGOS_THROW("CLEAR-options must be optional.");
            break;
        }
    }
}
