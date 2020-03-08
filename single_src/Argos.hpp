//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#define ARGOS_THROW(msg) \
        throw ::Argos::ArgosException((msg), __FILE__, __LINE__, __FUNCTION__)

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
        std::string valueName;
        ArgumentCallback callback;
        unsigned minCount = 1;
        unsigned maxCount = 1;
        Visibility visibility = Visibility::NORMAL;
        int id = 0;
        int valueId_ = 0;
    };
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
// Created by Jan Erik Breimo on 2020-02-18.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <optional>
#include <string_view>
#include <vector>

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
    struct OptionData
    {
        std::vector<std::string> flags;
        std::string text;
        std::string section;
        std::string valueName;
        std::string argument;
        std::string value;
        OptionCallback callback;
        OptionOperation operation = OptionOperation::ASSIGN;
        OptionType type = OptionType::NORMAL;
        Visibility visibility = Visibility::NORMAL;
        bool mandatory = false;
        int id = 0;
        int valueId = 0;
    };
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
    namespace
    {
        template <typename IntT>
        IntT fromDigit(char c)
        {
            if ('0' <= c && c <= '9')
                return IntT(c - '0');
            auto u = uint8_t(c);
            u &= 0xDFu;
            if ('A' <= u && u <= 'Z')
                return IntT(10 + u - 'A');
            return std::numeric_limits<IntT>::max();
        }

        template <typename T, T Base>
        constexpr T maxPrecedingValueNegative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) / U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T maxFinalDigitNegative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) % U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T maxPrecedingValuePositive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) / U(Base));
            }
            else
            {
                return T(~T(0)) / Base;
            }
        }

        template <typename T, T Base>
        constexpr T maxFinalDigitPositive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) % U(Base));
            }
            else
            {
                return T(~T(0)) % Base;
            }
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parsePositiveIntegerImpl(std::string_view str)
        {
            if (str.empty())
                return {};
            IntT value = fromDigit<IntT>(str[0]);
            if (value >= Base)
                return {};
            for (size_t i = 1; i < str.size(); ++i)
            {
                auto digit = fromDigit<IntT>(str[i]);
                if (digit < Base
                    && (value < maxPrecedingValuePositive<IntT, Base>()
                        || (value == maxPrecedingValuePositive<IntT, Base>()
                            && digit <= maxFinalDigitPositive<IntT, Base>())))
                {
                    value *= Base;
                    value += digit;
                }
                else if (str[i] != '_' || i == str.size() - 1
                         || str[i - 1] == '_')
                {
                    return {};
                }
            }
            return value;
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parseNegativeIntegerImpl(std::string_view str)
        {
            if constexpr (std::is_signed<IntT>::value)
            {
                if (str.empty())
                    return {};
                IntT value = fromDigit<IntT>(str[0]);
                if (value >= Base)
                    return {};
                for (size_t i = 1; i < str.size(); ++i)
                {
                    auto digit = fromDigit<IntT>(str[i]);
                    if (digit < Base
                        && (value < maxPrecedingValueNegative<IntT, Base>()
                            || (value == maxPrecedingValueNegative<IntT, Base>()
                                && digit <= maxFinalDigitNegative<IntT, Base>())))
                    {
                        value *= Base;
                        value += digit;
                    }
                    else if (str[i] != '_' || i == str.size() - 1
                             || str[i - 1] == '_')
                    {
                        return {};
                    }
                }
                return -value;
            }
            else
            {
                if (str.empty())
                    return {};
                for (char c : str)
                {
                    auto digit = fromDigit<IntT>(c);
                    if (digit > 0)
                        return {};
                }
                return 0;
            }
        }
    }

    template <typename IntT, typename std::enable_if<std::is_integral<IntT>::value, int>::type = 0>
    std::optional<IntT> parseValue(std::string_view num)
    {
        static_assert(std::is_integral<IntT>());

        if (num.empty())
            return {};

        auto str = num;
        bool positive = true;
        if (str[0] == '-')
        {
            positive = false;
            str = str.substr(1);
        }
        else if (str[0] == '+')
        {
            str = str.substr(1);
        }

        if (str.empty())
            return {};

        if (str[0] == '0' && str.size() >= 3)
        {
            auto str2 = str.substr(2);
            switch (uint8_t(str[1]) | 0x20u)
            {
            case 'b':
                return positive ? parsePositiveIntegerImpl<IntT, 2>(str2)
                                : parseNegativeIntegerImpl<IntT, 2>(str2);
            case 'o':
                return positive ? parsePositiveIntegerImpl<IntT, 8>(str2)
                                : parseNegativeIntegerImpl<IntT, 8>(str2);
            case 'x':
                return positive ? parsePositiveIntegerImpl<IntT, 16>(str2)
                                : parseNegativeIntegerImpl<IntT, 16>(str2);
            default:
                break;
            }
        }
        if ('0' <= str[0] && str[0] <= '9')
        {
            return positive ? parsePositiveIntegerImpl<IntT, 10>(str)
                            : parseNegativeIntegerImpl<IntT, 10>(str);
        }
        if (num == "false")
            return 0;
        if (num == "true")
            return 1;
        return {};
    }

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
    std::optional<T> parseValue(std::string_view str);

    template <>
    std::optional<float> parseValue<float>(std::string_view str);

    template <>
    std::optional<double> parseValue<double>(std::string_view str);

    template <>
    std::optional<long double> parseValue<long double>(std::string_view str);
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
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <iosfwd>

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

        bool empty() const;

        unsigned lineWidth() const;

        void setLineWidth(unsigned width);

        std::string_view currentLine() const;
    private:
        std::ostream* m_Stream;
        std::string m_Line;
        unsigned m_LineWidth;
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
#include <deque>
#include <map>

namespace Argos
{
    class WordSplitter
    {
    public:
        void addWord(std::string wordRule);

        std::tuple<std::string_view, char, std::string_view>
        split(std::string_view word, size_t startPos, size_t maxLength,
              bool mustSplit) const;
    private:
        std::tuple<std::string_view, char, std::string_view>
        defaultRule(std::string_view word, size_t startPos,
                    size_t maxLength) const;

        using Split = std::pair<unsigned, char>;
        std::map<std::string_view, std::vector<Split>> m_Splits;
        std::deque<std::string> m_Strings;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo-> All rights reserved->
// Created by Jan Erik Breimo on 2020-01-07->
//
// This file is distributed under the BSD License->
// License text is included with the source distribution->
//****************************************************************************

#define CHECK_ARGUMENT_EXISTS() \
    if (!m_Argument) \
        ARGOS_THROW("Cannot use Argument instance after release() has been called.")

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
        CHECK_ARGUMENT_EXISTS();
        m_Argument->text = text;
        return *this;
    }

