	.syntax	unified
	.text

	@ Guest start code
    .global _start
    .func _start
_start:
    B       __reset          /* Reset: relative branch allows remap */
    B       .               /* Undefined Instruction */
    B       .               /* Software Interrupt    */
    B       .               /* Prefetch Abort        */
    B       .               /* Data Abort            */
    B       .               /* Reserved              */
    B       __irq               /* IRQ                   */
    B       .               /* FIQ                   */

    .align 4
__reset:
    ldr     r1,=__bss_start__
    ldr     r2,=__bss_end__
    mov     r3,#0

    /* Stack pointer initialization for svc, irq, and system/user modes */
    @ sp for  svc mode
    ldr     sp, =__svc_stack_top__
    @ sp_irq
    msr     cpsr_c, #0xd2
    ldr     sp, =__irq_stack_top__
    @ sp for  (system/user)
    msr     cpsr_c, #0xdf
    ldr     sp, =__c_stack_top__
    @ exception vector
    ldr     r0, =_start
    mcr     p15, 0, r0, c12, c0, 0      @ VBAR

	@ And call the C entrypoint
	bl  main 
    b   __error
__error:
    b   __error

__irq:
	@ Push registers
	push	{r0-r12}
	mrs	    r0, spsr            /* CPSR */
	push	{r0, lr}

	mov	    r0, sp
	@bl	    do_irq

	@ Pop registers
	pop 	{r0,lr}             /* CPSR, LR */
	msr	    spsr, r0
	pop	    {r0-r12}

    @ movs	pc, lr
    subs    pc, lr, #4
