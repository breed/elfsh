/**
 * @file signal.c
 *
 * The debugger file for signal handlers
 * 
 * Started on  Tue Feb 11 21:17:33 2003 jfv
 * Last update Wed Aug 13 23:22:59 2005 jfv
 *
 * $Id: signal.c,v 1.1 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libe2dbg.h"


/** ARCH/OS dependent handler for checking values in registers */
void		e2dbg_watch_check_ia32_sysv(u_int regidx, char *regstr)
{
  char		buff[BUFSIZ];
  u_int		idx;

  for (idx = 0; idx < E2DBG_STEPCMD_MAX && e2dbgworld.tracedata[idx]; idx++)
    if (e2dbgworld.curthread->context->uc_mcontext.gregs[regidx] == e2dbgworld.tracedata[idx])
      {
	snprintf(buff, BUFSIZ, " [*] TRACED %s ("XFMT") found in register %s \n", 
		 e2dbgworld.tracedstr[idx], e2dbgworld.tracedata[idx], regstr);
	e2dbg_output(buff);

	/* Disable stepping and tracing */
	if (e2dbgworld.curthread->trace)
	  e2dbg_step();
	e2dbgworld.curthread->trace = 0;
      }
}

/** Check if any traced variable is used in the current instruction */
void		e2dbg_watch()
{
  e2dbg_watch_check_ia32_sysv(REG_EAX, "EAX");
  e2dbg_watch_check_ia32_sysv(REG_EBX, "EBX");
  e2dbg_watch_check_ia32_sysv(REG_ECX, "ECX");
  e2dbg_watch_check_ia32_sysv(REG_EDX, "EDX");
  e2dbg_watch_check_ia32_sysv(REG_ESI, "ESI");
  e2dbg_watch_check_ia32_sysv(REG_EDI, "EDI");
  e2dbg_watch_check_ia32_sysv(REG_ESP, "ESP");
  e2dbg_watch_check_ia32_sysv(REG_EBP, "EBP");
  e2dbg_watch_check_ia32_sysv(REG_EIP, "EIP");
}



/**
 * Signal handler for SIGSEGV 
 * @param signum
 * @param info
 * @param pcontext 
 * @param
 */
void            e2dbg_sigsegv_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  e2dbgparams_t	params;

  CLRSIG;
  context = (ucontext_t *) pcontext;
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;

  switch (signum)
    {
    case SIGSEGV:
      fprintf(stderr, "\n\n ******* SIGSEGV: entering E2dbg ******** \n\n");
      break;
    case SIGBUS:
      fprintf(stderr, "\n\n ******* SIGBUS : entering E2dbg ******** \n\n");
      break;
    default:
      fprintf(stderr, "\n\n ******* SIGNAL : entering E2dbg ******** \n\n");
      break;
    }
  sleep(1);

  params.ac = 1;
  params.av = argv;
  e2dbg_presence_set();
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}

/**
 * Internal Signal handler for SIGSEGV 
 * @param signum
 * @param info
 * @param pcontext
 * @return
 */
void            e2dbg_internal_sigsegv_handler(int signum, siginfo_t *info, 
					       void *pcontext)
{
  ucontext_t	*context;

  CLRSIG;
  if (profiler_enabled())
    profiler_disable_all();
  context = (ucontext_t *) pcontext;
  fprintf(stderr, "\n [!] Segfault in E2dbg, exiting ...\n\n");
  e2dbg_bt();
  cmd_quit();
  SETSIG;
}

