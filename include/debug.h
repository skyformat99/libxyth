/**
 * @file   debug.h
 * @author rodrigo
 * @date   13/10/2014
 * @brief  Debug macros
 *
 * Copyright (C) Rodrigo Dias Correa - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */


#ifndef DEBUG_H
#define DEBUG_H

#include <bergamota.h>

#include <stddef.h>
#include <stdio.h>

#define ESC   "\x1b"
#define RESET ESC "[0m"

#define BRIGHT      ESC "[1m"
#define UNDERSCORE  ESC "[4m"

#define F_BLACK    ESC "[30m"
#define F_RED      ESC "[31m"
#define F_GREEN    ESC "[32m"
#define F_YELLOW   ESC "[33m"
#define F_BLUE     ESC "[34m"
#define F_MAGENTA  ESC "[35m"
#define F_CYAN     ESC "[36m"
#define F_WHITE    ESC "[37m"

#define B_BLACK    ESC "[40m"
#define B_RED      ESC "[41m"
#define B_GREEN    ESC "[42m"
#define B_YELLOW   ESC "[43m"
#define B_BLUE     ESC "[44m"
#define B_MAGENTA  ESC "[45m"
#define B_CYAN     ESC "[46m"
#define B_WHITE    ESC "[47m"

#ifndef FILENAME_COLOR
#define FILENAME_COLOR F_BLUE B_BLACK
#endif
#ifndef FUNC_COLOR
#define FUNC_COLOR F_GREEN B_BLACK
#endif
#ifndef LINE_COLOR
#define LINE_COLOR F_RED B_BLACK
#endif

#ifdef _BGM_DEBUG
#warning "DEBUG mode!"
#define PDEBUG(fmt, args...) printf(FILENAME_COLOR __FILE__ ":"\
									FUNC_COLOR "%s:"\
									LINE_COLOR "%d: " RESET\
									fmt , __FUNCTION__, __LINE__, ## args)
#define PERROR(fmt, args...) printf(F_RED B_BLACK "[ ERROR ]" FILENAME_COLOR __FILE__ ":"\
                                    FUNC_COLOR "%s:"\
                                    LINE_COLOR "%d: " RESET\
                                    fmt , __FUNCTION__, __LINE__, ## args)
#else
#define PDEBUG(fmt, args...)
#define PERROR(fmt, args...)
#endif

#define PRINT_IF_ERROR(s) \
    if (s != BGM_SUCCESS)  { PERROR("%s = %d\n", (#s), s); }


#define PRINT_IF_NULL(ptr) \
    if (ptr == NULL) { PERROR("%s == NULL\n", (#ptr)); }

#define PRINT_IF_TRUE(cond) \
    if (cond) { PERROR("%s\n", (#cond)); }


#endif // DEBUG_H
