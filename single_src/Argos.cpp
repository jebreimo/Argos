#include "Argos.hpp"
//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#define _ARGOS_THROW_3(file, line, msg) \
    throw ::Argos::ArgosException(file ":" #line ": " msg)

#define _ARGOS_THROW_2(file, line, msg) \
    _ARGOS_THROW_3(file, line, msg)
    
#define ARGOS_THROW(msg) \
    _ARGOS_THROW_2(__FILE__, __LINE__, msg)

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <string>

namespace Argos
{
    struct ArgumentData
    {
        std::string name;
        std::string text;
        std::string section;
        std::string value;
        ArgumentCallback callback;
        unsigned minCount = 1;
        unsigned maxCount = 1;
        Visibility visibility = Visibility::NORMAL;
        int id = 0;
        ValueId valueId = {};
        ArgumentId argumentId = {};
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo-> All rights reserved->
// Created by Jan Erik Breimo on 2020-01-07->
//
// This file is distributed under the BSD License->
// License text is included with the source distribution->
//****************************************************************************

namespace Argos
{
    Argument::Argument()
        : m_Argument(std::make_unique<ArgumentData>())
    {}

    Argument::Argument(const std::string& name)
        : m_Argument(std::make_unique<ArgumentData>())
    {
        m_Argument->name = name;
    }

    Argument::Argument(const Argument& rhs)
        : m_Argument(rhs.m_Argument
                     ? std::make_unique<ArgumentData>(*rhs.m_Argument)
                     : std::unique_ptr<ArgumentData>())
    {}

    Argument::Argument(Argument&& rhs) noexcept
        : m_Argument(std::move(rhs.m_Argument))
    {}

    Argument::~Argument() = default;

    Argument& Argument::operator=(const Argument& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_Argument)
                m_Argument = std::make_unique<ArgumentData>(*rhs.m_Argument);
            else
                m_Argument = {};
        }
        return *this;
    }

    Argument& Argument::operator=(Argument&& rhs) noexcept
    {
        m_Argument = std::move(rhs.m_Argument);
        return *this;
    }

    Argument& Argument::text(const std::string& text)
    {
        checkArgument();
        m_Argument->text = text;
        return *this;
    }

    Argument& Argument::section(const std::string& name)
    {
        checkArgument();
        m_Argument->section = name;
        return *this;
    }

    Argument& Argument::value(const std::string& id)
    {
        checkArgument();
        m_Argument->value = id;
        return *this;
    }

    Argument& Argument::callback(ArgumentCallback callback)
    {
        checkArgument();
        m_Argument->callback = move(callback);
        return *this;
    }

    Argument& Argument::visibility(Visibility visibility)
    {
        checkArgument();
        m_Argument->visibility = visibility;
        return *this;
    }

    Argument& Argument::id(int id)
    {
        checkArgument();
        m_Argument->id = id;
        return *this;
    }

    Argument& Argument::name(const std::string& name)
    {
        checkArgument();
        m_Argument->name = name;
        return *this;
    }

    Argument& Argument::optional(bool optional)
    {
        checkArgument();
        if (optional)
            m_Argument->minCount = 0;
        else if (m_Argument->minCount == 0)
            m_Argument->minCount = 1;
        return *this;
    }

    Argument& Argument::count(unsigned n)
    {
        if (n <= 0)
            ARGOS_THROW("Argument's count must be greater than 0.");
        checkArgument();
        m_Argument->minCount = m_Argument->maxCount = n;
        return *this;
    }

    Argument& Argument::count(unsigned minCount, unsigned maxCount)
    {
        if (maxCount == 0)
            ARGOS_THROW("Argument's max count must be greater than 0.");
        if (maxCount < minCount)
            ARGOS_THROW("Argument's max count cannot be less than its min count.");
        checkArgument();
        m_Argument->minCount = minCount;
        m_Argument->maxCount = maxCount;
        return *this;
    }

    std::unique_ptr<ArgumentData> Argument::release()
    {
        checkArgument();
        return std::move(m_Argument);
    }

    void Argument::checkArgument() const
    {
        if (!m_Argument)
            ARGOS_THROW("Cannot use Argument instance after"
                        " release() has been called.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <memory>
#include <vector>

namespace Argos
{
    class ArgumentCounter
    {
    public:
        ArgumentCounter();

        explicit ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        ArgumentCounter(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                size_t argumentCount);

        const ArgumentData* nextArgument();

        size_t count() const;

        bool isComplete() const;

        static std::pair<size_t, size_t> getMinMaxCount(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);

        static bool requiresArgumentCount(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments);
    private:
        using Counter = std::pair<size_t, const ArgumentData*>;
        std::vector<Counter> m_Counters;
        size_t m_Index = 0;
        size_t m_FirstOptional = 0;
        size_t m_Counter = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <optional>

namespace Argos
{
    namespace
    {
        size_t findFirstOptional(const std::vector<std::unique_ptr<ArgumentData>>& arguments)
        {
            size_t result = 0;
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                if (arguments[i]->minCount > 0)
                    result = i + 1;
            }
            return result;
        }

        void makeArgumentCounters(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                std::vector<std::pair<size_t, const ArgumentData*>>& counters,
                size_t& firstOptional)
        {
            firstOptional = findFirstOptional(arguments);
            for (size_t i = 0; i < arguments.size(); ++i)
            {
                auto& a = arguments[i];
                if (i + 1 == firstOptional && a->minCount != a->maxCount)
                {
                    counters.emplace_back(a->minCount, a.get());
                    counters.emplace_back(a->maxCount - a->minCount, a.get());
                }
                else
                {
                    counters.emplace_back(a->maxCount, a.get());
                }
            }
        }

        std::vector<std::pair<size_t, const ArgumentData*>>
        makeArgumentCounters(
                const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                size_t n)
        {
            auto minmax = ArgumentCounter::getMinMaxCount(arguments);
            if (n < minmax.first)
                n = 0;
            else if (n > minmax.second)
                n = minmax.second - minmax.first;
            else
                n -= minmax.first;

            std::vector<std::pair<size_t, const ArgumentData*>> result;
            for (auto& arg : arguments)
            {
                if (n == 0 || arg->minCount == arg->maxCount)
                {
                    result.emplace_back(arg->minCount, arg.get());
                }
                else if (arg->minCount + n <= arg->maxCount)
                {
                    result.emplace_back(arg->minCount + n, arg.get());
                    n = 0;
                }
                else
                {
                    result.emplace_back(arg->maxCount, arg.get());
                    n -= arg->maxCount - arg->minCount;
                }
            }
            return result;
        }
    }

    ArgumentCounter::ArgumentCounter()
        : m_Counters()
    {}

    ArgumentCounter::ArgumentCounter(const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        makeArgumentCounters(arguments, m_Counters, m_FirstOptional);
    }

    ArgumentCounter::ArgumentCounter(const std::vector<std::unique_ptr<ArgumentData>>& arguments,
                                     size_t argumentCount)
        : m_Counters(makeArgumentCounters(arguments, argumentCount)),
          m_FirstOptional(m_Counters.size())
    {}

    const ArgumentData* ArgumentCounter::nextArgument()
    {
        while (m_Index != m_Counters.size() && m_Counters[m_Index].first == 0)
            ++m_Index;

        if (m_Index == m_Counters.size())
            return nullptr;

        ++m_Counter;
        --m_Counters[m_Index].first;
        return m_Counters[m_Index].second;
    }

    size_t ArgumentCounter::count() const
    {
        return m_Counter;
    }

    bool ArgumentCounter::isComplete() const
    {
        return m_Index >= m_FirstOptional
               || (m_Index + 1 == m_FirstOptional
                   && m_Counters[m_Index].first == 0);
    }

    std::pair<size_t, size_t>
    ArgumentCounter::getMinMaxCount(const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        size_t lo = 0, hi = 0;
        for (auto& arg : arguments)
        {
            lo += arg->minCount;
            if (hi != SIZE_MAX)
            {
                if (arg->maxCount > SIZE_MAX - hi)
                    hi = SIZE_MAX;
                else
                    hi += arg->maxCount;
            }
        }
        return {lo, hi};
    }

    bool ArgumentCounter::requiresArgumentCount(
            const std::vector<std::unique_ptr<ArgumentData>>& arguments)
    {
        bool deterministic = true;
        for (auto& arg : arguments)
        {
            if (!deterministic)
                return true;
            if (arg->minCount != arg->maxCount)
                deterministic = false;
        }
        return false;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    struct OptionData
    {
        std::vector<std::string> flags;
        std::string text;
        std::string section;
        std::string value;
        std::string argument;
        std::string constant;
        OptionCallback callback;
        OptionOperation operation = OptionOperation::ASSIGN;
        OptionType type = OptionType::NORMAL;
        Visibility visibility = Visibility::NORMAL;
        bool optional = true;
        int id = 0;
        ArgumentId argumentId = {};
        ValueId valueId = {};
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iosfwd>
#include <string_view>

namespace Argos
{
    class TextWriter
    {
    public:
        explicit TextWriter(unsigned lineWidth = 80);

        std::ostream* stream() const;

        void setStream(std::ostream* stream);

        unsigned indentation() const;

        bool setIndentation(unsigned indent);

        bool write(std::string_view str, bool force = false);

        void newline();

        void flush();

        void tab();

        unsigned spaces() const;

        void setSpaces(unsigned n);

        unsigned currentWidth() const;

        unsigned remainingWidth() const;

        bool isCurrentLineEmpty() const;

        unsigned lineWidth() const;

        void setLineWidth(unsigned width);

        std::string_view currentLine() const;
    private:
        std::ostream* m_Stream;
        std::string m_Line;
        unsigned m_LineWidth;
        unsigned m_CurrentLineWidth = 0;
        unsigned m_TabSize = 4;
        unsigned m_Indent = 0;
        unsigned m_Spaces = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <list>
#include <map>

namespace Argos
{
    class WordSplitter
    {
    public:
        void addWord(std::string wordRule);

        std::tuple<std::string_view, char, std::string_view>
        split(std::string_view word, size_t startIndex, size_t maxLength,
              bool mustSplit) const;
    private:
        std::tuple<std::string_view, char, std::string_view>
        defaultRule(std::string_view word, size_t maxLength) const;

        struct Split
        {
            unsigned index;
            char separator;
        };
        std::map<std::string_view, std::vector<Split>> m_Splits;
        std::list<std::string> m_Strings;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <climits>

namespace Argos
{
    class TextFormatter
    {
    public:
        TextFormatter();

        explicit TextFormatter(std::ostream* stream);

        TextFormatter(std::ostream* stream, unsigned lineWidth);

        WordSplitter& wordSplitter();

        std::ostream* stream() const;

        void setStream(std::ostream* stream);

        unsigned lineWidth() const;

        void setLineWidth(unsigned lineWidth);

        unsigned currentLineWidth() const;

        bool isCurrentLineEmpty() const;

        static constexpr unsigned CURRENT_COLUMN = UINT_MAX;

        void pushIndentation(unsigned indent);

        void popIndentation();

        void writeWords(std::string_view text);

        void writeLines(std::string_view text);

        void newline();

        void flush();
    private:
        void appendWord(std::string_view word);

        TextWriter m_Writer;
        std::vector<unsigned> m_Indents;
        WordSplitter m_WordSplitter;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <variant>

#ifndef ARGOS_EX_USAGE
    #ifdef EX_USAGE
        #define ARGOS_EX_USAGE EX_USAGE
    #else
        #define ARGOS_EX_USAGE 64
    #endif
#endif

namespace Argos
{
    struct ParserSettings
    {
        ArgumentCallback argumentCallback;
        OptionCallback optionCallback;
        OptionStyle optionStyle = OptionStyle::STANDARD;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool caseInsensitive = false;
        bool generateHelpOption = true;
        int errorExitCode = ARGOS_EX_USAGE;
    };

    struct HelpSettings
    {
        std::string programName;
        std::map<TextId, std::string> texts;
        std::ostream* outputStream = nullptr;
    };

    struct ParserData
    {
        std::vector<std::unique_ptr<ArgumentData>> arguments;
        std::vector<std::unique_ptr<OptionData>> options;

        ParserSettings parserSettings;
        HelpSettings helpSettings;

        TextFormatter textFormatter;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class IOptionIterator
    {
    public:
        virtual ~IOptionIterator() = default;

        virtual std::optional<std::string> next() = 0;

        virtual std::optional<std::string> nextValue() = 0;

        virtual std::string_view current() const = 0;

        virtual std::vector<std::string_view> remainingArguments() const = 0;

        virtual IOptionIterator* clone() const = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class StandardOptionIterator : public IOptionIterator
    {
    public:
        StandardOptionIterator();

        explicit StandardOptionIterator(std::vector<std::string_view> args);

        StandardOptionIterator(const StandardOptionIterator& rhs);

        std::optional<std::string> next() final;

        std::optional<std::string> nextValue() final;

        std::string_view current() const final;

        std::vector<std::string_view> remainingArguments() const final;

        IOptionIterator* clone() const final;
    private:
        std::vector<std::string_view> m_Args;
        std::vector<std::string_view>::const_iterator m_ArgsIt;
        size_t m_Pos = 0;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class ArgumentIteratorImpl;

    class ParsedArgumentsImpl
    {
    public:
        explicit ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        bool has(ValueId valueId) const;

        const std::vector<std::string>& unprocessedArguments() const;

        void addUnprocessedArgument(const std::string& arg);

        std::string_view assignValue(ValueId valueId,
                                     const std::string& value,
                                     ArgumentId argumentId);

        std::string_view appendValue(ValueId valueId,
                                     const std::string& value,
                                     ArgumentId argumentId);

        void clearValue(ValueId valueId);

        ValueId getValueId(std::string_view valueName) const;

        std::optional<std::pair<std::string_view, ArgumentId>>
        getValue(ValueId valueId) const;

        std::vector<std::pair<std::string_view, ArgumentId>>
        getValues(ValueId valueId) const;

        std::vector<std::unique_ptr<IArgumentView>>
        getArgumentViews(ValueId valueId) const;

        std::unique_ptr<IArgumentView>
        getArgumentView(ArgumentId argumentId) const;

        const std::shared_ptr<ParserData>& parserData() const;

        ParserResultCode resultCode() const;

        void setResultCode(ParserResultCode resultCode);

        const OptionData* stopOption() const;

        void setBreakingOption(const OptionData* option);

        void error(const std::string& message);

        void error(const std::string& message, ArgumentId argumentId);
    private:
        std::multimap<ValueId, std::pair<std::string, ArgumentId>> m_Values;
        std::vector<std::tuple<std::string_view, ValueId, ArgumentId>> m_Ids;
        std::vector<std::string> m_UnprocessedArguments;
        std::shared_ptr<ParserData> m_Data;
        ParserResultCode m_ResultCode = ParserResultCode::NONE;
        const OptionData* m_StopOption = nullptr;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    enum class IteratorResultCode
    {
        ARGUMENT,
        OPTION,
        DONE,
        UNKNOWN,
        ERROR
    };

    using IteratorResult = std::tuple<
            IteratorResultCode,
            const void*,
            std::string_view>;

    class ArgumentIteratorImpl
    {
    public:
        ArgumentIteratorImpl(std::vector<std::string_view> args,
                             std::shared_ptr<ParserData> data);

        IteratorResult next();

        static std::shared_ptr<ParsedArgumentsImpl>
        parse(std::vector<std::string_view> args,
              const std::shared_ptr<ParserData>& data);

        const std::shared_ptr<ParsedArgumentsImpl>& parsedArguments() const;
    private:
        enum class OptionResult
        {
            NORMAL,
            LAST_ARGUMENT,
            STOP,
            HELP,
            ERROR
        };

        std::pair<OptionResult, std::string_view>
        processOption(const OptionData& opt, const std::string& flag);

        IteratorResult processOption(const std::string& flag);

        IteratorResult processArgument(const std::string& name);

        void copyRemainingArgumentsToParserResult();

        size_t countArguments() const;

        bool checkArgumentAndOptionCounts();

        void error(const std::string& message = {});

        std::shared_ptr<ParserData> m_Data;
        std::vector<std::pair<std::string_view, const OptionData*>> m_Options;
        std::shared_ptr<ParsedArgumentsImpl> m_ParsedArgs;
        std::unique_ptr<IOptionIterator> m_Iterator;
        ArgumentCounter m_ArgumentCounter;
        enum class State
        {
            ARGUMENTS_AND_OPTIONS,
            ARGUMENTS_ONLY,
            DONE,
            ERROR
        };
        State m_State = State::ARGUMENTS_AND_OPTIONS;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    ArgumentIterator::ArgumentIterator(std::vector<std::string_view> args,
                                       std::shared_ptr<ParserData> parserData)
        : m_Impl(std::make_unique<ArgumentIteratorImpl>(move(args),
                                                        move(parserData)))
    {}

    ArgumentIterator::ArgumentIterator(ArgumentIterator&& rhs) noexcept
        : m_Impl(move(rhs.m_Impl))
    {}

    ArgumentIterator::~ArgumentIterator() = default;

    ArgumentIterator& ArgumentIterator::operator=(ArgumentIterator&& rhs) noexcept
    {
        m_Impl = move(rhs.m_Impl);
        return *this;
    }

    bool ArgumentIterator::next(std::unique_ptr<IArgumentView>& arg,
                                std::string_view& value)
    {
        auto res = impl().next();
        switch (std::get<0>(res))
        {
        case IteratorResultCode::ARGUMENT:
            arg = std::make_unique<ArgumentView>(
                    static_cast<const ArgumentData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::OPTION:
            arg = std::make_unique<OptionView>(
                    static_cast<const OptionData*>(std::get<1>(res)));
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::UNKNOWN:
            arg = {};
            value = std::get<2>(res);
            return true;
        case IteratorResultCode::DONE:
        case IteratorResultCode::ERROR:
            break;
        }
        arg = {};
        value = {};
        return false;
    }

    ParsedArguments ArgumentIterator::parsedArguments() const
    {
        return ParsedArguments(impl().parsedArguments());
    }

    ArgumentIteratorImpl& ArgumentIterator::impl()
    {
        if (!m_Impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_Impl;
    }

    const ArgumentIteratorImpl& ArgumentIterator::impl() const
    {
        if (!m_Impl)
            ARGOS_THROW("This ArgumentIterator has been moved from.");
        return *m_Impl;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    void writeHelpText(ParserData& data);

    void writeErrorMessage(ParserData& data, const std::string& msg);

    void writeErrorMessage(ParserData& data,
                           const std::string& msg,
                           ArgumentId argumentId);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-14.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

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

    std::vector<std::string_view>
    splitString(std::string_view s, char delimiter, size_t maxSplit);

    std::string_view getBaseName(std::string_view str);

    size_t countCodePoints(std::string_view str);

    size_t findNthCodePoint(std::string_view str, size_t n);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class OptionIterator : public IOptionIterator
    {
    public:
        OptionIterator();

        explicit OptionIterator(std::vector<std::string_view> args,
                                char prefix);

        OptionIterator(const OptionIterator& rhs);

        std::optional<std::string> next() final;

        std::optional<std::string> nextValue() final;

        std::string_view current() const final;

        std::vector<std::string_view> remainingArguments() const final;

        OptionIterator* clone() const final;
    private:
        std::vector<std::string_view> m_Args;
        std::vector<std::string_view>::const_iterator m_ArgsIt;
        size_t m_Pos = 0;
        char m_Prefix = '-';
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <algorithm>
#include <cassert>

namespace Argos
{
    namespace
    {
        using OptionTable = std::vector<std::pair<std::string_view, const OptionData*>>;

        OptionTable makeOptionIndex(
                const std::vector<std::unique_ptr<OptionData>>& options,
                bool caseInsensitive)
        {
            OptionTable index;
            for (auto& option : options)
            {
                for (auto& flag : option->flags)
                    index.emplace_back(flag, option.get());
            }
            sort(index.begin(), index.end(), [&](auto& a, auto& b)
            {
                return isLess(a.first, b.first, caseInsensitive);
            });
            auto it = adjacent_find(index.begin(), index.end(),
                                    [&](auto& a, auto& b)
            {
                return areEqual(a.first, b.first, caseInsensitive);
            });
            if (it == index.end())
                return index;

            if (it->first == next(it)->first)
            {
                ARGOS_THROW("Multiple definitions of flag "
                            + std::string(it->first));
            }
            else
            {
                ARGOS_THROW("Conflicting flags: " + std::string(it->first)
                            + " and " + std::string(next(it)->first));
            }
        }

        const OptionData* findOptionImpl(const OptionTable& options,
                                         std::string_view arg,
                                         bool allowAbbreviations,
                                         bool caseInsensitive)
        {
            auto it = std::lower_bound(
                options.begin(), options.end(),
                OptionTable::value_type(arg, nullptr),
                [&](auto& a, auto& b)
                {return isLess(a.first, b.first, caseInsensitive);});
            if (it == options.end())
                return nullptr;
            if (it->first == arg)
                return it->second;
            if (caseInsensitive && areEqualCI(it->first, arg))
                return it->second;
            if (!allowAbbreviations)
                return nullptr;
            if (!startsWith(it->first, arg, caseInsensitive))
                return nullptr;
            auto nxt = next(it);
            if (nxt != options.end()
                && startsWith(nxt->first, arg, caseInsensitive))
                return nullptr;
            return it->second;
        }

        const OptionData* findOption(const OptionTable& options,
                                     std::string_view arg,
                                     bool allowAbbreviations,
                                     bool caseInsensitive)
        {
            auto opt = findOptionImpl(options, arg, allowAbbreviations,
                                      caseInsensitive);
            if (opt == nullptr && arg.size() > 2 && arg.back() == '=')
            {
                arg = arg.substr(0, arg.size() - 1);
                opt = findOptionImpl(options, arg, allowAbbreviations,
                                     caseInsensitive);
                if (opt && opt->argument.empty())
                    opt = nullptr;
            }
            return opt;
        }

        bool isOption(const std::string& s, OptionStyle style)
        {
            if (s.size() < 2)
                return false;

            return s[0] == (style == OptionStyle::SLASH ? '/' : '-');
        }

        std::unique_ptr<IOptionIterator> makeOptionIterator(
                OptionStyle style,
                std::vector<std::string_view> args)
        {
            switch (style)
            {
            case OptionStyle::SLASH:
                return std::make_unique<OptionIterator>(move(args), '/');
            case OptionStyle::DASH:
                return std::make_unique<OptionIterator>(move(args), '-');
            default:
                return std::make_unique<StandardOptionIterator>(move(args));
            }
        }
    }

    ArgumentIteratorImpl::ArgumentIteratorImpl(std::vector<std::string_view> args,
                                               std::shared_ptr<ParserData> data)
        : m_Data(move(data)),
          m_Options(makeOptionIndex(m_Data->options, m_Data->parserSettings.caseInsensitive)),
          m_ParsedArgs(std::make_shared<ParsedArgumentsImpl>(m_Data)),
          m_Iterator(makeOptionIterator(m_Data->parserSettings.optionStyle, move(args)))
    {
        if (!ArgumentCounter::requiresArgumentCount(m_Data->arguments))
            m_ArgumentCounter = ArgumentCounter(m_Data->arguments);
        else
            m_ArgumentCounter = ArgumentCounter(m_Data->arguments,
                                                countArguments());
    }

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(move(args), data);
        while (true)
        {
            auto code = std::get<0>(iterator.next());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.m_ParsedArgs;
    }

    IteratorResult ArgumentIteratorImpl::next()
    {
        if (m_State == State::ERROR)
            ARGOS_THROW("next() called after error.");
        if (m_State == State::DONE)
            return {IteratorResultCode::DONE, nullptr, {}};

        auto arg = m_State == State::ARGUMENTS_AND_OPTIONS
                   ? m_Iterator->next()
                   : m_Iterator->nextValue();
        if (!arg)
        {
            if (checkArgumentAndOptionCounts())
                return {IteratorResultCode::DONE, nullptr, {}};
            else
                return {IteratorResultCode::ERROR, nullptr, {}};
        }

        if (m_State == State::ARGUMENTS_AND_OPTIONS
            && isOption(*arg, m_Data->parserSettings.optionStyle))
        {
            return processOption(*arg);
        }
        else
        {
            return processArgument(*arg);
        }
    }

    const std::shared_ptr<ParsedArgumentsImpl>& ArgumentIteratorImpl::parsedArguments() const
    {
        return m_ParsedArgs;
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::processOption(const OptionData& opt,
                                        const std::string& flag)
    {
        std::string_view arg;
        switch (opt.operation)
        {
        case OptionOperation::ASSIGN:
            if (!opt.constant.empty())
            {
                m_ParsedArgs->assignValue(opt.valueId, opt.constant,
                                          opt.argumentId);
            }
            else if (auto value = m_Iterator->nextValue())
            {
                arg = m_ParsedArgs->assignValue(opt.valueId, *value,
                                                opt.argumentId);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::APPEND:
            if (!opt.constant.empty())
            {
                m_ParsedArgs->appendValue(opt.valueId, opt.constant,
                                          opt.argumentId);
            }
            else if (auto value = m_Iterator->nextValue())
            {
                arg = m_ParsedArgs->appendValue(opt.valueId, *value,
                                                opt.argumentId);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            m_ParsedArgs->clearValue(opt.valueId);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (opt.callback
            && !opt.callback(OptionView(&opt), arg,
                             ParsedArgumentsBuilder(m_ParsedArgs)))
        {
            error();
            return {OptionResult::ERROR, {}};
        }

        if (m_Data->parserSettings.optionCallback
            && !m_Data->parserSettings.optionCallback(
                    OptionView(&opt), arg,
                    ParsedArgumentsBuilder(m_ParsedArgs)))
        {
            error();
            return {OptionResult::ERROR, {}};
        }
        switch (opt.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            writeHelpText(*m_Data);
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&opt);
            return {OptionResult::HELP, arg};
        case OptionType::STOP:
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&opt);
            return {OptionResult::STOP, arg};
        case OptionType::LAST_ARGUMENT:
            m_State = State::DONE;
            return {OptionResult::LAST_ARGUMENT, arg};
        case OptionType::LAST_OPTION:
            m_State = State::ARGUMENTS_ONLY;
            return {OptionResult::NORMAL, arg};
        }
        return {};
    }

    IteratorResult
    ArgumentIteratorImpl::processOption(const std::string& flag)
    {
        auto option = findOption(
                m_Options, flag,
                m_Data->parserSettings.allowAbbreviatedOptions,
                m_Data->parserSettings.caseInsensitive);
        if (option)
        {
            auto optRes = processOption(*option, flag);
            switch (optRes.first)
            {
            case OptionResult::HELP:
                if (m_Data->parserSettings.autoExit)
                    exit(0);
                copyRemainingArgumentsToParserResult();
                return {IteratorResultCode::OPTION, option, optRes.second};
            case OptionResult::ERROR:
                return {IteratorResultCode::ERROR, option, {}};
            case OptionResult::LAST_ARGUMENT:
                if (!checkArgumentAndOptionCounts())
                    return {IteratorResultCode::ERROR, nullptr, {}};
                [[fallthrough]];
            case OptionResult::STOP:
                copyRemainingArgumentsToParserResult();
                [[fallthrough]];
            default:
                return {IteratorResultCode::OPTION, option, optRes.second};
            }
        }
        if (!m_Data->parserSettings.ignoreUndefinedOptions
            || !startsWith(m_Iterator->current(), flag))
        {
            error("Unknown option: " + std::string(m_Iterator->current()));
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        else
        {
            m_ParsedArgs->addUnprocessedArgument(
                std::string(m_Iterator->current()));
            return {IteratorResultCode::UNKNOWN, nullptr, m_Iterator->current()};
        }
    }

    IteratorResult
    ArgumentIteratorImpl::processArgument(const std::string& name)
    {
        if (auto argument = m_ArgumentCounter.nextArgument())
        {
            auto s = m_ParsedArgs->appendValue(argument->valueId, name,
                                               argument->argumentId);
            if (argument->callback
                && !argument->callback(ArgumentView(argument), s,
                                       ParsedArgumentsBuilder(m_ParsedArgs)))
            {
                error();
                return {IteratorResultCode::ERROR, nullptr, {}};
            }
            if (m_Data->parserSettings.argumentCallback
                && !m_Data->parserSettings.argumentCallback(
                        ArgumentView(argument), s,
                        ParsedArgumentsBuilder(m_ParsedArgs)))
            {
                error();
                return {IteratorResultCode::ERROR, nullptr, {}};
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        else if (m_Data->parserSettings.ignoreUndefinedArguments)
        {
            m_ParsedArgs->addUnprocessedArgument(name);
        }
        else
        {
            error("Too many arguments, starting with \"" + name + "\"");
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        return {IteratorResultCode::UNKNOWN, nullptr, m_Iterator->current()};
    }

    void ArgumentIteratorImpl::copyRemainingArgumentsToParserResult()
    {
        for (auto str : m_Iterator->remainingArguments())
            m_ParsedArgs->addUnprocessedArgument(std::string(str));
    }

    size_t ArgumentIteratorImpl::countArguments() const
    {
        size_t result = 0;
        std::unique_ptr<IOptionIterator> it(m_Iterator->clone());
        bool argumentsOnly = false;
        for (auto arg = it->next(); arg && !argumentsOnly; arg = it->next())
        {
            auto option = findOption(m_Options, *arg,
                                     m_Data->parserSettings.allowAbbreviatedOptions,
                                     m_Data->parserSettings.caseInsensitive);
            if (option)
            {
                if (!option->argument.empty())
                    it->nextValue();
                switch (option->type)
                {
                case OptionType::HELP:
                case OptionType::LAST_ARGUMENT:
                case OptionType::STOP:
                    return result;
                case OptionType::LAST_OPTION:
                    argumentsOnly = true;
                    break;
                default:
                    break;
                }
            }
            else if (!isOption(*arg, m_Data->parserSettings.optionStyle))
            {
                ++result;
            }
        }

        for (auto arg = it->next(); arg; arg = it->next())
            ++result;
        return result;
    }

    bool ArgumentIteratorImpl::checkArgumentAndOptionCounts()
    {
        for (auto& o : m_Data->options)
        {
            if (!o->optional && !m_ParsedArgs->has(o->valueId))
            {
                auto flags = o->flags.front();
                for (unsigned i = 1; i < o->flags.size(); ++i)
                    flags += ", " + o->flags[i];
                error("Mandatory option \"" + flags + "\" is missing.");
                return false;
            }
        }
        if (m_ArgumentCounter.isComplete())
        {
            m_State = State::DONE;
            m_ParsedArgs->setResultCode(ParserResultCode::SUCCESS);
            return true;
        }
        else
        {
            auto ns = ArgumentCounter::getMinMaxCount(m_Data->arguments);
            error((ns.first == ns.second
                   ? "Too few arguments. Expected "
                   : "Too few arguments. Expected at least ")
                  + std::to_string(ns.first) + ", received "
                  + std::to_string(m_ArgumentCounter.count()) + ".");
            return false;
        }
    }

    void ArgumentIteratorImpl::error(const std::string& message)
    {
        if (!message.empty())
            writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(m_Data->parserSettings.errorExitCode);
        copyRemainingArgumentsToParserResult();
        m_ParsedArgs->setResultCode(ParserResultCode::ERROR);
        m_State = State::ERROR;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    namespace
    {
        bool checkFlagWithEqual(const std::string& flag, const OptionData& od)
        {
            auto eqPos = flag.find('=');
            if (eqPos == std::string::npos)
                return true;
            if (eqPos != flag.size() - 1)
                return false;
            if (od.argument.empty())
                ARGOS_THROW("Options ending with '=' must take an argument: " + flag);
            return true;
        }

        bool checkStandardFlag(const std::string& flag, const OptionData& od)
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
            return checkFlagWithEqual(flag, od);
        }

        bool checkFlag(const std::string& flag, char prefix, const OptionData& od)
        {
            if (flag.size() < 2 || flag[0] != prefix)
                return false;
            if (flag.find_first_of(" \t\n\r") != std::string::npos)
                return false;
            if (flag.size() == 2)
                return true;
            return checkFlagWithEqual(flag, od);
        }

        std::unique_ptr<ParserData> makeCopy(const ParserData& data)
        {
            auto result = std::make_unique<ParserData>();
            result->parserSettings = data.parserSettings;
            result->helpSettings = data.helpSettings;
            result->arguments.reserve(data.arguments.size());
            for (const auto& a : data.arguments)
                result->arguments.push_back(std::make_unique<ArgumentData>(*a));
            result->options.reserve(data.options.size());
            for (const auto& o : data.options)
                result->options.push_back(std::make_unique<OptionData>(*o));
            return result;
        }

        void setValueIds(const ParserData& data)
        {
            struct InternalIdMaker
            {
                ValueId n = ValueId(0);
                std::map<std::string_view, ValueId> explicitIds;

                std::optional<ValueId> findValueId(std::string_view name)
                {
                    auto it = explicitIds.find(name);
                    if (it == explicitIds.end())
                        return {};
                    return it->second;
                }

                ValueId makeValueId(std::string_view name)
                {
                    if (auto id = findValueId(name))
                        return *id;
                    n = ValueId(n + 1);
                    explicitIds.emplace(name, n);
                    return n;
                }

                ValueId makeValueId(const std::vector<std::string>& names)
                {
                    for (const auto& name : names)
                    {
                        if (auto id = findValueId(name))
                            return *id;
                    }
                    n = ValueId(n + 1);
                    for (const auto& name : names)
                        explicitIds.emplace(name, n);
                    return n;
                }
            };
            InternalIdMaker idMaker;
            for (const auto& a : data.arguments)
            {
                if (!a->value.empty())
                {
                    a->valueId = idMaker.makeValueId(a->value);
                    idMaker.explicitIds.emplace(a->name, a->valueId);
                }
                else
                {
                    a->valueId = idMaker.makeValueId(a->name);
                }
            }
            for (const auto& o : data.options)
            {
                if (o->operation == OptionOperation::NONE)
                    continue;
                if (!o->value.empty())
                {
                    o->valueId = idMaker.makeValueId(o->value);
                    for (auto& f : o->flags)
                        idMaker.explicitIds.emplace(f, o->valueId);
                }
                else
                {
                    o->valueId = idMaker.makeValueId(o->flags);
                }
            }
        }

        inline bool hasHelpOption(const ParserData& data)
        {
            return std::any_of(data.options.begin(), data.options.end(),
                               [](const auto& o)
                               {return o->type == OptionType::HELP;});
        }

        inline bool hasFlag(const ParserData& data, std::string_view flag)
        {
            bool ci = data.parserSettings.caseInsensitive;
            return any_of(data.options.begin(), data.options.end(),
                          [&](const auto& o)
                          {return any_of(o->flags.begin(), o->flags.end(),
                                         [&](const auto& f)
                                         {return areEqual(f, flag, ci);});
                          });
        }

        void addMissingHelpOption(ParserData& data)
        {
            if (!data.parserSettings.generateHelpOption)
                return;
            if (hasHelpOption(data))
                return;
            std::string flag;
            switch (data.parserSettings.optionStyle)
            {
            case OptionStyle::STANDARD:
                flag = "--help";
                break;
            case OptionStyle::SLASH:
                flag = "/?";
                break;
            case OptionStyle::DASH:
                flag = "-help";
                break;
            }
            if (hasFlag(data, flag))
                return;

            auto opt = Option{flag}.type(OptionType::HELP)
                .text("Show help text.")
                .constant("1").release();
            opt->argumentId = ArgumentId(data.options.size()
                                         + data.arguments.size() + 1);
            data.options.push_back(move(opt));
        }

        ParsedArguments parseImpl(std::vector<std::string_view> args,
                                  const std::shared_ptr<ParserData>& data)
        {
            addMissingHelpOption(*data);
            setValueIds(*data);
            return ParsedArguments(
                    ArgumentIteratorImpl::parse(std::move(args), data));
        }

        ArgumentIterator
        makeIteratorImpl(std::vector<std::string_view> args,
                         const std::shared_ptr<ParserData>& data)
        {
            addMissingHelpOption(*data);
            setValueIds(*data);
            return ArgumentIterator(std::move(args), data);
        }
    }

    ArgumentParser::ArgumentParser()
            : ArgumentParser("UNINITIALIZED")
    {}

    ArgumentParser::ArgumentParser(std::string_view programName,
                                   bool extractFileName)
        : m_Data(std::make_unique<ParserData>())
    {
        m_Data->helpSettings.programName = extractFileName
                                           ? getBaseName(programName)
                                           : programName;
    }

    ArgumentParser::ArgumentParser(ArgumentParser&& rhs) noexcept
        : m_Data(std::move(rhs.m_Data))
    {}

    ArgumentParser::~ArgumentParser() = default;

    ArgumentParser& ArgumentParser::operator=(ArgumentParser&& rhs) noexcept
    {
        m_Data = std::move(rhs.m_Data);
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Argument argument)
    {
        checkData();
        auto ad = argument.release();
        if (ad->name.empty())
            ARGOS_THROW("Argument must have a name.");
        ad->argumentId = nextArgumentId();
        m_Data->arguments.emplace_back(std::move(ad));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option option)
    {
        checkData();
        auto od = option.release();
        if (od->flags.empty())
            ARGOS_THROW("Option must have one or more flags.");
        for (auto& flag : od->flags)
        {
            bool ok = false;
            switch (m_Data->parserSettings.optionStyle)
            {
            case OptionStyle::STANDARD:
                ok = checkStandardFlag(flag, *od);
                break;
            case OptionStyle::SLASH:
                ok = checkFlag(flag, '/', *od);
                break;
            case OptionStyle::DASH:
                ok = checkFlag(flag, '-', *od);
                break;
            default:
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!od->argument.empty() && !od->constant.empty())
            ARGOS_THROW("Option cannot have both argument and value set.");
        switch (od->operation)
        {
        case OptionOperation::NONE:
            if (!od->constant.empty())
                ARGOS_THROW("NONE-options cannot have value set.");
            if (!od->value.empty())
                ARGOS_THROW("NONE-options cannot have valueName set.");
            if (!od->optional)
                ARGOS_THROW("NONE-options must be optional.");
            break;
        case OptionOperation::ASSIGN:
            if (od->argument.empty() && od->constant.empty())
                od->constant = "1";
            break;
        case OptionOperation::APPEND:
            if (od->argument.empty() && od->constant.empty())
                ARGOS_THROW("Options that appends must have either value or argument set.");
            break;
        case OptionOperation::CLEAR:
            if (!od->argument.empty() ||!od->constant.empty())
                od->constant = "1";
            if (!od->optional)
                ARGOS_THROW("CLEAR-options must be optional.");
            break;
        }
        od->argumentId = nextArgumentId();
        m_Data->options.push_back(std::move(od));
        return *this;
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv)
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(int argc, char** argv) const
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return parse(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args)
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return parseImpl(std::move(args), std::move(m_Data));
    }

    ParsedArguments ArgumentParser::parse(std::vector<std::string_view> args) const
    {
        checkData();
        return parseImpl(std::move(args), makeCopy(*m_Data));
    }

    ArgumentIterator ArgumentParser::makeIterator(int argc, char** argv)
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return makeIterator(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ArgumentIterator ArgumentParser::makeIterator(int argc, char** argv) const
    {
        if (argc <= 0)
            ARGOS_THROW("argc and argv must at least contain the command name.");
        return makeIterator(std::vector<std::string_view>(argv + 1, argv + argc));
    }

    ArgumentIterator ArgumentParser::makeIterator(
            std::vector<std::string_view> args)
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return makeIteratorImpl(std::move(args), std::move(m_Data));
    }

    ArgumentIterator ArgumentParser::makeIterator(std::vector<std::string_view> args) const
    {
        checkData();
        return makeIteratorImpl(std::move(args), makeCopy(*m_Data));
    }

    bool ArgumentParser::allowAbbreviatedOptions() const
    {
        checkData();
        return m_Data->parserSettings.allowAbbreviatedOptions;
    }

    ArgumentParser& ArgumentParser::allowAbbreviatedOptions(bool value)
    {
        checkData();
        m_Data->parserSettings.allowAbbreviatedOptions = value;
        return *this;
    }

    bool ArgumentParser::autoExit() const
    {
        checkData();
        return m_Data->parserSettings.autoExit;
    }

    ArgumentParser& ArgumentParser::autoExit(bool value)
    {
        checkData();
        m_Data->parserSettings.autoExit = value;
        return *this;
    }

    bool ArgumentParser::caseInsensitive() const
    {
        checkData();
        return m_Data->parserSettings.caseInsensitive;
    }

    ArgumentParser& ArgumentParser::caseInsensitive(bool value)
    {
        checkData();
        m_Data->parserSettings.caseInsensitive = value;
        return *this;
    }

    bool ArgumentParser::generateHelpOption() const
    {
        checkData();
        return m_Data->parserSettings.generateHelpOption;
    }

    ArgumentParser& ArgumentParser::generateHelpOption(bool value)
    {
        checkData();
        m_Data->parserSettings.generateHelpOption = value;
        return *this;
    }

    OptionStyle ArgumentParser::optionStyle() const
    {
        checkData();
        return m_Data->parserSettings.optionStyle;
    }

    ArgumentParser& ArgumentParser::optionStyle(OptionStyle value)
    {
        checkData();
        if (value != m_Data->parserSettings.optionStyle)
        {
            if (!m_Data->options.empty())
                ARGOS_THROW("Can't change option style after"
                            " options have been added.");
            m_Data->parserSettings.optionStyle = value;
        }
        return *this;
    }

    bool ArgumentParser::ignoreUndefinedArguments() const
    {
        checkData();
        return m_Data->parserSettings.ignoreUndefinedArguments;
    }

    ArgumentParser& ArgumentParser::ignoreUndefinedArguments(bool value)
    {
        checkData();
        m_Data->parserSettings.ignoreUndefinedArguments = value;
        return *this;
    }

    bool ArgumentParser::ignoreUndefinedOptions() const
    {
        checkData();
        return m_Data->parserSettings.ignoreUndefinedOptions;
    }

    ArgumentParser& ArgumentParser::ignoreUndefinedOptions(bool value)
    {
        checkData();
        m_Data->parserSettings.ignoreUndefinedOptions = value;
        return *this;
    }

    const ArgumentCallback& ArgumentParser::argumentCallback() const
    {
        checkData();
        return m_Data->parserSettings.argumentCallback;
    }

    ArgumentParser& ArgumentParser::argumentCallback(ArgumentCallback callback)
    {
        checkData();
        m_Data->parserSettings.argumentCallback = std::move(callback);
        return *this;
    }

    const OptionCallback& ArgumentParser::optionCallback() const
    {
        checkData();
        return m_Data->parserSettings.optionCallback;
    }

    ArgumentParser& ArgumentParser::optionCallback(OptionCallback callback)
    {
        checkData();
        m_Data->parserSettings.optionCallback = std::move(callback);
        return *this;
    }

    std::ostream* ArgumentParser::stream() const
    {
        checkData();
        return m_Data->helpSettings.outputStream;
    }

    ArgumentParser& ArgumentParser::stream(std::ostream* stream)
    {
        checkData();
        m_Data->helpSettings.outputStream = stream;
        return *this;
    }

    const std::string& ArgumentParser::programName() const
    {
        checkData();
        return m_Data->helpSettings.programName;
    }

    ArgumentParser& ArgumentParser::programName(const std::string& name)
    {
        checkData();
        m_Data->helpSettings.programName = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::text(std::string text)
    {
        return this->text(TextId::TEXT, std::move(text));
    }

    ArgumentParser& ArgumentParser::text(TextId textId, std::string text)
    {
        checkData();
        m_Data->helpSettings.texts[textId] = std::move(text);
        return *this;
    }

    void ArgumentParser::writeHelpText()
    {
        checkData();
        Argos::writeHelpText(*m_Data);
    }

    ArgumentParser& ArgumentParser::addWordSplittingRule(std::string str)
    {
        checkData();
        m_Data->textFormatter.wordSplitter().addWord(std::move(str));
        return *this;
    }

    ArgumentParser&& ArgumentParser::move()
    {
        return std::move(*this);
    }

    void ArgumentParser::checkData() const
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
    }

    ArgumentId ArgumentParser::nextArgumentId() const
    {
        auto& d = *m_Data;
        return ArgumentId(d.options.size() + d.arguments.size() + 1);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <cerrno>
#include <cinttypes>
#include <type_traits>

namespace Argos
{
    template <typename T>
    std::optional<T> parseInteger(const std::string& str, int base);

    template <>
    std::optional<int> parseInteger<int>(const std::string& str, int base);

    template <>
    std::optional<unsigned>
    parseInteger<unsigned>(const std::string& str, int base);

    template <>
    std::optional<long> parseInteger<long>(const std::string& str, int base);

    template <>
    std::optional<long long>
    parseInteger<long long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long>
    parseInteger<unsigned long>(const std::string& str, int base);

    template <>
    std::optional<unsigned long long>
    parseInteger<unsigned long long>(const std::string& str, int base);

    template <typename T>
    std::optional<T> parseFloatingPoint(const std::string& str);

    template <>
    std::optional<float> parseFloatingPoint<float>(const std::string& str);

    template <>
    std::optional<double> parseFloatingPoint<double>(const std::string& str);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    namespace
    {
        template <typename T>
        T getInteger(const ArgumentValue& value, T defaultValue, int base)
        {
            auto s = value.value();
            if (!s)
                return defaultValue;
            auto n = parseInteger<T>(std::string(*s), base);
            if (!n)
                value.error();
            return *n;
        }

        template <typename T>
        T getFloatingPoint(const ArgumentValue& value, T defaultValue)
        {
            auto s = value.value();
            if (!s)
                return defaultValue;
            auto n = parseFloatingPoint<T>(std::string(*s));
            if (!n)
                value.error();
            return *n;
        }
    }

    ArgumentValue::ArgumentValue(std::optional<std::string_view> value,
                                 std::shared_ptr<ParsedArgumentsImpl> args,
                                 ValueId valueId,
                                 ArgumentId argumentId)
        : m_Value(value),
          m_Args(std::move(args)),
          m_ValueId(valueId),
          m_ArgumentId(argumentId)
    {}

    ArgumentValue::ArgumentValue(const ArgumentValue&) = default;

    ArgumentValue::ArgumentValue(ArgumentValue&& ) noexcept = default;

    ArgumentValue::~ArgumentValue() = default;

    ArgumentValue&
    ArgumentValue::operator=(const ArgumentValue&) = default;

    ArgumentValue&
    ArgumentValue::operator=(ArgumentValue&&) noexcept = default;

    std::unique_ptr<IArgumentView>
    ArgumentValue::argument() const
    {
        return m_Args->getArgumentView(m_ArgumentId);
    }

    bool ArgumentValue::hasValue() const
    {
        return m_Value.has_value();
    }

    std::optional<std::string_view> ArgumentValue::value() const
    {
        return m_Value;
    }

    bool ArgumentValue::asBool(bool defaultValue) const
    {
        if (!m_Value)
            return defaultValue;
        return m_Value != "0" && m_Value != "false";
    }

    int ArgumentValue::asInt(int defaultValue, int base) const
    {
        return getInteger<int>(*this, defaultValue, base);
    }

    unsigned ArgumentValue::asUInt(unsigned defaultValue, int base) const
    {
        return getInteger<unsigned>(*this, defaultValue, base);
    }

    long ArgumentValue::asLong(long defaultValue, int base) const
    {
        return getInteger<long>(*this, defaultValue, base);
    }

    long long ArgumentValue::asLLong(long long defaultValue, int base) const
    {
        return getInteger<long long>(*this, defaultValue, base);
    }

    unsigned long
    ArgumentValue::asULong(unsigned long defaultValue, int base) const
    {
        return getInteger<unsigned long>(*this, defaultValue, base);
    }

    unsigned long long
    ArgumentValue::asULLong(unsigned long long defaultValue, int base) const
    {
        return getInteger<unsigned long long>(*this, defaultValue, base);
    }

    float ArgumentValue::asFloat(float defaultValue) const
    {
        return getFloatingPoint<float>(*this, defaultValue);
    }

    double ArgumentValue::asDouble(double defaultValue) const
    {
        return getFloatingPoint<double>(*this, defaultValue);
    }

    std::string ArgumentValue::asString(const std::string& defaultValue) const
    {
        return m_Value ? std::string(*m_Value) : defaultValue;
    }

    ArgumentValues
    ArgumentValue::split(char separator,
                         size_t minParts, size_t maxParts) const
    {
        if (!m_Value)
            return ArgumentValues({}, m_Args, m_ValueId);
        auto parts = splitString(*m_Value, separator, maxParts - 1);
        if (parts.size() < minParts)
        {
            error("Invalid value: \"" + std::string(*m_Value)
                  + "\". Must have at least " + std::to_string(minParts)
                  + " values separated by \"" + separator + "\".");
            return ArgumentValues({}, m_Args, m_ValueId);
        }
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        values.reserve(parts.size());
        for (auto& part : parts)
            values.emplace_back(part, m_ArgumentId);
        return {move(values), m_Args, m_ValueId};
    }

    void ArgumentValue::error(const std::string& message) const
    {
        m_Args->error(message, m_ArgumentId);
    }

    void ArgumentValue::error() const
    {
        error("Invalid value: " + std::string(*m_Value) + ".");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    namespace
    {
        ArgumentId getArgumentId(
                const std::vector<std::pair<std::string_view, ArgumentId>>& values)
        {
            if (values.empty())
                return {};
            for (auto it = next(values.begin()); it != values.end(); ++it)
            {
                if (it->second != prev(it)->second)
                    return {};
            }
            return values.front().second;
        }

        void error(const ArgumentValues& values, std::string_view value)
        {
            values.error("Invalid value: " + std::string(value) + ".");
        }

        template <typename T>
        std::vector<T> parseFloats(const ArgumentValues& values,
                                   const std::vector<T>& defaultValue)
        {
            if (values.empty())
                return defaultValue;

            std::vector<T> result;
            result.reserve(values.size());
            for (auto& v : values.rawValues())
            {
                auto value = parseFloatingPoint<T>(std::string(v));
                if (!value)
                    error(values, v);
                result.push_back(*value);
            }
            return result;
        }

        template <typename T>
        std::vector<T> parseIntegers(const ArgumentValues& values,
                                     const std::vector<T>& defaultValue,
                                     int base)
        {
            if (values.empty())
                return defaultValue;

            std::vector<T> result;
            result.reserve(values.size());
            for (auto& v : values.rawValues())
            {
                auto value = parseInteger<T>(std::string(v), base);
                if (!value)
                    error(values, v);
                result.push_back(*value);
            }
            return result;
        }
    }

    ArgumentValues::ArgumentValues(
            std::vector<std::pair<std::string_view, ArgumentId>> values,
            std::shared_ptr<ParsedArgumentsImpl> args,
            ValueId valueId)
        : m_Values(move(values)),
          m_Args(move(args)),
          m_ValueId(valueId)
    {}

    ArgumentValues::ArgumentValues(const ArgumentValues&) = default;

    ArgumentValues::ArgumentValues(ArgumentValues&&) noexcept = default;

    ArgumentValues::~ArgumentValues() = default;

    ArgumentValues&
    ArgumentValues::operator=(const ArgumentValues&) = default;

    ArgumentValues&
    ArgumentValues::operator=(ArgumentValues&&) noexcept = default;

    std::vector<std::unique_ptr<IArgumentView>>
    ArgumentValues::arguments() const
    {
        return m_Args->getArgumentViews(m_ValueId);
    }

    void ArgumentValues::error(const std::string& message) const
    {
        m_Args->error(message, getArgumentId(m_Values));
    }

    bool ArgumentValues::empty() const
    {
        return m_Values.empty();
    }

    size_t ArgumentValues::size() const
    {
        return m_Values.size();
    }

    std::vector<ArgumentValue> ArgumentValues::values() const
    {
        std::vector<ArgumentValue> result;
        for (auto& v : m_Values)
            result.emplace_back(v.first, m_Args, m_ValueId, v.second);
        return result;
    }

    std::vector<std::string_view> ArgumentValues::rawValues() const
    {
        std::vector<std::string_view> result;
        for (auto& s : m_Values)
            result.push_back(s.first);
        return result;
    }

    ArgumentValue ArgumentValues::value(size_t index) const
    {
        if (m_Values.empty())
            return {{}, m_Args, m_ValueId, {}};

        auto& v = m_Values.at(index);
        return {v.first, m_Args, m_ValueId, v.second};
    }

    std::vector<int>
    ArgumentValues::asInts(const std::vector<int>& defaultValue,
                           int base) const
    {
        return parseIntegers<int>(*this, defaultValue, base);
    }

    std::vector<unsigned>
    ArgumentValues::asUInts(const std::vector<unsigned>& defaultValue,
                            int base) const
    {
        return parseIntegers<unsigned>(*this, defaultValue, base);
    }

    std::vector<long>
    ArgumentValues::asLongs(const std::vector<long>& defaultValue,
                            int base) const
    {
        return parseIntegers<long>(*this, defaultValue, base);
    }

    std::vector<long long>
    ArgumentValues::asLLongs(const std::vector<long long>& defaultValue,
                             int base) const
    {
        return parseIntegers<long long>(*this, defaultValue, base);
    }

    std::vector<unsigned long>
    ArgumentValues::asULongs(const std::vector<unsigned long>& defaultValue,
                             int base) const
    {
        return parseIntegers<unsigned long>(*this, defaultValue, base);
    }

    std::vector<unsigned long long> ArgumentValues::asULLongs(
        const std::vector<unsigned long long>& defaultValue,
        int base) const
    {
        return parseIntegers<unsigned long long>(*this, defaultValue, base);
    }

    std::vector<float>
    ArgumentValues::asFloats(const std::vector<float>& defaultValue) const
    {
        return parseFloats(*this, defaultValue);
    }

    std::vector<double>
    ArgumentValues::asDoubles(const std::vector<double>& defaultValue) const
    {
        return parseFloats(*this, defaultValue);
    }

    std::vector<std::string> ArgumentValues::asStrings(
            const std::vector<std::string>& defaultValue) const
    {
        if (m_Values.empty())
            return defaultValue;

        std::vector<std::string> result;
        result.reserve(m_Values.size());
        for (auto& v : m_Values)
            result.emplace_back(v.first);
        return result;
    }

    ArgumentValues
    ArgumentValues::split(char separator,
                          size_t minParts, size_t maxParts) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> values;
        for (auto value : m_Values)
        {
            auto parts = splitString(value.first, separator, maxParts - 1);
            if (parts.size() < minParts)
            {
                error("Invalid value: \"" + std::string(value.first)
                      + "\". Must be at least " + std::to_string(minParts)
                      + " values separated by \"" + separator + "\".");
                return {{}, m_Args, m_ValueId};
            }
            for (auto& part : parts)
                values.emplace_back(part, value.second);
        }
        return {move(values), m_Args, m_ValueId};
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    ArgumentView::ArgumentView(const ArgumentData* data)
            : m_Argument(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    const std::string& ArgumentView::text() const
    {
        return m_Argument->text;
    }

    const std::string& ArgumentView::section() const
    {
        return m_Argument->section;
    }

    const std::string& ArgumentView::value() const
    {
        return m_Argument->value;
    }

    Visibility ArgumentView::visibility() const
    {
        return m_Argument->visibility;
    }

    int ArgumentView::id() const
    {
        return m_Argument->id;
    }

    const std::string& ArgumentView::name() const
    {
        return m_Argument->name;
    }

    bool ArgumentView::optional() const
    {
        return m_Argument->minCount == 0;
    }

    std::pair<unsigned, unsigned> ArgumentView::count() const
    {
        return {m_Argument->minCount, m_Argument->maxCount};
    }

    ValueId ArgumentView::valueId() const
    {
        return m_Argument->valueId;
    }

    ArgumentId ArgumentView::argumentId() const
    {
        return m_Argument->argumentId;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    unsigned getConsoleWidth();

    unsigned getConsoleWidth(unsigned minWidth, unsigned defaultWidth = 80);
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)
    #include <sys/ioctl.h>
    #include <unistd.h>
#elif defined(WIN32)
    #include <Windows.h>
#endif

namespace Argos
{
#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)

    unsigned getConsoleWidth()
    {
        winsize ws = {};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) <= -1)
            return 0;
        return unsigned(ws.ws_col);
    }

#elif defined(WIN32)

    unsigned getConsoleWidth()
    {
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hCon == INVALID_HANDLE_VALUE)
            return 0;

        CONSOLE_SCREEN_BUFFER_INFO conInfo;
        if (!GetConsoleScreenBufferInfo(hCon, &conInfo))
            return 0;

        return unsigned(conInfo.srWindow.Right - conInfo.srWindow.Left + 1);
    }

#else

    unsigned getConsoleWidth()
    {
        return 0;
    }

#endif

    unsigned getConsoleWidth(unsigned minWidth, unsigned int defaultWidth)
    {
        auto width = getConsoleWidth();
        if (width == 0)
            return std::max(minWidth, defaultWidth);
        return width < minWidth ? minWidth : width;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-21.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <iostream>

namespace Argos
{
    namespace
    {
        std::string getArgumentName(const ArgumentData& arg)
        {
            if (arg.name[0] == '<' || arg.name[0] == '[')
                return arg.name;

            std::string result;
            for (unsigned i = 0; i < arg.minCount; ++i)
            {
                if (!result.empty())
                    result += " ";
                result += "<" + arg.name + ">";
            }

            if (arg.maxCount == arg.minCount)
                return result;

            if (!result.empty())
                result += " ";
            if (arg.maxCount - arg.minCount == 1)
                result += "[<" + arg.name + ">]";
            else
                result += "[<" + arg.name + ">]...";
            return result;
        }

        std::string getBriefOptionName(const OptionData& opt)
        {
            std::string optTxt;
            bool braces = opt.optional
                          && opt.type != OptionType::STOP
                          && opt.type != OptionType::HELP;
            if (braces)
                optTxt.push_back('[');
            const auto& flag = opt.flags.front();
            optTxt += flag;
            if (!opt.argument.empty())
            {
                if (flag.back() != '=')
                    optTxt += " ";
                if (opt.argument.front() != '<')
                {
                    optTxt += "<";
                    optTxt += opt.argument;
                    optTxt.push_back('>');
                }
                else
                {
                    optTxt += opt.argument;
                }
            }
            if (braces)
                optTxt.push_back(']');
            return optTxt;
        }

        std::string getLongOptionName(const OptionData& opt)
        {
            std::string optTxt;
            for (const auto& flag : opt.flags)
            {
                if (!optTxt.empty())
                    optTxt.append(", ");
                optTxt += flag;
                if (!opt.argument.empty())
                {
                    if (flag.back() != '=')
                        optTxt.push_back(' ');
                    if (opt.argument.front() != '<')
                    {
                        optTxt += "<";
                        optTxt += opt.argument;
                        optTxt.push_back('>');
                    }
                    else
                    {
                        optTxt += opt.argument;
                    }
                }
            }
            return optTxt;
        }

        std::optional<std::string>
        getCustomText(ParserData& data, TextId textId)
        {
            auto it = data.helpSettings.texts.find(textId);
            if (it != data.helpSettings.texts.end())
                return it->second;
            return {};
        }

        bool isEmpty(const std::optional<std::string>& str)
        {
            return !str || str->empty();
        }

        std::optional<std::string>
        writeCustomText(ParserData& data, TextId textId,
                        bool prependNewline = false)
        {
            auto text = getCustomText(data, textId);
            if (!isEmpty(text))
            {
                if (prependNewline)
                    data.textFormatter.newline();
                data.textFormatter.writeWords(*text);
                if (!data.textFormatter.isCurrentLineEmpty())
                    data.textFormatter.newline();
            }
            return text;
        }

        void writeStopAndHelpUsage(ParserData& data)
        {
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (opt->type != OptionType::HELP
                    && opt->type != OptionType::STOP)
                    continue;

                data.textFormatter.writeWords(data.helpSettings.programName);
                data.textFormatter.writeWords(" ");
                data.textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
                data.textFormatter.writeLines(getBriefOptionName(*opt));
                data.textFormatter.writeWords(" ");
                data.textFormatter.popIndentation();
                data.textFormatter.newline();
            }
        }

        using HelpText = std::pair<std::string, std::string_view>;
        using HelpTextVector = std::vector<HelpText>;
        using SectionHelpTexts = std::pair<std::string_view, HelpTextVector>;

        unsigned int getHelpTextLabelWidth(
            const ParserData& data,
            const std::vector<SectionHelpTexts>& sections)
        {
            // Determine what width should be reserved for the argument names
            // and option flags.
            std::vector<unsigned> nameWidths;
            std::vector<unsigned> textWidths;
            for (const auto& entry : sections)
            {
                for (const auto& [name, txt] : entry.second)
                {
                    nameWidths.push_back(static_cast<unsigned>(name.size()));
                    textWidths.push_back(static_cast<unsigned>(txt.size()));
                }
            }

            std::sort(nameWidths.begin(), nameWidths.end());
            std::sort(textWidths.begin(), textWidths.end());
            auto lineWidth = data.textFormatter.lineWidth();
            // Check if both the longest name and the longest help text
            // can fit on the same line.
            auto nameWidth = nameWidths.back() + 3;
            if (nameWidth > 24 || nameWidth + textWidths.back() > lineWidth)
                return 0;
            return nameWidth;
        }

        void writeArgumentSections(ParserData& data, bool prependNewline)
        {
            std::vector<SectionHelpTexts> sections;

            auto addHelpText = [&](std::string_view s, std::string a, std::string_view b)
            {
                auto it = find_if(sections.begin(), sections.end(),
                                  [&](const auto& v)
                                  {return v.first == s;});
                if (it == sections.end())
                {
                    sections.push_back({s, {}});
                    it = std::prev(sections.end());
                }
                it->second.emplace_back(std::move(a), b);
            };

            auto argTitle = getCustomText(data, TextId::ARGUMENTS_TITLE);
            if (!argTitle)
                argTitle = "ARGUMENTS";
            for (auto& a : data.arguments)
            {
                if ((a->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = a->section.empty() ? *argTitle : a->section;
                addHelpText(section, getArgumentName(*a), a->text);
            }
            auto optTitle = getCustomText(data, TextId::OPTIONS_TITLE);
            if (!optTitle)
                optTitle = "OPTIONS";
            for (auto& o : data.options)
            {
                if ((o->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = o->section.empty() ? *optTitle : o->section;
                addHelpText(section, getLongOptionName(*o), o->text);
            }

            if (sections.empty())
                return;
            unsigned int nameWidth = getHelpTextLabelWidth(data, sections);

            auto& formatter = data.textFormatter;
            for (auto&[section, txts] : sections)
            {
                if (prependNewline)
                    formatter.newline();
                formatter.writeWords(section);
                formatter.newline();
                formatter.pushIndentation(2);
                for (auto& [name, text] : txts)
                {
                    formatter.writeWords(name);
                    if (nameWidth)
                    {
                        if (formatter.currentLineWidth() >= nameWidth)
                            formatter.writeWords("  ");
                        formatter.pushIndentation(nameWidth);
                    }
                    else
                    {
                        formatter.newline();
                        formatter.pushIndentation(8);
                    }
                    formatter.writeWords(text);
                    formatter.popIndentation();
                    formatter.newline();
                }
                formatter.popIndentation();
                prependNewline = true;
            }
        }

        void writeBriefUsage(ParserData& data, bool prependNewline)
        {
            auto& formatter = data.textFormatter;
            if (prependNewline)
                formatter.newline();

            formatter.pushIndentation(2);
            writeStopAndHelpUsage(data);
            formatter.writeWords(data.helpSettings.programName);
            formatter.writeWords(" ");
            formatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (opt->type == OptionType::HELP
                    || opt->type == OptionType::STOP)
                    continue;

                formatter.writeLines(getBriefOptionName(*opt));
                formatter.writeWords(" ");
            }
            for (auto& arg : data.arguments)
            {
                if ((arg->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                formatter.writeLines(getArgumentName(*arg));
                formatter.writeWords(" ");
            }
            formatter.popIndentation();
            formatter.newline();
            formatter.popIndentation();
        }

        bool writeUsage(ParserData& data, bool prependNewline = false)
        {
            if (auto t = getCustomText(data, TextId::USAGE); t && t->empty())
                return false;

            auto text1 = writeCustomText(data, TextId::USAGE_TITLE,
                                         prependNewline);
            if (!text1)
            {
                if (prependNewline)
                    data.textFormatter.newline();
                data.textFormatter.writeWords("USAGE");
                data.textFormatter.newline();
                prependNewline = false;
            }
            else
            {
                prependNewline = prependNewline && isEmpty(text1);
            }
            auto text2 = writeCustomText(data, TextId::USAGE,
                                         prependNewline);
            if (text2)
                return !isEmpty(text1) || !isEmpty(text2);
            writeBriefUsage(data, prependNewline);
            return true;
        }

        std::string getName(ParserData& data, ArgumentId argumentId)
        {
            for (const auto& a : data.arguments)
            {
                if (a->argumentId == argumentId)
                    return a->name;
            }
            for (const auto& o : data.options)
            {
                if (o->argumentId == argumentId)
                {
                    std::string name = o->flags.front();
                    for (size_t i = 1; i < o->flags.size(); ++i)
                        name += ", " + o->flags[i];
                    return name;
                }
            }
            return {};
        }
    }

    void writeHelpText(ParserData& data)
    {
        if (data.helpSettings.outputStream)
            data.textFormatter.setStream(data.helpSettings.outputStream);
        bool newline = !isEmpty(writeCustomText(data, TextId::INITIAL_TEXT));
        newline = writeUsage(data, newline) || newline;
        newline = !isEmpty(writeCustomText(data, TextId::TEXT, newline)) || newline;
        writeArgumentSections(data, newline);
        writeCustomText(data, TextId::FINAL_TEXT, true);
    }

    void writeErrorMessage(ParserData& data, const std::string& msg)
    {
        if (data.helpSettings.outputStream)
            data.textFormatter.setStream(data.helpSettings.outputStream);
        else
            data.textFormatter.setStream(&std::cerr);
        data.textFormatter.writeWords(data.helpSettings.programName + ": ");
        data.textFormatter.writeWords(msg);
        data.textFormatter.newline();
        if (!writeCustomText(data, TextId::ERROR_USAGE))
            writeUsage(data);
    }

    void writeErrorMessage(ParserData& data, const std::string& msg,
                           ArgumentId argumentId)
    {
        if (auto name = getName(data, argumentId); !name.empty())
            writeErrorMessage(data, name + ": " + msg);
        else
            writeErrorMessage(data, msg);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    Option::Option()
        : m_Option(std::make_unique<OptionData>())
    {}

    Option::Option(std::initializer_list<std::string> flags)
        : m_Option(std::make_unique<OptionData>())
    {
        m_Option->flags = flags;
    }

    Option::Option(const Option& rhs)
        : m_Option(rhs.m_Option
                   ? std::make_unique<OptionData>(*rhs.m_Option)
                   : std::unique_ptr<OptionData>())
    {}

    Option::Option(Option&& rhs) noexcept
        : m_Option(std::move(rhs.m_Option))
    {}

    Option::~Option() = default;

    Option& Option::operator=(const Option& rhs)
    {
        if (this != &rhs)
        {
            if (rhs.m_Option)
                m_Option = std::make_unique<OptionData>(*rhs.m_Option);
            else
                m_Option = {};
        }
        return *this;
    }

    Option& Option::operator=(Option&& rhs) noexcept
    {
        m_Option = std::move(rhs.m_Option);
        return *this;
    }

    Option& Option::text(const std::string& text)
    {
        checkOption();
        m_Option->text = text;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        checkOption();
        m_Option->section = name;
        return *this;
    }

    Option& Option::value(const std::string& id)
    {
        checkOption();
        m_Option->value = id;
        return *this;
    }

    Option& Option::operation(OptionOperation operation)
    {
        checkOption();
        m_Option->operation = operation;
        return *this;
    }

    Option& Option::visibility(Visibility visibility)
    {
        checkOption();
        m_Option->visibility = visibility;
        return *this;
    }

    Option& Option::id(int id)
    {
        checkOption();
        m_Option->id = id;
        return *this;
    }

    Option& Option::flag(const std::string& f)
    {
        checkOption();
        m_Option->flags = {f};
        return *this;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        checkOption();
        m_Option->flags = std::move(f);
        return *this;
    }

    Option& Option::argument(const std::string& name)
    {
        checkOption();
        m_Option->argument = name;
        return *this;
    }

    Option& Option::constant(const char* value)
    {
        return this->constant(std::string(value));
    }

    Option& Option::constant(const std::string& value)
    {
        checkOption();
        m_Option->constant = value;
        return *this;
    }

    Option& Option::constant(bool value)
    {
        return this->constant(value ? 1LL : 0LL);
    }

    Option& Option::constant(int value)
    {
        return this->constant(static_cast<long long>(value));
    }

    Option& Option::constant(long long value)
    {
        checkOption();
        m_Option->constant = std::to_string(value);
        return *this;
    }

    Option& Option::callback(OptionCallback callback)
    {
        checkOption();
        m_Option->callback = move(callback);
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        checkOption();
        m_Option->type = type;
        return *this;
    }

    Option& Option::optional(bool optional)
    {
        checkOption();
        m_Option->optional = optional;
        return *this;
    }

    const OptionData& Option::data() const
    {
        checkOption();
        return *m_Option;
    }

    std::unique_ptr<OptionData> Option::release()
    {
        checkOption();
        return std::move(m_Option);
    }

    void Option::checkOption() const
    {
        if (!m_Option)
            ARGOS_THROW("Cannot use Option instance after"
                        " release() has been called.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    OptionIterator::OptionIterator()
        : m_ArgsIt(m_Args.begin())
    {}

    OptionIterator::OptionIterator(std::vector<std::string_view> args, char prefix)
        : m_Args(move(args)),
          m_ArgsIt(m_Args.begin()),
          m_Prefix(prefix)
    {}

    OptionIterator::OptionIterator(const OptionIterator& rhs)
        : m_Args(rhs.m_Args),
          m_ArgsIt(m_Args.begin() + std::distance(rhs.m_Args.begin(), rhs.m_ArgsIt)),
          m_Pos(rhs.m_Pos),
          m_Prefix(rhs.m_Prefix)
    {}

    std::optional<std::string> OptionIterator::next()
    {
        if (m_Pos != 0)
        {
            m_Pos = 0;
            ++m_ArgsIt;
        }

        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_ArgsIt->size() <= 2 || (*m_ArgsIt)[0] != m_Prefix)
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
        }

        auto eq = m_ArgsIt->find('=');
        if (eq == std::string_view::npos)
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
        }

        m_Pos = eq + 1;
        return std::string(m_ArgsIt->substr(0, m_Pos));
    }

    std::optional<std::string> OptionIterator::nextValue()
    {
        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_Pos != std::string_view::npos)
        {
            auto result = m_ArgsIt->substr(m_Pos);
            m_Pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_ArgsIt == m_Args.end())
        {
            m_Pos = 0;
            return {};
        }

        m_Pos = m_ArgsIt->size();
        return std::string(*m_ArgsIt);
    }

    std::string_view OptionIterator::current() const
    {
        if (m_ArgsIt == m_Args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_ArgsIt;
    }

    std::vector<std::string_view> OptionIterator::remainingArguments() const
    {
        auto it = m_Pos == 0 ? m_ArgsIt : std::next(m_ArgsIt);
        return std::vector<std::string_view>(it, m_Args.end());
    }

    OptionIterator* OptionIterator::clone() const
    {
        return new OptionIterator(*this);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    OptionView::OptionView(const OptionData* data)
        : m_Option(data)
    {
        if (!data)
            ARGOS_THROW("data can not be null");
    }

    const std::string& OptionView::text() const
    {
        return m_Option->text;
    }

    const std::string& OptionView::section() const
    {
        return m_Option->section;
    }

    const std::string& OptionView::value() const
    {
        return m_Option->value;
    }

    OptionOperation OptionView::operation() const
    {
        return m_Option->operation;
    }

    Visibility OptionView::visibility() const
    {
        return m_Option->visibility;
    }

    int OptionView::id() const
    {
        return m_Option->id;
    }

    ValueId OptionView::valueId() const
    {
        return m_Option->valueId;
    }

    const std::vector<std::string>& OptionView::flags() const
    {
        return m_Option->flags;
    }

    const std::string& OptionView::argument() const
    {
        return m_Option->argument;
    }

    const std::string& OptionView::constant() const
    {
        return m_Option->constant;
    }

    OptionType OptionView::type() const
    {
        return m_Option->type;
    }

    bool OptionView::optional() const
    {
        return m_Option->optional;
    }

    ArgumentId OptionView::argumentId() const
    {
        return m_Option->argumentId;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <cstdlib>

namespace Argos
{
    namespace
    {
        template <typename T>
        T strToInt(const char* str, char** endp, int base);

        template <>
        long strToInt<long>(const char* str, char** endp, int base)
        {
            return strtol(str, endp, base);
        }

        template <>
        long long strToInt<long long>(const char* str, char** endp, int base)
        {
            return strtoll(str, endp, base);
        }

        template <>
        unsigned long
        strToInt<unsigned long>(const char* str, char** endp, int base)
        {
            return strtoul(str, endp, base);
        }

        template <>
        unsigned long long
        strToInt<unsigned long long>(const char* str, char** endp, int base)
        {
            return strtoull(str, endp, base);
        }

        template <typename T>
        std::optional<T> parseIntegerImpl(const std::string& str, int base)
        {
            if (str.empty())
                return {};
            char* endp = nullptr;
            errno = 0;
            auto value = strToInt<T>(str.c_str(), &endp, base);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<int> parseInteger<int>(const std::string& str, int base)
    {
        auto n = parseIntegerImpl<long>(str, base);
        if (!n)
            return {};

        if constexpr (sizeof(int) != sizeof(long))
        {
            if (*n < INT_MIN || INT_MAX < *n)
                return {};
        }
        return static_cast<int>(*n);
    }

    template <>
    std::optional<unsigned> parseInteger<unsigned>(const std::string& str, int base)
    {
        auto n = parseIntegerImpl<unsigned long>(str, base);
        if (!n)
            return {};

        if constexpr (sizeof(unsigned) != sizeof(unsigned long))
        {
            if (UINT_MAX < *n)
                return {};
        }
        return static_cast<unsigned>(*n);
    }

    template <>
    std::optional<long> parseInteger<long>(const std::string& str, int base)
    {
        return parseIntegerImpl<long>(str, base);
    }

    template <>
    std::optional<long long>
    parseInteger<long long>(const std::string& str, int base)
    {
        return parseIntegerImpl<long long>(str, base);
    }

    template <>
    std::optional<unsigned long>
    parseInteger<unsigned long>(const std::string& str, int base)
    {
        return parseIntegerImpl<unsigned long>(str, base);
    }

    template <>
    std::optional<unsigned long long>
    parseInteger<unsigned long long>(const std::string& str, int base)
    {
        return parseIntegerImpl<unsigned long long>(str, base);
    }

    namespace
    {
        template <typename T>
        T strToFloat(const char* str, char** endp);

        template <>
        float strToFloat<float>(const char* str, char** endp)
        {
            return strtof(str, endp);
        }

        template <>
        double strToFloat<double>(const char* str, char** endp)
        {
            return strtod(str, endp);
        }

        template <typename T>
        std::optional<T> parseFloatingPointImpl(const std::string& str)
        {
            if (str.empty())
                return {};
            char* endp = nullptr;
            errno = 0;
            auto value = strToFloat<T>(str.c_str(), &endp);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<float> parseFloatingPoint<float>(const std::string& str)
    {
        return parseFloatingPointImpl<float>(str);
    }

    template <>
    std::optional<double> parseFloatingPoint<double>(const std::string& str)
    {
        return parseFloatingPointImpl<double>(str);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-26.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    ParsedArguments::ParsedArguments() = default;

    ParsedArguments::ParsedArguments(std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_Impl(move(impl))
    {}

    ParsedArguments::ParsedArguments(ParsedArguments&& rhs) noexcept
        : m_Impl(move(rhs.m_Impl))
    {}

    ParsedArguments::~ParsedArguments() = default;

    ParsedArguments& ParsedArguments::operator=(ParsedArguments&& rhs) noexcept
    {
        m_Impl = move(rhs.m_Impl);
        return *this;
    }

    bool ParsedArguments::has(const std::string& name) const
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArguments::has(const IArgumentView& arg) const
    {
        return m_Impl->has(arg.valueId());
    }

    ArgumentValue ParsedArguments::value(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (value)
            return {value->first, m_Impl, id, value->second};
        else
            return {{}, m_Impl, id, {}};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        auto value = m_Impl->getValue(arg.valueId());
        if (value)
            return {value->first, m_Impl, arg.valueId(), arg.argumentId()};
        else
            return {{}, m_Impl, arg.valueId(), arg.argumentId()};
    }

    ArgumentValues ParsedArguments::values(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        return {values, m_Impl, id};
    }

    ArgumentValues ParsedArguments::values(const IArgumentView& arg) const
    {
        auto values = m_Impl->getValues(arg.valueId());
        return {values, m_Impl, arg.valueId()};
    }

    std::vector<std::unique_ptr<ArgumentView>>
    ParsedArguments::allArguments() const
    {
        std::vector<std::unique_ptr<ArgumentView>> result;
        for (auto& a : m_Impl->parserData()->arguments)
            result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        return result;
    }

    std::vector<std::unique_ptr<OptionView>>
    ParsedArguments::allOptions() const
    {
        std::vector<std::unique_ptr<OptionView>> result;
        for (auto& o : m_Impl->parserData()->options)
            result.emplace_back(std::make_unique<OptionView>(o.get()));
        return result;
    }

    ParserResultCode ParsedArguments::resultCode() const
    {
        return m_Impl->resultCode();
    }

    OptionView ParsedArguments::stopOption() const
    {
        auto option = m_Impl->stopOption();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }

    const std::vector<std::string>& ParsedArguments::unprocessedArguments() const
    {
        return m_Impl->unprocessedArguments();
    }

    void ParsedArguments::filterParsedArguments(int& argc, char**& argv)
    {
        if (argc <= 1)
            return;
        const auto& unprocessed = m_Impl->unprocessedArguments();
        auto it = unprocessed.begin();
        int out = 1;
        if (!unprocessed.empty())
        {
            for (int in = 1; in < argc; ++in)
            {
                if (argv[in] == *it)
                {
                    argv[out++] = argv[in];
                    if (++it == unprocessed.end())
                        break;
                }
            }
        }
        argc = out;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(
            std::shared_ptr<ParsedArgumentsImpl> impl)
        : m_Impl(move(impl))
    {}

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->appendValue(m_Impl->getValueId(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->appendValue(arg.valueId(), value, arg.argumentId());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->assignValue(m_Impl->getValueId(name), value, {});
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->assignValue(arg.valueId(), value, arg.argumentId());
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const std::string& name)
    {
        m_Impl->clearValue(m_Impl->getValueId(name));
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::clear(const IArgumentView& arg)
    {
        m_Impl->clearValue(arg.valueId());
        return *this;
    }

    ArgumentValue ParsedArgumentsBuilder::value(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto value = m_Impl->getValue(id);
        if (value)
            return {value->first, m_Impl, id, value->second};
        else
            return {{}, m_Impl, id, {}};
    }

    ArgumentValue
    ParsedArgumentsBuilder::value(const IArgumentView& arg) const
    {
        auto value = m_Impl->getValue(arg.valueId());
        if (value)
            return {value->first, m_Impl, arg.valueId(), arg.argumentId()};
        else
            return {{}, m_Impl, arg.valueId(), arg.argumentId()};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const std::string& name) const
    {
        auto id = m_Impl->getValueId(name);
        auto values = m_Impl->getValues(id);
        return {move(values), m_Impl, id};
    }

    ArgumentValues
    ParsedArgumentsBuilder::values(const IArgumentView& arg) const
    {
        auto values = m_Impl->getValues(arg.valueId());
        return {move(values), m_Impl, arg.valueId()};
    }

    bool ParsedArgumentsBuilder::has(const std::string& name) const
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg) const
    {
        return m_Impl->has(arg.valueId());
    }

    void ParsedArgumentsBuilder::error(const std::string& errorMessage)
    {
         m_Impl->error(errorMessage);
    }

    void ParsedArgumentsBuilder::error(const std::string& errorMessage,
                                       const IArgumentView& arg)
    {
        m_Impl->error(errorMessage, arg.argumentId());
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    namespace
    {
        template <typename It, typename Value, typename IsLess>
        It lowerBound(It begin, It end, Value&& v, IsLess isLess)
        {
            while (begin != end)
            {
                auto offset = std::distance(begin, end) / 2;
                auto mid = begin + offset;
                if (isLess(*mid, v))
                    begin = mid + 1;
                else
                    end = mid;
            }
            return begin;
        }
    }

    ParsedArgumentsImpl::ParsedArgumentsImpl(std::shared_ptr<ParserData> data)
        : m_Data(move(data))
    {
        assert(m_Data);
        for (auto& a : m_Data->arguments)
        {
            m_Ids.emplace_back(a->name, a->valueId, a->argumentId);
            if (!a->value.empty())
                m_Ids.emplace_back(a->value, a->valueId, a->argumentId);
        }
        for (auto& o : m_Data->options)
        {
            if (o->operation == OptionOperation::NONE)
                continue;

            for (auto& f : o->flags)
                m_Ids.emplace_back(f, o->valueId, o->argumentId);
            if (!o->value.empty())
                m_Ids.emplace_back(o->value, o->valueId, o->argumentId);
        }
        if (!m_Ids.empty())
        {
            using std::get;
            sort(m_Ids.begin(), m_Ids.end());
            for (auto it = next(m_Ids.begin()); it != m_Ids.end(); ++it)
            {
                auto p = prev(it);
                if (get<0>(*it) == get<0>(*p) && get<2>(*it) != get<2>(*p))
                    get<2>(*it) = get<2>(*p) = {};
            }
            m_Ids.erase(unique(m_Ids.begin(), m_Ids.end()), m_Ids.end());
        }
    }

    bool ParsedArgumentsImpl::has(ValueId valueId) const
    {
        return m_Values.find(valueId) != m_Values.end();
    }

    const std::vector<std::string>& ParsedArgumentsImpl::unprocessedArguments() const
    {
        return m_UnprocessedArguments;
    }

    void ParsedArgumentsImpl::addUnprocessedArgument(const std::string& arg)
    {
        m_UnprocessedArguments.push_back(arg);
    }

    std::string_view
    ParsedArgumentsImpl::assignValue(ValueId valueId,
                                     const std::string& value,
                                     ArgumentId argumentId)
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
            return appendValue(valueId, value, argumentId);

        it->second = {value, argumentId};
        auto nxt = next(it);
        while (nxt != m_Values.end() && nxt->first == valueId)
            m_Values.erase(nxt++);
        return it->second.first;
    }

    std::string_view
    ParsedArgumentsImpl::appendValue(ValueId valueId,
                                     const std::string& value,
                                     ArgumentId argumentId)
    {
        return m_Values.insert({valueId, {value, argumentId}})->second.first;
    }

    void ParsedArgumentsImpl::clearValue(ValueId valueId)
    {
        m_Values.erase(valueId);
    }

    ValueId ParsedArgumentsImpl::getValueId(std::string_view valueName) const
    {
        using std::get;
        auto it = lowerBound(m_Ids.begin(), m_Ids.end(), valueName,
                             [](auto& p, auto& s) {return get<0>(p) < s;});
        if (it == m_Ids.end() || get<0>(*it) != valueName)
            ARGOS_THROW("Unknown value: " + std::string(valueName));
        return get<1>(*it);
    }

    std::optional<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::getValue(ValueId valueId) const
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
            return {};
        auto nx = next(it);
        if (nx != m_Values.end() && nx->first == valueId)
            ARGOS_THROW("Attempt to read multiple values as a single value.");
        return it->second;
    }

    std::vector<std::pair<std::string_view, ArgumentId>>
    ParsedArgumentsImpl::getValues(ValueId valueId) const
    {
        std::vector<std::pair<std::string_view, ArgumentId>> result;
        for (auto it = m_Values.lower_bound(valueId);
             it != m_Values.end() && it->first == valueId; ++it)
            result.emplace_back(it->second);
        return result;
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ParsedArgumentsImpl::getArgumentViews(ValueId valueId) const
    {
        std::vector<std::unique_ptr<IArgumentView>> result;
        for (auto& a : m_Data->arguments)
        {
            if (a->valueId == valueId)
                result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        }
        for (auto& o : m_Data->options)
        {
            if (o->valueId == valueId)
                result.emplace_back(std::make_unique<OptionView>(o.get()));
        }
        return result;
    }

    std::unique_ptr<IArgumentView>
    ParsedArgumentsImpl::getArgumentView(ArgumentId argumentId) const
    {
        for (auto& a : m_Data->arguments)
        {
            if (a->argumentId == argumentId)
                return std::make_unique<ArgumentView>(a.get());
        }
        for (auto& o : m_Data->options)
        {
            if (o->argumentId == argumentId)
                return std::make_unique<OptionView>(o.get());
        }
        return {};
    }

    const std::shared_ptr<ParserData>& ParsedArgumentsImpl::parserData() const
    {
        return m_Data;
    }

    ParserResultCode ParsedArgumentsImpl::resultCode() const
    {
        return m_ResultCode;
    }

    void ParsedArgumentsImpl::setResultCode(ParserResultCode resultCode)
    {
        m_ResultCode = resultCode;
    }

    const OptionData* ParsedArgumentsImpl::stopOption() const
    {
        return m_StopOption;
    }

    void ParsedArgumentsImpl::setBreakingOption(const OptionData* option)
    {
        m_ResultCode = ParserResultCode::STOP;
        m_StopOption = option;
    }

    void ParsedArgumentsImpl::error(const std::string& message)
    {
        writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(m_Data->parserSettings.errorExitCode);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }

    void ParsedArgumentsImpl::error(const std::string& message, ArgumentId argumentId)
    {
        writeErrorMessage(*m_Data, message, argumentId);
        if (m_Data->parserSettings.autoExit)
            exit(m_Data->parserSettings.errorExitCode);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-09.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    StandardOptionIterator::StandardOptionIterator()
        : m_ArgsIt(m_Args.end()),
          m_Pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(std::vector<std::string_view> args)
        : m_Args(move(args)),
          m_ArgsIt(m_Args.begin()),
          m_Pos(0)
    {}

    StandardOptionIterator::StandardOptionIterator(const StandardOptionIterator& rhs)
        : m_Args(rhs.m_Args),
          m_ArgsIt(m_Args.begin() + std::distance(rhs.m_Args.begin(), rhs.m_ArgsIt)),
          m_Pos(rhs.m_Pos)
    {}

    std::optional<std::string> StandardOptionIterator::next()
    {
        if (m_Pos == std::string_view::npos)
        {
            m_Pos = 0;
            ++m_ArgsIt;
        }
        else if (m_Pos != 0)
        {
            if (m_Pos < m_ArgsIt->size() && (*m_ArgsIt)[1] != '-')
                return std::string{'-', (*m_ArgsIt)[m_Pos++]};

            ++m_ArgsIt;
            m_Pos = 0;
        }

        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_ArgsIt->size() <= 2 || (*m_ArgsIt)[0] != '-')
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
        }

        if ((*m_ArgsIt)[1] != '-')
        {
            m_Pos = 2;
            return std::string(m_ArgsIt->substr(0, 2));
        }

        auto eq = m_ArgsIt->find('=');
        if (eq == std::string_view::npos)
        {
            m_Pos = std::string_view::npos;
            return std::string(*m_ArgsIt);
        }

        m_Pos = eq + 1;
        return std::string(m_ArgsIt->substr(0, m_Pos));
    }

    std::optional<std::string> StandardOptionIterator::nextValue()
    {
        if (m_ArgsIt == m_Args.end())
            return {};

        if (m_Pos != std::string_view::npos)
        {
            auto result = m_ArgsIt->substr(m_Pos);
            m_Pos = std::string_view::npos;
            return std::string(result);
        }

        if (++m_ArgsIt == m_Args.end())
        {
            m_Pos = 0;
            return {};
        }

        return std::string(*m_ArgsIt);
    }

    std::string_view StandardOptionIterator::current() const
    {
        if (m_ArgsIt == m_Args.end())
            ARGOS_THROW("There is no current argument.");
        return *m_ArgsIt;
    }

    std::vector<std::string_view> StandardOptionIterator::remainingArguments() const
    {
        auto it = m_Pos == 0 ? m_ArgsIt : std::next(m_ArgsIt);
        return std::vector<std::string_view>(it, m_Args.end());
    }

    IOptionIterator* StandardOptionIterator::clone() const
    {
        return new StandardOptionIterator(*this);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    bool areEqualCharsCI(char a, char b)
    {
        if (a == b)
            return true;
        auto ua = uint8_t(a), ub = uint8_t(b);
        if ((ua ^ ub) != 32)
            return false;
        return 'A' <= (ua & 0xDFu) && (ua & 0xDFu) <= 'Z';
    }

    bool areEqualCI(std::string_view str1, std::string_view str2)
    {
        if (str1.size() != str2.size())
            return false;
        return std::equal(str2.begin(), str2.end(), str1.begin(), areEqualCharsCI);
    }

    bool areEqual(std::string_view str1, std::string_view str2,
                  bool caseInsensitive)
    {
        if (caseInsensitive)
            return areEqualCI(str1, str2);
        return str1 == str2;
    }

    bool startsWith(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool startsWithCI(std::string_view str, std::string_view prefix)
    {
        if (str.size() < prefix.size())
            return false;
        return std::equal(prefix.begin(), prefix.end(), str.begin(), areEqualCharsCI);
    }

    bool startsWith(std::string_view str, std::string_view prefix, bool caseInsensitive)
    {
        if (caseInsensitive)
            return startsWithCI(str, prefix);
        return startsWith(str, prefix);
    }

    int compareCI(int c1, int c2)
    {
        if (c1 == c2)
            return 0;
        auto ic1 = int(uint8_t(c1) & 0xDFu);
        if (ic1 < 'A' || 'Z' < ic1)
            return c1 - c2;
        auto ic2 = int(uint8_t(c2) & 0xDFu);
        return ic1 - ic2;
    }

    bool isLessCI(std::string_view str1, std::string_view str2)
    {
        auto size = std::min(str1.size(), str2.size());
        for (size_t i = 0; i < size; ++i)
        {
            if (auto cmp = compareCI(str1[i], str2[i]); cmp != 0)
                return cmp < 0;
        }
        return str1.size() < str2.size();
    }

    bool isLess(std::string_view str1, std::string_view str2, bool caseInsensitive)
    {
        if (caseInsensitive)
            return isLessCI(str1, str2);
        return str1 < str2;
    }

    std::vector<std::string_view>
    splitString(std::string_view s, char delimiter, size_t maxSplit)
    {
        if (maxSplit == 0)
            return {s};

        std::vector<std::string_view> result;
        size_t pos = 0;
        while (true)
        {
            auto nextPos = s.find(delimiter, pos);
            result.push_back(s.substr(pos, nextPos - pos));
            if (nextPos == std::string_view::npos)
                break;
            pos = nextPos + 1;
            if (result.size() == maxSplit)
            {
                result.push_back(s.substr(pos));
                break;
            }
        }
        return result;
    }

    std::string_view getBaseName(std::string_view str)
    {
        auto pos = str.find_last_of("/\\");
        return pos == std::string_view::npos ? str : str.substr(pos + 1);
    }

    constexpr size_t getCodePointLength(char c) noexcept
    {
        auto u = unsigned(static_cast<uint8_t>(c));
        if (u < 0x80)
            return 1;
        if (u > 0xF7)
            return 0;
        switch (unsigned(u >> 4u) & 7u)
        {
        case 7: return 4;
        case 6: return 3;
        case 5:
        case 4: return 2;
        default: break;
        }
        return 0;
    }

    size_t countCodePoints(std::string_view str)
    {
        size_t count = 0;
        size_t charLen = 0;
        for (auto c : str)
        {
            if (charLen == 0)
            {
                charLen = getCodePointLength(c);
                if (charLen == 0)
                    return str.size();
                ++count;
                --charLen;
            }
            else if ((unsigned(static_cast<uint8_t>(c)) & 0xC0u) == 0x80u)
            {
                --charLen;
            }
            else
            {
                return str.size();
            }
        }
        return count;
    }

    size_t findNthCodePoint(std::string_view str, size_t n)
    {
        if (n >= str.size())
            return std::string_view::npos;
        size_t count = 0;
        size_t charLen = 0;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (charLen == 0)
            {
                if (count == n)
                    return i;
                charLen = getCodePointLength(str[i]);
                if (charLen == 0)
                    return n;
                ++count;
                --charLen;
            }
            else if ((static_cast<uint8_t>(str[i]) & 0xC0u) == 0x80u)
            {
                --charLen;
            }
            else
            {
                return n;
            }
        }
        return charLen ? n : std::string_view::npos;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <array>

namespace Argos
{
    namespace
    {
        std::pair<std::string_view, std::string_view>
        nextLine(std::string_view text)
        {
            auto pos = text.find_first_of("\n\r");
            if (pos == std::string_view::npos)
                return {text, {}};
            if (text[pos] == '\n'
                || (pos + 1 != text.size() && text[pos + 1] != '\n'))
            {
                return {text.substr(0, pos), text.substr(pos + 1)};
            }
            return {text.substr(0, pos), text.substr(pos + 2)};
        }

        std::tuple<char, std::string_view, std::string_view>
        nextToken(std::string_view text)
        {
            if (text.empty())
                return {'0', text, {}};

            switch (text[0])
            {
            case '\t':
                return {'\t', text.substr(0, 1), text.substr(1)};
            case '\r':
                if (text.size() > 1 || text[1] == '\n')
                    return {'\n', text.substr(0, 2), text.substr(2)};
                [[fallthrough]];
            case '\n':
                return {'\n', text.substr(0, 1), text.substr(1)};
            case ' ':
                if (auto n = text.find_first_not_of(' '); n != std::string_view::npos)
                    return {' ', text.substr(0, n), text.substr(n)};
                else
                    return {' ', text, {}};
            default:
                if (auto n = text.find_first_of("\t\r\n "); n != std::string_view::npos)
                    return {'A', text.substr(0, n), text.substr(n)};
                else
                    return {'A', text, {}};
            }
        }
    }

    TextFormatter::TextFormatter()
        : TextFormatter(&std::cout, getConsoleWidth(32))
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, getConsoleWidth(32))
    {}

    TextFormatter::TextFormatter(std::ostream* stream, unsigned lineWidth)
        : m_Writer(lineWidth)
    {
        if (lineWidth <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_Writer.setStream(stream);
        m_Indents.push_back(0);
    }

    WordSplitter& TextFormatter::wordSplitter()
    {
        return m_WordSplitter;
    }

    std::ostream* TextFormatter::stream() const
    {
        return m_Writer.stream();
    }

    void TextFormatter::setStream(std::ostream* stream)
    {
        m_Writer.setStream(stream);
    }

    void TextFormatter::pushIndentation(unsigned indent)
    {
        if (indent == CURRENT_COLUMN)
        {
            indent = m_Writer.currentWidth();
            m_Writer.setSpaces(0);
        }
        m_Indents.push_back(indent);
        m_Writer.setIndentation(indent);
    }

    void TextFormatter::popIndentation()
    {
        if (m_Indents.size() == 1)
            ARGOS_THROW("No more indentations to pop.");
        m_Indents.pop_back();
        m_Writer.setIndentation(m_Indents.back());
    }

    void TextFormatter::writeWords(std::string_view text)
    {
        while (!text.empty())
        {
            auto [type, token, remainder] = nextToken(text);
            switch (type)
            {
            case '\t':
                m_Writer.tab();
                break;
            case '\n':
                m_Writer.newline();
                break;
            case ' ':
                m_Writer.setSpaces(static_cast<unsigned>(token.size()));
                break;
            default:
                appendWord(token);
                break;
            }
            text = remainder;
        }
    }

    void TextFormatter::writeLines(std::string_view text)
    {
        auto remainder = text;
        while (!remainder.empty())
        {
            auto [line, rem] = nextLine(remainder);
            if (!line.empty())
                appendWord(line);
            if (!rem.empty())
                newline();
            remainder = rem;
        }
        if (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
            newline();
    }

    void TextFormatter::newline()
    {
        m_Writer.newline();
        m_Writer.setSpaces(0);
    }

    void TextFormatter::flush()
    {
        m_Writer.flush();
    }

    void TextFormatter::appendWord(std::string_view word)
    {
        auto remainder = word;
        while (!m_Writer.write(remainder))
        {
            auto width = m_Writer.remainingWidth();
            auto [w, s, r] = m_WordSplitter.split(
                    word,
                    word.size() - remainder.size(),
                    width,
                    m_Writer.isCurrentLineEmpty());
            if (!w.empty())
            {
                m_Writer.write(w);
                if (s)
                    m_Writer.write(std::string_view(&s, 1));
                newline();
                remainder = r;
            }
            else if (m_Writer.isCurrentLineEmpty())
            {
                if (m_Writer.spaces() != 0)
                {
                    m_Writer.setSpaces(0);
                }
                else
                {
                    m_Writer.write(remainder, true);
                    return;
                }
            }
            else
            {
                newline();
                m_Writer.setSpaces(0);
            }
        }
    }

    unsigned TextFormatter::lineWidth() const
    {
        return m_Writer.lineWidth();
    }

    void TextFormatter::setLineWidth(unsigned lineWidth)
    {
        if (lineWidth <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_Writer.setLineWidth(lineWidth);
    }

    unsigned int TextFormatter::currentLineWidth() const
    {
        return m_Writer.currentWidth();
    }

    bool TextFormatter::isCurrentLineEmpty() const
    {
        return m_Writer.isCurrentLineEmpty();
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    TextWriter::TextWriter(unsigned lineWidth)
        : m_Stream(&std::cout),
          m_LineWidth(lineWidth)
    {}

    std::ostream* TextWriter::stream() const
    {
        return m_Stream;
    }

    void TextWriter::setStream(std::ostream* stream)
    {
        m_Stream = stream;
    }

    unsigned TextWriter::indentation() const
    {
        return m_Indent;
    }

    bool TextWriter::setIndentation(unsigned indent)
    {
        if (m_Indent >= m_LineWidth)
            return false;
        m_Indent = indent;
        return true;
    }

    bool TextWriter::write(std::string_view str, bool force)
    {
        auto width = currentWidth();
        auto remaining = std::max(width, m_LineWidth) - width;
        auto strWidth = static_cast<unsigned>(countCodePoints(str));
        if (!force && strWidth > remaining)
            return false;
        m_Line.append(width - m_CurrentLineWidth, ' ');
        m_Spaces = 0;
        m_Line.append(str);
        m_CurrentLineWidth += width - m_CurrentLineWidth + strWidth;
        return true;
    }

    void TextWriter::newline()
    {
        m_Line.push_back('\n');
        m_CurrentLineWidth = 0;
        flush();
    }

    void TextWriter::flush()
    {
        m_Stream->write(m_Line.data(), m_Line.size());
        m_Line.clear();
    }

    void TextWriter::tab()
    {
        m_Spaces += m_TabSize - (currentWidth() % m_TabSize);
    }

    unsigned TextWriter::spaces() const
    {
        return m_Spaces;
    }

    void TextWriter::setSpaces(unsigned n)
    {
        m_Spaces = n;
    }

    unsigned TextWriter::currentWidth() const
    {
        return std::max(m_CurrentLineWidth, m_Indent) + m_Spaces;
    }

    unsigned TextWriter::remainingWidth() const
    {
        return m_LineWidth - std::min(currentWidth(), m_LineWidth);
    }

    bool TextWriter::isCurrentLineEmpty() const
    {
        return m_Line.empty();
    }

    unsigned TextWriter::lineWidth() const
    {
        return m_LineWidth;
    }

    void TextWriter::setLineWidth(unsigned width)
    {
        m_LineWidth = width;
    }

    std::string_view TextWriter::currentLine() const
    {
        return m_Line;
    }
}

    //****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    namespace
    {
        bool isUtf8Continuation(char c)
        {
            return (uint8_t(c) & 0xC0u) == 0x80;
        }
    }

    void WordSplitter::addWord(std::string wordRule)
    {
        size_t offset = 0;
        std::vector<Split> splits;
        for (auto pos = wordRule.find_first_of(' '); pos != std::string::npos;
             pos = wordRule.find_first_of(' ', pos + 1))
        {
            if (pos == 0 || wordRule[pos - 1] == ' ')
                ARGOS_THROW("Invalid split rule: '" + wordRule + "'");
            auto sep = wordRule[pos - 1] == '-' ? '\0' : '-';
            splits.push_back({unsigned(pos - offset), sep});
            ++offset;
        }
        splits.push_back({unsigned(wordRule.size() - offset), '\0'});
        wordRule.erase(remove(wordRule.begin(), wordRule.end(), ' '),
                       wordRule.end());
        m_Strings.push_back(move(wordRule));
        m_Splits.insert({std::string_view(m_Strings.back()), move(splits)});
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t startIndex,
                        size_t maxLength, bool mustSplit) const
    {
        auto it = m_Splits.find(word);
        if (it != m_Splits.end())
        {
            Split prev = {unsigned(startIndex), '\0'};
            size_t length = 0;
            for (auto split : it->second)
            {
                if (split.index < startIndex + 1)
                    continue;
                length += countCodePoints(word.substr(prev.index, split.index - prev.index));
                if (length + (split.separator ? 1 : 0) > maxLength)
                    break;
                prev = split;
            }
            if (prev.index > startIndex + 1)
                return {word.substr(startIndex, prev.index - startIndex),
                        prev.separator,
                        word.substr(prev.index)};
        }
        if (mustSplit)
            return defaultRule(word.substr(startIndex), maxLength);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::defaultRule(std::string_view word, size_t maxLength) const
    {
        if (maxLength <= 2)
            return {{}, '\0', word};
        auto maxPos = findNthCodePoint(word, maxLength);
        if (maxPos == std::string_view::npos)
            return {word, '\0', {}};
        auto ignoreUtf8 = maxPos == maxLength;
        --maxPos;
        while (!ignoreUtf8 && isUtf8Continuation(word[maxPos]))
            --maxPos;

        auto minPos = (maxLength + 2) / 3;
        auto index = maxPos;
        for (auto count = maxLength - 1; count-- > minPos;)
        {
            --index;
            while (!ignoreUtf8 && isUtf8Continuation(word[index]))
                --index;
            if (uint8_t(word[index - 1]) >= 127 || uint8_t(word[index]) >= 127)
                continue;
            if ((isalnum(word[index - 1]) == 0) != (isalnum(word[index]) == 0))
                return {word.substr(0, index), '\0', word.substr(index)};
            if ((isdigit(word[index - 1]) == 0) != (isdigit(word[index]) == 0))
                return {word.substr(0, index), '-', word.substr(index)};
        }
        return {word.substr(0, maxPos),
                '-',
                word.substr(maxPos)};
    }
}
