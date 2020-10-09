#define NPIPE 4
#define PSIZE 8

typedef struct pipe{
  char buf[PSIZE];
  int head, tail, data, room;
  int status;
}PIPE;

PIPE pipe[NPIPE];

extern UART uart[4];

int pipe_init()
{
  int i;
  printf("pipe_init()\n");
  for (i=0; i<NPIPE; i++){
    pipe[i].status = 0;
  }
}

PIPE *create_pipe()
{
  int i; PIPE *p;
  printf("creating pipe ");
  for (i=0; i<NPIPE; i++){
    p = &pipe[i];
    if (p->status==0){
      p->status = 1;
      break;
    }
  }
  p->head = p->tail = p->data = 0;
  p->room = PSIZE;
  printf("OK\n");
  return p;
}

int read_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;

  if (n==0) return 0;
  if (p->status==0) return -1;
  while(n){

    while(p->data){
      *buf = p->buf[p->tail++];
      p->tail %= PSIZE;
      p->data--; p->room++; buf++; r++; n--;
      if (n==0)
	break;
    }

    kwakeup((int)&p->room);
    if (r)
      return r;
    ksleep((int)&p->data);
  }
}

int write_pipe(PIPE *p, char *buf, int n)
{
  int r = 0; 
  
  if (p->status == 0) return -1;
  if (n<=0) return 0;
    
  while(n){
    while(p->room){
      p->buf[p->head++] = *buf;
      p->head %= PSIZE;
      p->data++; p->room--; buf++; r++; n--;
      if (n==0)
	break;
    }
    printf("proc%d wrote %d bytes\n", running->pid, r);
    kwakeup((int)&p->data);
    if (n==0){
      printf("proc%d finished writing %d bytes\n", running->pid, r);
      return r;
    }
    ksleep((int)&p->room);
  }
}


  
