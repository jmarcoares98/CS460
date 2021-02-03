// test.c file

#include "ucode.c"

int main(int argc, char *argv[ ])
{
  int i, fd, n;
  char buf[1024];
  
  printf("KCW this is a test\n");

  printf("argc = %d\n", argc);
  for (i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);

  fd = open("/etc/passwd", O_RDONLY);
  printf("fd = %d\n", fd);

  n = read(fd, buf, 1024);
  buf[n] = 0;

  printf("buf = %s\n", buf);

  close(fd);
  
}
