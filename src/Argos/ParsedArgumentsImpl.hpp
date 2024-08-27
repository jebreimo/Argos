//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <map>
#include "Argos/IArgumentView.hpp"
#include "ParserData.hpp"

namespace argos
{
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        explicit ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        [[nodiscard]] bool has(ValueId value_id) const;

        [[nodiscard]] const std::vector<std::string>& unprocessed_arguments() const;

        void add_unprocessed_argument(const std::string& arg);

        std::string_view assign_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id);

        std::string_view append_value(ValueId value_id,
                                      const std::string& value,
                                      ArgumentId argument_id);

        void clear_value(ValueId value_id);

        [[nodiscard]] ValueId get_value_id(std::string_view value_name) const;

        [[nodiscard]] std::optional<std::pair<std::string_view, ArgumentId>>
        get_value(ValueId value_id) const;

        [[nodiscard]] std::vector<std::pair<std::string_view, ArgumentId>>
        get_values(ValueId value_id) const;

        [[nodiscard]] std::vector<std::unique_ptr<IArgumentView>>
        get_argument_views(ValueId value_id) const;

        [[nodiscard]] std::unique_ptr<IArgumentView>
        get_argument_view(ArgumentId argument_id) const;

        [[nodiscard]] const std::shared_ptr<ParserData>& parser_data() const;

        [[nodiscard]] ParserResultCode result_code() const;

        void set_result_code(ParserResultCode result_code);

        [[nodiscard]] const OptionData* stop_option() const;

        void set_breaking_option(const OptionData* option);

        [[noreturn]]
        void error(const std::string& message) const;

        [[noreturn]]
        void error(const std::string& message, ArgumentId argument_id);
    private:
        std::multimap<ValueId, std::pair<std::string, ArgumentId>> m_values;
        std::vector<std::tuple<std::string_view, ValueId, ArgumentId>> m_ids;
        std::vector<std::string> m_unprocessed_arguments;
        std::shared_ptr<ParserData> m_data;
        ParserResultCode m_result_code = ParserResultCode::NONE;
        const OptionData* m_stop_option = nullptr;
    };
}
