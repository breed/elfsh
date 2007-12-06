/*
 * @file printing.c
 * 
 * This file contains various printing facilities API
 *
 * Started on  Wed Dec 05 23:02:04 2003 jfv
 * $Id: printing.c,v 1.1 2007-12-06 05:11:58 may Exp $
 */
#include "revm.h"



/** 
 * Debug purpose 
 */
int		revm_printscript(revmargv_t *start)
{
  revmargv_t	*list;
  u_int		index;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("  .:: Printing Script: \n");

  if (start)
    list = start;
  else
    list = world.curjob->script[world.curjob->sourced];

  for (index = 0; list; list = list->next, index++)
    {
      snprintf(logbuf, BUFSIZ - 1, "[%03u] ~%s %p \n",
	       index, list->name, list);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Scripting report purpose 
 */
void		revm_print_actual(revmargv_t *cur)
{
  int		idx;
  char		logbuf[BUFSIZ];
    
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1, "~%s ", cur->name);
  revm_output(logbuf);
  for (idx = 0; cur->param[idx] && idx < 10; idx++)
    {
      snprintf(logbuf, BUFSIZ - 1, "%s ", cur->param[idx]);
      revm_output(logbuf);
    }
  putchar('\n');
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



