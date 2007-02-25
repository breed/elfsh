/*
** opt.c
** 
** Started on  Fri Nov  2 15:17:02 2001 mayhem
*/
#include "revm.h"


/* Read the input file parameter */
int		vm_getoption(u_int index, u_int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->argc = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Read the input file parameter */
int		vm_getinput(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.state.input = argv[index + 1];
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Read the output file parameter */
int		vm_getoutput(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.state.output = argv[index + 1];
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Activate a 2-non-regx-mandatory-parameters option */
int		vm_getoption2(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 2 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->param[1] = argv[index + 2];
  world.curjob->curcmd->argc = 2;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (2));
}

/* Activate a 2-non-regx-mandatory-parameters option */
int		vm_getoption3(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 3 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->param[1] = argv[index + 2];
  world.curjob->curcmd->param[2] = argv[index + 3];
    world.curjob->curcmd->argc = 3;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (3));
}

/* Activate a non-mandatory-regex-parameter option */
int		vm_getregxoption(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 < argc && argv[index + 1][0] != ELFSH_MINUS)		
    {									
      if (regcomp(&world.curjob->curcmd->regx[0], argv[index + 1], 
		  REG_EXTENDED) != 0 || *argv[index + 1] == '*')
	  
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Parameter not available", (-1));
      world.curjob->curcmd->use_regx[0] = 1;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
    }				
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Fetch parameters until we find NULL or something starting by '-' */
int		vm_getvarparams(u_int index, u_int argc, char **argv)
{
  u_int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (world.curjob->curcmd->argc = idx = 0; 
       idx < 254 && index + idx + 1 < argc;
       idx++)
    {
      world.curjob->curcmd->param[idx] = argv[index + idx + 1];
      world.curjob->curcmd->argc++;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
}

/* Add an entry to the requested dump list */
static int      vm_add2list(char outtype, u_int index, int argc, char **argv)
{
  char		*off;							
  int		idx = index;
  revmlist_t	*cur;
  char		*used;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  while (idx < index + 2)
    {
      cur  = world.curjob->curcmd->disasm   + idx - index;
      used = world.curjob->curcmd->use_regx + idx - index;
	
      if (argv[idx + 1] == NULL)								
	{
	  *used = 0;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     (idx == index ? -1 : 1));
	}

      cur->rname = argv[idx + 1];
      cur->otype = outtype;	
      cur->size = 0;					
      cur->off = 0;
      off = strchr(argv[idx + 1], '%');						  

      if (off)     			
	{									
	  cur->size = atoi(off + 1);					
	  *off = 0;								
	}									
      off = strchr(argv[idx + 1], ':');						
      if (off)		       
	{									
	  cur->off = atoi(off + 1);
	  *off = 0;								
	}									
      
      if (regcomp(&cur->name, argv[idx + 1], REG_EXTENDED | REG_ICASE) != 0 ||
	  *cur->rname == '*')
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Regular computation failed", (-1));
      *used = 1;
      idx++;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (2));
}

/* Add an DISASM typed entry */
int		vm_getdisasm(u_int index, u_int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     vm_add2list(ELFSH_DISASM_VIEW, index, argc, argv));
}

/* Add an HEXA typed entry */
int		vm_gethexa(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     vm_add2list(ELFSH_HEXA_VIEW, index, argc, argv));
}

/* Parse the commands */
int			vm_parseopt(int argc, char **argv)
{
  u_int			index;
  int			ret;
  volatile revmcmd_t	*actual;
  char			*name;
  char			label[16];
  char			c;
  static u_int		pendinglabel = 0;
  static revmargv_t	*new = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Main option reading loop : using the command hash table */
  for (index = 1; index < argc; index++)
    {

      /* Allocate command descriptor */
      bzero(label, sizeof(label));
      if (!pendinglabel)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,new, sizeof(revmargv_t), -1);
	  world.curjob->curcmd = new;
	  if (world.curjob->script[world.curjob->sourced] == NULL)
	    world.curjob->script[world.curjob->sourced] = new;
	}
      else
	pendinglabel = 0;
      
      /* Retreive command descriptor in the hash table */
      name = argv[index] + ((world.state.vm_mode == ELFSH_VMSTATE_CMDLINE) && !(world.state.vm_net));
      actual = hash_get(&cmd_hash, name);
    
      /* We matched a command : call the registration handler */
      if (actual != NULL)
	{
	  if (actual->reg != NULL)
	    {
	      ret = actual->reg(index, argc, argv);
	      if (ret < 0)
              PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
				"Command not found", 
				(vm_doerror(vm_badparam, argv[index])));
	      index += ret;
	    }
	}

      /* We did -NOT- match command */
      else if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
	{
	  /* Try to match a label */
	  ret = sscanf(name, "%15[^:]%c", label, &c);

	  if (ret == 2 && c == ':')
	    {
	      hash_add(&labels_hash[world.curjob->sourced], 
		       strdup(label), new); 
 

	      printf(" [*] Found label %s \n", label);

	      pendinglabel = 1;
	      continue;
	    }
	  
	  /* No label matched, enable lazy evaluation */
	  /* because it may be a module command */
	  ret = vm_getvarparams(index - 1, argc, argv);
	  index += ret;
	}
      
      /* We matched nothing known, error */
      else
          PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unknown parsing error", 
			    (vm_doerror(vm_unknown, argv[index])));

      /* Put the new command at the end of the list */
      new->name = name;
      new->cmd  = (revmcmd_t *) actual;
      
      if (!world.curjob->lstcmd[world.curjob->sourced])
	world.curjob->lstcmd[world.curjob->sourced] = new;
      else
	{
	  world.curjob->lstcmd[world.curjob->sourced]->next = new;
	  new->prev = world.curjob->lstcmd[world.curjob->sourced];
	  world.curjob->lstcmd[world.curjob->sourced] = new;
	}
      
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}











