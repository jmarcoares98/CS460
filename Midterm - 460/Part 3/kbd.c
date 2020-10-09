#include "keymap2"

#define LSHIFT 0x12
#define RSHIFT 0x59
#define ENTER  0x5A
#define LCTRL  0x14
#define RCTRL  0xE014

#define ESC    0x76
#define F1     0x05
#define F2     0x06
#define F3     0x04
#define F4     0x0C

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail;
  struct semaphore data;  // data is a sempahore
}KBD;

volatile KBD kbd;
int release;

int kbd_init()
{
  char scode;
  
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x11; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data.value = 0; kp->data.queue = 0;  
  release = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);

  if (scode == 0xF0){ // key release 
    release = 1;
    return;
  }
  if (release){
    release = 0;
    return;
  }
  
  c = ltab[scode];

  if (c!='\r')
     printf("%c", c);
  else
    printf("\n");
  
  kp->buf[kp->head++] = c;
  kp->head %= 128;

  V(&kp->data);
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;
  
  P(&kp->data);
  c = kp->buf[kp->tail++];
  kp->tail %= 128;
  
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    if (c=='\b'){
      s--;
      continue;
    }
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}
