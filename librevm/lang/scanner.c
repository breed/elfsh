/**
 * @file lexer.c
 * 
 * Started on  Fri Feb  7 20:53:25 2003 jfv
 * Updated on  Fri Mar  5 18:47:41 2007 jfv
 *
 * $Id: scanner.c,v 1.13 2007-08-03 11:51:00 heroine Exp $
 *
 */
#include "revm.h"


/**
 * Replace \xNUM by the value, I wished readline could have done that 
 */
void			revm_findhex(u_int argc, char **argv)
{
  u_int			index;
  char			*buf;
  char			*ptr;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* For each of the argv[] entry */
  for (index = 0; index < argc; index++)
    {
      if (argv[index] == NULL)
	continue;
      buf = argv[index];

      /* Find "\x" sequences */
      for (ptr = strstr(buf, "\\x"); ptr != NULL; ptr = strstr(buf, "\\x"))
	buf = revm_filter_param(buf, ptr);
    }
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * Translate a speblanks string 
 */
int			revm_trans_speblank(const char *in, char ***av, u_int *ac)
{
  char			**argv;
  u_int			argc = 1;
  u_int			index;
  u_int			count = 0;
  size_t		len;
  char			*ptr;
  char			str[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!str || !str[0])
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  snprintf(str, BUFSIZ - 1, "%s", in);
  len = strlen(str);

  /* Trim the string */
  while (str[count] == ' ')
    count++;

  /* Only spaces ? */
  if (count == len)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  if (count > 0)
    {
      memmove(str, str+count, len - count + 1);
      len = strlen(str);
    }

  /* Trim the end */
  while (str[len - 1] == ' ' && str[len - 2] != '\\')
  {
    str[len - 1] = 0x00;
    len--;
  }

  for (ptr = strchr(str, ' '); ptr != NULL; ptr = strchr(ptr+1, ' '))
    {
      /* Check if its a valid space */
      if ((ptr - str) > 0 && ptr[-1] != '\\')
	{
	  *ptr = 0x00;
	  argc++;
	}
    }

  XALLOC(__FILE__, __FUNCTION__, __LINE__, argv, sizeof(char*)*argc, -1);
  
  /* Copy each parts */
  for (index = 0, ptr = str; index < argc; index++, ptr += strlen(ptr)+1)
    argv[index] = strdup(ptr);

  /* Replace '\ ' */
  revm_replace_speblanks(argc, argv);

  if (av)
    *av = argv;

  if (ac)
    *ac = argc;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Replace '\ ' by the ' ', I wished readline could have done that 
 */
void			revm_replace_speblanks(u_int argc, char **argv)
{
  u_int			index;
  char			*buf;
  char			*ptr;
  size_t		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* For each of the argv[] entry */
  for (index = 0; index < argc; index++)
    {
      if (argv[index] == NULL)
	continue;
      buf = argv[index];
      len = strlen(buf);

      /* Find "\ " sequences */
      for (ptr = strstr(buf, "\\ "); ptr != NULL; ptr = strstr(ptr+1, "\\ "))
	{
	  *ptr = ' ';
	  memmove(ptr+1, ptr+2, len - (ptr - buf + 2));
	}
    }
   PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * Count blanks, so that we can allocate argv 
 */
u_int		revm_findblanks(char *buf)
{
  int		index;
  int		len;
  int		nbr;
  char		*ptr;
  char		*sav;
#if __DEBUG_SCANNER__
  char		logbuf[BUFSIZ];
#endif

  len = strlen(buf);
  index = 0;
  nbr = 1;
  sav = buf;
  do
    {
      while (IS_BLANK(*sav))
	sav++;
      if (!*sav)
	break;
      ptr = strchr(sav, ' ');
      if (!ptr)
	{
	  ptr = strchr(sav, '\t');
	  if (!ptr)
	    break;
	}
      while (IS_BLANK(*ptr))
	ptr++;
      if (!*ptr)
	break;
      nbr++;
      sav = ptr;
    }
  while (ptr && sav < buf + len);

#if __DEBUG_SCANNER__
  snprintf(logbuf, BUFSIZ - 1, "[DEBUG_SCANNER:findblanks] nbr = %u \n", nbr);
  revm_output(logbuf);
#endif

  return (nbr);
}


/** 
 * Cut words of the newline and create argv 
 */
char		**revm_doargv(u_int nbr, u_int *argc, char *buf)
{
  u_int		index;
  char		*sav;
  char		**argv;
  char		*ptr;
#if __DEBUG_SCANNER__
  char		logbuf[BUFSIZ];
#endif

  NOPROFILER_IN();

  XALLOC(__FILE__, __FUNCTION__, __LINE__,argv, sizeof(char *) * (nbr + 2), NULL);
  argv[0] = argv[nbr + 1] = NULL;
  sav = buf;

  for (index = 1; index < nbr + 1; index++)
    {

      assert(sav >= buf);

      while (IS_BLANK(*sav))
	sav++;
      argv[index] = sav;

#if __DEBUG_SCANNER__
      snprintf(logbuf, BUFSIZ - 1,
	       "[DEBUG_SCANNER:lexer_doargv] Adding argv[%u] = *%s* \n", 
	       index, sav);
      revm_output(logbuf);
#endif 

      ptr = strchr(sav, ' ');
      if (!ptr)
	ptr = strchr(sav, '\t');
      if (ptr)
	{
	  *ptr = 0;
	  sav = ptr + 1;
	}
    }

#if __DEBUG_SCANNER__
  revm_output("\n");
  for (index = 0; index < nbr + 1; index++)
    if (argv[index])
      {
	snprintf(logbuf, BUFSIZ - 1, "[DEBUG_SCANNER:lexer_doargv] %u/ *%s* \n",
		 index, argv[index]);
	revm_output(logbuf);
      }
  revm_output("\n");
#endif

  *argc = nbr + 1;

  NOPROFILER_ROUT(argv);
}








/* Its lighter than flex ... */
char		**revm_input(int *argc)
{
  char		**argv;
  char		*buf;
  int		nbr;
  int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Read, and sanitize a first time to avoid blank lines and various special returns */
  buf = revm_getln();
  if ((int) buf == 0 || 
      (int) buf == REVM_INPUT_VOID || 
      (int) buf == REVM_INPUT_EXIT || 
      (int) buf == REVM_INPUT_TRANSFERED)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (char **) buf);

  /* Log the read line */
  if (world.state.revm_mode != REVM_STATE_SCRIPT &&
      world.curjob->ws.io.type == REVM_IO_STD)
    {
#if defined(USE_READLN)
      readln_input_log(buf);
#else
      revm_log(buf);
      revm_log("\n\n");
#endif
    }

  /* Save the line for future references */
  if (world.curjob->ws.oldline && *world.curjob->ws.oldline)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, 
	    world.curjob->ws.oldline);
      world.curjob->ws.oldline = NULL;
    }

  len = strlen(buf);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,
	 world.curjob->ws.oldline, len + 1, NULL);
  strcpy(world.curjob->ws.oldline, buf);

  /* If we are in the client part of the debugger, 
     we have a special behavior */
  if (world.state.revm_mode == REVM_STATE_DEBUGGER && 
      world.state.revm_side == REVM_SIDE_CLIENT)
    {
      write(world.fifo_c2s, buf, len);
#if defined(USE_READLN)
      write(world.fifo_c2s, "\n", 1);
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		    ((char **) REVM_INPUT_TRANSFERED));      
    }

  /* Allocate the correct pointer array for argv */
  nbr = revm_findblanks(buf);
  argv = revm_doargv(nbr, (u_int *) argc, buf);

  /* Find and replace "\xXX" sequences, then return the array */
  revm_findhex(*argc, argv);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (argv));
}
