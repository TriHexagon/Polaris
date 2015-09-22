#include <drivers/drivers.h>

#if DEVICE != stm32f4discovery
#error "Wrong device"
#endif

const void* drivers_intCount = (void*)82;
