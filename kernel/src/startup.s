/* asm startup code for kernel
 *
 * loads stack pointer and jump to kernel_start()
 */

.global handler_reset

//kernel start address from linker script
.extern _stackStart

.section .text
handler_reset:
    ldr r1, =_stackStart /* get stack address from linkerscript */
    msr msp, r1
    bl kernel_start
loop:
  b loop
