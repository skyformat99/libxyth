// Copyright 2011-2017 Rodrigo Dias Correa
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef DEBUG_H
#define DEBUG_H

#include <xyth.h>

#include <stddef.h>
#include <stdio.h>

#define ESC "\x1b"
#define RESET ESC "[0m"

#define BRIGHT ESC "[1m"
#define UNDERSCORE ESC "[4m"

#define F_BLACK ESC "[30m"
#define F_RED ESC "[31m"
#define F_GREEN ESC "[32m"
#define F_YELLOW ESC "[33m"
#define F_BLUE ESC "[34m"
#define F_MAGENTA ESC "[35m"
#define F_CYAN ESC "[36m"
#define F_WHITE ESC "[37m"

#define B_BLACK ESC "[40m"
#define B_RED ESC "[41m"
#define B_GREEN ESC "[42m"
#define B_YELLOW ESC "[43m"
#define B_BLUE ESC "[44m"
#define B_MAGENTA ESC "[45m"
#define B_CYAN ESC "[46m"
#define B_WHITE ESC "[47m"

#ifndef FILENAME_COLOR
#define FILENAME_COLOR F_BLUE B_BLACK
#endif
#ifndef FUNC_COLOR
#define FUNC_COLOR F_GREEN B_BLACK
#endif
#ifndef LINE_COLOR
#define LINE_COLOR F_RED B_BLACK
#endif

#ifdef _XYTH_DEBUG
#warning "DEBUG mode!"
#define PDEBUG(fmt, args...)                                       \
    printf(FILENAME_COLOR __FILE__ ":" FUNC_COLOR "%s:" LINE_COLOR \
                                   "%d: " RESET fmt,               \
           __FUNCTION__,                                           \
           __LINE__,                                               \
           ##args)
#define PERROR(fmt, args...)                                                \
    printf(F_RED B_BLACK "[ ERROR ]" FILENAME_COLOR __FILE__ ":" FUNC_COLOR \
                         "%s:" LINE_COLOR "%d: " RESET fmt,                 \
           __FUNCTION__,                                                    \
           __LINE__,                                                        \
           ##args)
#else
#define PDEBUG(fmt, args...)
#define PERROR(fmt, args...)
#endif

#define PRINT_IF_ERROR(s)             \
    if (s != XYTH_SUCCESS) {           \
        PERROR("%s = %d\n", (#s), s); \
    }

#define PRINT_IF_NULL(ptr)              \
    if (ptr == NULL) {                  \
        PERROR("%s == NULL\n", (#ptr)); \
    }

#define PRINT_IF_TRUE(cond)      \
    if (cond) {                  \
        PERROR("%s\n", (#cond)); \
    }

#endif // DEBUG_H
