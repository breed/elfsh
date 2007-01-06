/*
** job.c for elfsh
** 
** Started on  Wed Jul 20 22:22:35 2005 yann_malcom 
*/
#include "revm.h"

int		cmd_workspace()
{
  revmjob_t	*job;
  u_int		idx;
  u_int		index;
  u_short	new = 0;
  char		logbuf[BUFSIZ];
  char		*nl;
  char		*time;
  elfshobj_t	*obj;
  char		**keys;
  int		keynbr;
  char		**loadedkeys;
  int		loadedkeynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  //printf("workspace argc %u \n", world.curjob->curcmd->argc);

  switch (world.curjob->curcmd->argc)
    {
      
      /* $ workspace */
    case 0: 
      vm_output(" .::. Workspaces .::. \n");
      keys = hash_get_keys(&world.jobs, &keynbr);
      for (index = 0; index < keynbr; index++)
	{
	  job = (revmjob_t *) hash_get(&world.jobs, keys[index]);
	  if (vm_own_job(job))
	    {
	      time = ctime(&job->createtime);
	      nl = strchr(time, '\n');
	      if (nl)
		*nl = 0x00;
	      snprintf(logbuf, BUFSIZ - 1, " [%s] %s %c \n", keys[index], 
		       time, (job->active ? '*' : ' '));
	      vm_output(logbuf);
	      
	      if (hash_size(&job->loaded))
		{
		  loadedkeys = hash_get_keys(&job->loaded, &loadedkeynbr);
		  for (idx = 0; idx < loadedkeynbr; idx++)
		    {
		      obj = hash_get(&job->loaded, loadedkeys[idx]);
		      snprintf(logbuf, BUFSIZ - 1, " \t %c %s \n", 
			       (job->current == obj ? '*' : ' '), obj->name);
		      vm_output(logbuf);
		    }
		  
		}
	      else
		{
		  snprintf(logbuf, BUFSIZ - 1, " \t   No files\n");
		  vm_output(logbuf);
		}
	    }
	}
      vm_output("\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
      
      /* $ workspace name */      
    case 1:
      if (!vm_valid_workspace(world.curjob->curcmd->param[0]))
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Incorrect workspace name", -1);

      job = hash_get(&world.jobs, world.curjob->curcmd->param[0]);
      if (!job)
	{
	  /* create a new workspace */
	  job = vm_clone_job(world.curjob);
	  job->name = elfsh_strdup(world.curjob->curcmd->param[0]);
	  hash_add(&world.jobs, world.curjob->curcmd->param[0], (void *) job);
	  new = 1;
	}
      else
	{
	  if (job->active)
	    {
	      snprintf(logbuf, BUFSIZ - 1,
		       "\n [+] Already in workspace : %s\n\n", world.curjob->curcmd->param[0]);
	      vm_output(logbuf);

	      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	    }
	}

      if (vm_own_job(job))
	{
	  /* switch */
	  snprintf(logbuf, BUFSIZ - 1, 
		   "\n [+] Workspace : %s \n\n", world.curjob->curcmd->param[0]);
	  vm_output(logbuf);

#if defined (USE_READLN)
	  /* Add a prompt for return */
	  vm_log(vm_get_prompt());
#endif

	  /* Switch to the new job */
	  vm_switch_job(job);

	  /* Update the screen */
	  vm_screen_update(new, 0);

	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}

      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Incorrect workspace name", -1);

      /* Unknown command format */
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong arg number", -1);
    }
}

/* Switch to the next workspace */
int		cmd_next_workspace()
{
  u_int		index, entrie;
  char	        **keys;
  int		keynbr;
  revmjob_t	*curjob;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  keys = hash_get_keys(&world.jobs, &keynbr);
  if (keynbr <= 1)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Search the current index */
  for (index = 0; index < keynbr; index++)
    {
      curjob = hash_get(&world.jobs, keys[index]);
      if (vm_own_job(curjob) && curjob == world.curjob)
	{
	  entrie = index;
	  break;
	}
    }

  /* Search the next entrie */
  for (entrie = (entrie+1) % keynbr; entrie < keynbr; entrie = (entrie+1) % keynbr)
    {
      curjob = hash_get(&world.jobs, keys[entrie]);
      if (vm_own_job(curjob))
	{
	  /* If we found the current job, we made a loop, so we break */
	  if (curjob == world.curjob)
	    break;

	  vm_switch_job(curjob);
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	}
    }

  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to find workspace to switch on", -1);
}
