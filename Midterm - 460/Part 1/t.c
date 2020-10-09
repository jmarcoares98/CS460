#include <stdio.h>
#include "type.h"
#include "string.c"

int color;

PROC proc[NPROC], * running, *sleepList, * freeList, * readyQueue, * sleepList;
int procsize = sizeof(PROC);
TQE* tq, tqe[NPROC];		// tq = timer queue pointer and the head of the queue
volatile TIMER timer[4];	// 4 timers; 2 timers per unit; at 0x00 and 0x20

#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"
#include "timer.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    if (vicstatus & (1 << 4)) {           // bit4 = 1: timer0,1
        if (*((&timer[0])->base + TVALUE) == 0) // timer 0
            timer_handler(0);
        if (*((&timer[1])->base + TVALUE) == 0) // timer 1
            timer_handler(1);
    }

    if (vicstatus & (1 << 5)) {           // bit5 = 1: timer2,3
        if (*((&timer[2])->base + TVALUE) == 0) // timer 2
            timer_handler(2);
        if (*((&timer[3])->base + TVALUE) == 0) // timer 3
            timer_handler(3);
    }
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

int body();

int main()
{ 
   color = WHITE;
   fbuf_init();
   kbd_init();
   
   // enable VIC for IRQ 31
   VIC_INTENABLE |= (1 << 4);   // enable timr0,1
   VIC_INTENABLE |= (1 << 5);   // enable timr2,3
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
   // enable KBD IRQ 
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
 
   kprintf("Welcome to WANIX in Arm\n");
   timer_init();
   timer_start(0);
   init();
   kfork((int)body, 1);
   while(1){
     if (readyQueue)
        tswitch();
   }
}
