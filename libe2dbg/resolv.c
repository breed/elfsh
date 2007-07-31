/*
** resolv.c for e2dbg
**
** Contains the linkmap related resolving builtins in e2dbg
**
** Last Update Thu Oct 13 19:37:26 2005 mm
**
** $Id: resolv.c,v 1.16 2007-07-31 23:30:35 may Exp $
**
*/
#include "libe2dbg.h"

#include <dlfcn.h>

/* Reference symbol for the debugger */
int			reference = 42;


/* Our own dlsect without malloc */
/* Useful in the early stage of mapping when malloc symbol is not yet known */
elfsh_Addr		e2dbg_dlsect(char *objname, char *sect2resolve, 
				     elfsh_Addr refaddr, char *refsym)
{
  e2dbgobj_t		obj;
  elfsh_Phdr		*pht;
  elfsh_Dyn		*dyn;
  u_int			nbr, nbr2;
  elfsh_Sym		cursym;
  char			*strtab;
  elfsh_Addr		got;
  u_int			curoff;
  elfsh_Addr		found_ref = 0;

#if __DEBUG_E2DBG__
  char		buf[BUFSIZ];
  u_int		len;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  write(2, " [*] e2dbg_dlsect called for resolving ", 39);
  write(2, sect2resolve, strlen(sect2resolve));
  write(2, " in ", 4);
  write(2, objname, strlen(objname));
  write(2, "\n", 1);
#endif

  memset(&obj, 0x00, sizeof(obj));
  XOPEN(obj.fd, objname, O_RDONLY, 0, 0);
  XREAD(obj.fd, &obj.e, sizeof(elfsh_Ehdr), 0);

#if __DEBUG_E2DBG__
  write(2, " [*] 1", 6);
#endif

  XSEEK(obj.fd, obj.e.e_phoff, SEEK_SET, 0);
  pht = alloca(obj.e.e_phnum * sizeof(elfsh_Phdr));

#if __DEBUG_E2DBG__
  write(2, "2", 1);
#endif

  XREAD(obj.fd, pht, obj.e.e_phnum * sizeof(elfsh_Phdr), 0);
  for (nbr = 0; nbr < obj.e.e_phnum; nbr++)
    if (pht[nbr].p_type == PT_DYNAMIC)
      {
	obj.dynoff = pht[nbr].p_offset;
	break;
      }

#if __DEBUG_E2DBG__
  write(2, "3", 1);
#endif

  XSEEK(obj.fd, obj.dynoff, SEEK_SET, 0);
  dyn = alloca(pht[nbr].p_filesz);

#if __DEBUG_E2DBG__
  write(2, "4", 1);
#endif

  XREAD(obj.fd, dyn, pht[nbr].p_filesz, 0);
  for (nbr2 = 0; nbr2 < pht[nbr].p_filesz / sizeof(elfsh_Dyn); nbr2++)
    {
      if (dyn[nbr2].d_tag == DT_SYMTAB)
	obj.symoff = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_STRTAB)
	obj.stroff = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_STRSZ)
	obj.strsz = dyn[nbr2].d_un.d_val;
      else if (dyn[nbr2].d_tag == DT_PLTGOT)
	got = (elfsh_Addr) dyn[nbr2].d_un.d_val;
    }

#if __DEBUG_E2DBG__
  write(2, "5", 1);
#endif
 
  strtab = alloca(obj.strsz);
  if (!strtab)
    {
      write(2, " Failed to alloca string table\n", 31);
      return (-1);
    }
  if (!obj.symoff)
    {
     write(2, " Unable to find SYMOFF from PT_DYNAMIC\n", 39);
     return (-1);
    }

  if (!obj.stroff)
    {
      write(2, " Unable to find STROFF rom PT_DYNAMIC\n", 39);
      return (-1);
    }

  if (!obj.strsz)
    {
      write(2, " Unable to find STRSZ from PT_DYNAMIC\n", 39);
      return (-1);
    }
  
  if (!got)
    {
      write(2, " Unable to find PLTGOT from PT_DYNAMIC\n", 39);
      return (-1);
    }
 
#if __DEBUG_E2DBG__
  write(1, "6", 1);
#endif

  XSEEK(obj.fd, obj.stroff, SEEK_SET, 0);
  XREAD(obj.fd, strtab, obj.strsz, 0);

