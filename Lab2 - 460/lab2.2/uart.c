/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

/** WRITE YOUR uprintf(UART *up, char *fmt, . . .) for formatted print **/
int urpu(UART* up, int x)
{
    char c;
    if (x) {
        c = tab[x % 10];
        urpu(up, x / 10);
        uputc(up, c);
    }
}

/*Prints unsigned integers*/
int uprintu(UART* up, int x)
{
    (x == 0) ? uputc(up, '0') : urpu(up, x);
    uputc(up, ' ');
}

/*Prints an integer (Can be negative)*/
int uprintd(UART* up, int x)
{
    if (x < 0) {
        uputc(up, '-');
        x *= -1;
    }
    uprintu(up, x);
}

int urpx(UART* up, int x)
{
    char c;
    if (x) {
        c = tab[x % 16];
        urpu(up, x / 16);
        uputc(up, c);
    }
}

/*Prints x in HEX (Starts with 0x)*/
int uprintx(UART* up, int x)
{
    uputc(up, '0');
    uputc(up, 'x');
    (x == 0) ? uputc(up, '0') : urpx(up, x);
}

void ufprintf(UART* up, char* fmt, ...)
{
    char* cp = fmt;
    int* ip = (int*)&fmt + 1;

    while (*cp) {
        if (*cp != '%')
        {
            uputc(up, *cp);
            if (*cp == '\n')
                uputc(up, '\r');
        }
        else {
            cp++;
            switch (*cp) {
            case 'c': uputc(up, (char)*ip);
                break;
            case 's': uprints(up, (char*)*ip);
                break;
            case 'u': uprintu(up, *ip);
                break;
            case 'd': uprintd(up, *ip);
                break;
            case 'x': uprintx(up, *ip);
                break;
            }
            ip++;
        }
        cp++;
    }
}