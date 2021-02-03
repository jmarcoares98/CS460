int bcopy(const void *src, void *dest, unsigned int n) {
	const char *s = (const char *)src;
	char *d = (char *)dest;
	if (s <= d)
		for (; n>0; --n)
			d[n-1] = s[n-1];
	else
		for (; n>0; --n)
			*d++ = *s++;
}


int bzero(void *dest, unsigned int n) {
	memset(dest, 0, n);
}


char *memcpy(void *dest, const void *src, unsigned int n) {
	bcopy(src, dest, n);
	return dest;
}


char *memset(void *dest, int c, unsigned int n) {
	char *d = (char *)dest;
	for (; n>0; --n)
		*d++ = (char)c;
	return dest;
}


int memcmp(const void *s1, const void *s2, unsigned int n) {
	const char *s3 = (const char *)s1;
	const char *s4 = (const char *)s2;
	for (; n>0; --n) {
		if (*s3 > *s4)
			return 1;
		else if (*s3 < *s4)
			return -1;
		++s3;
		++s4;
	}
	return 0;
}

int strcmp(const char *s1, const char *s2)
{
  while ((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if (*s1==0 && *s2==0)
    return 0;
  return *s1-*s2;
}

char *strcpy(char *dest, const char *src) {
	char *p = dest;
	while ( (*dest++ = *src++))
		;
	*dest = 0;
	return p;
}


int strlen(const char *s) {
	unsigned int n = 0;
	while (*s++)
		++n;
	return n;
}

int strcat(char *s1, char *s2)
{
  while(*s1)
    s1++;
  while(*s2)
    *s1++ = *s2++;
  *s1 = 0;
}

int strncpy(char *s1, char *s2, int n)
{
  char *p = s1;
  while(n && *s2){
    *s1++ = *s2++;
    n--;
  }
  *s1 = 0;
  return (int)p;
}

int strncmp(char *s1, char *s2, int n)
{
  if (n==0) return 0;
  do{
    if (*s1 != *s2++)
      return *s1-*(s2-1);
    if (*s1++ == 0)
      break;
  } while(--n != 0);
  return 0;
}

char *strstr(char *s1, char *s2)
{
  /* s1="....abc...", s2="abc" ==> find first occurrence of "abc" */

  int i, len;
  len = strlen(s2);

  for (i=0; i<strlen(s1)-strlen(s2); i++){
    if (strncmp(&s1[i], s2, len)==0)
         return &s1[i];
  }
  return 0;
}


int setzero(char *dst, int size)
{
  int i;
  for (i=0; i<size; i++)
    *dst++ = 0;
}
void delay()
{
   int i; for (i=0; i<10000; i++);
}
 
int copy(char *dest, char *src)
{
  int i;
  for (i=0; i<1024; i++)
    *dest++ = *src++;
}

int atoi(char *s)
{
  int v = 0;
  while(*s){
    v = 10*v + (*s - '0');
    s++;
  }
  return v;
}

