/**
* @file libe2dbg/common/backtrace.c
** @brief Implement the backtrace functionality.
** @ingroup common
**    
** Started on  Tue Aug 16 09:38:03 2005 jfv
** $Id$
*/
#include "libe2dbg.h"


/* Display backtrace with sym resolution */
int		e2dbg_bt()
{
  e2dbgthread_t	*t;
  eresi_Addr	frame;
  eresi_Addr	ret;
  char		*name, *name2;
  elfsh_SAddr	off, off2;
  char		logbuf[BUFSIZ];
  int		i = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!world.curjob)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No current workspace", -1);
  if (!world.curjob->curfile)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No current file", -1);

  /* Get the current frame by calling the hook */
  frame = (eresi_Addr)(uintptr_t)e2dbg_getfp();
  if (!frame)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "No context", (-1));

  /* Backtrace frames */
  while (frame && frame != 0xFFFFFFFF)
    {
      /* Check if the next frame is pointing on our stack */
      if (e2dbgworld.curthread->stackaddr > frame ||
	  e2dbgworld.curthread->stackaddr + e2dbgworld.curthread->stacksize < frame)
	{
	  snprintf(logbuf, BUFSIZ, 
		   " [*] Invalid next frame address " AFMT 
		   " (stackaddr = " AFMT ", size = %u, max = " AFMT " )\n\n", 
		   frame, e2dbgworld.curthread->stackaddr, 
		   e2dbgworld.curthread->stacksize,
		   e2dbgworld.curthread->stackaddr + e2dbgworld.curthread->stacksize);
	  e2dbg_output(logbuf);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      /* Call the getret hook and check its result. */
      ret = (eresi_Addr) e2dbg_getret(world.curjob->curfile, (eresi_Addr) frame);
      if (!ret || ret == 0xffffffff)
	break;

      /* Resolve and print current trace frame */
      if (i == 0)
	ret = *(e2dbg_getpc());

      name = revm_resolve(world.curjob->curfile, (eresi_Addr) ret, &off);
      if (!name)
	name = "?";

      /* Just insert the real entry point where we reach the thread
	 entry point of e2dbg */
      if (strstr(name, "e2dbg_thread_start"))
	{
	  snprintf(logbuf, BUFSIZ - 1, "%u", 
		   (unsigned int) e2dbgworld.stoppedthread->tid);
	  t = hash_get(&e2dbgworld.threads, logbuf);
	  name2 = revm_resolve(world.curjob->curfile,
			       (eresi_Addr)(uintptr_t) t->entry, &off2);
	  if (name2)
	    {
	      if (off2)
		snprintf(logbuf, BUFSIZ - 1, 
			 " [%02d] "XFMT" in "XFMT" <%s + " UFMT "> -ENTRY-\n", i, 
			 ret, (eresi_Addr)(uintptr_t) t->entry, name2, off2);
	      else
		snprintf(logbuf, BUFSIZ - 1, 
			 " [%02d] "XFMT" in "XFMT " <%s> -ENTRY-\n", i, 
			 ret, (eresi_Addr)(uintptr_t) t->entry, name2);
	      e2dbg_output(logbuf);
	      i++;
	    }
	}

      /* Filter the name in case we have a known-pattern address */
      if (ret == E2DBG_VSYSCALL_RETADDR)
	name = "KERNEL VSYSCALL PAGE : one function missing";
      else if (ret == E2DBG_SIGTRAMP_RETADDR)
	name = "KERNEL SIGNAL TRAMPOLINE";
      
      /* Print the current level frame */
      if (off)
	snprintf(logbuf, BUFSIZ - 1, " [%02d] " XFMT " in " XFMT
		 " <%s + " UFMT ">\n", i, (eresi_Addr) ret, ret - off, name, off);
		 
      else
	snprintf(logbuf, BUFSIZ - 1, " [%02d] "XFMT" in "XFMT" <%s>\n", i, 
		 (eresi_Addr) ret, ret, name);
      e2dbg_output(logbuf);

      /* Call the nextfp hook */
      frame = e2dbg_nextfp(world.curjob->curfile, (eresi_Addr) frame);    
      i++;
    }
  
  e2dbg_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Backtrace command */
int		cmd_bt()
{
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!elfsh_is_runtime_mode())
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Not in dynamic or debugger mode", -1);
  if (e2dbgworld.curthread == NULL || e2dbgworld.curthread->context == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No current thread context available", -1);    
  e2dbg_output(" .:: Backtrace ::. \n");
  ret = e2dbg_bt();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

