/*
** $Id$
**
*/
#include <stdio.h>

int	myputs(char *str)
{
  printf("Hijacked puts !!! arg = %s \n", str);
  putchar('e');
  putchar('x');
  putchar('t');
  putchar('c');
  putchar('a');
  putchar('l');
  putchar('l');
  putchar('!');
  putchar('\n');
  return (old_puts(str));
}

int	new_legit_func(char *str)
{
  printf("Hijacked legit_func !!! (param = %s) \n", str);
  return (old_legit_func(str));
}
