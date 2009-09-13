/**
* @file libstderesi/files/reladd.c
 * @ingroup files
 * Started on  Fri Mar 28 14:58:57 2003 jfv
 * Last update Thu Mar 11 14:39:48 2004 jfv
 *
 * $Id$
 *
 */
#include "libstderesi.h"


/**
 * Inject a .o into an executable 
 */
int		cmd_relinject()
{
  elfshobj_t	*host;
  elfshobj_t	*rel;
  int		idx;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check to avoid non-runtime (static) injection from e2bdg the debugger 
     that would desynchronize the memory perspective of the program. 
     The debugger is not supposed to do that, it is a job for elfsh */
  if (world.state.revm_mode == REVM_STATE_EMBEDDED &&
      elfsh_is_static_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "E2dbg must inject in memory, switch to dynamic mode.", -1);
  
  /* Load host file */
  idx = atoi(world.curjob->curcmd->param[0]);
  host = (idx ? revm_getfile(idx) : 
	  hash_get(&file_hash, world.curjob->curcmd->param[0]));
  if (host == NULL)
    {
      host = elfsh_map_obj(world.curjob->curcmd->param[0]);
      if (host == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot map host file", -1);
    }

  /* Load relocatable file */
  idx = atoi(world.curjob->curcmd->param[1]);
  rel = (idx > 0 ? revm_getfile(idx) : 
	 hash_get(&file_hash, world.curjob->curcmd->param[1]));
  if (rel == NULL)
    {
      rel = elfsh_map_obj(world.curjob->curcmd->param[1]);
      if (rel == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot map relocatable file", -1);
    }

  /* Call libelfsh relocatable object injector */
  idx = elfsh_inject_etrel_hash(host, rel, 
				&world.curjob->loaded, 
				&world.shared_hash);
				
  if (idx < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to inject ET_REL with workspace", (-1));

  /* Success : put the modified object as current */
  world.curjob->curfile = host;
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [*] ET_REL %s injected succesfully in %s %s\n\n", 
	       rel->name, 
	       (host->hdr->e_type == ET_EXEC ? "ET_EXEC" : 
		host->hdr->e_type == ET_DYN  ? "ET_DYN"  : 
		"unknown host file"),
		host->name);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
