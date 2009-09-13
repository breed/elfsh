/**
* @file libstderesi/files/switch.c
** @ingroup files
**
** Started on  Sat Jan 25 11:20:49 2003 jfv
** $Id$
*/
#include "libstderesi.h"


/** 
 * Change the current object 
 */
int             cmd_doswitch()
{
  char          logbuf[BUFSIZ];
  int           ret;
  int           nbr;
  elfshobj_t    *cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (world.curjob->curcmd->param[0] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid NULL parameter", -1);
  nbr = atoi(world.curjob->curcmd->param[0]);
  ret = revm_doswitch(nbr);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to switch on object", -1);
  cur = world.curjob->curfile;
  mjr_set_current_context(&world.mjr_session, cur->name);
  snprintf(logbuf, BUFSIZ, "\n [*] Switched on object %u (%s) \n\n", cur->id, cur->name);
  revm_output(logbuf);
  if (elfsh_is_runtime_mode() && !cur->linkmap)
    revm_output("\n [!] Loaded file is not the linkmap, switching to STATIC mode\n\n");
  revm_proc_init();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}
