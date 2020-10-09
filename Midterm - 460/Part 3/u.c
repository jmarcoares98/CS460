#include <stdio.h>
int pd[2];

int main()
{
  int r, pid;
  char line[128];
  
  r = pipe(pd);
  printf("pd[0]=%d pd[1]=%d\n", pd[0], pd[1]);

  pid = fork();
  if (pid){   // parent as writer
    close(pd[0]);
    while(1){
       printf("input a line to write: ");

       fgets(line, 128, stdin);
       line[strlen(line)-1] = 0;

       if (line[0] == 0){
	 printf("writer %d exit\n", getpid());
	 exit(0);
       }
       write(pd[1], line, strlen(line));
       sleep(1);
    }
  }
  else{      // child as reader
    close(pd[1]);
    while(1){
      printf("reader %d read from pipe\n", getpid());
       r = read(pd[0], line, 10);
       line[r] = 0;
       printf("%d read %d bytes = %s ", getpid(), r, line);
       if (r==0)
	 break;
    }
  }
}
