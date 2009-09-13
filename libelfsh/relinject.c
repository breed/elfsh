/**
* @file libelfsh/relinject.c
 * @ingroup libelfsh
** relinject.c for libelfsh
** 
** This file contains all functions for relocating a ET_REL
** object. It calls the relocation hook for keeping an abstraction
** on various routines.
** 
** BeoS fixes by zadig
** 
** Started on  Fri Mar 28 14:55:37 2003 jfv
** 
**
** $Id$
**
*/
#include "libelfsh.h"



/**
 * Perform relocation on entry (Now use ELFsh 0.6 hooks model) 
 * @param new
 * @param reloc
 * @param dword
 * @param addr
 * @param mod
 * @return
 */
static int      elfsh_relocate_entry(elfshsect_t        *enew,
                                     void               *reloc,
				     eresi_Addr		*dword,
                                     eresi_Addr         addr,
				     elfshsect_t	*mod)
{
  int		ret;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = elfsh_rel(enew->parent, enew, reloc, dword, addr, mod);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to relocate object", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/**
 * Find the host symbol we rely on for performing the relocation 
 * @param enew
 * @param reltab
 * @param sym
 * @param name
 * @param stage
 * @param symtype
 * @return
 */
static int	elfsh_find_relocsym(elfshsect_t *enew, elfshsect_t *reltab,
				    elfsh_Sym **sym, char *name, char stage, 
				    elfsh_Half symtype)
     
{
  elfshobj_t	*dep;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* If this symbol is a old_ one, accept it to be relocated in 2nd stage */
  /* This is because function redirection may be done after ET_REL injection */
  if (strstr(name, "old_") && stage == ELFSH_RELOC_STAGE1)
    {
#if __DEBUG_RELADD__
      printf("[DEBUG_RELADD] %s symbol not found at RELOC_STAGE1, continue\n", name);
#endif
      if (enew->parent->nbrel < ELFSH_MAXREL)
	{
	  if (enew->parent->listrel[enew->parent->nbrel] != reltab->parent)
	    {

	      enew->parent->listrel[enew->parent->nbrel++] = reltab->parent;
#if __DEBUG_RELADD__
	      printf("[DEBUG_RELADD] %s object relocation will have a second stage\n", 
		     reltab->parent->name);
#endif
	    }
	  
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
	}
    }


  /* We have a different behavior depending on the symbol type */
  switch (symtype)
    {
      /* The symbol is not found so we request a enew PLT entry for it */
    case STT_NOTYPE:
      if (!elfsh_static_file(enew->parent))
	{
	  *sym = elfsh_request_pltent(enew->parent, name);
	  if (*sym)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
	}
      else
	{
#if	__DEBUG_STATIC__	      
	  printf("STT_NOTYPE in static file while relocating %s\n", reltab->parent->name);
#endif

	  dep = elfsh_find_obj_by_symbol(name);
	  
	  /* no symbol found */
	  if (dep == ((void *) -1))
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				"Unable to inject ET_REL dependence", -1);

	  /* Use stage 2 relocation as the best et_rel is beeing inserted */
	  if (dep == NULL)
	    {
#if	__DEBUG_STATIC__	      
	      printf("[DEBUG_STATIC] Loop in dependency detected -> STAGE2 relocation\n");
#endif
	      if (enew->parent->nbrel < ELFSH_MAXREL &&
		  enew->parent->listrel[enew->parent->nbrel] != reltab->parent)
		{
		  enew->parent->listrel[enew->parent->nbrel++] = reltab->parent;
#if	__DEBUG_STATIC__
		  printf("[DEBUG_STATIC] %s object relocation will have a second stage\n", reltab->parent->name);
#endif
		  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
		}
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				"Unable to add stage 2 relocation", -1);
	    }

	  /* symbol found, gonna try to inject et_rel */
	  if (dep != NULL && dep != reltab->parent && elfsh_inject_etrel(enew->parent, dep) < 0)
	    {
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				"Unable to inject ET_REL dependence", -1);
	    }
	  
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (2));

	}
      break;
    case STT_SECTION:
      printf("STT_SECTION\n");
      break;
    case STT_FUNC:
      printf("STT_FUNC\n");
      break;
    case STT_OBJECT:
      printf("STT_OBJECT\n");
      break;
    case STT_COMMON:
      printf("STT_COMMON\n");
      break;
    case STT_BLOCK:
      printf("STT_BLOCK\n");
    }

  
