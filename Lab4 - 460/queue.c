/************************  queue.c file ************************
1. Write your C code for the functions specified below
2. Compile queue.c into .o file by
      arm-none-eabi-gcc -c -mcpu=arm926ej-s queue.c -o queue.o
3. Use it in the LINK step by
      arm-none-eabi-ld -T t.ld ts.o t.o   queue.o   -o t.elf
****************************************************************/

//#include "type.h"
extern PROC *freeList, *readyQueue, *running;

// write YOUR C cdoe for the following functions

PROC *get_proc(PROC **list)  // called as get_proc(&freeList)s
{
    // return a free PROC from freeList
    PROC* p = *list;
    *list = p->next;
    return p;
}
   
int put_proc(PROC **list, PROC *p)   // called as put_proc(&freeList, p)
{
    // enter PROC p to end of freeList
    p->next = *list;
    *list = p;
}

int enqueue(PROC **queue, PROC *p)   // called as enqueue(&readyQueue, p)
{
    // enter p into readyQueue by priority; FIFO if same priority
    PROC* q = *queue;   // variable for queue

    if (q == 0) {       // if the queue is empty
        *queue = p;
        p->next = 0;
        return;
    }
    if (q->priority < p->priority) {    // if the new process has greater priority than queue
        p->next = *queue;
        *queue = p;
        return;
    }

    while (q->next->priority >= p->priority)
        q = q->next;    // adjust by priority

    p->next = q->next;
    q->next = p;
}

PROC *dequeue(PROC **queue)         // called as dequeue(&readyQueue)
{
    // remove and return FIRST entry from queue
    PROC* p = *queue;
    if (p)
        *queue = p->next;

    return p;
}

int printList(char *listname, PROC *list)
{
    // print link list as      listname = ...... NULL
    kprintf("%s = ", listname);
    while(list){
        kprintf("[%d%d]->", list->pid, list->priority);
        list = list->next;
    }
    kprintf("NULL\n");
}

