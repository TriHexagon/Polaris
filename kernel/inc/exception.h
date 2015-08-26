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

