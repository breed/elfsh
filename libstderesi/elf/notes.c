/**
* @file libstderesi/elf/notes.c
** @ingroup elf
*/
/**
 *
** 
** Started on  Fri Nov  2 15:20:58 2001 jfv
**
**
** $Id$
**
*/
#include "libstderesi.h"


/** 
 * @brief Display the note section 
 */
int			cmd_notes()
{
  elfshsect_t		*sect;
  elfshnotent_t		*e;
  regex_t		*tmp;
  int			index;
  int			range;
  char			buff[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buff, sizeof(buff),
	   " [SHT_NOTES]\n [Object %s]\n\n", 
	   world.curjob->curfile->name);
  revm_output(buff);

  FIRSTREGX(tmp);
  range = 0;
  sect = elfsh_get_notes(world.curjob->curfile, range);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get notes", (-1));

  while (sect)
    {
      snprintf(buff, sizeof(buff),
	       " {Section %s} \n", elfsh_get_section_name(world.curjob->curfile, sect));
      revm_output(buff);
      for (index = 0, e = sect->altdata; e; e = e->next, index++)
	{
	  snprintf(buff, sizeof(buff), " [%02u] \t NOTES_ENTRY(%s)\n", 
		   index, e->note);
	  if (!tmp || (tmp && e->note && !regexec(tmp, buff, 0, 0, 0)))
	    revm_output(buff);
	}
      revm_output("\n");
      range++;
      sect = elfsh_get_notes(world.curjob->curfile, range);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
