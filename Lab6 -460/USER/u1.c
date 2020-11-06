#include "ucode.c"

int main(int argc, char *argv[])
{
  int i, pid, ppid, r;
  char line[64]; char uc;

  for (i=0; i<argc; i++){
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  
  while(1){
    pid  = getpid();
    ppid = getppid();
    printf("This is process %d in Umode at %x parent=%d\n", 
	   pid, getPA(), ppid);
 
    umenu();
    printf("input a command : ");
    ugets(line); 
    uprintf("\n"); 
 
    if (strcmp(line, "ps")==0)
      ups();
    if (strcmp(line, "switch")==0)
       uswitch();
    if (strcmp(line, "exit")==0)
       uexit();

    if (strcmp(line, "wait")==0)
       uwait();
    if (strcmp(line, "fork")==0)
       ufork();
    if (strcmp(line, "exec")==0)
       uexec();

  }
}
