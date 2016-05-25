/* The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Christopher Cichos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file exception.h
 *
 * @brief Exception module. Installs interrupt handlers and handles exceptions.
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <kernel.h>

#define ERROR_EXCPT_HANDLER_INSTALL_FAILED (ERROR_MODULE_DEFINED)
#define ERROR_EXCPT_HANDLER_DEINSTALL_FAILED (ERROR_MODULE_DEFINED+1)

#define ERROR_EXCPT_MNI (ERROR_MODULE_DEFINED+2)
#define ERROR_EXCPT_HARDFAULT (ERROR_MODULE_DEFINED+3)
#define ERROR_EXCPT_MMUFAULT (ERROR_MODULE_DEFINED+4)
#define ERROR_EXCPT_BUSFAULT (ERROR_MODULE_DEFINED+5)
#define ERROR_EXCPT_USAGEFAULT (ERROR_MODULE_DEFINED+6)

/**
 * @brief Initialize exception module. Install exception handlers and initialize kernel stack overflow detection.
 */
void excpt_init(void);

/**
 * @brief Deinitialize exception module.
 */
void excpt_deinit(void);

#endif // EXCEPTION_H

