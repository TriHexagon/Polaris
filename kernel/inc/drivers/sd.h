#ifndef SD_H
#define SD_H

#include <kernel.h>

#define ERROR_NO_SDCARD (ERROR_MODULE_DEFINED)

error_t sd_init(void);

error_t sd_read(void* buf, size_t size);

#endif // SD_H

