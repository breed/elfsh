/*
* @file libstderesi/analysis/fcthijack.c
** @ingroup analysis
** Started on  Thu Jun 19 17:02:55 2003 jfv
**
** $Id$
**
*/
#include "libstderesi.h"




/* Redirect a function on a OS independant manner */
int		cmd_hijack()
{
  elfsh_Sym	*dst;
  eresi_Addr	addr;
  int		err;
  char		*rev;
  char		logbuf[BUFSIZ];

  elfshredir_t	*redir;
  listent_t     *actual;
  int		idx;
  int		idx2;
  int		printed;
  eresi_Addr	hookedaddr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If no parameter is given, print the redirection list */
  if (!world.curjob->curcmd->param[0])
    {
      printed = 0;

      /* Simple printing */
      for (idx2 = idx = 0; idx < world.curjob->curfile->redir_hash.size; idx++)
	for (actual = world.curjob->curfile->redir_hash.ent + idx;
	     actual != NULL && actual->key != NULL;
	     actual = actual->next)
	  {
	    redir = (elfshredir_t *) actual->data;
	    
	    if (!printed)
	      {
		revm_output("\t .::. ELFsh redirection list \n\n");
		printed = 1;
	      }
	    snprintf(logbuf, BUFSIZ, 
		     "\t [%02u] TYPE:%-6s [" AFMT "] <%s> redirected on [" AFMT "] <%s> \n",
		     idx2, (redir->type == ELFSH_REDIR_CFLOW  ? "CFLOW"  : 
			    redir->type == ELFSH_REDIR_ALTPLT ? "ALTPLT" : 
			    redir->type == ELFSH_REDIR_ALTGOT ? "ALTGOT" : "UNK"),
		     redir->addr[0], redir->name[0], redir->addr[1], redir->name[1]);
	    
	    revm_output(logbuf);
	    idx2++;
	  }

      /* End message */
      if (!printed)
	revm_output("\t .::. No redirection performed on current file \n\n");
      else
	revm_output("\n\n");

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Some sanity checks first */
  if (!world.curjob->curcmd->param[1])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Redirection destination needed", (-1));

  /* Resolve destination parameter */
  dst = elfsh_get_metasym_by_name(world.curjob->curfile, 
				  world.curjob->curcmd->param[1]);
  if (!dst && elfsh_is_runtime_mode())
    {
      elfsh_toggle_mode();
      dst = elfsh_get_metasym_by_name(world.curjob->curfile, 
				      world.curjob->curcmd->param[1]);
      elfsh_toggle_mode();
    }

  /* If not found */
  if (dst == NULL)
    {
      err = sscanf(world.curjob->curcmd->param[1], XFMT, 
		   (eresi_Addr *) &addr);
      
      /* If the hook function is not supplied as an address */
      if (err != 1 && elfsh_dynamic_file(world.curjob->curfile))
	{
	  elfsh_setup_hooks();

	  /* First bootstrap ALTPLT if not done */
	  err = elfsh_copy_plt(world.curjob->curfile, 
			       elfsh_get_pagesize(world.curjob->curfile));
	  if (err < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Failed at copying PLT", (-1));		  
	  
	  /* Request a PLT entry since we have no symbol yet */
	  dst = elfsh_request_pltent(world.curjob->curfile, 
				     world.curjob->curcmd->param[1]);
	  if (dst)
	    addr = dst->st_value;
	  else
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "PLT entry request failed", (-1));		  
	}

      /* Insert a symbol on the requested address to avoid this */
      else	  
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Need a symbol to redirect", -1);
      
      rev = revm_reverse(world.curjob->curfile, addr);
    }

  /* The first resolution worked, we take the address */
  else
    {
      addr = dst->st_value;
      rev = NULL;
    }
  
#if	__DEBUG_HIJACK__
  printf("[cmd_hijack] Resolved %s as %08X \n", 
	 world.curjob->curcmd->param[1], addr);
#endif

  /* Hijack function */
  err = elfsh_hijack_function_by_name(world.curjob->curfile, 
				      ELFSH_HIJACK_TYPE_FLOW,
				      world.curjob->curcmd->param[0], 
				      addr, &hookedaddr);

  /* Last checks */
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Function redirection failed", (-1));
  
  /* Add it to redirection hash table */
  redir = revm_create_REDIR((u_char) err, world.curjob->curcmd->param[0], 
			  world.curjob->curcmd->param[1], hookedaddr, addr);
  hash_add(&world.curjob->curfile->redir_hash, 
	   world.curjob->curcmd->param[0], 
	   (void *) redir);

  /* Print final information */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       "\n [*] Function %s redirected to addr " XFMT " <%s> \n\n", 
	       world.curjob->curcmd->param[0], (eresi_Addr) addr, 
	       (rev == NULL ? world.curjob->curcmd->param[1] : rev));
      revm_output(logbuf);
    }
  if (rev != NULL)
    XFREE(__FILE__, __FUNCTION__, __LINE__,rev);
  
  /* Everything is ok */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
