/* asm startup code for kernel
 *
 * loads stack pointer and jump to kernel_start()
 */

.global handler_reset

//kernel start address from drivers/src/*device*/device_specific.c
.extern device_kernelStackStart

.section .text
handler_reset:
    //ldr r1, =_stackStart /* get stack address from linkerscript */
	ldr r2, =device_kernelStackStart
	ldr r1, [r2]
    msr msp, r1
    bl kernel_start
loop:
  b loop
