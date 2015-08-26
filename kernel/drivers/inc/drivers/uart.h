#ifndef UART_H
#define UART_H

#include <kernel.h>

/**
 * @brief Returns the driver initialization state.
 * @return true if driver is initialized.
 * Otherwise it returns false.
 */
bool uart_isInit(void);

error_t uart_init(void);

void uart_send(const void *data, size_t size);

#endif // UART_H