/** 
 * Signal handler for SIGINT 
 * @param signum
 * @param info
 * @param pcontext
 * @return 
 */
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  e2dbgparams_t	params;
  e2dbgthread_t	*curthread;
  char		key[15];

  CLRSIG;
  e2dbg_presence_set();

  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) e2dbg_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

  /* Set all registers as variables and get PC */
  //e2dbgworld.context = (ucontext_t *) pcontext;

  e2dbg_user_hooks_install();
  e2dbg_getregs();
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  e2dbg_output(" [*] Interrupted, entering E2dbg ...\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}


/**
 * Signal handler for SIGSTOP 
 * @param signum
 * @param info
 * @param pcontext
 * @return
 */
void            e2dbg_sigstop_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  e2dbgparams_t	params;
  e2dbgthread_t	*curthread;
  char		key[15];

  CLRSIG;
  e2dbg_presence_set();

#if __DEBUG_THREADS__
  printf("\n [*] SIGSTOP handler for thread %u \n", (unsigned int) e2dbg_self());
#endif

  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) e2dbg_self());
  curthread = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;

  /* Set all registers as variables and get PC */
  //e2dbgworld.context = (ucontext_t *) pcontext;

  e2dbg_user_hooks_install();
  e2dbg_getregs();
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  e2dbg_output(" [*] Interrupted, entering E2dbg ...\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}

/**
 * Signal handler for SIGUSR2 
 * @param signum
 * @param info
 * @param pcontext
 * @return
 */
void            e2dbg_thread_sigusr2(int signum, siginfo_t *info, void *pcontext)
{
  e2dbgthread_t	*curthread;
  char		key[15];
  
  /* Get the current thread */
  snprintf(key, sizeof(key), "%u", (unsigned int) e2dbg_self());
  curthread          = hash_get(&e2dbgworld.threads, key);
  curthread->context = (ucontext_t *) pcontext;
  curthread->state   = E2DBG_THREAD_SIGUSR2;


#if __DEBUG_THREADS__
  fprintf(stderr, " ************ [T] SIGUSR2 received by %u ******** \n", 
	  (unsigned int) curthread->tid);
#endif
  
  /* Make it wait until the breakpoint is finished to process */
  e2dbgworld.threadsyncnbr++;
  e2dbg_mutex_lock(&e2dbgworld.dbgbp);
  e2dbg_mutex_unlock(&e2dbgworld.dbgbp);
  curthread->state = E2DBG_THREAD_RUNNING;

#if __DEBUG_THREADS__
  fprintf(stderr, " ********** [T] Thread %u getting out of SIGUSR2 handler ******* \n", 
	  (unsigned int) curthread->tid);
#endif

}




/**
 * Signal handler for SIGTRAP 
 * @param signum
 * @param info
 * @param pcontext
 * @return
 */
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *pcontext)
{
  char		*argv[2];
  ucontext_t	*context;
  e2dbgparams_t	params;

  CLRSIG;
  e2dbg_presence_set();

#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__)
  if (!e2dbg_presence_get())
    e2dbg_output(" [*] Debuggee in SIGTRAP handler\n");
  else 
    e2dbg_output(" [*] Debugger in SIGTRAP handler\n");
#endif

  context = (ucontext_t *) pcontext;
  argv[0] = E2DBG_ARGV0;
  argv[1] = NULL;
  e2dbg_output(" [*] SIGTRAP : Entering E2dbg.\n");
  params.ac = 1;
  params.av = argv;
  e2dbg_entry(&params);
  e2dbg_presence_reset();
  SETSIG;
}


#if __DEBUG_BP__
/**
 * @param str
 * @param bp
 * @param pc
 * @param parent
 * @return
 */
void		bpdebug(char *str, elfshbp_t *bp, eresi_Addr pc, elfshobj_t *parent)
{
  eresi_Addr	addr;
  int		off;
  char		*name;
  elfshsect_t	*sect;
  elfsh_Sym	 *sym;

  off  = (bp ? 0 : e2dbgworld.stoppedthread->count == E2DBG_BREAK_HIT ? 3 : 6);
  addr = (bp ? bp->addr : pc - off);
  
  fprintf(stderr, "%s (PC = %08X) ::: parent = %s (BP DESCRIPTOR = %08X) \n",
	  str, pc, (parent ? parent->name : "NONE !!!!"), (eresi_Addr) bp);
  if (!bp)
    return;
  
  sect   = elfsh_get_parent_section(parent, addr, NULL);
  name   = revm_resolve(parent, addr, &off);
  sym    = elfsh_get_metasym_by_value(parent, addr, &off, ELFSH_LOWSYM);
  revm_object_display(sect, sym, 16, off, 0, addr, name, REVM_VIEW_DISASM, 0);
}
#endif


/**
 * Signal handler for SIGUSR1 in debugger 
 *
 */