#if __DEBUG_E2DBG__
  write(1, "7", 1);
#endif

  /* XXX: Assume that strtab is always just after symtab */
  for (curoff = 0; obj.symoff + curoff < obj.stroff; curoff += sizeof(elfsh_Sym))
    {
      XSEEK(obj.fd, obj.symoff + curoff, SEEK_SET, 0);
      XREAD(obj.fd, &cursym, sizeof(elfsh_Sym), 0);
      if (cursym.st_name >= obj.strsz)
	continue;
      if (!strcmp(strtab + cursym.st_name, refsym))
	found_ref = cursym.st_value;
    }

#if __DEBUG_E2DBG__
  write(2, "8", 1);
#endif

  if (!found_ref)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to find reference symbol in object", 0);

#if __DEBUG_E2DBG__
  write(2, " Success !\n", 11);
  len = snprintf(buf, sizeof(buf), 
		 " [*] REFADDR = %08X and FOUNDREF = %08X and GOT = %08X \n", 
		 refaddr, found_ref, got);
  write(2, buf, len);
#endif

  /* Close the file */
  XCLOSE(obj.fd, 0);

  /* The reference addr is useful to deduce library base addresses */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     got + refaddr - found_ref);
}



/* Our own dlsym without malloc */
/* Useful in the early stage of mapping when malloc symbol is not yet known */
elfsh_Addr		e2dbg_dlsym(char *sym2resolve)
{
  e2dbgobj_t		obj;
  elfsh_Dyn		*dyn;
  u_int			nbr2;
  elfsh_Sym		cursym;
  char			*strtab;
  u_int			curoff;
  elfsh_Addr		found_sym = 0;
  elfshlinkmap_t	*curobj;
  elfsh_Ehdr		hdr;

#if __DEBUG_E2DBG__
  char		buf[BUFSIZ];
  u_int		len;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First go after the e2dbg and the binary linkmap entries */
  curobj = e2dbgworld.syms.map;

  /* Come back at the beginning of the linkmap */
  while (curobj->lprev)
    curobj = curobj->lprev;

  /* Iterate on the linkmap to resolve symbols in library priority order */
  for (; curobj; curobj = curobj->lnext)
    {

      if (!curobj->lname || !*curobj->lname || strstr(curobj->lname, "e2dbg"))
	continue;
      memset(&obj, 0x00, sizeof(obj));
      XOPEN(obj.fd, curobj->lname, O_RDONLY, 0, 0);
      XREAD(obj.fd, &hdr, sizeof(hdr), 0);
      if (hdr.e_type != ET_DYN)
	{
	  XCLOSE(obj.fd, 0);
	  continue;
	}

#if __DEBUG_E2DBG__
      write(2, " [*] e2dbg_dlsym called for resolving ", 38);
      write(2, sym2resolve, strlen(sym2resolve));
      write(2, " in ", 4);
      write(2, curobj->lname, strlen(curobj->lname));
      write(2, "\n", 1);
#endif

      dyn = curobj->lld;

      /* Getting needed address in the memory mapped PT_DYNAMIC segment */
      for (nbr2 = 0; dyn[nbr2].d_tag != DT_NULL; nbr2++)
	{
	  if (dyn[nbr2].d_tag == DT_SYMTAB)
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(sun)
	    obj.symoff = curobj->laddr + dyn[nbr2].d_un.d_val;
#else
	    obj.symoff = dyn[nbr2].d_un.d_val;
#endif
	  else if (dyn[nbr2].d_tag == DT_STRTAB)
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(sun)
	    obj.stroff = curobj->laddr + dyn[nbr2].d_un.d_val;
#else
	    obj.stroff = dyn[nbr2].d_un.d_val;
#endif
	  else if (dyn[nbr2].d_tag == DT_STRSZ)
	    obj.strsz = dyn[nbr2].d_un.d_val;
	}
      strtab = (char *) obj.stroff;

      /* Checking if everything is ok */
      if (!obj.symoff)
	{
	  write(2, " Unable to find SYMOFF from PT_DYNAMIC\n", 39);
	  return (-1);
	}
      
      if (!obj.stroff)
	{
	  write(2, " Unable to find STROFF rom PT_DYNAMIC\n", 39);
	  return (-1);
	}
      
      if (!obj.strsz)
	{
	  write(2, " Unable to find STRSZ from PT_DYNAMIC\n", 39);
	  return (-1);
	}
      
#if __DEBUG_E2DBG__
      len = snprintf(buf, sizeof(buf), 
		     " [*] SYMOFF = %u (%08X), STROFF = %u (%08X), STRSZ = %u \n",
		     obj.symoff, obj.symoff, obj.stroff, obj.stroff, obj.strsz);
      write(2, buf, len);
#endif

      XCLOSE(obj.fd, 0);
      
      /* XXX: Assume that dynamic strtab is always just after dynamic symtab */
      for (curoff = 0; obj.symoff + curoff < obj.stroff; curoff += sizeof(elfsh_Sym))
	{
	  memcpy(&cursym, (void *)obj.symoff + curoff, sizeof(elfsh_Sym));
	  if (cursym.st_name >= obj.strsz)
	    continue;
	  if (!strcmp(strtab + cursym.st_name, sym2resolve) && 
	      cursym.st_value)
	    {
	      found_sym = cursym.st_value;
      
#if __DEBUG_E2DBG__
	      len = snprintf(buf, sizeof(buf), 
			     " [*] FOUNDSYM (%s) = %08X \n", 
			     strtab + cursym.st_name, found_sym);
	      write(2, buf, len);
#endif

	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
				 curobj->laddr + found_sym);
	    }
	}
    }

  /* We did not find the symbol in the linkmap ... */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to find looked up symbol in object", 0);
}



