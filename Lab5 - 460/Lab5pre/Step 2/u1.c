typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#include "string.c"
#include "uio.c"
#include "ucode.c"

int main()
{
  int pid, ppid;
  char line[64];
  u32 mode,  *up;

  mode = getcpsr();
  mode = mode & 0x1F;
  printf("CPU mode=%x\n", mode);  // verify we are in USER mode

  pid = getpid();
  ppid = getppid();
  
  while(1){
    printf("This is process %d in Umode parent=%d\n", pid, ppid);
    umenu();
    printf("input a command : ");
    ugets(line); 
    printf("\n"); 
 
    if (strcmp(line, "getpid")==0)
       ugetpid();
    if (strcmp(line, "getppid")==0)
       ugetppid();
    if (strcmp(line, "ps")==0)
       ups();
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "switch")==0)
       uswitch();
  }
}

