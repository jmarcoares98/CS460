
#define kstrcmp strcmp
#define kstrcpy strcpy
#define kstrlen strlen
#define kstrcat strcat

int kstrcmp(const char *s1, const char *s2)
{
  while ((*s1 && *s2) && (*s1==*s2)){
    s1++; s2++;
  }
  if (*s1==0 && *s2==0)
    return 0;
  return *s1-*s2;
}


char *kstrcpy(char *dest, const char *src) {
  char *p = dest;
  while ( (*dest++ = *src++))
    ;
  *dest = 0;
  return p;
}

int kstrlen(const char *s) {
  unsigned int n = 0;
  while (*s++)
    ++n;
  return n;
}

int kstrcat(char *s1, char *s2)
{
  while(*s1)
    s1++;
  while(*s2)
    *s1++ = *s2++;
  *s1 = 0;
}

