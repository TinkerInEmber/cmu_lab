#include<stdio.h>
int main()
{
  char buf[20] = {0};
  int val = 14;
  sprintf(buf, "%.8x", val);
  printf("%x",val);
  for(int i = 0; i< 20; ++i)
  {
    printf("%x ",buf[i]);
  }
  putchar('\n');
  return 0;
}
