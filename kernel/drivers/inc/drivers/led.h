#ifndef STATUSLED_H
#define STATUSLED_H

#include <kernel.h>

/**
 * @brief LED change parameter.
 */
typedef enum LED_SET
{
	LED_ENABLE,		/**< Enable LED. */
	LED_DISABLE,	/**< Disable LED. */
	LED_TOGGLE		/**< Toggle LED. */
} LED_SET;

/**
 * @brief Returns the driver initialization state.
 * @return true if driver is initialized.
 * Otherwise it returns false.
 */
bool led_isInit(void);

/**
 * @brief Initializes LED driver.
 * @return An error code if initializing process failed.
 */
error_t led_init(void);

/**
 * @brief Returns the number of controllable LEDs.
 * LED index enumeration begins at 0.
 */
unsigned led_getCount(void);

/**
 * @brief Sets a LED to the specified state.
 * @param index Index of the LED.
 * @param set The LED change parameter (see LED_SET enum).
 * @return ERROR_INVALID_INDEX if index represents no existing LED.
 * ERROR_INVALID_ARGUMENT if set isn't a member of LED_SET.
 * A driver specific error code if LED setting failed.
 * Otherwise it returns ERROR_NONE.
 */
error_t led_set(unsigned index, LED_SET set);

#endif // STATUSLED_H