void			e2dbg_do_breakpoint()
{
  char			*argv[2];
  char			buf[32];
  elfshbp_t		*bp;
  int			prot;
  char			*name;
  elfsh_SAddr		off;
  int			ret;
  asm_instr		ptr;
  char			*s;
  eresi_Addr		*pc; 
  eresi_Addr		savedpc;
  u_int			bpsz;
  elfshsect_t		*sect;
  elfshobj_t		*parent;
  elfsh_Sym		*sym;

#if __DEBUG_BP__
  fprintf(stderr, "Entering breakpoint handler ... \n");
#endif

  /* Set all registers as variables and get PC */
  argv[0] = "e2dbg";
  argv[1] = NULL;
  e2dbg_user_hooks_install();
  e2dbg_getregs(); 
  pc = e2dbg_getpc();

  parent = e2dbg_get_parent_object((eresi_Addr) *pc);
  bpsz = elfsh_get_breaksize(parent);

  /* Print variables and registers on breakpoints */
  //if (!world.state.revm_quiet)
  //cmd_vlist();

  /* Try to find the breakpoint at current instruction pointer */
#if __DEBUG_BP__
  fprintf(stderr, "[PC = %08X] Trying to find breakpoint at addr %08X (bpsize = %u)\n", 
	  *pc, *pc - bpsz, bpsz);
#endif

  snprintf(buf, sizeof(buf), XFMT, *pc - bpsz);
  bp = hash_get(&e2dbgworld.bp, buf);

#if __DEBUG_BP__
  bpdebug("BEFORE", bp, *pc, parent);
  if (bp)
    fprintf(stderr, " SAVED INSTR BYTE = %02X and PC-BPSZ BYTE = %02X \n",
	    bp->savedinstr[0], *((u_char *) *pc - bpsz));
  else
    fprintf(stderr, "No BP found at %08X ! \n", *pc);
#endif

  /* If we are single-stepping or if we are stepping the breaked instruction */
  if (!bp || (bp->savedinstr[0] == *((u_char *) *pc - bpsz)))
    {

      /* We are single-stepping, display the instruction at $pc */
      if (e2dbgworld.stoppedthread->step)
	{

#if __DEBUG_BP__
	  fprintf(stderr, "Single-stepping -IS- enabled \n");
#endif

	  ret = asm_read_instr(&ptr, (u_char *) *pc, 16, &world.proc);
	  if (!ret)
	    ret++;
	  sect   = elfsh_get_parent_section(parent, (eresi_Addr) *pc, NULL);
	  name   = revm_resolve(parent, (eresi_Addr) *pc, &off);
	  off = 0;
	  sym    = elfsh_get_metasym_by_value(parent, (eresi_Addr) *pc, 
					      &off, ELFSH_LOWSYM);

#if __DEBUG_BP__
	  printf("Found parent = %08X (%s) in step (name = %s, parentsect = %s) off = %u\n", 
		 (eresi_Addr) parent, parent->name, name, sect->name, off);
#endif

	  revm_instr_display(-1, 0, *pc, 0, 20, name, off, (char *) *pc);
	  e2dbg_display(e2dbgworld.displaycmd, e2dbgworld.displaynbr);
	  if (!e2dbgworld.stoppedthread->trace)
	    e2dbg_entry(NULL);
	  else
	    e2dbg_watch();
	  return;
	}
#if __DEBUG_BP__
      else
	fprintf(stderr, "Single-stepping is -NOT- enabled \n");
#endif

      /* Here starts the real stuff 
      **
      ** count == E2DBG_BREAK_EXEC     -> execute restored instruction
      ** count == E2DBG_BREAK_FINISHED -> restore breakpoint
      ** count >  E2DBG_BREAK_MAX      -> e2dbg is getting debugged by a third party debugger
      */
      e2dbgworld.stoppedthread->count++;	
      
#if __DEBUG_BP__
      printf(" [C] Count %u -> %u for thread ID %u \n", 
	     e2dbgworld.stoppedthread->count - 1, 
	     e2dbgworld.stoppedthread->count, 
	     ((unsigned int) e2dbgworld.stoppedthread->tid));
#endif

      /* execute the previously restored instruction */
      if (e2dbgworld.stoppedthread->count == E2DBG_BREAK_EXEC &&
	  !e2dbgworld.stoppedthread->step)
	{
#if __DEBUG_BP__
	  printf(" [*] [pc = %08x] Debuggee executed restored instruction \n", *pc);
#endif
	  return;
	}
      
      /* Suggested by andrewg, useful when debugging valgrind */
      if (e2dbgworld.stoppedthread->count > E2DBG_BREAK_MAX &&
	  !e2dbgworld.stoppedthread->step)
	{
	  printf(".::- E2DBG WARNING -::.\n"
		 "Breakpoint triggered at location " AFMT " which we don't know about.\n\n"
		 "This may be an anti-debug trick or the program MAY be inside another\n"
		 "debugger that uses breakpoints. (count = " UFMT ", step is off)\n\n" 
		 "This use of e2dbg is unsupported for now, exiting .. \n\n", 
		 *pc - bpsz, e2dbgworld.stoppedthread->count);
	  return;
	}

      /* Test if we need to reinstall a breakpoint */
      snprintf(buf, sizeof(buf), XFMT, e2dbgworld.stoppedthread->past);
      bp = hash_get(&e2dbgworld.bp, buf);

      /* Call the architecture dependant hook for putting back the breakpoint */
      if (bp)
	{
	  ret = e2dbg_setbreak(bp->obj, bp);
	  if (ret < 0)
	    {
	      e2dbg_output(" [E] Breakpoint reinsertion failed");
	      return;
	    }

#if __DEBUG_BP__
	  fprintf(stderr, " Breakpoint reinserted at " AFMT " ! \n", bp->addr);
#endif

	  e2dbgworld.stoppedthread->past = 0;
	}
#if __DEBUG_BP__
      else
	fprintf(stderr, "Breakpoint was deleted from " AFMT " : not reinstalling ! \n",
		e2dbgworld.stoppedthread->past);
      bpdebug("AFTER SETBREAK", bp, *pc, parent);
#endif
      
      /* remove trace flag */	  
      if (!e2dbgworld.stoppedthread->step)
	{
#if __DEBUG_BP__
	  fprintf(stderr, " [S] [PC = " AFMT "] RESETING STEP MODE ! \n", *pc);
#endif
	  e2dbg_resetstep();
	  return;
	}
    }


  /* Breakpoint case */
  else
    {
      name = revm_resolve(parent, (eresi_Addr) *pc - bpsz, &off);
      s    = (e2dbg_is_watchpoint(bp) ? "Watch" : "Break");


#if __DEBUG_BP__
      printf(" [C] Count set to 1 for thread ID %u \n", 
	     (unsigned int) e2dbgworld.stoppedthread->tid);
#endif      

      if (off)
	printf(" [*] %spoint found at " XFMT " <%s + " DFMT "> in thread %u \n\n", 
	       s, *pc - bpsz, name, off, (unsigned int) e2dbgworld.stoppedthread->tid);
      else 
	printf(" [*] %spoint found at " XFMT " <%s> in thread %u \n\n",   
	       s, *pc - bpsz, name, (unsigned int) e2dbgworld.stoppedthread->tid);
      
      *pc -= bpsz;
      prot = elfsh_munprotect(bp->obj, *pc,  bpsz);
      memcpy((u_char *) *pc, bp->savedinstr, bpsz);
      elfsh_mprotect(*pc, bpsz, prot);
      e2dbg_setstep();

#if __DEBUG_BP__
      fprintf(stderr, " [S] SETTING STEP MODE ! \n");
#endif

      e2dbgworld.stoppedthread->past  = *pc;
      e2dbgworld.stoppedthread->count = E2DBG_BREAK_HIT;
      e2dbgworld.curbp                = bp;
      bp->tid                         = (uint32_t) e2dbgworld.stoppedthread->tid;

#if __DEBUG_BP__
      bpdebug("AFTER RESET BREAK", bp, (eresi_Addr) *pc + bpsz, parent);
#endif

#if __DEBUG_BP__
      fprintf(stderr, " Breakpoint desinstalled at addr %08X \n", *pc);
#endif
      e2dbg_display(bp->cmd, bp->cmdnbr);
#if __DEBUG_BP__
      fprintf(stderr, " e2dbg display ! \n");
#endif

#if __DEBUG_BP__
      fprintf(stderr, "PC before entry is addr %08X \n", *pc);
#endif
      
      savedpc = *pc;
      e2dbg_entry(NULL);
      *pc = savedpc;

#if __DEBUG_BP__
      fprintf(stderr, "[PC = %08X] RETURNED FROM HANDLER WITH STEP ENABLED \n",
	      *pc);
#endif
    }
}


