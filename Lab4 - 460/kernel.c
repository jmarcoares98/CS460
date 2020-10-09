int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->priority = 0;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;

  // creat P0 as initial process
  p = get_proc(&freeList);

  p->priority = 0;      // P0 priority = 0
  p->ppid = 0;
  p->child = 0;
  p->sibling = 0;
  p->parent = p;        // P0 parent = P0
  p->status = READY;
  
  running = p;
  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

int printSleepList(char* listname, PROC* list)
{
    // print link list as      listname = ...... NULL
    kprintf("%s = ", listname);
    while (list) {
        kprintf("[%d%d]->", list->pid, list->exitCode);
        list = list->next;
    }
    kprintf("NULL\n");
}

//void kexit()
//{
//  printf("proc %d kexit\n", running->pid);
//  running->status = FREE;
//  running->priority = 0;
//  put_proc(&freeList, running);
//  tswitch();
//}
  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = get_proc(&freeList);;
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }

  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;
  p->sibling = running->sibling;
  running->child = p;
  
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);

  enqueue(&readyQueue, p);
  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}  

int body()
{
  char cmd[64], line[64];
  int i, pid, event, status = 0, waitID, exitValue;
  PROC* p;

  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
     pid = running->pid;
     if (pid==0) color=BLUE;
     if (pid==1) color=GREEN;
     if (pid==2) color=CYAN;
     if (pid==3) color=YELLOW;
     if (pid==4) color=RED;
     if (pid==5) color=GREEN;
     if (pid==6) color=WHITE;   
     if (pid==7) color=YELLOW;
     if (pid==8) color=CYAN;

     printList("freeList  ", freeList);
     printList("readyQueue", readyQueue);
     
     kprintf("proc %d running, command = [ps|switch|kfork|ksleep|kwakeup|kwait|exit|] : ", running->pid);
     kgets(cmd);
     printf("\n");

     if (strcmp(cmd, "switch")==0)
        tswitch();
     if (strcmp(cmd, "kfork")==0)
        kfork((int)body, 1);
     if (strcmp(cmd, "exit") == 0)
         kexit(running->pid);
     if (strcmp(cmd, "ps") == 0) {
         kprintf("PID   PPID    status\n");
         kprintf("===   ====    ======\n");
         for (i = 0; i < NPROC; i++) {
             p = &proc[i];
             kprintf(" %d    %d      ", p->pid, p->ppid);
             if (p == running)
                 kprintf("RUNNING\n");
             else if (p->status == 0)
                 kprintf("FREE\n");
             else if (p->status == 1)
                 kprintf("READY\n");
             else if (p->status == 2)
                 kprintf("SLEEP\n");
             else if (p->status == 3)
                 kprintf("BLOCK\n");
             else if (p->status == 4)
                 kprintf("ZOMBIE\n");
         }
     }
     if (strcmp(cmd, "kwait") == 0) {
         waitID = kwait(&status);
     }
     if (strcmp(cmd, "ksleep") == 0) {
         kprintf("enter event value: ");
         kgets(line);
         event = atoi(line);
         ksleep(event);
     }
     if (strcmp(cmd, "kwakeup") == 0) {
         kprintf("enter event value: ");
         kgets(line);
         event = atoi(line);
         kwakeup(event);
     }
         
   }
}