/* Get the address of the linkmap without any malloc, for early stage resolving */
elfshlinkmap_t*		e2dbg_linkmap_getaddr()
{
  elfsh_Addr		baseaddr;
  char			path[BUFSIZ];
  elfsh_Addr		*got;
  elfshlinkmap_t	*lm;
  char			*version;
#if defined(__FreeBSD__) || defined(__NetBSD__)
  Obj_Entry		*oe;
#elif defined(sun)
  Dl_info		info;
#endif


#if __DEBUG_E2DBG__
  char      buf[BUFSIZ];
  u_int     len;
#endif	

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] e2dbg_linkmap_getaddr called\n");
  write(2, buf, len);
#endif

#if defined(ERESI32)
  version = "32";
#elif defined(ERESI64)
  version = "64";
#else
  printf("Unknown debugger version : fatal error \n");
  exit(-1);
#endif

  snprintf(path, BUFSIZ, "%s/libe2dbg%s.so", ELFSH_DBGPATH, version);

#if defined(linux)
  baseaddr = e2dbg_dlsect(path, ".got.plt", 
			  (elfsh_Addr) &reference, "reference");
#else
  baseaddr = e2dbg_dlsect(path, ".got", 
			  (elfsh_Addr) &reference, "reference");
#endif

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] Base address - 1st = %08X\n", baseaddr);
  write(2, buf, len);
#endif

  got = (elfsh_Addr *) baseaddr;

#if __DEBUG_E2DBG__
  len = sprintf(buf, " [*] GOT address = %08X\n", (elfsh_Addr) got);
  write(2, buf, len);
#endif

  /* BSD and Solaris have an intermediate structure between GOT[1] and the linkmap entry */
#if defined(__FreeBSD__) || defined(__NetBSD__)
  oe = (Obj_Entry *) got[1];
  lm = (elfshlinkmap_t *) &oe->linkmap;
#elif defined(sun)
  dladdr1(&reference, &info, (void **) &lm, RTLD_DL_LINKMAP);
#else
  lm   = (elfshlinkmap_t *) got[1];
#endif

#if __DEBUG_E2DBG__
  len = sprintf(buf, 
		" [*] Guessed Linkmap address = %08X \n--------------\n", 
		(elfsh_Addr) lm);
  write(2, buf, len);
#endif

  return (lm);
}



/* Resolve malloc/realloc/free from standard libc */
int		e2dbg_dlsym_init()
{
  static int	done = 0;
  u_char	dbgmode;

#if !defined(__FreeBSD__)
  void		*handle;
#endif

#if __DEBUG_E2DBG__
  char		buf[BUFSIZ];
  u_int		len;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_E2DBG__
  write(2, " [D] e2dbg_dlsym_init CALLED\n", 29);
#endif

  if (done)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));