/**
 * The breakpoint routine. Save registers and callback 
 * @param signum
 * @param info
 * @param pcontext
 * @return
*/
void			e2dbg_generic_breakpoint(int		signum, 
						 siginfo_t	*info, 
						 void		*pcontext)
{
  ucontext_t		*context;
  char			key[15];
  pthread_t		stopped;

  /* Do not allow processing of 2 breakpoint at the same time */
  /* We update the current thread information */
  e2dbg_presence_set();
  if (!e2dbgworld.curbp || e2dbgworld.curbp->tid != e2dbg_self())
    {
      e2dbg_mutex_lock(&e2dbgworld.dbgbp);

#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__ || __DEBUG_BP__)
      e2dbg_output("-------------- ON-BREAK REGISTERS ----------------------->\n");
      //cmd_dumpregs();
      e2dbg_output("<------------------------------------------------------\n");
      e2dbg_output("------------------------------------->\n");
      e2dbg_output(" [*] BP MUTEX LOCKED [e2dbg_generic_breakpoint] \n");
      //e2dbg_threads_print();
#endif
    }
  
  /*
  if (e2dbgworld.stoppedthread == NULL)
    {
      fprintf(stderr, "************** STOPPED THREAD == NULL !!!!!!!!! *********************** \n");
      sleep(20);
    }
  */

  /* Get the current thread */
  stopped = (pthread_t) e2dbg_self();
  snprintf(key, sizeof(key), "%u", (unsigned int) stopped);
  e2dbgworld.curthread = hash_get(&e2dbgworld.threads, key);
  e2dbgworld.stoppedthread = e2dbgworld.curthread;
  
#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__ || __DEBUG_BP__)
  fprintf(stderr, "\n [*] Thread entering generic breakpoint (ID %u) \n",
	 (unsigned int) e2dbgworld.stoppedthread->tid);
  fflush(stdout);
#endif
  
  e2dbgworld.stoppedthread->state = E2DBG_THREAD_BREAKING;
  context = (ucontext_t *) pcontext;
  e2dbgworld.stoppedthread->context = context;

  /* We first get contexts for all other threads (except debugger) using SIGUSR2 */
  /* Then we stop all threads */
  /* We do this only at the first state (count = 0) of the breakpoint */
  if (!e2dbgworld.stoppedthread->count)
    {
      if (e2dbg_thread_stopall(SIGUSR2))
	usleep(100000);
    }
#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__ || __DEBUG_BP__)
  else
    fprintf(stderr, " NOT COLLECTING THREADS CONTEXTS (stopped thread %u state count = %u) \n", 
	    (u_int) e2dbgworld.stoppedthread->tid, e2dbgworld.stoppedthread->count);
#endif

  /* Call the real breakpoint code */
  e2dbg_do_breakpoint();

  /* Continue all threads */
  if (e2dbgworld.stoppedthread->count == E2DBG_BREAK_FINISHED || 
      e2dbgworld.curthread->step || e2dbgworld.curthread->was_step)
    {
      e2dbg_thread_contall();
      e2dbgworld.curthread->was_step = 0;
      e2dbgworld.stoppedthread->count = E2DBG_BREAK_NONE;
      e2dbgworld.curbp                = NULL;
  
      /* Allow another breakpoint to be processed */
      if (e2dbg_mutex_unlock(&e2dbgworld.dbgbp) != 0)
	e2dbg_output(" [*] Debuggee Cannot unlock breakpoint mutex ! \n");
#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__ || __DEBUG_BP__)
      else
	{
	  e2dbg_output(" [*] BP MUTEX UNLOCKED [e2dbg_generic_breakpoint] \n");
	  e2dbg_output("<-------------------------------------\n");
	}
#endif
    }

#if (__DEBUG_THREADS__ || __DEBUG_E2DBG__ || __DEBUG_MUTEX__ || __DEBUG_BP__)
  else
    fprintf(stderr, " NOT CONTINUING BECAUSE STOPPED THREAD (%u) STATE COUNT = %u \n", 
	    (u_int) e2dbgworld.stoppedthread->tid, e2dbgworld.stoppedthread->count);
#endif

  e2dbgworld.stoppedthread->state = E2DBG_THREAD_RUNNING;
  e2dbg_presence_reset();
  SETSIG;

#if __DEBUG_BP__
  fprintf(stderr, " RETURNING FROM GENERIC SIGNAL HANDLER \n");
#endif
}

