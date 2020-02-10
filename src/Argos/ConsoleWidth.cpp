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
    #include <Windows.h>
#endif

namespace Argos
{
#if defined(__APPLE__) || defined(unix) || defined(__unix) || defined(__unix__)

    unsigned getConsoleWidth(unsigned defaultWidth)
    {
        winsize ws = {};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) <= -1)
            return defaultWidth;
        return ws.ws_col != 0 ? unsigned(ws.ws_col) : defaultWidth;
    }

#elif defined(WIN32)

    unsigned getConsoleWidth(unsigned defaultWidth)
    {
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hCon == INVALID_HANDLE_VALUE)
            return defaultWidth;

        CONSOLE_SCREEN_BUFFER_INFO conInfo;
        if (!GetConsoleScreenBufferInfo(hCon, &conInfo))
            return defaultWidth;

        return unsigned(conInfo.srWindow.Right - conInfo.srWindow.Left + 1);
    }

#else

    unsigned getConsoleWidth(unsigned defaultWidth)
    {
        return defaultWidth;
    }

#endif
}
