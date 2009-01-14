/*
** section_injection.c
** 
** Started on  Sun Mar 17 05:57:23 2002 jfv
** Last update Mon May 19 13:12:23 2003 jfv
**
** $Id$
**
*/
#include "libelfsh.h"

#if ERESI32
 #define		TROJANED_FILE	"./hijackme32"
 #define		OUTPUT_FILE	"./fake_aout32"
#elif ERESI64
 #define		TROJANED_FILE	"./hijackme64"
 #define		OUTPUT_FILE	"./fake_aout64"
#endif


/* test for non-mapped data */
char	*str = "stop looking at me ! \n";

/* Here we map the shellcode in a newly inserted mapped section (.devhell) and modify the entry point */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Shdr	hdr;
  elfsh_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;

  file = elfsh_map_obj(TROJANED_FILE);
  if (!file)
    {
      elfsh_error();
      exit(-1);
    }

  printf(" => Injection section and adding section header table entry ... \n");

  /* Create a new unmapped section */
  unmapped_new = elfsh_create_section(".dhstrtab");
  if (!unmapped_new)
    {
      elfsh_error();
      exit(-1);
    }

  /* Create a new section header */
  unmapped_hdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, strlen(str), 0, 0, 0, 0);
  if (elfsh_insert_unmapped_section(file, unmapped_new, unmapped_hdr, str) < 0)
    {
      elfsh_error();
      exit(-1);
    }

  /* Finally save the object */
  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", (ret  < 0 ? "Error" : "OK"));
  return (ret);
}







