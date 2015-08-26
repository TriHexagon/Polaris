/**
 * @file interrupt.h
 *
 * @brief Interrupt module. Initializes interrupt vector table and controlles the NVIC.
 * Also provides functions of interrupt handler installation/deinstallation for kernel software (especially for drivers and exception module).
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <kernel.h>

/**
 * @brief The exception number constant of the first IRQ.
 */
#define INT_IRQ_EXCPT_START 16

/**
 * @brief Returns the exception number of the IRQ number.
 */
#define INT_IRQ_EXCPT_NUM(irq) (INT_IRQ_EXCPT_START+irq)

/**
 * @brief Returns the IRQ number of exception number.
 */
#define INT_EXCPT_IRQ_NUM(excp) (excp-INT_IRQ_EXCPT_START)

/**
 * @brief The number of interrupts.
 */
#define INT_COUNT 255


#define INT_IRQ_COUNT (INT_COUNT-INT_IRQ_EXCPT_START)

/**
 * @brief The first IRQ number.
 */
#define INT_IRQ_FIRST 0

/**
 * @brief The last IRQ number.
 */
#define INT_IRQ_LAST (INT_IRQ_COUNT-1)

#define ERROR_INT_ALREADY_IN_USE (ERROR_MODULE_DEFINED)
#define ERROR_INT_NOT_USED_YET (ERROR_MODULE_DEFINED+1)

#define ERROR_INT_NO_HANDLER (ERROR_MODULE_DEFINED+2)

#define INT_PRIORITY_LOWEST 15

#define INT_SOFTWARE_HIGH_PRIORITY (INT_PRIORITY_LOWEST-2)
#define INT_SOFTWARE_AVERAGE_PRIORITY (INT_PRIORITY_LOWEST-1)
#define INT_SOFTWARE_LOW_PRIORITY (INT_PRIORITY_LOWEST)

/**
 * @brief System interrupt types.
 */
typedef enum INT_TYPE
{
	INT_RESET		= 1,	/**< The reset interrupt. */
	INT_NMI			= 2,	/**< The NMI interrupt. */
	INT_HARDFAULT	= 3,	/**< The hard fault interrupt. */
	INT_MMUFAULT	= 4,	/**< The memory managment fault interrupt. */
	INT_BUSFAULT	= 5,	/**< The bus fault interrupt. */
	INT_USAGEFAULT	= 6,	/**< The usage fault interrupt. */
	INT_SVCALL		= 11,	/**< The svcall interrupt. */
	INT_PENDSV		= 14,	/**< The pendsv interrupt. */
	INT_SYSTICK		= 15,	/**< The systick interrupt. */
	INT_IRQ,				/**< An IRQ interrupt. */
} INT_TYPE;

/**
 * @brief Interrupt handler type.
 */
typedef void (*IntHandler)(void);

/**
 * @brief Initializes interrupt (int) driver and saves address of old interrupt vector table.
 *
 * MUST be called before module usage!
 */
void int_init(void);

/**
 * @brief Reloads old interrupt vector table address.
 *
 * After call driver is in uninitialized state. So module usage is permited until int_init call.
 */
void int_deinit(void);

/**
 * @brief Sets interrupt handler in vector table.
 * @param type The interrupt type (see INT_TYPE enum).
 * @param irqNum The irq number (0-239) of interrupt in vector table.
 * This parameter is only valid if it is INT_HARDWARE or INT_SOFTWARE.
 * Otherwise it will be ignored.
 * @param priority The priority of the interrupt (see INT_PRIORITY enum).
 * This parameter is only valid if type parameter is INT_SOFTWARE.
 * Otherwise it will be ignored.
 * @param handler The interrupt handler to install.
 * MUST be NOT NULL.
 * @return ERROR_INT_ALREADY_IN_USE if an interrupt handler is already installed.
 * ERROR_INVALID_ARGUMENT if type isn't a member of INT_TYPE, exceptionNum is 0, priority isn't a member of INT_PRIORITY or handler is NULL.
 * Otherwise it returns ERROR_NONE.
 */
error_t int_install(INT_TYPE type, uint8_t irqNum, uint8_t priority, IntHandler handler);

/**
 * @brief Remove interrupt handler from vector table and resets it to default handler.
 * @param type The interrupt type (see INT_TYPE enum).
 * @param irqNum The exception number (0-239) of interrupt in vector table.
 * @return ERROR_INT_NOT_USED_YET if there is no interrupt handler to deinstall.
 * ERROR_INVALID_ARGUMENT if type isn't a member of INT_TYPE or exceptionNum is 0.
 * Otherwise it returns ERROR_NONE.
 */
error_t int_deinstall(INT_TYPE type, uint8_t irqNum);

#endif // INTERRUPT_H

