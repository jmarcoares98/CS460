	.text
.code 32

.global vectors_start, vectors_end
.global proc, procsize
.global tswitch, scheduler, running
.global lock, unlock, int_off, int_on

reset_handler:
  // set SVC mode stack to proc[0]'stack[ ]
  LDR r0, =proc
  LDR r1, =procsize
  LDR r2,[r1,#0]
  ADD r0, r0, r2
  MOV sp, r0
	
// copy vector table to address 0 
	BL copy_vectors

// to IRQ mode, set IRQ sp
  MSR cpsr, #0x92
  LDR sp, =irq_stack

// back to SVC mode, call main() with IRQ masked in
  MSR cpsr, #0x13
  BL main
  B .

.align 4
irq_handler:
  sub	lr, lr, #4
  stmfd	sp!, {r0-r10, fp, ip, lr}

  bl	IRQ_handler  

  ldmfd	sp!, {r0-r10, fp, ip, pc}^

tswitch:
//        -1  -2 -3  -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14
//       ---------------------------------------------------
// stack= LR,r12,r11,r10,r9,r8,r7,r6,r5, r4, r3, r2, r1, r0
//       --------------------------------------------------
SAVE:
  stmfd	sp!, {r0-r12, lr}      // push registers in stack

  LDR r0, =running             // r0=&running
  LDR r1, [r0, #0]             // r1->runningPROC
  str sp, [r1, #4]             // running->ksp = sp

FIND:	
  bl	scheduler              // set new running 

RESUME:	
  LDR r0, =running
  LDR r1, [r0, #0]             // r1->running PROC
  lDR sp, [r1, #4]             // restore sp from PROC.ksp
	
  ldmfd	sp!, {r0-r12, pc}      // pop and return

lock:  // mask out IRQ
  MRS r4, cpsr
  ORR r4, r4, #0x80   // set bit means MASK off IRQ interrupt  
  MSR cpsr, r4
  mov pc, lr

unlock: // mask in IRQ
  MRS r4, cpsr
  BIC r4, r4, #0x80   // clear bit means UNMASK IRQ interrupt
  MSR cpsr, r4
  mov pc, lr	
	
int_off: // int sr = int_off(); return CPSR in r0
  MRS r0, cpsr
  MRS r1, cpsr
  ORR r1, r1, #0x80   // set bit means MASK off IRQ interrupt 
  MSR cpsr, r1
  mov pc, lr	

int_on: // int int_on(sr); restore CPSR from r0
  MSR cpsr, r0
  mov pc, lr	
	
vectors_start:
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, swi_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
swi_handler_addr:            .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:
	
.end
