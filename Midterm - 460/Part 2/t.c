#include "type.h"
#include "string.c"

int color;

PROC proc[NPROC], * running, * freeList, * readyQueue, * sleepList;
int procsize = sizeof(PROC);

#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"
#include "semaphore.c"

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
   semaphore_init();
   
   // enable VIC for IRQ 31
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
   // enable KBD IRQ 
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
 
   kprintf("Welcome to WANIX in Arm\n");
   init();
   kfork((int)body, 1);
   while(1){
     if (readyQueue)
        tswitch();
   }
}
