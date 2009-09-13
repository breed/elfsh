/**
* @file libstderesi/elf/comment.c
** @ingroup elf
*/
/**
 *
 *
 * Made by <jfv@devhell.org>
 *
 * Started on  Tue Dec 17 06:43:01 2002 jfv
 *
 *
 * $Id$
 *
 */
#include "libstderesi.h"


/** 
 * @brief Display the note section 
 */
int			cmd_comments()
{
  elfshsect_t		*sect;
  regex_t		*tmp;
  int			range;
  char			buff[256];
  char			*ent;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  revm_output("\n [SHT_COMMENT]\n");
  sect = elfsh_get_comments(world.curjob->curfile);
  if (!sect)
    RET(-1);
  FIRSTREGX(tmp);

  ent = elfsh_get_comments_entry(world.curjob->curfile, 0);
  for (range = 0; ent != NULL; range++)
    {
      snprintf(buff, sizeof(buff),
	       " [%02u] \t COMMENT ENTRY: %s\n", range, ent);
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	revm_output(buff);
      ent = elfsh_get_comments_entry(world.curjob->curfile, range + 1);
    }

  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
