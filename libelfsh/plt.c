/**
* @file libelfsh/plt.c
 * @ingroup libelfsh
** plt.c for libelfsh
** 
** Started on  Tue Jul 24 06:40:07 2001 jfv
** 
**
** $Id$
**
*/
#include "libelfsh.h"


/**
 * Load the Procedure Linkage Table 
 * @param file
 * @param num
 * @return
 */
elfshsect_t	*elfsh_get_plt(elfshobj_t *file, int *num)
{
  elfshsect_t	*actual;
  void		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  for (actual = file->sectlist; actual != NULL; actual = actual->next)
    if (elfsh_is_plt(file, actual))
      {
	ret = elfsh_get_anonymous_section(file, actual);
	if (!ret)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unable to get PLT", NULL);
	file->secthash[ELFSH_SECTION_PLT] = actual;
      }
    else if (elfsh_is_altplt(file, actual))
      {
	ret = elfsh_get_anonymous_section(file, actual);
	if (!ret)
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unable to get ALTPLT", NULL);
	file->secthash[ELFSH_SECTION_ALTPLT] = actual;
      }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
	                 (file->secthash[ELFSH_SECTION_PLT]));
}


/**
 * Return 1 if the section is the plt or 0 if its not 
 * @param file
 * @param sct
 * @return
 */
int		elfsh_is_plt(elfshobj_t *file, elfshsect_t *sct)
{
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  name = elfsh_get_section_name(file, sct);
  if (name != NULL && !strcmp(name, ELFSH_SECTION_NAME_PLT))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return 1 if the section is the plt or 0 if its not 
 * @param file
 * @param sct
 * @return
 */
int		elfsh_is_altplt(elfshobj_t *file, elfshsect_t *sct)
{
  char		*name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  name = elfsh_get_section_name(file, sct);
  if (name != NULL && !strcmp(name, ELFSH_SECTION_NAME_ALTPLT))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return 1 if the symbol points in the PLT, 0 if not, -1 if the PLT cant be found 
 * @param file
 * @param sym
 * @return
 */
int			elfsh_is_pltentry(elfshobj_t *file, elfsh_Sym *sym)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sym == NULL || elfsh_get_plt(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (sym->st_size != file->secthash[ELFSH_SECTION_PLT]->shdr->sh_size)
  {
    if (INTERVAL(file->secthash[ELFSH_SECTION_PLT]->shdr->sh_addr, 
                 sym->st_value, 
                 file->secthash[ELFSH_SECTION_PLT]->shdr->sh_addr + 
                 file->secthash[ELFSH_SECTION_PLT]->shdr->sh_size))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the size of 1 PLT entry 
 * @param file
 * @return
 */
int		elfsh_get_pltentsz(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (12));
    case EM_SPARCV9:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (32));
    case EM_ALPHA:
#if (EM_ALPHA != EM_ALPHA_EXP)
    case EM_ALPHA_EXP:
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (12));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", -1);
    }
}


/**
 * Return the size of the FIRST PLT entry on this arch
 * Its a different size on Alpha and Sparc 
 * @param file
 * @return
 */
int		 elfsh_get_first_pltentsz(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (48));
    case EM_SPARCV9:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (124));
    case EM_ALPHA:
#if (EM_ALPHA != EM_ALPHA_EXP)
    case EM_ALPHA_EXP:
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (32));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", -1);
    }
}














