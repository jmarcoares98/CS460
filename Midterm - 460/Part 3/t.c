#include "type.h"
#include "string.c"

int kprintf(char *fmt, ...);
int uprintf(char *fmt, ...);
#define printf kprintf

#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "pipe.c"

PIPE *kpipe;

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

// IRQ interrupts handler entry point
void IRQ_handler()
{
    int vicstatus, sicstatus;
    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & (1<<12)){ // Bit12: uart0 
       uart_handler(&uart[0]);
    }
    if (vicstatus & (1<<13)){ // bit13: uart1
         uart_handler(&uart[1]);
    }
    if (vicstatus & (1<<31)){ // SIC interrupts=bit_31=>KBD at bit 3 
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

int pipe_writer()
{
  char c, *cp; 
  int i;
  struct uart *up = &uart[0];
  char line[128];
  
  while(1){     // for (i=0; i<2; i++){ // for writer to exit
    uprintf("Enter a line for proc1 to get : ");
    ugets(up, line);
    uprintf("\n");
    
    printf("\nproc%d writes line=[%s] to pipe\n", running->pid, line);
    write_pipe(kpipe, line, strlen(line));
  }
  printf("pipe wrtier exit\n", running->pid);
  kexit(0);
}

int pipe_reader()
{
  char c, *cp; 
  char line[128];
  int i, n;
  
  while(1){   // for (i=0; i<2; i++){  // for reader to exit
    printf("proc%d reading from pipe\n", running->pid);
    n = read_pipe(kpipe, line, 20);
    line[n] = 0;
    
    printf("proc%d read n=%d bytes from pipe : %s\n", running->pid, n, line);
    
    if (n==0){
      printf("proc%d read 0 bytes\n", running->pid);
      kgetc();
    }
  }
  printf("pipe reader %d exit\n", running->pid);
  kexit(1);
}
  
int main()
{ 
   int i; 
   char line[128]; 
   u8 kbdstatus, key, scode;
   KBD *kp = &kbd;
   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kprintf("Welcome to Wanix in ARM\n");

   
   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE = 0;
   VIC_INTENABLE |= (1<<12); // UART0 at bit12
   VIC_INTENABLE |= (1<<13); // UART2 at bit13 
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
   
   /* enable KBD IRQ */
   SIC_INTENABLE = (1<<3); // KBD int=bit3 on SIC
   SIC_ENSET = (1<<3);  // KBD int=3 on SIC
   *(kp->base+KCNTL) = 0x12;

   uart_init();
   kbd_init();

   pipe_init();
   kpipe = create_pipe();   

   init();

   kprintf("P0 kfork tasks: ");
   kfork((int)pipe_writer, 1);
   kfork((int)pipe_reader, 1);

   printList("readyQueue", readyQueue);

   unlock();
   while(1){
     if (readyQueue)
        tswitch();
   }
}
