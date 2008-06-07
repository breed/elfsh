/*
** unlink.c for unlinked
** 
** Started on  Sun Mar 10 01:18:51 2002 jfv
** Last update Sun Mar 10 02:47:22 2002 jfv
**
** $Id: ctors_hijack.c,v 1.3 2007-07-31 03:28:48 may Exp $
**
*/
#include	"libelfsh.h"


#define		TROJANED_FILE	"./a.out"
#define		OUTPUT_FILE	"./fake_aout"


int		main(int argc, char **argv)
{
  elfshobj_t	*file;
  elfsh_Sym	*new_ctors;
  int		ret;


  file = elfsh_map_obj(TROJANED_FILE);
  new_ctors = elfsh_get_symbol_by_name(file, "new_ctors");
  if (!file || !new_ctors)
    {
      elfsh_error();
      exit(-1);
    }


  /***********************************************************/
  /**************** TEST FOR CTORS HIJACKING *****************/
  /***********************************************************/

  //ret = elfsh_set_ctors_entry_by_index(file, 1, new_ctors->st_value);
  ret = elfsh_set_ctors_entry_by_name(file, "my_ctors", new_ctors->st_value);
  if (ret < 0)
    {
      elfsh_error();
      exit(-1);
    }


  /***********************************************************/
  /************************ RELINKING ************************/
  /***********************************************************/

  ret = elfsh_save_obj(file, OUTPUT_FILE);
  if (ret < 0)
    elfsh_error();

  printf("Relinking *%s* \n", ret ? "Error" : "OK");
  return (0);
}