    Argument& Argument::section(const std::string& name)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->section = name;
        return *this;
    }

    Argument& Argument::valueName(const std::string& id)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->valueName = id;
        return *this;
    }

    Argument& Argument::callback(ArgumentCallback callback)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->callback = callback;
        return *this;
    }

    Argument& Argument::visibility(Visibility visibility)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->visibility = visibility;
        return *this;
    }

    Argument& Argument::id(int id)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->id = id;
        return *this;
    }

    Argument& Argument::name(const std::string& name)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->name = name;
        return *this;
    }

    Argument& Argument::optional(bool optional)
    {
        CHECK_ARGUMENT_EXISTS();
        m_Argument->minCount = optional ? 0 : 1;
        return *this;
    }

    Argument& Argument::count(unsigned n)
    {
        if (n <= 0)
            ARGOS_THROW("Argument's count must be greater than 0.");
        CHECK_ARGUMENT_EXISTS();
        m_Argument->minCount = m_Argument->maxCount = n;
        return *this;
    }

    Argument& Argument::count(unsigned minCount, unsigned maxCount)
    {
        if (maxCount == 0)
            ARGOS_THROW("Argument's max count cannot be 0.");
        if (maxCount < minCount)
            ARGOS_THROW("Argument's max count cannot be less than min count.");
        CHECK_ARGUMENT_EXISTS();
        m_Argument->minCount = minCount;
        m_Argument->maxCount = maxCount;
        return *this;
    }

    std::unique_ptr<ArgumentData> Argument::release()
    {
        CHECK_ARGUMENT_EXISTS();
        return std::move(m_Argument);
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
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    ArgumentView::ArgumentView(const ArgumentData* data)
            : m_Argument(data)
    {}

    const std::string& ArgumentView::text() const
    {
        return m_Argument->text;
    }

    const std::string& ArgumentView::section() const
    {
        return m_Argument->section;
    }

    const std::string& ArgumentView::valueName() const
    {
        return m_Argument->valueName;
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

    int ArgumentView::valueId() const
    {
        return m_Argument->valueId_;
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <algorithm>

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
// Created by Jan Erik Breimo on 2020-01-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#define CHECK_OPTION_EXISTS() \
    if (!m_Option) \
        ARGOS_THROW("Cannot use Option instance after release() has been called.")

namespace Argos
{
    Option::Option()
        : m_Option(std::make_unique<OptionData>())
    {}

    Option::Option(const Option& rhs)
        : m_Option(rhs.m_Option
                   ? std::make_unique<OptionData>(*rhs.m_Option)
                   : std::unique_ptr<OptionData>())
    {}

    Option::Option(std::vector<std::string> flags)
        : m_Option(std::make_unique<OptionData>())
    {
        m_Option->flags = move(flags);
    }

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
        CHECK_OPTION_EXISTS();
        m_Option->text = text;
        return *this;
    }

    Option& Option::section(const std::string& name)
    {
        CHECK_OPTION_EXISTS();
        m_Option->section = name;
        return *this;
    }

    Option& Option::valueName(const std::string& id)
    {
        CHECK_OPTION_EXISTS();
        m_Option->valueName = id;
        return *this;
    }

    Option& Option::operation(OptionOperation operation)
    {
        CHECK_OPTION_EXISTS();
        m_Option->operation = operation;
        return *this;
    }

    Option& Option::visibility(Visibility visibility)
    {
        CHECK_OPTION_EXISTS();
        m_Option->visibility = visibility;
        return *this;
    }

    Option& Option::id(int id)
    {
        CHECK_OPTION_EXISTS();
        m_Option->id = id;
        return *this;
    }

    Option& Option::flag(const std::string& f)
    {
        CHECK_OPTION_EXISTS();
        m_Option->flags = {f};
        return *this;
    }

    Option& Option::flags(std::vector<std::string> f)
    {
        CHECK_OPTION_EXISTS();
        m_Option->flags = std::move(f);
        return *this;
    }

    Option& Option::argument(const std::string& name)
    {
        CHECK_OPTION_EXISTS();
        m_Option->argument = name;
        return *this;
    }

    Option& Option::value(const std::string& value)
    {
        CHECK_OPTION_EXISTS();
        m_Option->value = value;
        return *this;
    }

    Option& Option::value(bool value)
    {
        return this->value(value ? 1 : 0);
    }

    Option& Option::value(int value)
    {
        CHECK_OPTION_EXISTS();
        m_Option->value = std::to_string(value);
        return *this;
    }

    Option& Option::value(double value)
    {
        CHECK_OPTION_EXISTS();
        m_Option->value = std::to_string(value);
        return *this;
    }

    Option& Option::callback(OptionCallback callback)
    {
        CHECK_OPTION_EXISTS();
        m_Option->callback = move(callback);
        return *this;
    }

    Option& Option::type(OptionType type)
    {
        CHECK_OPTION_EXISTS();
        m_Option->type = type;
        return *this;
    }

    Option& Option::mandatory(bool mandatory)
    {
        CHECK_OPTION_EXISTS();
        m_Option->mandatory = mandatory;
        return *this;
    }

    const OptionData& Option::data() const
    {
        CHECK_OPTION_EXISTS();
        return *m_Option;
    }

    std::unique_ptr<OptionData> Option::release()
    {
        CHECK_OPTION_EXISTS();
        return std::move(m_Option);
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
// Created by Jan Erik Breimo on 2020-01-28.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    OptionView::OptionView(const OptionData* data)
        : m_Option(data)
    {}

    const std::string& OptionView::text() const
    {
        return m_Option->text;
    }

    const std::string& OptionView::section() const
    {
        return m_Option->section;
    }

    const std::string& OptionView::valueName() const
    {
        return m_Option->valueName;
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

    int OptionView::valueId() const
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

    const std::string& OptionView::value() const
    {
        return m_Option->value;
    }

    OptionType OptionView::type() const
    {
        return m_Option->type;
    }

    bool OptionView::mandatory() const
    {
        return m_Option->mandatory;
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
        template <typename T, typename Func>
        std::optional<T> parseFloatingPointImpl(std::string str, Func func)
        {
            char* endp = nullptr;
            errno = 0;
            auto value = func(str.c_str(), &endp);
            if (endp == str.c_str() + str.size() && errno == 0)
                return value;
            return {};
        }
    }

    template <>
    std::optional<float> parseValue<float>(std::string_view str)
    {
        return parseFloatingPointImpl<float>(std::string(str), strtof);
    }

    template <>
    std::optional<double> parseValue(std::string_view str)
    {
        return parseFloatingPointImpl<double>(std::string(str), strtod);
    }

    template <>
    std::optional<long double> parseValue(std::string_view str)
    {
        return parseFloatingPointImpl<long double>(std::string(str), strtold);
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
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-05.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    class TextFormatter
    {
    public:
        TextFormatter();

        explicit TextFormatter(std::ostream* stream);

        TextFormatter(std::ostream* stream, size_t lineWidth);

        std::ostream* stream() const;

        void setStream(std::ostream* stream);

        size_t lineWidth() const;

        void setLineWidth(size_t lineWidth);

        static constexpr size_t CURRENT_COLUMN = SIZE_MAX;

        void pushIndentation(size_t indent);

        void popIndentation();

        void writeText(std::string_view text);

        void writePreformattedText(std::string_view text);

        void newline();

        void flush();
    private:
        void appendWord(std::string_view word);

        TextWriter m_Writer;
        std::vector<size_t> m_Indents;
        WordSplitter m_WordSplitter;
    };
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-27.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <iostream>

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
        if (!force && str.size() > remaining)
            return false;
        m_Line.append(width - m_Line.size(), ' ');
        m_Spaces = 0;
        m_Line.append(str);
        return true;
    }

    void TextWriter::newline()
    {
        m_Line.push_back('\n');
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
        return std::max(unsigned(m_Line.size()), m_Indent) + m_Spaces;
    }

    unsigned TextWriter::remainingWidth() const
    {
        return m_LineWidth - std::min(currentWidth(), m_LineWidth);
    }

    bool TextWriter::empty() const
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

#include <cstring>

namespace Argos
{
    namespace
    {
        bool isVowel(char c)
        {
            constexpr auto VOWELS = "aeiouyAEIOUY";
            return memchr(VOWELS, c, 6) != nullptr;
        }
    }

    void WordSplitter::addWord(std::string wordRule)
    {
        size_t offset = 0;
        std::vector<Split> splits;
        for (auto i = wordRule.find_first_of(' '); i != std::string::npos;
             i = wordRule.find_first_of(' ', i + 1))
        {
            if (i == 0 || wordRule[i - 1] == ' ')
                ARGOS_THROW("Invalid split rule: '" + wordRule + "'");
            auto sep = wordRule[i - 1] == '-' ? '\0' : '-';
            splits.emplace_back(unsigned(i - offset), sep);
            ++offset;
        }
        wordRule.erase(remove(wordRule.begin(), wordRule.end(), ' '),
                       wordRule.end());
        m_Strings.push_back(move(wordRule));
        m_Splits.insert({std::string_view(m_Strings.back()), move(splits)});
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::split(std::string_view word, size_t startPos,
                        size_t maxLength, bool mustSplit) const
    {
        if (word.size() - startPos <= maxLength)
            return {word.substr(startPos), '\0', {}};
        auto it = m_Splits.find(word);
        if (it != m_Splits.end())
        {
            Split prev = {};
            for (auto split : it->second)
            {
                auto length = split.first - startPos + (split.second ? 1 : 0);
                if (length > maxLength)
                {
                    if (prev.first > startPos + 1)
                        return {word.substr(startPos, prev.first - startPos),
                                prev.second,
                                word.substr(prev.first)};
                    break;
                }
                prev = split;
            }
        }
        if (mustSplit)
            return defaultRule(word.substr(startPos), startPos, maxLength);
        return {{}, '\0', word};
    }

    std::tuple<std::string_view, char, std::string_view>
    WordSplitter::defaultRule(std::string_view word, size_t startPos,
                              size_t maxLength) const
    {
        if (word.size() - startPos <= maxLength)
            return {word.substr(startPos), '\0', {}};
        if (maxLength <= 2)
            return {{}, '\0', word};
        auto index = startPos + maxLength - 1;
        auto minPos = startPos + (maxLength + 2) / 3;
        while (index-- > minPos)
        {
            if (isalnum(word[index - 1]) != isalnum(word[index]))
                return {word.substr(startPos, index - startPos),
                        '\0',
                        word.substr(index)};
            if (isdigit(word[index - 1]) != isdigit(word[index]))
                break;
            if (isalpha(word[index]) && !isVowel(word[index])
                && word[index] != word[index - 1]
                && word[index] != word[index + 1])
                break;
        }
        return {word.substr(startPos, index - startPos),
                '-',
                word.substr(index)};
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

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
// Created by Jan Erik Breimo on 2020-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <variant>

namespace Argos
{
    struct ParserSettings
    {
        OptionStyle optionStyle = OptionStyle::STANDARD;
        bool autoExit = true;
        bool allowAbbreviatedOptions = false;
        bool ignoreUndefinedOptions = false;
        bool ignoreUndefinedArguments = false;
        bool caseInsensitive = false;
    };

    struct HelpSettings
    {
        std::string programName;
        std::map<TextId, std::string> texts;
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
        : TextFormatter(&std::cout, getConsoleWidth(20))
    {}

    TextFormatter::TextFormatter(std::ostream* stream)
        : TextFormatter(stream, getConsoleWidth(20))
    {}

    TextFormatter::TextFormatter(std::ostream* stream, size_t lineWidth)
        : m_Writer(lineWidth)
    {
        if (lineWidth <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_Writer.setStream(stream);
        m_Indents.push_back(0);
    }

    std::ostream* TextFormatter::stream() const
    {
        return m_Writer.stream();
    }

    void TextFormatter::setStream(std::ostream* stream)
    {
        m_Writer.setStream(stream);
    }

    void TextFormatter::pushIndentation(size_t indent)
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

    void TextFormatter::writeText(std::string_view text)
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
                m_Writer.setSpaces(token.size());
                break;
            default:
                appendWord(token);
                break;
            }
            text = remainder;
        }
    }

    void TextFormatter::writePreformattedText(std::string_view text)
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
                    m_Writer.empty());
            if (!w.empty())
            {
                m_Writer.write(w);
                if (s)
                    m_Writer.write(std::string_view(&s, 1));
                newline();
                remainder = r;
            }
            else if (m_Writer.empty())
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

    size_t TextFormatter::lineWidth() const
    {
        return m_Writer.lineWidth();
    }

    void TextFormatter::setLineWidth(size_t lineWidth)
    {
        if (lineWidth <= 2)
            ARGOS_THROW("Line width must be greater than 2.");
        m_Writer.setLineWidth(lineWidth);
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
        ParsedArgumentsImpl(std::shared_ptr<ParserData> data);

        bool has(int valueId) const;

        const std::vector<std::string>& unprocessedArguments() const;

        void addUnprocessedArgument(const std::string& arg);

        std::string_view assignValue(int valueId, const std::string& value);

        std::string_view appendValue(int valueId, const std::string& value);

        void clearValue(int valueId);

        int getValueId(std::string_view valueName) const;

        std::optional<std::string_view> getValue(int valueId) const;

        std::vector<std::string_view> getValues(int valueId) const;

        std::vector<std::unique_ptr<IArgumentView>>
        getArgumentViews(int valueId) const;

        const std::shared_ptr<ParserData>& parserData() const;

        ParserResultCode resultCode() const;

        void setResultCode(ParserResultCode resultCode);

        const OptionData* breakingOption() const;

        void setBreakingOption(const OptionData* option);

        void error(const std::string& message);

        void error(const std::string& message, int valueId);
    private:
        std::multimap<int, std::string> m_Values;
        std::vector<std::pair<std::string_view, int>> m_ValueIds;
        std::vector<std::string> m_UnprocessedArguments;
        std::shared_ptr<ParserData> m_Data;
        ParserResultCode m_ResultCode = ParserResultCode::NONE;
        const OptionData* m_SpecialOption = nullptr;
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
        processOption(const OptionData& option, const std::string& flag);

        IteratorResult doNext();

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
// Created by Jan Erik Breimo on 2020-01-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

namespace Argos
{
    ArgumentValue::ArgumentValue(std::optional<std::string_view> value,
                                 std::shared_ptr<ParsedArgumentsImpl> args,
                                 int valueId)
        : m_Value(value),
          m_Args(std::move(args)),
          m_ValueId(valueId)
    {}

    ArgumentValue::ArgumentValue(const ArgumentValue&) = default;

    ArgumentValue::ArgumentValue(ArgumentValue&& ) noexcept = default;

    ArgumentValue::~ArgumentValue() = default;

    ArgumentValue&
    ArgumentValue::operator=(const ArgumentValue&) = default;

    ArgumentValue&
    ArgumentValue::operator=(ArgumentValue&&) noexcept = default;

    std::vector<std::unique_ptr<IArgumentView>>
    ArgumentValue::arguments() const
    {
        return m_Args->getArgumentViews(m_ValueId);
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
        return getValue(defaultValue ? 1 : 0);
    }

    int8_t ArgumentValue::asInt8(int8_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    int16_t ArgumentValue::asInt16(int16_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    int32_t ArgumentValue::asInt32(int32_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    int64_t ArgumentValue::asInt64(int64_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint8_t ArgumentValue::asUint8(uint8_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint16_t ArgumentValue::asUint16(uint16_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint32_t ArgumentValue::asUint32(uint32_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    uint64_t ArgumentValue::asUint64(uint64_t defaultValue) const
    {
        return getValue(defaultValue);
    }

    float ArgumentValue::asFloat(float defaultValue) const
    {
        return getValue(defaultValue);
    }

    double ArgumentValue::asDouble(double defaultValue) const
    {
        return getValue(defaultValue);
    }

    long double ArgumentValue::asLongDouble(long double defaultValue) const
    {
        return getValue(defaultValue);
    }

    std::string
    ArgumentValue::asString(const std::string& defaultValue) const
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
                  + "\". Must be at least " + std::to_string(minParts)
                  + " values separated by \"" + separator + "\".");
            return ArgumentValues({}, m_Args, m_ValueId);
        }
        return ArgumentValues(move(parts), m_Args, m_ValueId);
    }

    void ArgumentValue::error(const std::string& message) const
    {
        m_Args->error(message, m_ValueId);
    }

    template <typename T>
    T ArgumentValue::getValue(const T& defaultValue) const
    {
        if (!m_Value)
            return defaultValue;
        auto v = parseValue<T>(*m_Value);
        if (!v)
            m_Args->error("Invalid value: " + std::string(*m_Value) + ".",
                          m_ValueId);
        return *v;
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
    ArgumentValues::ArgumentValues(std::vector<std::string_view> values,
                                   std::shared_ptr<ParsedArgumentsImpl> args,
                                   int valueId)
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
        m_Args->error(message, m_ValueId);
    }

    bool ArgumentValues::empty() const
    {
        return m_Values.empty();
    }

    size_t ArgumentValues::size() const
    {
        return m_Values.size();
    }

    const std::vector<std::string_view>& ArgumentValues::values() const
    {
        return m_Values;
    }

    std::vector<int8_t> ArgumentValues::asInt8s(const std::vector<int8_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int16_t> ArgumentValues::asInt16s(const std::vector<int16_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int32_t> ArgumentValues::asInt32s(
            const std::vector<int32_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<int64_t> ArgumentValues::asInt64s(const std::vector<int64_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint8_t> ArgumentValues::asUint8s(const std::vector<uint8_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint16_t>
    ArgumentValues::asUint16s(const std::vector<uint16_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint32_t>
    ArgumentValues::asUint32s(const std::vector<uint32_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<uint64_t>
    ArgumentValues::asUint64s(const std::vector<uint64_t>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<float> ArgumentValues::asFloats(const std::vector<float>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<double> ArgumentValues::asDoubles(const std::vector<double>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<long double>
    ArgumentValues::asLongDoubles(const std::vector<long double>& defaultValue) const
    {
        return getValues(defaultValue);
    }

    std::vector<std::string>
    ArgumentValues::asStrings(const std::vector<std::string>& defaultValue) const
    {
        if (m_Values.empty())
            return defaultValue;

        std::vector<std::string> result;
        result.reserve(m_Values.size());
        for (auto& v : m_Values)
            result.emplace_back(v);
        return result;
    }

    ArgumentValues
    ArgumentValues::split(char separator,
                          size_t minParts, size_t maxParts) const
    {
        std::vector<std::string_view> values;
        for (auto value : m_Values)
        {
            auto parts = splitString(value, separator, maxParts - 1);
            if (parts.size() < minParts)
            {
                error("Invalid value: \"" + std::string(value)
                      + "\". Must be at least " + std::to_string(minParts)
                      + " values separated by \"" + separator + "\".");
                return ArgumentValues({}, m_Args, m_ValueId);
            }
            values.insert(values.end(), parts.begin(), parts.end());
        }
        return ArgumentValues(move(values), m_Args, m_ValueId);
    }

    template <typename T>
    std::vector<T> ArgumentValues::getValues(
            const std::vector<T>& defaultValue) const
    {
        if (m_Values.empty())
            return defaultValue;

        std::vector<T> result;
        result.reserve(m_Values.size());
        for (auto& v : m_Values)
        {
            auto value = parseValue<T>(v);
            if (!value)
            {
                m_Args->error("Invalid value: " + std::string(v) + ".",
                              m_ValueId);
            }
            result.push_back(*value);
        }
        return result;
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
    namespace
    {
        std::string getArgumentName(const ArgumentData& arg)
        {
            if (arg.name[0] == '<' || arg.name[0] == '[')
                return arg.name;
            else if (arg.minCount == 0)
                return "[<" + arg.name + ">]";
            else
                return "<" + arg.name + ">";
        }

        std::string getBriefOptionName(const OptionData& opt)
        {
            std::string optTxt;
            bool braces = !opt.mandatory
                          && opt.type != OptionType::STOP
                          && opt.type != OptionType::HELP;
            if (braces)
                optTxt.push_back('[');
            auto& flag = opt.flags.front();
            optTxt += flag;
            if (!opt.argument.empty())
            {
                if (flag.back() != '=')
                    optTxt += " ";
                optTxt += "<";
                optTxt += opt.argument;
                optTxt.push_back('>');
            }
            if (braces)
                optTxt.push_back(']');
            return optTxt;
        }

        std::string getLongOptionName(const OptionData& opt)
        {
            std::string optTxt;
            for (auto& flag : opt.flags)
            {
                if (!optTxt.empty())
                    optTxt.append(", ");
                optTxt += flag;
                if (!opt.argument.empty())
                {
                    if (flag.back() != '=')
                        optTxt.push_back(' ');
                    optTxt.push_back('<');
                    optTxt += opt.argument;
                    optTxt.push_back('>');
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

        bool writeCustomText(ParserData& data, TextId textId)
        {
            auto text = getCustomText(data, textId);
            if (!text)
                return false;
            if (!text->empty())
            {
                data.textFormatter.writeText(*text);
                data.textFormatter.newline();
            }
            return true;
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

                data.textFormatter.writeText(data.helpSettings.programName);
                data.textFormatter.writeText(" ");
                data.textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
                data.textFormatter.writePreformattedText(getBriefOptionName(*opt));
                data.textFormatter.writeText(" ");
                data.textFormatter.popIndentation();
                data.textFormatter.newline();
            }
        }

        void writeArgumentSections(ParserData& data)
        {
            using HelpText = std::pair<std::string, std::string_view>;
            using HelpTextVector = std::vector<HelpText>;
            using SectionHelpTexts = std::pair<std::string_view, HelpTextVector>;
            std::vector<SectionHelpTexts> sections;
            auto addHelpText = [&](std::string_view s, std::string a, std::string_view b)
            {
                auto it = find_if(sections.begin(), sections.end(),
                                  [&](auto& t) {return t.first == s;});
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
            for (auto& arg : data.arguments)
            {
                if ((arg->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = arg->section.empty() ? *argTitle : arg->section;
                addHelpText(section, getArgumentName(*arg), arg->text);
            }
            auto optTitle = getCustomText(data, TextId::OPTIONS_TITLE);
            if (!optTitle)
                optTitle = "OPTIONS";
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::TEXT) == Visibility::HIDDEN)
                    continue;
                auto& section = opt->section.empty() ? *optTitle : opt->section;
                addHelpText(section, getLongOptionName(*opt), opt->text);
            }

            if (sections.empty())
                return;

            std::vector<size_t> nameWidths;
            std::vector<size_t> textWidths;
            for (auto&[sec, txts] : sections)
            {
                for (auto&[name, txt] : txts)
                {
                    nameWidths.push_back(name.size());
                    textWidths.push_back(txt.size());
                }
            }

            std::sort(nameWidths.begin(), nameWidths.end());
            std::sort(textWidths.begin(), textWidths.end());
            auto nameWidth = nameWidths.back() + 3;
            if (nameWidth + textWidths.back() > data.textFormatter.lineWidth())
            {
                auto index75 = 3 * nameWidths.size() / 4;
                nameWidth = nameWidths[index75] + 3;
                if (nameWidth + textWidths[index75] > data.textFormatter.lineWidth())
                    nameWidth = data.textFormatter.lineWidth() / 4;
            }
            for (auto&[section, txts] : sections)
            {
                data.textFormatter.writeText(section);
                data.textFormatter.newline();
                data.textFormatter.pushIndentation(2);
                for (auto&[name, text] : txts)
                {
                    data.textFormatter.writeText(name);
                    data.textFormatter.pushIndentation(nameWidth);
                    data.textFormatter.writeText(text);
                    data.textFormatter.popIndentation();
                    data.textFormatter.newline();
                }
                data.textFormatter.popIndentation();
            }
        }

        void writeBriefUsage(ParserData& data)
        {
            data.textFormatter.pushIndentation(2);

            writeStopAndHelpUsage(data);

            data.textFormatter.writeText(data.helpSettings.programName);
            data.textFormatter.writeText(" ");
            data.textFormatter.pushIndentation(TextFormatter::CURRENT_COLUMN);
            for (auto& opt : data.options)
            {
                if ((opt->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                if (opt->type == OptionType::HELP
                    || opt->type == OptionType::STOP)
                    continue;

                data.textFormatter.writePreformattedText(getBriefOptionName(*opt));
                data.textFormatter.writeText(" ");
            }
            for (auto& arg : data.arguments)
            {
                if ((arg->visibility & Visibility::USAGE) == Visibility::HIDDEN)
                    continue;
                data.textFormatter.writePreformattedText(getArgumentName(*arg));
                data.textFormatter.writeText(" ");
            }
            data.textFormatter.popIndentation();
            data.textFormatter.newline();
            data.textFormatter.popIndentation();
        }

        void writeUsage(ParserData& data)
        {
            if (!writeCustomText(data, TextId::USAGE_TITLE))
            {
                data.textFormatter.writeText("USAGE");
                data.textFormatter.newline();
            }
            if (!writeCustomText(data, TextId::USAGE))
                writeBriefUsage(data);
        }
    }

    void writeHelpText(ParserData& data)
    {
        writeCustomText(data, TextId::INITIAL_TEXT);
        writeUsage(data);
        writeCustomText(data, TextId::TEXT);
        writeArgumentSections(data);
        writeCustomText(data, TextId::FINAL_TEXT);
    }

    void writeErrorMessage(ParserData& data, const std::string& msg)
    {
        data.textFormatter.writeText(data.helpSettings.programName + ":");
        data.textFormatter.writeText(msg);
        data.textFormatter.newline();
        if (!writeCustomText(data, TextId::ERROR_USAGE))
            writeBriefUsage(data);
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
        return {value, m_Impl, id};
    }

    ArgumentValue ParsedArguments::value(const IArgumentView& arg) const
    {
        auto value = m_Impl->getValue(arg.valueId());
        return {value, m_Impl, arg.valueId()};
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
        auto option = m_Impl->breakingOption();
        if (!option)
            ARGOS_THROW("There is no special option.");
        return OptionView(option);
    }

    const std::vector<std::string>& ParsedArguments::unprocessedArguments() const
    {
        return m_Impl->unprocessedArguments();
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
    ParsedArgumentsBuilder::ParsedArgumentsBuilder(ParsedArgumentsImpl* impl)
        : m_Impl(impl)
    {}

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->appendValue(m_Impl->getValueId(name), value);
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::append(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->appendValue(arg.id(), value);
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const std::string& name,
                                   const std::string& value)
    {
        m_Impl->assignValue(m_Impl->getValueId(name), value);
        return *this;
    }

    ParsedArgumentsBuilder&
    ParsedArgumentsBuilder::assign(const IArgumentView& arg,
                                   const std::string& value)
    {
        m_Impl->assignValue(arg.id(), value);
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
        m_Impl->clearValue(arg.id());
        return *this;
    }

    std::optional<std::string_view>
    ParsedArgumentsBuilder::value(const std::string& name)
    {
        return m_Impl->getValue(m_Impl->getValueId(name));
    }

    std::optional<std::string_view>
    ParsedArgumentsBuilder::value(const IArgumentView& arg)
    {
        return m_Impl->getValue(arg.id());
    }

    std::vector<std::string_view>
    ParsedArgumentsBuilder::values(const std::string& name)
    {
        return m_Impl->getValues(m_Impl->getValueId(name));
    }

    std::vector<std::string_view>
    ParsedArgumentsBuilder::values(const IArgumentView& arg)
    {
        return m_Impl->getValues(arg.id());
    }

    bool ParsedArgumentsBuilder::has(const std::string& name)
    {
        return m_Impl->has(m_Impl->getValueId(name));
    }

    bool ParsedArgumentsBuilder::has(const IArgumentView& arg)
    {
        return m_Impl->has(arg.id());
    }

    void ParsedArgumentsBuilder::error(const std::string& errorMessage)
    {
        m_Impl->error(errorMessage);
    }
}

//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

#include <cassert>

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
            m_ValueIds.emplace_back(a->name, a->valueId_);
            if (!a->valueName.empty())
                m_ValueIds.emplace_back(a->valueName, a->valueId_);
        }
        for (auto& o : m_Data->options)
        {
            if (o->operation == OptionOperation::NONE)
                continue;

            for (auto& f : o->flags)
                m_ValueIds.emplace_back(f, o->valueId);
            if (!o->valueName.empty())
                m_ValueIds.emplace_back(o->valueName, o->valueId);
        }
        sort(m_ValueIds.begin(), m_ValueIds.end());
        m_ValueIds.erase(unique(m_ValueIds.begin(), m_ValueIds.end()),
                         m_ValueIds.end());
    }

    bool ParsedArgumentsImpl::has(int valueId) const
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

    std::string_view ParsedArgumentsImpl::assignValue(int valueId, const std::string& value)
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
            return m_Values.emplace(valueId, value)->second;

        it->second = value;
        auto nxt = next(it);
        while (nxt != m_Values.end() && nxt->first == valueId)
            m_Values.erase(nxt++);
        return it->second;
    }

    std::string_view ParsedArgumentsImpl::appendValue(int valueId, const std::string& value)
    {
        return m_Values.emplace(valueId, value)->second;
    }

    void ParsedArgumentsImpl::clearValue(int valueId)
    {
        m_Values.erase(valueId);
    }

    int ParsedArgumentsImpl::getValueId(std::string_view valueName) const
    {
        auto idIt = lowerBound(m_ValueIds.begin(), m_ValueIds.end(),
                               valueName,
                               [](auto& p, auto& s) {return p.first < s;});
        if (idIt == m_ValueIds.end() || idIt->first != valueName)
            ARGOS_THROW("Unknown value: " + std::string(valueName));
        return idIt->second;
    }

    std::optional<std::string_view>
    ParsedArgumentsImpl::getValue(int valueId) const
    {
        auto it = m_Values.lower_bound(valueId);
        if (it == m_Values.end() || it->first != valueId)
            return {};
        auto nx = next(it);
        if (nx != m_Values.end() && nx->first == valueId)
            ARGOS_THROW("Attempt to read multiple values as a single value.");
        return it->second;
    }

    std::vector<std::string_view>
    ParsedArgumentsImpl::getValues(int valueId) const
    {
        std::vector<std::string_view> result;
        auto it = m_Values.lower_bound(valueId);
        for (; it != m_Values.end() && it->first == valueId; ++it)
            result.emplace_back(it->second);
        return result;
    }

    std::vector<std::unique_ptr<IArgumentView>>
    ParsedArgumentsImpl::getArgumentViews(int valueId) const
    {
        std::vector<std::unique_ptr<IArgumentView>> result;
        for (auto& a : m_Data->arguments)
        {
            if (a->valueId_ == valueId)
                result.emplace_back(std::make_unique<ArgumentView>(a.get()));
        }
        for (auto& o : m_Data->options)
        {
            if (o->valueId == valueId)
                result.emplace_back(std::make_unique<OptionView>(o.get()));
        }
        return result;
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

    const OptionData* ParsedArgumentsImpl::breakingOption() const
    {
        return m_SpecialOption;
    }

    void ParsedArgumentsImpl::setBreakingOption(const OptionData* option)
    {
        m_ResultCode = ParserResultCode::STOP;
        m_SpecialOption = option;
    }

    void ParsedArgumentsImpl::error(const std::string& message)
    {
        writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(1);
        else
            ARGOS_THROW("Error while parsing arguments.");
    }

    void ParsedArgumentsImpl::error(const std::string& message, int valueId)
    {
        writeErrorMessage(*m_Data, message);
        if (m_Data->parserSettings.autoExit)
            exit(1);
        else
            ARGOS_THROW("Error while parsing arguments.");
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
        auto res = m_Impl->next();
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
        case IteratorResultCode::DONE:
        case IteratorResultCode::UNKNOWN:
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
// Created by Jan Erik Breimo on 2020-01-07.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************

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
            OptionTable::iterator it;
            if (caseInsensitive)
            {
                sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return isLessCI(a.first, b.first);
                });
                it = adjacent_find(index.begin(), index.end(),
                                   [](auto& a, auto& b)
                {
                    return areEqualCI(a.first, b.first);
                });
            }
            else
            {
                sort(index.begin(), index.end(), [](auto& a, auto& b)
                {
                    return a.first < b.first;
                });
                it = adjacent_find(index.begin(), index.end(),
                                   [](auto& a, auto& b)
                {
                    return a.first == b.first;
                });
            }
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

        OptionTable::const_iterator findOptionCS(const OptionTable& options,
                                                 std::string_view arg)
        {
            OptionTable::value_type key = {arg, nullptr};
            return std::lower_bound(
                    options.begin(), options.end(),
                    key,
                    [&](auto& a, auto& b) {return a.first < b.first;});
        }

        OptionTable::const_iterator findOptionCI(const OptionTable& options,
                                                 std::string_view arg)
        {
            OptionTable::value_type key = {arg, nullptr};
            return std::lower_bound(
                    options.begin(), options.end(),
                    key,
                    [&](auto& a, auto& b) {return isLessCI(a.first, b.first);});
        }

        const OptionData* findOptionImpl(const OptionTable& options,
                                         std::string_view arg,
                                         bool allowAbbreviations,
                                         bool caseInsensitive)
        {
            auto it = caseInsensitive ? findOptionCI(options, arg)
                                      : findOptionCS(options, arg);
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

    IteratorResult ArgumentIteratorImpl::next()
    {
        auto result = doNext();
        while (std::get<0>(result) == IteratorResultCode::UNKNOWN)
            result = doNext();
        return result;
    }

    std::shared_ptr<ParsedArgumentsImpl>
    ArgumentIteratorImpl::parse(std::vector<std::string_view> args,
                                const std::shared_ptr<ParserData>& data)
    {
        ArgumentIteratorImpl iterator(move(args), data);
        while (true)
        {
            auto code = std::get<0>(iterator.doNext());
            if (code == IteratorResultCode::ERROR
                || code == IteratorResultCode::DONE)
            {
                break;
            }
        }
        return iterator.m_ParsedArgs;
    }

    const std::shared_ptr<ParsedArgumentsImpl>& ArgumentIteratorImpl::parsedArguments() const
    {
        return m_ParsedArgs;
    }

    std::pair<ArgumentIteratorImpl::OptionResult, std::string_view>
    ArgumentIteratorImpl::processOption(const OptionData& option, const std::string& flag)
    {
        std::string_view arg;
        switch (option.operation)
        {
        case OptionOperation::ASSIGN:
            if (!option.value.empty())
            {
                m_ParsedArgs->assignValue(option.valueId, option.value);
            }
            else if (auto value = m_Iterator->nextValue(); value)
            {
                arg = m_ParsedArgs->assignValue(option.valueId, *value);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::APPEND:
            if (!option.value.empty())
            {
                m_ParsedArgs->appendValue(option.valueId, option.value);
            }
            else if (auto value = m_Iterator->nextValue(); value)
            {
                arg = m_ParsedArgs->appendValue(option.valueId, *value);
            }
            else
            {
                error(flag + ": no value given.");
                return {OptionResult::ERROR, {}};
            }
            break;
        case OptionOperation::CLEAR:
            m_ParsedArgs->clearValue(option.valueId);
            break;
        case OptionOperation::NONE:
            break;
        }

        if (option.callback
            && !option.callback(OptionView(&option), arg,
                                ParsedArgumentsBuilder(m_ParsedArgs.get())))
        {
            error();
            return {OptionResult::ERROR, {}};
        }

        switch (option.type)
        {
        case OptionType::NORMAL:
            return {OptionResult::NORMAL, arg};
        case OptionType::HELP:
            writeHelpText(*m_Data);
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&option);
            return {OptionResult::HELP, arg};
        case OptionType::STOP:
            m_State = State::DONE;
            m_ParsedArgs->setBreakingOption(&option);
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

    IteratorResult ArgumentIteratorImpl::doNext()
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
            auto option = findOption(m_Options, *arg,
                                     m_Data->parserSettings.allowAbbreviatedOptions,
                                     m_Data->parserSettings.caseInsensitive);
            if (option)
            {
                auto optRes = processOption(*option, *arg);
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
            else
            {
                if (m_Data->parserSettings.ignoreUndefinedOptions
                    && startsWith(m_Iterator->current(), *arg))
                {
                    m_ParsedArgs->addUnprocessedArgument(*arg);
                }
                else
                {
                    error("Invalid option: " + std::string(m_Iterator->current()));
                    return {IteratorResultCode::ERROR, nullptr, {}};
                }
            }
        }
        else if (auto argument = m_ArgumentCounter.nextArgument())
        {
            auto s = m_ParsedArgs->appendValue(argument->valueId_, *arg);
            if (argument->callback
                && !argument->callback(ArgumentView(argument), s,
                                       ParsedArgumentsBuilder(m_ParsedArgs.get())))
            {
                error();
                return {IteratorResultCode::ERROR, nullptr, {}};
            }
            return {IteratorResultCode::ARGUMENT, argument, s};
        }
        else if (m_Data->parserSettings.ignoreUndefinedArguments)
        {
            m_ParsedArgs->addUnprocessedArgument(*arg);
        }
        else
        {
            error("Too many arguments, starting with \"" + *arg + "\"");
            return {IteratorResultCode::ERROR, nullptr, {}};
        }
        return {IteratorResultCode::UNKNOWN, nullptr, {}};
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
            if (o->mandatory && !m_ParsedArgs->has(o->valueId))
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
            m_ParsedArgs->setResultCode(ParserResultCode::NORMAL);
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
            exit(1);
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
                ARGOS_THROW(flag + ": options ending with '=' must take an argument.");
            return true;
        }

        bool checkStandardFlag(const std::string& flag, const OptionData& od)
        {
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
            for (auto& a : data.arguments)
                result->arguments.push_back(std::make_unique<ArgumentData>(*a));
            result->options.reserve(data.options.size());
            for (auto& o : data.options)
                result->options.push_back(std::make_unique<OptionData>(*o));
            return result;
        }

        void generateValueIds(const ParserData& data)
        {
            struct InternalIdMaker
            {
                int n = 1;
                std::map<std::string, int> explicitIds;

                int makeNumericId(const std::string& stringId)
                {
                    if (stringId.empty())
                        return n++;

                    auto it = explicitIds.find(stringId);
                    if (it == explicitIds.end())
                        it = explicitIds.emplace(stringId, n++).first;
                    return it->second;
                }
            };
            InternalIdMaker idMaker;
            for (auto& a : data.arguments)
            {
                if (!a->valueName.empty())
                {
                    a->valueId_ = idMaker.makeNumericId(a->valueName);
                    idMaker.explicitIds.emplace(a->name, a->valueId_);
                }
                else
                {
                    a->valueId_ = idMaker.makeNumericId(a->name);
                }
            }
            for (auto& o : data.options)
            {
                if (o->operation == OptionOperation::NONE)
                    continue;
                o->valueId = idMaker.makeNumericId(o->valueName);
                for (auto& f : o->flags)
                    idMaker.explicitIds.emplace(f, o->valueId);
            }
        }

        ParsedArguments parseImpl(std::vector<std::string_view> args,
                                  const std::shared_ptr<ParserData>& data)
        {
            generateValueIds(*data);
            return ParsedArguments(
                    ArgumentIteratorImpl::parse(std::move(args), data));
        }

        ArgumentIterator
        makeIteratorImpl(std::vector<std::string_view> args,
                         const std::shared_ptr<ParserData>& data)
        {
            generateValueIds(*data);
            return ArgumentIterator(std::move(args), data);
        }
    }

    ArgumentParser::ArgumentParser()
            : ArgumentParser("UNINITIALIZED")
    {}

    ArgumentParser::ArgumentParser(const std::string& programName)
            : m_Data(std::make_unique<ParserData>())
    {
        data().helpSettings.programName = programName;
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
        auto ad = argument.release();
        data().arguments.emplace_back(std::move(ad));
        return *this;
    }

    ArgumentParser& ArgumentParser::add(Option option)
    {
        auto od = option.release();

        if (od->flags.empty())
            ARGOS_THROW("Option must have one or more flags.");
        for (auto& flag : od->flags)
        {
            bool ok;
            switch (data().parserSettings.optionStyle)
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
                ok = false;
                break;
            }
            if (!ok)
                ARGOS_THROW("Invalid flag: '" + flag + "'.");
        }

        if (!od->argument.empty() && !od->value.empty())
            ARGOS_THROW("Option cannot have both argument and value set.");
        switch (od->operation)
        {
        case OptionOperation::NONE:
            if (!od->value.empty())
                ARGOS_THROW("NONE-options cannot have value set.");
            if (!od->valueName.empty())
                ARGOS_THROW("NONE-options cannot have valueName set.");
            if (od->mandatory)
                ARGOS_THROW("NONE-options cannot be mandatory.");
            break;
        case OptionOperation::ASSIGN:
            if (od->argument.empty() && od->value.empty())
                od->value = "1";
            break;
        case OptionOperation::APPEND:
            if (od->argument.empty() && od->value.empty())
                ARGOS_THROW("Options that appends must have either value or argument set.");
            break;
        case OptionOperation::CLEAR:
            if (!od->argument.empty() ||!od->value.empty())
                od->value = "1";
            if (od->mandatory)
                ARGOS_THROW("CLEAR-options cannot be mandatory.");
            break;
        }
        data().options.push_back(std::move(od));
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
        return parseImpl(std::move(args), makeCopy(data()));
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
        return makeIteratorImpl(std::move(args), makeCopy(data()));
    }

    bool ArgumentParser::allowAbbreviatedOptions() const
    {
        return data().parserSettings.allowAbbreviatedOptions;
    }

    ArgumentParser& ArgumentParser::allowAbbreviatedOptions(bool value)
    {
        data().parserSettings.allowAbbreviatedOptions = value;
        return *this;
    }

    bool ArgumentParser::autoExit() const
    {
        return data().parserSettings.autoExit;
    }

    ArgumentParser& ArgumentParser::autoExit(bool value)
    {
        data().parserSettings.autoExit = value;
        return *this;
    }

    bool ArgumentParser::caseInsensitive() const
    {
        return data().parserSettings.caseInsensitive;
    }

    ArgumentParser& ArgumentParser::caseInsensitive(bool value)
    {
        data().parserSettings.caseInsensitive = value;
        return *this;
    }

    OptionStyle ArgumentParser::optionStyle() const
    {
        return data().parserSettings.optionStyle;
    }

    ArgumentParser& ArgumentParser::optionStyle(OptionStyle value)
    {
        if (value != data().parserSettings.optionStyle)
        {
            if (!data().options.empty())
                ARGOS_THROW("Can't change option style after"
                            " options have been added.");
            data().parserSettings.optionStyle = value;
        }
        return *this;
    }

    std::ostream* ArgumentParser::outputStream() const
    {
        return m_Data->textFormatter.stream();
    }

    ArgumentParser& ArgumentParser::outputStream(std::ostream* stream)
    {
        m_Data->textFormatter.setStream(stream);
        return *this;
    }

    const std::string& ArgumentParser::programName() const
    {
        return data().helpSettings.programName;
    }

    ArgumentParser& ArgumentParser::programName(const std::string& name)
    {
        data().helpSettings.programName = name;
        return *this;
    }

    ArgumentParser& ArgumentParser::text(std::string text)
    {
        return this->text(TextId::TEXT, std::move(text));
    }

    ArgumentParser& ArgumentParser::text(TextId textId, std::string text)
    {
        data().helpSettings.texts[textId] = std::move(text);
        return *this;
    }

    ArgumentParser&& ArgumentParser::move()
    {
        return std::move(*this);
    }

    void ArgumentParser::writeHelpText()
    {
        Argos::writeHelpText(data());
    }

    const ParserData& ArgumentParser::data() const
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return *m_Data;
    }

    ParserData& ArgumentParser::data()
    {
        if (!m_Data)
            ARGOS_THROW("This instance of ArgumentParser can no longer be used.");
        return *m_Data;
    }
}
