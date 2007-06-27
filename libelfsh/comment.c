/**
 * @file comment.c
** comment.c for elfsh in 
** 
** Made by mayhem
** Login   <mayhem@devhell.org>
** 
** Started on  Sun Dec  1 09:22:45 2002 mayhem
**
**
** $Id: comment.c,v 1.6 2007-06-27 11:25:12 heroine Exp $
**
*/
#include "libelfsh.h"


/**
 * Get comment section 
 */
elfshsect_t		*elfsh_get_comments(elfshobj_t *file)
{
  elfshsect_t		*new;
  int			size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL paramater", NULL);

  new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_COMMENT, 
				  NULL, NULL, &size);
  if (NULL == new)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get .comment by name", NULL);

  if (NULL == elfsh_get_raw(new))
    {
      new->data = elfsh_load_section(file, new->shdr);
      if (NULL == elfsh_get_raw(new))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load .comment", NULL);
    }

  file->secthash[ELFSH_SECTION_COMMENT] = new;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


/**
 * Get an entry from .comment 
 */
char			*elfsh_get_comments_entry(elfshobj_t *file, u_int range)
{
  int			index;
  int			act;
  char			*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#define	CHECK_SZ	(index < file->secthash[ELFSH_SECTION_COMMENT]->shdr->sh_size)

  if (!file->secthash[ELFSH_SECTION_COMMENT] && !elfsh_get_comments(file))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get .comments", NULL);

  index = act = 0;
  data = elfsh_get_raw(file->secthash[ELFSH_SECTION_COMMENT]);
  while (!data[index] && CHECK_SZ)
    index++;
  while (act != range && CHECK_SZ) 
    if (!data[index])
      {
	act++;
	while (!data[index] && CHECK_SZ)
	  index++;
      }
    else
      index++;

#undef CHECK_SZ

  if (index < file->secthash[ELFSH_SECTION_COMMENT]->shdr->sh_size)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (data + index));

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to get .comments entry", NULL);
}
