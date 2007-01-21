/*
** elfsh.c for elfsh
**
** Started on  Wed Feb 21 22:02:36 2001 mayhem
** Updated on  Tue Jun 27 23:51:04 2006 mxatone
*/

#include "revm.h"


/* The ELFsh world */
revmworld_t	world;



/* Signal handler for SIGINT */
void		sigint_handler(int signum)
{
  if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE)
    vm_exit(0);
}



/* The infinite main loop */
int		vm_loop(int argc, char **argv)
{
  int		ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = 0;

  do {

    /* Come back to the loop if a script turns into interactive mode */
  reenter:
    //__asm__(".byte 0xCC");

    /* Fill argv from stdin if we are in interactive mode */
    if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || world.state.vm_net == 1)
      {
	if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
	  {
	    if (vm_select() < 0)
	      {
		fprintf(stderr,"vm_select : failed \n");
		vm_exit(-1);
	      }
	  }

	/* Take a line, execute old command if void line */
	argv = vm_input(&argc);
	if (world.state.vm_mode == ELFSH_VMSTATE_IMODE     ||
	    world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER  ||
	    world.state.vm_mode == ELFSH_VMSTATE_SCRIPT    ||
	    world.state.vm_net)
	  {
	    if (argv == ((char **) ELFSH_VOID_INPUT))
	      continue;
	  }

	/* CTRL-D -> !argv */
	if (!argv)
	  {
	    /* when debugging -> back to main program */
	    if (world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
	      {
		vm_output("\n");
		goto e2dbg_cleanup;
	      }

	    /* if net is enable but we are not in e2dbg -> ignore */
	    if (world.state.vm_net)
	      continue;

	    /* othewise exit */
	    break;

	  }
      }

    /* Fetch the current scripting command */
    if (vm_parseopt(argc, argv) < 0)
      {
	if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE)
	  {
	    //XFREE(argv[1]);
	    XFREE(argv);
	    if (world.state.vm_mode != ELFSH_VMSTATE_IMODE &&
		world.state.vm_mode != ELFSH_VMSTATE_DEBUGGER)
	      goto end;
	  }
	else if(!world.state.vm_net)
	  vm_exit(-1);
      }

    /* Just execute one command if we are not in script mode */
    if (world.state.vm_mode != ELFSH_VMSTATE_SCRIPT)
      {
	world.curjob->curcmd = world.curjob->script[0];
	switch (vm_execmd())
	  {
	  case E2DBG_SCRIPT_CONTINUE:
	    printf(" [*] e2dbg continue from vm_execcmd \n");
	    goto e2dbg_continue;
	  case -1:
	    elfsh_error();
	  default:
	    break;
	  }
      }

    /* Quit parsing if necessary */
    if ((!world.curjob->curcmd && world.state.vm_mode == ELFSH_VMSTATE_SCRIPT) ||
	(world.curjob->curcmd && world.curjob->curcmd->name &&
	 (!strcmp(world.curjob->curcmd->name, CMD_QUIT) ||
	  !strcmp(world.curjob->curcmd->name, CMD_QUIT2))))
      break;
  }
  while (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE || world.state.vm_net);

  /* If we are in scripting, execute commands list now */
  if (world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    {
      world.curjob->curcmd = world.curjob->script[0];
      if (vm_execscript() == ELFSH_SCRIPT_STOP)
	{
	  XCLOSE(world.curjob->io.input_fd, -1);
	  world.curjob->io.input_fd = 0;
	  goto reenter;
	}
    }

 end:
  if (!world.state.vm_quiet && world.state.vm_mode == ELFSH_VMSTATE_SCRIPT)
    vm_output("\n [*] Script execution ended. \n\n");

  /* Implicit unload or save if we are not in interactive mode */
  if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE && world.curjob->current)
    ret = vm_unload_cwfiles();

#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));

  /* Clean the script machine state when a script is over */
 e2dbg_cleanup:
  world.curjob->script[world.curjob->sourced] = world.curjob->curcmd = NULL;
  world.curjob->lstcmd[world.curjob->sourced] = NULL;
  
    /* We arrive here when we execute a continue command from a debugger script */
 e2dbg_continue:
#if defined(USE_READLN)
  rl_callback_handler_remove();
#endif

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}


/* Only one time initialisations */
/* Called from CTORS */
int		vm_init()
{
 /* Must be here in case of script params presence */
  bzero(&world, sizeof (world));

  /* Set the world up */
  asm_init_i386(&world.proc);
  asm_init_sparc(&world.proc_sparc);
  vm_initio();

  /* setting libelfsh profile function */
  /* error on stdout, profile on stderr */
  elfsh_set_profile(vm_outerr, vm_output);
  return (0);
}


