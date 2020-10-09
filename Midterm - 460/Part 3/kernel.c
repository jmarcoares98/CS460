extern PROC *dequeue();

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];
  sleepList = 0;
  readyQueue = 0;

  running = dequeue(&freeList);
  running->priority = 0;
  running->status = READY;
}

int scheduler()
{
  if (running->status==READY)
     enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  color = RED + running->pid;
}  

int ksleep(int event)
{
  printf("proc%d sleep on event=%x\n", running->pid, event);  
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  tswitch();
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;
  while((p = dequeue(&sleepList))!=0){
    if (p->event==event){
      printf("proc%d wakeup %d\n", running->pid, p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else{
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
}

int kexit(int value)
{
  printf("%d in kexit\n", running->pid);
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);
  tswitch();
}

PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    kprintf("kfork failed\n");
    return (PROC *)0;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->parent = running;
  p->status = READY;
  p->priority = priority;
  
  for (i=1; i<15; i++)
    p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
 
  enqueue(&readyQueue, p);
  
  return p;
}
