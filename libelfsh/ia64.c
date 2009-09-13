/**
* @file libelfsh/ia64.c
 * @ingroup libelfsh
** ia64.c for libelfsh
** 
** Started on  Sat Feb 26 23:25:37 2005 jfv
** 
** Skeleton cut & pasted from the sparc64 backend
** 
**
** $Id$
**
*/
#include "libelfsh.h"


/**
 * Static hooking for IA64 
 * @param null
 * @param sname
 * @param null2
 * @param null3
 * @return
 */
int	elfsh_cflow_ia64(elfshobj_t     *null,
			 char		*sname,
			 elfsh_Sym	*null2,
			 eresi_Addr	null3)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}


/**
 * PLT hijacking on IA64 
 *
 * @param file
 * @param symbol
 * @param addr
 * @return
 */
int		elfsh_hijack_plt_ia64(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * ALTPLT hijacking on IA64 
 *
 * @param file
 * @param symbol
 * @param addr
 * @return
 */
int		elfsh_hijack_altplt_ia64(elfshobj_t *file, 
					 elfsh_Sym *symbol,
					 eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/**
 * Perform relocation on entry for IA64 architecture 
 *
 * @param new
 * @param cur
 * @param dword
 * @param addr
 * @param mod
 * @return
 */
int       elfsh_relocate_ia64(elfshsect_t       *new,
			      elfsh_Rela        *cur,
			      eresi_Addr        *dword,
			      eresi_Addr        addr,
			      elfshsect_t	*mod)
{
  int	  retval;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  retval = 0;
  switch (elfsh_get_reltype((elfsh_Rel *)cur))
    {
      /* cases ... */

    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unsupported relocation type",
		     -1);
      break;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (retval));
}


