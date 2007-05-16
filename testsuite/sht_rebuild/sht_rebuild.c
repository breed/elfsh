/*
** sht_extend.c
** 
** Started on  Sun Mar 17 05:57:23 2002 mayhem
** Last update Fri Jan 24 12:56:13 2003 mayhem
**
** $Id: sht_rebuild.c,v 1.3 2007-05-16 23:36:38 may Exp $
**
*/
#include "libelfsh.h"
#include <stdio.h>

#define         STRIPPED_FILE   "./sht_stripped_file"
#define         OUTPUT_FILE     "./sht_rebuilt_file"

/* We just map and save the object since ELFsh automatically rebuid SHT */
int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Shdr	hdr;
  elfsh_Shdr	unmapped_hdr;
  elfshsect_t	*new;
  elfshsect_t	*unmapped_new;
  int		ret;

  /* map the object */
  file = elfsh_map_obj(STRIPPED_FILE);
  if (!file)
    {
      elfsh_error();
      exit(-1);
    }

  /* Get (and reconstruct if unexistant) SHT and SYMTAB */
  elfsh_get_sht(file, NULL);
  elfsh_get_symtab(file, NULL);

  /* Save the object */
  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);

}







