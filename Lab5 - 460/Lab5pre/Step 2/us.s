   .global entryPoint, main, syscall, getcpsr
	.text
.code 32
// upon entry, bl main => r0 contains pointer to the string in ustack

entryPoint:	
	bl main
	
@ user process issues int syscall(a,b,c,d) ==> a,b,c,d are in r0-r3	
syscall:

   stmfd sp!, {lr} // save lr in Ustack
   swi #0
   ldmfd sp!, {pc} // restore lr

getcpsr:
   mrs r0, cpsr
   mov pc, lr

	
	
