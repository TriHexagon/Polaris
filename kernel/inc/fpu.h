/**
 * @file fpu.h
 *
 * @brief FPU module. Initializes the floating point unit co processor if aviable.
 */

#ifndef FPU_H
#define FPU_H

#ifndef NOFPU

/**
 * @brief Enable FPU.
 * Set full access for priviliged and unpriviliged software.
 */
void fpu_init(void);

/**
 * @brief Disable FPU.
 */
void fpu_deinit(void);

#endif

#endif // FPU_H

