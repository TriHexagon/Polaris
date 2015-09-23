/* asm startup code for kernel
 *
 * loads stack pointer and jumps to kernel_start()
 */

.global handler_reset
.type	handler_reset, %function

//kernel start address from linker script
.extern _stackStart

.section .text
handler_reset:
    ldr r0, =_stackStart
    msr msp, r0
    bl kernel_start
loop:
  b loop