#if __DEBUG_E2DBG__  
  write(2, " [D] e2dbg_dlsym_init EXECUTED\n", 31);
#endif

  /* Get the address of the linkmap without calling malloc */
  e2dbgworld.syms.map = e2dbg_linkmap_getaddr();

  /* Only use our own dlsym here, do not use the libc handler */
  e2dbgworld.syms.mallocsym = (elfsh_Addr) e2dbg_dlsym("malloc");
  if (!e2dbgworld.syms.mallocsym)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig malloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc MALLOC() sym = %08X \n", 
		 e2dbgworld.syms.mallocsym);
  write(2, buf, len);
#endif

  e2dbgworld.syms.callocsym = (elfsh_Addr) e2dbg_dlsym("calloc");
  if (!e2dbgworld.syms.callocsym)
    {
      dlerror();
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Orig calloc not found", (-1));
    }

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc CALLOC() sym = %08X \n", 
		 e2dbgworld.syms.callocsym);
  write(2, buf, len);
#endif

  e2dbgworld.syms.reallocsym = (elfsh_Addr) e2dbg_dlsym("realloc");
  if (!e2dbgworld.syms.reallocsym)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig realloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc REALLOC() sym = %08X \n", 
		 e2dbgworld.syms.reallocsym);
  write(2, buf, len);
#endif

  e2dbgworld.syms.freesym = (elfsh_Addr) e2dbg_dlsym("free");
  if (!e2dbgworld.syms.freesym)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig free not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc FREE() sym = %08X \n", e2dbgworld.syms.freesym);
  write(2, buf, len);
#endif

  e2dbgworld.syms.vallocsym = (elfsh_Addr) e2dbg_dlsym("valloc");
  if (!e2dbgworld.syms.vallocsym)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Orig valloc not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc VALLOC() sym = %08X \n", 
		 e2dbgworld.syms.vallocsym);
  write(2, buf, len);
#endif

  e2dbgworld.syms.signal        = (elfsh_Addr) e2dbg_dlsym("signal");
  if (!e2dbgworld.syms.signal)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Orig signal not found", (-1));

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] Libc SIGNAL() sym = %08X \n", 
		 e2dbgworld.syms.signal);
  write(2, buf, len);
#endif


  /* Non fatal symbols, especially on BSD */
  e2dbgworld.syms.memalignsym = (elfsh_Addr) e2dbg_dlsym("memalign");
  e2dbgworld.syms.pthreadcreate = (elfsh_Addr) e2dbg_dlsym("pthread_create");
  e2dbgworld.syms.pthreadexit   = (elfsh_Addr) e2dbg_dlsym("pthread_exit");

  /* Now we can use malloc cause allocation symbols are resolved */
  /* The allocator proxy will select between legit or alternative syms */
  done = 1;
  dbgmode = e2dbg_presence_get();
  e2dbg_presence_set();
  hash_init(&e2dbgworld.threads, "threads"    , 29, ASPECT_TYPE_UNKNOW);
  hash_init(&e2dbgworld.bp     , "breakpoints", 51, ASPECT_TYPE_UNKNOW);

  /* If we are dealing with a PIE binary, compute his base address using 
     _end. This symbol is always defined in dynamic binaries, even when 
     stripped. It is also important that the chosen symbol points in the 
     data PT_LOAD, since each PT_LOAD have a different base address and 
     we want the one that contains the GOT section, which itself contains 
     the linkmap address in its second entry */
#if !defined(__FreeBSD__)
  handle = dlopen(NULL, RTLD_LAZY);
  e2dbgworld.syms.piebase  = (elfsh_Addr) dlsym(handle, "_end");
  dlclose(handle);
#endif

  if (!dbgmode)
    e2dbg_presence_reset();

#if __DEBUG_E2DBG__
  len = snprintf(buf, sizeof(buf), 
		 " [*] PIE _end resolved = %08X \n", 
		 e2dbgworld.syms.piebase);
  write(2, buf, len);
#endif

#if __DEBUG_E2DBG__
  write(2, " [D] e2dbg_dlsym_init FINISHED\n", 31);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}

