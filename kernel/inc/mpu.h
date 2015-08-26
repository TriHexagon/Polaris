/**
 * @file mpu.h
 *
 * @brief MPU module. Initializes and controlles the memory protection unit (MPU).
 * Also provides functions for kernel software (especially for exception and process module).
 */

#ifndef MPU_H
#define MPU_H

#ifndef NOMPU

#include <kernel.h>

#define ERROR_MPU_NOT_SUPPORTED (ERROR_MODULE_DEFINED+1)
#define ERROR_MPU_INVALID_ACCESS_COMBINATION (ERROR_MODULE_DEFINED+2)

typedef enum MPU_REGION_ACCESS
{
	MPU_ACCESS_NO,	/**< No access */
	MPU_ACCESS_RW,	/**< Read and write access */
	MPU_ACCESS_RO	/**< Read only access */
} MPU_REGION_ACCESS;

typedef struct MPU_Region
{
	const void* baseAddress;						/**< Base address of the region (MUST BE ALIGNED!)*/
	size_t size;							/**< Size = log2(size of region) */
	MPU_REGION_ACCESS accessPrivileged;		/**< Access of priviliged software */
	MPU_REGION_ACCESS accessUnpriviliged;	/**< Access of unprivilged software */
	bool instructionAccessible;				/**< Is region instruction accessible (if true then software can be executed from there) */
} MPU_Region;

/**
 * @brief Initialize and enable the memory protection unit co-processor.
 * And set the default settings (background region = memory map, no regions enabled but background region for priviliged software).
 * @return ERROR_MPU_NOT_SUPPORTED if there is no MPU or MPU has other properties than the standard MPU, which is discribed in ARM Cortex M4 Generic Guide (4.5.1).
 * Otherwise it returns ERROR_NONE.
 */
error_t mpu_init(void);

/**
 * @brief Disable MPU.
 */
void mpu_deinit(void);

/**
 * @brief Set region settings (address, attributes, etc.) and enable region.
 * @param index Index of the region (0-7). Index 0 is reserved for kernel stack overflow detection.
 * @param settings Region settings (see MPU_Region type). MUST BE NOT NULL.
 * settings->baseAddress must be a valid address, which points to RAM.
 * settings->size must be greator than 5 (32 Byte size).
 * settings->accessPriviliged (P) and settings->accessUnpriviliged (U) valid combinations:
 * accessPriviliged | accessUnpriviliged
 * ---------------- | ------------------
 * MPU_ACCESS_NO    | MPU_ACCESS_NO
 * MPU_ACCESS_RW    | MPU_ACCESS_NO
 * MPU_ACCESS_RW    | MPU_ACCESS_RO
 * MPU_ACCESS_RW    | MPU_ACCESS_RW
 * MPU_ACCESS_RO    | MPU_ACCESS_NO
 * MPU_ACCESS_RO    | MPU_ACCESS_RO
 * settings->instructionAccessible must be false or true.
 * @return ERROR_INVALID_INDEX if index is not between 0 and 7.
 * ERROR_MPU_INVALID_ACCESS_COMBINATION if access combination is invalid (see above).
 * ERROR_INVALID_ARGUMENT if one argument in settings is invalid.
 * Otherwise it returns ERROR_NONE.
 */
error_t mpu_enableRegion(uint8_t index, MPU_Region* settings);

/**
 * @brief Disable region.
 * @param index Index of the region (0-7).
 * @return ERROR_INVALID_INDEX if index is not between 0 and 7.
 * Otherwise it returns ERROR_NONE.
 */
error_t mpu_disableRegion(uint8_t index);

#endif

#endif // MPU_H

