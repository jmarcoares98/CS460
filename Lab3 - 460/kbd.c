#include "keymap"
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
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

#define KSTAT 0x04
#define KDATA 0x08

int shift;
int ctrl;
int release;

int kbd_init()
{
  char scode;
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  shift = ctrl = release = 0;
}

// kbd_handler1() for scan code set 1
void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;
  color = RED;
  scode = *(kp->base + KDATA);

  //printf("scan code = %x ", scode);
  
  // this is where key release
  if (scode == 0xF0) { 
      release = 1;
      return;
  }

  if (release && scode) {
      release = 0;
      if (scode == LSHIFT)
          shift = 0;
      else if (scode == LCTRL)
          ctrl = 0;
      return;
  }

  // shift key
  if (scode == LSHIFT) {
      shift = 1;
      return;
  }

  // ctrl key
  if (scode == LCTRL) {
      ctrl = 1;
      return;
  }

  if (!shift)
      c = ltab[scode];
  else
      c = utab[scode];

  // ctrl + c
  if (ctrl && (scode == 0x21)) {
      kprintf("Control-C key\n");
      return;
  }
  
  // ctrl + d
  if (ctrl && (scode == 0x23)) {
      kprintf("Control-D key \n");
      c = 0x04;
  }


  kprintf("kbd interrupt: c=%x %c\n", c, c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();
  while(kp->data == 0);

  lock();
  c = kp->buf[kp->tail++];
  kp->tail %= 128;
  kp->data--; kp->room++;
  unlock();
  return c;
}

int kgets(char s[])
{
    char c;
    while ((c = kgetc()) != '\r')
        *s++ = c;

    *s = 0;
    return strlen(s);
}