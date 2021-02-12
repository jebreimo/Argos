//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-02-10.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ConsoleWidth.hpp"

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

        return unsigned(conInfo.srWindow.Right - conInfo.srWindow.Left);
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
