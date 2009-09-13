/**
* @file etrace/misc.c
 * @ingroup etrace
 * Updated on  Wed Jan 03 17:31:11 2007 mxatone
 *
 * $Id$
 *
 */
#include "etrace.h"


/**
 * Mark the current object SHT to be removed on savnig
 * @ingroup etrace
 */
int             cmd_shtrm()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->curfile->shtrm = 1;
  if (!world.state.revm_quiet)
    revm_output(" [*] SHT marked as removed, you need to save\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Mark the file as stripped
 * @ingroup etrace
 */
int		cmd_strip()
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->curfile->strip = 1;
  if (!world.state.revm_quiet)
    revm_output(" [*] Object marked as stripped, you need to save\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Mark SHT as removed and file as stripped
 * @ingroup etrace
 */
int		cmd_sstrip()
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  world.curjob->curfile->strip = 1;
  world.curjob->curfile->shtrm = 1;
  if (!world.state.revm_quiet)
    revm_output(" [*] Object marked as sstripped, you need to save\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
