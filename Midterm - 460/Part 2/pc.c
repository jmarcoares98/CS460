/************ producer and consumer problem ****************/
#define	BUFSIZE	8

char buf[BUFSIZE];			// buffer cells
int head, tail;		// index to buffer cells
SEMAPHORE empty, full, pmutex, cmutex;

int pc_init()
{
	head = tail = 0;
	empty.value = BUFSIZE;
	full.value = 0;
	pmutex.value = 1;
	cmutex.value = 1;
}

int P(SEMAPHORE* s)
{
	int SR = int_off();
	s->value--;
	if (s->value < 0) {
		block(s);
	}

	s->lock = 0;
	int_on(SR);
}

int V(SEMAPHORE* s)
{
	int SR = int_off();
	s->value++;

	if (s->value <= 0)
		signal(s);

	s->lock = 0;
	int_on(SR);
}

int block(SEMAPHORE* s)
{
	int SR = int_off();
	running->status = BLOCK;
	enqueue(&s->queue, running);
	tswitch();
	int_on(SR);
}

int signal(SEMAPHORE* s)
{
	PROC* p;
	p = dequeue(&s->queue);
	p->status = READY;
	enqueue(&readyQueue, p);
}

int producer()
{
	char item, line[32];
	int i = 0;
	kprintf("produce item: ");
	kgets(line);
	kprintf("\n");

	while (1) {
		item = line[i++];
		P(&empty);
		P(&pmutex);
		buf[head++] = item;
		head %= BUFSIZE;
		V(&pmutex);
		V(&full);
	}
	kprintf("\n");
}

int consumer()
{
	char item;
	while (1) {
		P(&full);
		P(&cmutex);
		item = buf[tail++];
		kprintf("%c",item);
		tail %= BUFSIZE;
		V(&cmutex);
		V(&empty);
	}
}