#if __DEBUG_STATIC__
  printf("[DEBUG_STATIC] Not found after OLD check : sym = %s \n", name);
#endif  

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Cant find requested symbol in ET_EXEC\n", -1);
}
  



/**
 * Relocate the just injected section 
 * @param enew
 * @param reltab
 * @param stage
 * @return
 */
static int	elfsh_relocate_etrel_section(elfshsect_t	*enew,
					     elfshsect_t	*reltab,
					     u_char		stage)
{
  elfsh_Rel	*cur;
  volatile u_int		index;
  elfsh_Sym	*sym;
  volatile u_int		size;
  eresi_Addr	*dword;
  eresi_Addr   	addr;
  char		*name;
  char		tmpname[BUFSIZ];
  elfshsect_t	*sect;
  u_int		entsz;
  elfshsect_t   *plt;
  void		*data;
  elfsh_Half	symtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  /* ET_REL object is not mapped we use unconditionaly 
     the ondisk relocation tables for such operation */
  data = reltab->data;

#if __DEBUG_RELADD__
  fprintf(stderr, "[DEBUG_RELADD] Using reloc table from %s [%s] data at %p \n",
	  reltab->parent->name, reltab->name, data);
#endif

  /* Loop on the relocation table entries */
  size = (reltab->shdr->sh_type == SHT_RELA ?
	  sizeof(elfsh_Rela) : sizeof(elfsh_Rel));
  size = reltab->shdr->sh_size / size;
  
  plt = elfsh_get_plt(enew->parent, NULL);
  if (NULL == plt && elfsh_dynamic_file(enew->parent))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get plt", -1);

  entsz = elfsh_get_pltentsz(enew->parent);
  if (entsz < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get pltentsz", -1);

  for (index = 0; index < size; index++)
    {

#if __DEBUG_RELADD__
      fprintf(stderr, "[DEBUG_RELADD] relocation loop stage %u for section %s index %u \n", 
	      stage, enew->name, index);
#endif

      /* We try a enew relocation now that the ET_REL dependence is mapped */
    retry:

      /* Get symbol value in ET_REL */
      cur = (reltab->shdr->sh_type == SHT_RELA ?
	     (void *) (((elfsh_Rela *) data) + index) :
	     (void *) (((elfsh_Rel  *) data) + index));
      sym  = elfsh_get_symbol_from_reloc(reltab->parent, cur);
      name = elfsh_get_symname_from_reloc(reltab->parent, cur);

      if (sym == NULL || name == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to find symbol in ET_REL", -1);

      /* Grab a pointer on the dword that need to be relocated */
      dword = (eresi_Addr *) ((char *) elfsh_readmem(enew) + cur->r_offset);

      /*
      ** If symbol type is NOTYPE, we use ET_EXEC symtab, else if
      ** symbol link is COMMON, we use ET_REL symbol inserted in ET_EXEC
      ** during BSS sizescan in bss.c:elfsh_find_bsslen()
      */
      symtype = elfsh_get_symbol_type(sym);
      if (elfsh_get_symbol_bind(sym) != STB_LOCAL && /* patch BEOS */
	  (symtype == STT_NOTYPE || elfsh_get_symbol_link(sym) == SHN_COMMON))
	{
	  if (stage == ELFSH_RELOC_STAGE2 && !strstr(name, "old_"))
	    continue;

	  /* If the symbol is not found and we are still in
	     the first stage relocation, just pass it */
	  sym = elfsh_get_metasym_by_name(enew->parent, name);
	  if (!sym) 
	    {
	      switch (elfsh_find_relocsym(enew, reltab, &sym, name, stage, symtype))
		{
		case 2:
#if	__DEBUG_STATIC__
		  fprintf(stderr, "[DEBUG_STATIC] RETRY\n");
#endif
		  goto retry;
		  break;
		case 0:
		  continue;
		case 1:
		  break;
		case -1:
		default:
		  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			       "Unable to satisfy symbol in ET_REL", -1);
		}
	    }
	  
	  addr = sym->st_value;
 
#if	__DEBUG_RELADD__
	  fprintf(stderr, "[DEBUG_RELADD] Relocate using existing symbol %-20s " AFMT "]\n",
		  name, (eresi_Addr) addr);
#endif

	}


      /* Compute addr giving the injected section's vaddr in ET_EXEC */
      else
	{

	  /* All the following relocs are computed in stage 1 */
	  if (stage == ELFSH_RELOC_STAGE2)
	    continue;

	  /* Find target section in ET_REL */
	  sect = elfsh_get_section_by_index(reltab->parent, sym->st_shndx,
					    NULL, NULL);
	  if (sect == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Cant find extracted section in ET_REL", -1);

#if	__DEBUG_RELADD__
	  fprintf(stderr, "[DEBUG_RELADD] Found -%s- section (idx = %u), now looking at "
		  "injected base address\n", sect->name, sect->index);
#endif

	  /* Find corresponding inserted section in ET_EXEC */
	  snprintf(tmpname, sizeof(tmpname), "%s%s", reltab->parent->name, sect->name);
	  sect = elfsh_get_section_by_name(enew->parent, tmpname, NULL, NULL, NULL);
	  
	  if (sect == NULL)
	    {

#if   	__DEBUG_RELADD__
	      elfsh_print_sectlist(reltab->parent, "HEH");
	      fprintf(stderr, "[DEBUG_RELADD] Did not found %s section (sym = %s) \n", 
		      tmpname, name);
#endif
 
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Cant find inserted section in ET_EXEC", -1);
	    }
	  
	  /* Compute pointer value */
	  addr = sect->shdr->sh_addr;
	  addr += ((elfsh_get_symbol_type(sym) == STT_SECTION &&
		    !FILE_IS_SPARC(sect->parent) &&
		    !FILE_IS_ALPHA64(sect->parent) &&
		    !FILE_IS_MIPS(sect->parent)) ? 
		   *dword : sym->st_value);

#if __DEBUG_RELADD__
	  fprintf(stderr, "[DEBUG_RELADD] Relocate using section %-20s base [-> " AFMT "] \n",
		  sect->name, (eresi_Addr) addr);
#endif


	}

      /* Perform relocation */
      if (elfsh_relocate_entry(enew, cur, dword, addr, reltab) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unable to relocate entry", -1);

    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Relocate the object 
 * @param file
 * @param rel
 * @param stage
 * @return
 */
int		elfsh_relocate_object(elfshobj_t *file, elfshobj_t *rel, u_char stage)
{
  elfshsect_t	*sect;
  elfshsect_t	*reltab;
  char		sctname[BUFSIZ];
  u_int		index;
  u_int		found;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Last pass : relocate each inserted section */
  for (found = index = 0; index < rel->hdr->e_shnum; index++)
    {
      sect = elfsh_get_section_by_index(rel, index, NULL, NULL);
      if (sect == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cant get section in ET_REL",  -1);

      /* Check if the section is mapped */
      if (elfsh_get_section_allocflag(sect->shdr) && sect->shdr->sh_size &&
	  sect->shdr->sh_type == SHT_PROGBITS)
	{

	  /* Find the associate relocation section */
	  snprintf(sctname, sizeof(sctname), "%s%s",
		   (IS_REL(sect) ? ".rel" : ".rela"), sect->name);
	  reltab = elfsh_get_section_by_name(rel, sctname, NULL, NULL, NULL);
	  if (reltab == NULL)
	    continue;
	  found++;

	  /* Find the injected instance of this allocatable section in the ET_EXEC */
	  snprintf(sctname, sizeof(sctname), "%s%s", sect->parent->name, sect->name);
	  sect = elfsh_get_section_by_name(file, sctname, NULL, NULL, NULL);
	  if (sect == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cant get section in ET_EXEC",  -1);
	  if (elfsh_relocate_etrel_section(sect, reltab, stage) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to relocate section", -1);
	}
    }

  /* Note that we might have done no relocation if no table was available */
  /* This can happen on very simple .o files */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Inject a section from ET_REL object into ET_EXEC 
 * @param file
 * @param sect
 * @param mod
 * @return
 */
static int	elfsh_inject_etrel_section(elfshobj_t *file, elfshsect_t *sect, u_int mod)
{
  elfsh_Shdr	hdr;
  elfshsect_t	*enew;
  char		*newname;
  char		writable;
  int		mode;
  char		*data;
  u_int		modulo;
  elfshsect_t	*plt;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_dynamic_file(file) && NULL == (plt = elfsh_get_plt(file, NULL)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get PLT", -1);

  /* else create a new section */
  hdr = elfsh_create_shdr(0, sect->shdr->sh_type, sect->shdr->sh_flags,
			  0, 0, sect->shdr->sh_size, 0, 0, 0, 0);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,newname, strlen(sect->parent->name) + strlen(sect->name) + 2, -1);
  sprintf(newname, "%s%s", sect->parent->name, sect->name);
  enew = elfsh_create_section(newname);
  
  /* Copy the data */
  XALLOC(__FILE__, __FUNCTION__, __LINE__,data, sect->shdr->sh_size, -1);
  memcpy(data, sect->data, sect->shdr->sh_size);

  /* Inject new section by top or after bss depending on its type */
  writable = elfsh_get_section_writableflag(sect->shdr);

  /* FreeBSD is incompatible with pre-interp injection */
  ELFSH_SELECT_INJECTION(file,writable,mode);

   if (mode == ELFSH_DATA_INJECTION)
      modulo = sizeof(eresi_Addr);
   else
     {
       /* modulo = mod; (to be uncommented one day) */
       //modulo = elfsh_get_pagesize(file);
       modulo = sizeof(eresi_Addr);
     }

#if	__DEBUG_RELADD__
  printf("[DEBUG_RELADD] Mapping new section %s with data = %p \n", enew->name, data);
#endif

  if (elfsh_insert_mapped_section(file, enew, hdr, data, mode, modulo) < 0)
    goto bad;
  enew = elfsh_get_section_by_name(file, newname, NULL, NULL, NULL);
  if (enew == NULL)
    goto bad;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
 bad:
  XFREE(__FILE__, __FUNCTION__, __LINE__,newname);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to inject ET_REL section", -1);
}

/**
 * The intermediate pass of theglobal algorithm for ET_REL injection
 * We fuze symbol tables from the ET_REL and the host binary 
 *
 * @param file
 * @param rel
 * @return
 */
int		elfsh_fuse_etrel_symtab(elfshobj_t *file, elfshobj_t *rel)
{
  elfshsect_t	*sect;
  elfsh_Sym	newsym;
  elfsh_Half	type;
  u_int		index;
  char		sctname[BUFSIZ];
  elfsh_Sym	*sym;
  int		symnbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  sym = elfsh_get_symtab(rel, &symnbr);
  for (index = 0; index < symnbr; index++)
    {
      type = elfsh_get_symbol_type(sym + index);

      /* Avoid non-injectable symbols */
      if (type != STT_FUNC && type != STT_OBJECT)
	continue;

      if (sym[index].st_shndx >= rel->hdr->e_shnum)
	continue;

      /* Find target section in ET_REL */
      sect = elfsh_get_section_by_index(rel, sym[index].st_shndx, NULL, NULL);
      if (sect == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cant find extracted section in ET_REL\n", -1);

      /* Filter symbols using source section */
      if (sect->shdr->sh_type != SHT_PROGBITS || !sect->shdr->sh_size ||
	  !elfsh_get_section_allocflag(sect->shdr))
	continue;

      /* Find corresponding inserted section in ET_EXEC */
      snprintf(sctname, sizeof(sctname), "%s%s", rel->name, sect->name);
      sect = elfsh_get_section_by_name(file, sctname, NULL, NULL, NULL);
      if (sect == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cant find inserted section in ET_EXEC\n", -1);

#if __DEBUG_RELADD__
      printf("[DEBUG_RELADD] Injected ET_REL symbol %-20s ["XFMT"] \n",
	     elfsh_get_symbol_name(rel, sym + index),
	     (eresi_Addr) (sect->shdr->sh_addr + sym[index].st_value));
#endif

      /* Add symbol in host file */
      newsym = elfsh_create_symbol(sect->shdr->sh_addr + sym[index].st_value,
				   sym[index].st_size,
				   elfsh_get_symbol_type(sym + index),
				   elfsh_get_symbol_bind(sym + index),
				   0, sect->index);

      if (elfsh_insert_symbol(file->secthash[ELFSH_SECTION_SYMTAB], &newsym,
			      elfsh_get_symbol_name(rel, sym + index)) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to insert ET_REL symbol", -1);
    }

  /* Resynchronize sorted instances of symbol table */
  if (elfsh_sync_sorted_symtab(file->secthash[ELFSH_SECTION_SYMTAB]) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to synchronize host symtab", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Inject ET_REL bss inside host BSS 
 *
 * @param file
 * @param rel
 * @return
 */
int		elfsh_fuse_bss(elfshobj_t *file, elfshobj_t *rel)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_insert_runtime_bss(file, rel) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert runtime BSS", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Inject a ET_REL object into a ET_EXEC object 
 * @param file
 * @param rel
 * @return
 */
int		elfsh_inject_etrel(elfshobj_t *file, elfshobj_t *rel)
{
  u_int		mod;
  u_int		pgsize;
  u_int		index;
  elfshsect_t	*sect;
  elfshsect_t	*hooks;
  int		ret = 0;
  static int	depth = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || file->hdr == NULL || rel == NULL || rel->hdr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (rel->hdr->e_type != ET_REL ||
      (file->hdr->e_type != ET_EXEC && file->hdr->e_type != ET_DYN))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Bad parameter types", -1);

#if	__DEBUG_RELADD__
  if (rel->pending)
    {
      printf("[DEBUG_RELADD] BUG BUG BUG \n");
      exit(0);
    }
  printf("[DEBUG_RELADD] INJECTING %s in %s (depth %d)\n", 
	 rel->name, 
	 file->name, 
	 depth++);
#endif


  /* Set pending injection flag */
  rel->pending = 1;
  
  /* If not already done */
  elfsh_setup_hooks();

  /* First physically insert all BSS in the file and fuse 
  ** the module's BSS with the last one */
  if (elfsh_fuse_bss(file, rel) < 0)
    {
      rel->pending = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Cant fuze BSS sections", -1);
    }

  /* First pass : find and inject all allocatable sections */
  for (index = 0; index < rel->hdr->e_shnum; index++)
    {
      
      /* Get the current section */
      sect = elfsh_get_section_by_index(rel, index, NULL, NULL);
      if (sect == NULL)
	{
	  rel->pending = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Cant read section in ET_REL",  -1);
	}
      
      /* Check if the current section need to be mapped */
      if (elfsh_get_section_allocflag(sect->shdr) &&
	  sect->shdr->sh_size && sect->shdr->sh_type == SHT_PROGBITS)
	{
	  mod = 0;
	  
	  if (elfsh_inject_etrel_section(file, sect, mod) < 0) 
	    {
	      rel->pending = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				"Unable to inject ET_REL section", -1);
	    }
	}
    }
  
  /* compute the inject modulo */
  mod = elfsh_get_pagesize(file);
  //mod = sizeof(eresi_Addr);
      
  /* Do a copy of the procedure linkage table for eventual redirection */
  if (!elfsh_static_file(file) && elfsh_copy_plt(file, mod) < 0)
    {
      rel->pending = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Unable to copy PLT", -1);
    }

  /* Create an additional hook table for non-plt function redirection */
  hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 0, 0, 0); 
  if (!hooks)
    {
      int mode;

      /* get injection mode */
      ELFSH_SELECT_INJECTION(file,NULL,mode);

      pgsize = elfsh_get_pagesize(file);
      pgsize *= 4; /* We need a lot more than a page to trace big binaries like ssh */
      hooks = elfsh_insert_section(file, 
				   ELFSH_SECTION_NAME_HOOKS, 
				   NULL,
				   mode, 
				   pgsize - 1, pgsize);
      if (!hooks)
	{
	  rel->pending = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot inject .hooks", -1);
	}
      hooks->curend = 0;
    }
  
#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Entering intermediate symbol injection loop\n");
#endif

  /* Intermediate pass 2 : Inject ET_REL symbol table into host file */
  if (elfsh_fuse_etrel_symtab(file, rel) < 0)
    {
      rel->pending = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to fuze symbol tables", -1);
    }

#if __DEBUG_RELADD__
  printf("[DEBUG_RELADD] Entering final relocation loop\n");

  elfsh_print_sectlist(file, "before relocation"); 
#endif

  /* Now call the relocation on the object's sections */
  ret = elfsh_relocate_object(file, rel, ELFSH_RELOC_STAGE1);
  rel->pending = 0;
  depth--;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}

/**
 * Front end function for injection in static binaries with module dependences 
 *
 * @param host
 * @param rel
 * @param listw
 * @param listsh
 * @return
 */
int		elfsh_inject_etrel_hash(elfshobj_t *host, elfshobj_t *rel, 
					hash_t *listw, hash_t *listsh)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  elfsh_register_working_objects(listw, listsh);
  ret = elfsh_inject_etrel(host, rel);
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to inject ET_REL with list", ret);
  else
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (ret));
}
