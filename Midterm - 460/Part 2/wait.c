extern PROC* freeList, * readyQueue, * running, proc[NPROC];

/************ wait.c file ****************/
int ksleep(int event)
{
	// ksleep function
	int SR = int_off();	// disable IRQ and return CPSR
	running->event = event;
	running->status = SLEEP;
	tswitch();			// switch process
	int_on(SR);			// restore original CPSR
}

int kwakeup(int event)
{
	// kwakeup function
	PROC* p;
	int i, SR = int_off();	// disable IRQ and return CPSR

	for(i = 0; i < NPROC; i++) {
		p = &proc[i];
		if (p->status == SLEEP && p->event == event) {
			p->status = READY;
			enqueue(&readyQueue, p);
		}
	}
	int_on(SR);			// restore original CPSR
}

int kexit(int exitValue)
{
	// kexit function as in class notes
	PROC* p;
	int i, wakeupP1 = 0;

	kprintf("proc %d kexit, value = %d\n", running->pid, exitValue);
	if (running->pid == 1) {
		kprintf("other procs still exist, P! can't die yet\n");
		return -1;
	}
	
	//send children (dead or alive) to P1's orphanage
	for (i = 1; i < NPROC; i++) {
		p = &proc[i];

		if (p->status != FREE && p->ppid == running->pid) {
			p->ppid = 1;
			p->parent = &proc[1];
			wakeupP1++;
		}
	}

	running->exitCode = exitValue;	// record exitValue in PROC.exitCode for parent to get
	running->status = ZOMBIE;		// become a ZOMBIE (but do not free the PROC)
	running->priority = 0;

	// wake up parent and also P1 if necessary
	kwakeup(running->parent);

	if (wakeupP1)
		kwakeup(&proc[1]);

	tswitch();	// give up CPU
}

int kwait(int* status)
{
	// kwait function as in class notes
	PROC* p = running->child;

	if (!p) {
		kprintf("no children\n");
		return;
	}

	while (p) {								// search PROCs for a child
		if (p->status == ZOMBIE) {	// lay the dead child to rest
			*status = p->exitCode;	// collect its exitCode
			p->status = FREE;		// free its PROC
			enqueue(&freeList, p);	// to freeList

			return(p->pid);			// return its pid;
		}

		p = p->sibling;
	}

	kprintf("sleep code: %d", running);
	ksleep((int)running);					// still alive: sleep on PROC adress
}
