//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ConsoleWidth.hpp"

#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)
    #include <sys/ioctl.h>
    #include <unistd.h>
#elif defined(WIN32)
    #define NOMINMAX
    #include <Windows.h>
#endif

namespace argos
{
#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)

    unsigned get_console_width()
    {
        winsize ws = {};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) <= -1)
            return 0;
        return unsigned(ws.ws_col);
    }

#elif defined(WIN32)

    unsigned get_console_width()
    {
        const HANDLE h_con = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h_con == INVALID_HANDLE_VALUE)
            return 0;

        CONSOLE_SCREEN_BUFFER_INFO con_info;
        if (!GetConsoleScreenBufferInfo(h_con, &con_info))
            return 0;

        return unsigned(con_info.srWindow.Right - con_info.srWindow.Left);
    }

#else

    unsigned get_console_width()
    {
        return 0;
    }

#endif

    unsigned get_console_width(unsigned min_width, unsigned int default_width)
    {
        const auto width = get_console_width();
        if (width == 0)
            return default_width;
        return width < min_width ? min_width : width;
    }
}
