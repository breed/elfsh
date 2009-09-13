/**
* @file librevm/vm/fini.c
 * @ingroup vm
 * @brief Routines executed when leaving script mode.
 *
 * Started on  Wed Nov 19 23:02:04 2003 jfv
 * $Id$
 */
#include "revm.h"


/** Clean scripting context */
void		revm_clean()
{
  world.curjob->recur[world.curjob->curscope].script = NULL;
  world.curjob->recur[world.curjob->curscope].lstcmd = NULL;
}

/* Some REVM context cleanup when coming back from script to interactive mode */
void		revm_cleanup()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_clean();
  revm_conditional_rlquit();
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Mostly for some post-execution code inside etrace */
void		revm_postexec(int retval)
{
  int		ret;

  if (!world.state.revm_quiet && world.state.revm_mode == REVM_STATE_SCRIPT)
    {
      if (retval < 0)
	revm_output("\n [E] Script execution failed \n\n");
      else
	revm_output("\n [*] Script execution ended succesfully \n\n");
    }

  /* Implicit unload or save if we are not in interactive mode or in tracer mode */
  if (world.state.revm_mode == REVM_STATE_CMDLINE && world.curjob->curfile)
    ret = revm_workfiles_unload();
}
