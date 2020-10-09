/*********  t.c file *********************/

// call putc(c) to print string s;
int prints(char *s)
{
    while (*s){
        putc(*s);
        s++;
    }
}

// call getc() to input a string into s[ ]
int gets(char *s)
{
    while ((*s = getc()) != '\r') {
        putc(*s);
        s++;
    }
    *s = 0;
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  
