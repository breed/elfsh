/*
** unload.c for elfsh
** 
** Started on  Sat Jan 25 11:21:52 2003 mayhem
** Last update Thu Mar 11 07:56:36 2004 mayhem
*/
#include "elfsh.h"

/* Unload all dependence for an object */
static int	vm_unload_dep(elfshobj_t *obj)
{
  elfshobj_t	*actual;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Unload each dependence */
  for (actual = obj->deplist; actual != NULL; actual = actual->next)
    {
      if (!world.state.vm_quiet)
	{
	  snprintf(logbuf, BUFSIZ - 1, "\n [*] Object dependence %s unloaded\n",
		   actual->name);
	  vm_output(logbuf);	  
	}

      /* A dependence can't be on the main list */
      if (world.curjob->current && world.curjob->current->id == actual->id)
	  world.curjob->current = world.curjob->list;

      hash_del(&obj->child_hash, actual->name);

      /* A dependence can have its own dependences */
      vm_unload_dep(actual);

      elfsh_unload_obj(actual);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Unload a file from the shell */
int		cmd_unload()
{
  elfshobj_t	*cur;
  elfshobj_t	*act;
  elfshobj_t	*todel;
  time_t	uloadt;
  u_int		id;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

   /* Get the file structure */
  id = atoi(world.curjob->curcmd->param[0]);
  cur = vm_lookup_file(world.curjob->curcmd->param[0]);
  if (cur == NULL)
    goto bad;

  if (cur->parent != NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "You can't unload a dependence, you have to unload the main file", -1);

  /* If the element is the first of the list, update the head pointer ... */
  if (world.curjob->list && world.curjob->list->id == cur->id)
    {
      /* do not unload file referenced in linkmap */
      if (world.curjob->list->linkmap)
	goto end2;

      if (world.curjob->current == world.curjob->list)
	world.curjob->current = world.curjob->list->next;
      todel = world.curjob->list;
      world.curjob->list = world.curjob->list->next;
      goto end;
    }

  /* ... else find the object where it is in the list */
  for (act = world.curjob->list; act && act->next; act = act->next)
    if (act->next->id == cur->id)
      {
	/* do not unload file referenced in linkmap */
	if (act->next->linkmap)
	  goto end2;

	if (world.curjob->current == act->next)
	  world.curjob->current = act;
	todel = act->next;
	act->next = act->next->next;
	goto end;
      }

  /* Error msg */
 bad:
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Object not loaded", -1);
  
  /* OK msg */
 end:
  time(&uloadt);
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, "\n [*] Object %s unloaded on %s \n",
	       todel->name, ctime(&uloadt));
      vm_output(logbuf);
    }
  hash_del(&file_hash, todel->name);
  vm_unload_dep(todel);
  elfsh_unload_obj(todel);
  vm_output("\n");
 end2:
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



