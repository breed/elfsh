/**
* @file libasm/tools/modrm.c
 * $Id$
 * @brief Little utility to display modrm byte to debug ia32.
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct s_modrm 
{
  u_char        m:3;
  u_char        r:3;
  u_char        mod:2;
} asm_modrm;

/**
 * Will display content of 0xe0 for the modrm byte:
 * Main usage : modrm 0xe2
 * $ ./modrm e2
 * op = 3 r = 4, m = 2
 *
 */
int     main(int ac, char **av)
{
  struct s_modrm        val;
  char  hex;
  if (ac < 2)
    {
      printf("Usage: %s [hex modrmbyte]\n", av[0]);
      exit(-1);
    }
  hex = strtoul(av[1], 0, 16);
  memcpy(&val, &hex, 1);
  printf("op = %i r = %i, m = %i\n", 
         val.mod,
         val.r,
         val.m);
  return (0);
}