/* Setup ELFsh/e2dbg hash tables and structures */
int		vm_setup(int ac, char **av)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Detect the mode we are running in */
  if ((0 < ac) && (ac < 3) && !strncmp(av[0], E2DBG_ARGV0, 5))
    {
      /* Set the mode of e2dbg */
      world.state.vm_mode = ELFSH_VMSTATE_DEBUGGER;
#if defined(USE_READLN)
      world.curjob->io.buf = NULL;
#endif
      elfsh_set_debug_mode();
    }

  /* Set libelfsh in static mode */
  else
    {
      elfsh_set_static_mode();

      if (ac == 1)
	world.state.vm_mode = ELFSH_VMSTATE_IMODE;
      else if (vm_testscript(ac, av))
	{
	  world.state.vm_mode = ELFSH_VMSTATE_SCRIPT;
	  vm_setup_hashtables();
	  if (vm_openscript(&av[1]) < 0)
	    QUIT_ERROR(-1);
	  ac = 1;
	  av[1] = NULL;
	}
    }

  /* on.load. */
  elfsh_config_add_item(ELFSH_VMCONFIG_ONLOAD_RCONTROL,
			ELFSH_CONFIG_TYPE_INT,
			ELFSH_CONFIG_MODE_RW,
			(int) 1);
  
  if (!mjr_init_session(&world.mjr_session))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "mjollnir session can't be initialized.", -1);

  vm_setup_hashtables();
  elfsh_setup_hooks();
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Open the .elfshrc file and execute it */
int		vm_config()
{
  char		buff[BUFSIZ];
  char		*home;
  int		ret;
  static int	done = 0;
  revmargv_t	*new;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (done)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  ret = -1;
  home = getenv("HOME");
  if (home)
    {
      snprintf(buff, sizeof(buff), "%s/.elfshrc", home);
      XALLOC(new, sizeof(revmargv_t), -1);
      memset(new, 0, sizeof(revmargv_t));
      world.curjob->curcmd = new;
      world.curjob->curcmd->param[0] = buff;
      ret = cmd_source();
      world.curjob->curcmd = NULL;
      XFREE(new);
    }
  if (ret < 0)
    vm_output("\n [*] No configuration in ~/.elfshrc \n\n");
  done = 1;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Interface initialisation && loop entry point */
int		vm_run(int ac, char **av)
{
#if defined(USE_READLN)
  int		cmdnum;
#endif

  /* Do not handle signals in debugger mode */
  if (world.state.vm_mode != ELFSH_VMSTATE_DEBUGGER)
    {
      signal(SIGQUIT, SIG_IGN);
      signal(SIGTERM, SIG_IGN);
      signal(SIGINT, sigint_handler);
    }

#if __DEBUG_SIGHANDLER__
  vm_output("[elfsh:main] started !\n");
#endif

#if defined(USE_READLN)
  if (world.state.vm_mode == ELFSH_VMSTATE_IMODE ||
      world.state.vm_mode == ELFSH_VMSTATE_DEBUGGER)
    {

      world.comp.cmds[0]  = hash_get_keys(&cmd_hash    , &cmdnum);
      world.comp.cmds[1]  = hash_get_keys(&vars_hash   , &cmdnum);
      world.comp.cmds[2]  = hash_get_keys(&const_hash  , &cmdnum);
      world.comp.cmds[3]  = hash_get_keys(&mod_hash    , &cmdnum);
      world.comp.cmds[4]  = hash_get_keys(&L1_hash     , &cmdnum);
      world.comp.cmds[5]  = hash_get_keys(&elf_L2_hash , &cmdnum);
      world.comp.cmds[6]  = hash_get_keys(&sht_L2_hash , &cmdnum);
      world.comp.cmds[7]  = hash_get_keys(&pht_L2_hash , &cmdnum);
      world.comp.cmds[8]  = hash_get_keys(&sym_L2_hash , &cmdnum);
      world.comp.cmds[9]  = hash_get_keys(&rel_L2_hash , &cmdnum);
      world.comp.cmds[10] = hash_get_keys(&dynsym_L2_hash, &cmdnum);
      world.comp.cmds[11] = hash_get_keys(&dyn_L2_hash , &cmdnum);
      world.comp.cmds[12] = hash_get_keys(&sct_L2_hash , &cmdnum);
      world.comp.cmds[13] = hash_get_keys(&fg_color_hash, &cmdnum);
      world.comp.cmds[14] = hash_get_keys(&t_color_hash, &cmdnum);
      world.comp.cmds[15] = NULL;

      using_history();
      rl_attempted_completion_function = vm_completion;
      rl_callback_handler_install(vm_get_prompt(), vm_ln_handler);
      rl_bind_key(CTRL('x'), vm_screen_switch);
      vm_install_clearscreen();
      update_col();

      /* We will handle SIGWINCH */
      signal(SIGWINCH, (void*)update_col);
      rl_catch_sigwinch = 0;
      rl_set_signals();
    }
  else
    rl_bind_key ('\t', rl_insert);
#endif

  vm_flush();

#if defined (USE_READLN)
  vm_log(vm_get_prompt());
#endif

  /* Now run f0r3st */
  return (vm_loop(ac, av));
}
