/*
** unlink.c for unlinked
** 
** Started on  Sun Mar 10 01:18:51 2002 jfv
** Last update Sun Mar 10 02:56:20 2002 jfv
**
** $Id$
**
*/
#include	"libelfsh.h"

#if ERESI32
 #define		TROJANED_FILE	"./hijackme32"
 #define		OUTPUT_FILE	"./fake_aout32"
#elif ERESI64
 #define		TROJANED_FILE	"./hijackme64"
 #define		OUTPUT_FILE	"./fake_aout64"
#endif


int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Sym	*new_dtors;
  int		ret;

  file = elfsh_map_obj(TROJANED_FILE);
  new_dtors = elfsh_get_symbol_by_name(file, "new_dtors");

  if (!file || !new_dtors)
    {
      elfsh_error();
      exit(-1);
    }


  /***********************************************************/
  /**************** TEST FOR DTORS HIJACKING *****************/
  /***********************************************************/


  //ret = elfsh_set_dtors_entry_by_index(file, 1, new_dtors->st_value);
  ret = elfsh_set_dtors_entry_by_name(file, "my_dtors", new_dtors->st_value);
  if (ret < 0)
    {
      elfsh_error();
      exit(-1);
    }


  /* RELINKING THE FILE */

  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);
}

