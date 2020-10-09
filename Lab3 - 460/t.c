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

#include "defines.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int color;

#include "timer.c"
#include "vid.c"
#include "interrupts.c"
#include "kbd.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) IRQ_handler()
// timer0 base=0x101E2000; timer1 base=0x101E2020
// timer3 base=0x101E3000; timer1 base=0x101E3020
// currentValueReg=0x04
TIMER *tp;

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
         blink_cursor();
    }    
    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

void blink_cursor()
{
    char c;
    color = RED;

    if (tp->ss %= 30)
        putcursor(127);
    else
        clrcursor();

    return;
}

int main()
{
   int c;
   char line[128];
   tp = &timer[0];

   row = col = 0; 
   fbuf_init();
   kbd_init();
   
   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1 << 4);  // timer0,1 at bit4 
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
 
   kprintf("kbd start: test timer KBD drivers by interrupts\n");
   timer_init();
   timer_start(0);


   while(1){
      color = CYAN;
      kprintf("Enter a key from KBD\n");
      kgets(line);
      color = CYAN;
      kprintf("line = %s\n", line);
   }
}
