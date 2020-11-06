/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
	.text
.code 32
.global reset_handler
.global vectors_start, vectors_end
.global proc, procsize
.global tswitch, scheduler, running, goUmode
.global int_off, int_on, lock, unlock, getcsr
.global switchPgdir
.global get_fault_status, get_fault_addr, get_spsr

reset_handler:
  // set SVC stack to HIGH END of proc[0].kstack[]
  LDR r0, =proc      // r0 points to proc's
  LDR r1, =procsize  // r1 -> procsize
  LDR r2, [r1,#0]    // r2 = procsize
  ADD r0, r0, r2     // r0 -> high end of proc[0]
  MOV sp, r0
	
  // go in IRQ mode to set IRQ stack and enable IRQ interrupts
  MSR cpsr, #0x12
  LDR sp, =irq_stack_top  // set IRQ stack poiner

  /* go in ABT mode to set ABT stack */
  MSR cpsr, #0x17
  LDR sp, =abt_stack_top

  /* go in UND mode to set UND stack */
  MSR cpsr, #0x1B
  LDR sp, =und_stack_top

  /* go back in SVC mode */
  MSR cpsr, #0x13      // write to cspr, so in SVC mode now

  // set previous mode of SVC mode to USER mode with interrupts ON
  MSR spsr, #0x10

  // copy vector table to address 0 
  BL copy_vectors

  BL mkPtable                // build pagetable in C for P0

  ldr r0, Mtable
  mcr p15, 0, r0, c2, c0, 0  // set TTBase
  mcr p15, 0, r0, c8, c7, 0  // flush TLB

  // set domain0: 01=client(check permission) 11=master(no check)
  mov r0, #0x5               // 0101 for CLIENT
  mcr p15, 0, r0, c3, c0, 0
	
// enable MMU
  mrc p15, 0, r0, c1, c0, 0
  orr r0, r0, #0x00000001     // set bit0
  mcr p15, 0, r0, c1, c0, 0   // write to c1
  nop
  nop
  nop
  mrc p15, 0, r2, c2, c0
  mov r2, r2

  adr r0, mainstart
  ldr pc, [r0]
	
  B .
	
.align 4
Mtable:	    .word 0x400000    // at 4MB
mainstart:  .word main
	
.align 4

irq_handler:         // IRQ handler entry point
  sub	lr, lr, #4   
  stmfd	sp!, {r0-r12, lr}  // save all Umode regs in kstack
  bl	IRQ_handler  // call IRQ_handler() in C
  ldmfd	sp!, {r0-r12, pc}^ // pop from kstack but restore Umode SR

data_abort_handler:
  sub	lr, lr, #4         // jump over the bad instruction at PC-8 
  stmfd	sp!, {r0-r12, lr}
  bl	DATA_handler  
  ldmfd	sp!, {r0-r12, pc}^

tswitch: // tswitch() in Kmode
// HIGH    1  2   3   4  5  6  7  8  9  10  11  12  13  14
//       ---------------------------------------------------
// stack=| lr r12 r11 r10 r9 r8 r7 r6 r5 r4  r3  r2  r1 r0 |
//       -----------------------------------------------|---
//                                                  proc.ksp
// mask out IRQ
  MRS r0, cpsr
  ORR r0, r0, #0x80
  MSR cpsr, r0

  stmfd	sp!, {r0-r12, lr}

  LDR r0, =running          // r0=&running
  LDR r1, [r0, #0]          // r1->runningPROC
  str sp, [r1, #4]          // running->ksp = sp

  bl	scheduler

  LDR r0, =running
  LDR r1, [r0, #0]          // r1->runningPROC
  lDR sp, [r1, #4]          // sp = running->ksp

// mask in IRQ
  MRS r0, cpsr
  BIC r0, r0, #0x80
  MSR cpsr, r0

  ldmfd	sp!, {r0-r12, pc}  // all in Kmode


svc_entry:
   stmfd sp!, {r0-r12, lr}
	
// save Umode upc, usp, cpsr in proc.usp, proc.usp, proc.cpsr
   ldr r4, =running   // r4=&running
   ldr r5, [r4, #0]   // r5 -> PROC of running

   mrs r6, spsr       // spsr = cpsr of Umode
   str r6, [r5, #16]  // save cpsr of Umode into PROC.cpsr
	
// change to SYS mode to access Umode usp, ulr
   msr cpsr, #0xDF      // change cpsr in SYS mode (0x1F) with I-bit=1

// now in SYS mode, r13 same as User mode sp r14=user mode lr
   str sp, [r5, #8]   // save usp into proc.usp at offset 8
   str lr, [r5, #12]  // save Umode PC into PROC.upc at offset 12

// change back to SVC mode with IRQ allowed
   msr cpsr, #0x13

   bl	svc_handler  // handle syscalls in svc_handler()

goUmode:
	
// IRQ off
  MRS r1, cpsr
  ORR r1, r1, #0x80
  MSR cpsr, r1

  ldr r4, =running   // r4=&running
  ldr r5, [r4, #0]   // r5 -> PROC of running

  ldr r6, [r5, #16]
  msr spsr, r6       // restore spsr = saved Umode cpsr
 	
// change to SYS mode
  msr cpsr, #0xDF    // change cpsr to SYS mode	

// now in SYS mode: restore usp from PROC.usp
   ldr sp, [r5, #8]   // restore usp from PROC.usp

   msr cpsr, #0xD3    // back to SVC mode
	
// replace saved r0 on stack with the return value r from svc_handler()
   add sp, sp, #4     // pop saved r0 off stack
   stmfd sp!,{r0}     // push r as the saved r0 to Umode

// ^: pop regs from kstack BUT also copy spsr into cpsr ==> back to Umode
   ldmfd sp!, {r0-r12, pc}^ // ^ : pop kstack AND to previous mode

int_off:          // SR = int_off()
  MRS r0, cpsr
  mov r1, r0
  ORR r1, r1, #0x80
  MSR cpsr, r1    // return value in r0 = original cpsr
  mov pc,lr	

int_on:           // int_on(SR);  SR in r0
  MSR cpsr, r0
  mov pc,lr	

unlock:
  MRS r0, cpsr
  BIC r0, r0, #0x80
  MSR cpsr, r0
  mov pc, lr	

lock:
  MRS r0, cpsr
  ORR r0, r0, #0x80
  MSR cpsr, r0
  mov pc, lr	

getcsr:
   mrs r0, cpsr
   mov pc, lr
	
vectors_start:
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, svc_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
svc_handler_addr:            .word svc_entry
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:

switchPgdir:	// switch pgdir to new PROC's pgdir; passed in r0
  // r0 contains address of PROC's pgdir address	
  mcr p15, 0, r0, c2, c0, 0   // set TTBase
  mov r1, #0
  mcr p15, 0, r1, c8, c7, 0   // flush TLB
  mcr p15, 0, r1, c7, c7, 0   // Invalidate I and D cache
  mrc p15, 0, r2, c2, c0, 0

  // set domain: all 01=client(check permission) 11=manager(no check)
  mov r0, #0x5                // 0101 for CLIENT mode: check permission
  mcr p15, 0, r0, c3, c0, 0
  mov pc, lr                  // return
	
get_fault_status:	// read and return MMU reg 5
  MRC p15,0,r0,c5,c0,0    // read DFSR
  mov pc, lr	
get_fault_addr:	         // read and return MMU reg 6
  MRC p15,0,r0,c6,c0,0    // read DFSR
  mov pc, lr	
get_spsr:
  mrs r0, spsr
  mov pc, lr

	.end


