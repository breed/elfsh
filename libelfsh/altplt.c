/**
* @file libelfsh/altplt.c
 * @ingroup libelfsh
** altplt.c for libelfsh
** 
** This file contain the architecture independant code 
** for ALTPLT technique
**
** Started on  Tue May 26 11:40:07 2001 mm
**
**
** $Id$
**
*/
#include "libelfsh.h"


/**
 * @brief Patch the first PLT entry (a special case which must handled by this function).
 * @param enew
 * @param off
 * @param symtab Section descriptor for the symbol table.
 * @param file Host file.
 * @param extplt Section descriptor for the previously injected .elfsh.extplt 
 * @param plt Section descriptor for the .plt section.
 * @param diff
 * @return Success (0) or Error (-1).
 */
int		elfsh_altplt_firstent(elfshsect_t	*enew, 
				      u_int		*off,
				      elfshsect_t	*symtab, 
				      elfshobj_t	*file, 
				      elfshsect_t	*extplt,
				      elfshsect_t	*plt,
				      uint32_t		diff)
{
  u_int		entsz;
  elfsh_Sym	newsym;
  elfsh_Sym	*sym;
  eresi_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Insert plt+0 symbol */
  entsz  = elfsh_get_pltentsz(file);
  newsym = elfsh_create_symbol(enew->shdr->sh_addr, entsz, STT_FUNC, 0, 0, 0);
  if (elfsh_insert_symbol(symtab, &newsym, "old_dlresolve") < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to insert old_dlresolve symbol", -1);
  
  *off = *off - entsz + elfsh_get_first_pltentsz(file);
  
  /* On MIPS we have to callback __libc_start_main and not .plt+0 */
  if (FILE_IS_MIPS(plt->parent))
    {
      sym = elfsh_get_dynsymbol_by_name(file, "__libc_start_main");
      if (!sym)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot find __libc_start_main",  -1);
      
#if __DEBUG_COPYPLT__	      
      printf("[DEBUG_COPYPLT] Found __libc_start_main MIPS at addr "
	     XFMT "\n", sym->st_value);
#endif	      
      
      addr = sym->st_value;
    }
  else 
    addr = plt->shdr->sh_addr;
  
  /* Call the libelfsh hook ALTPLT */
  if (elfsh_altplt(file, &newsym, addr) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "ALTPLT failed", -1);
  
  /* On IA32, reencode the PLT and EXTPLT entries to use the ALTGOT */
  /* The hook does nothing on other archs for now */
  if (FILE_IS_IA32(file))
    if (elfsh_encodeplt1(file, plt, extplt, diff) < 0)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Reencoding of (EXT)PLT+0 failed", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}







/**
 * @brief Copy the PLT of an ET_EXEC object for the ALTPLT technique.
 * and the GOT of an ET_EXEC object for the ALTGOT technique.
 * @param file Host file.
 * @param mod Always inject sections with size being a multiple of mod.
 * @return Success (0) or Error (-1).
 */
int		elfsh_relink_plt(elfshobj_t *file, u_int mod)
{
  elfshsect_t	*got;
  elfshsect_t   *plt;
  elfshsect_t	*symtab;
  elfshsect_t	*dynsym;
  elfshsect_t	*prolog;
  elfshsect_t	*extplt = NULL;
  elfshsect_t	*altgot = NULL; /* shut the nice talkative */
  elfshsect_t	*enew    = NULL; /* compiler also know as gcc */
  elfsh_Shdr	hdr;
  elfsh_Sym	*sym;
  elfsh_Sym	newsym;
  char		buf[BUFSIZ];
  u_int		off;
  u_int		entsz;
  int		mode;
  eresi_Addr	addr;
  char		*prologdata;
  u_int		sz;
  char		*name;
  u_char	ostype;
  eresi_Addr	diff;
  u_int		extplt_size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get PLT */
  if (file->secthash[ELFSH_SECTION_ALTPLT] != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  plt = elfsh_get_plt(file, NULL);
  if (NULL == plt)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "PLT section not found", -1);
  entsz = elfsh_get_pltentsz(file);
  if (entsz < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Failed to get PLT entry size", -1);

  /* Get GOT (recent ld call it .got.plt) */
  got = elfsh_get_gotsct(file);
  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "GOT section not found", -1);
      
  /* Get symtabs */
  if (NULL == elfsh_get_dynsymtab(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "DYNSYM not found", -1);
  if (NULL == elfsh_get_symtab(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "SYMTAB not found", -1);

  /* Some fingerprint */
  ostype = elfsh_get_ostype(file);
  if (ostype == ELFSH_OS_ERROR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid OS target", -1);

  /* Insert alternative .plt */
  dynsym = file->secthash[ELFSH_SECTION_DYNSYM];
  symtab = file->secthash[ELFSH_SECTION_SYMTAB];

  /* FreeBSD and BeoS is incompatible with pre-interp injection */
  /* Solaris needs self-mutating code for ALTPLT technique */
  /* %gp offsets on ALPHA/MIPS requires data injection */
  ELFSH_SELECT_INJECTION(file,NULL,mode);

  /* Map .alt.plt.prolog on ALPHA, or .alt.got.prolog on MIPS */
  if (FILE_IS_MIPS(file) || FILE_IS_ALPHA64(file))
    {
      if (FILE_IS_MIPS(file))
	{
	  name = ELFSH_SECTION_NAME_ALTGOTPROLOG;
	  sz = 28;
	}
      else
	{
	  name = ELFSH_SECTION_NAME_ALTPLTPROLOG;
	  sz = 48;
	}
      prolog = elfsh_create_section(name);
      hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 
			      0, 0, sz, 0, 0, 0, 0);

      XALLOC(__FILE__, __FUNCTION__, __LINE__, prologdata, sz, -1);

      if (elfsh_insert_mapped_section(file, 
				      prolog, hdr, prologdata, 
				      mode, mod) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  ".alt.{plt,got}.prolog insertion failed", -1);

      enew = elfsh_get_section_by_name(file, name, NULL, NULL, NULL);
      if (enew == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  ".alt.{plt,got}.prolog insertion failed", -1);
      file->secthash[ELFSH_SECTION_ALTPLTPROLOG] = enew; 
    }

  /* Map .alt.plt (or .pad.got on MIPS)

     On MIPS we use .pad.got in order to align .alt.got on a 0x1000 
     bound boundary.
     
     On ALPHA and SPARC, .alt.plt will be relocated instead of .plt 
  */
  sz = plt->shdr->sh_size;
  if (FILE_IS_MIPS(file))
    {
      addr = enew->shdr->sh_addr + enew->shdr->sh_size;
      if ((addr - (got->shdr->sh_addr)) % 1024)
	sz = 1024 - ((addr - (got->shdr->sh_addr)) % 1024);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, prologdata, sz, -1);
      memset(prologdata, 0x00, sz);
      name = ELFSH_SECTION_NAME_PADGOT;
    }
  else
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, prologdata, sz, -1);
      memcpy(prologdata, elfsh_readmem(plt), sz);
      name = ELFSH_SECTION_NAME_ALTPLT;
    }
  enew = elfsh_create_section(name);
  hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, 
			  0, 0, sz, 0, 0, 0, 0);
  if (elfsh_insert_mapped_section(file, enew, hdr, prologdata, mode, mod) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      ".alt.plt|.pad.got insertion failed", -1);
  enew = elfsh_get_section_by_name(file, name, NULL, NULL, NULL);
  if (enew == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      ".alt.plt|.pad.got insertion failed", -1);
  file->secthash[ELFSH_SECTION_ALTPLT] = enew;
   

  /* Map .alt.got (all architectures except SPARC) */
  /* On IA32, remap GOT with a doubled size for non-present symbol resolving */
  if (FILE_IS_MIPS(file) || FILE_IS_ALPHA64(file) || FILE_IS_IA32(file))
    {
      sz = (FILE_IS_MIPS(file) ? got->shdr->sh_size     : 
	    FILE_IS_IA32(file) ? got->shdr->sh_size * 4 : 
	    plt->shdr->sh_size / elfsh_get_pltentsz(file) * sizeof(eresi_Addr));

      altgot = elfsh_create_section(ELFSH_SECTION_NAME_ALTGOT);
      hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC | SHF_WRITE, 
			      0, 0, sz, 0, 0, 0, sizeof(eresi_Addr));

      XALLOC(__FILE__, __FUNCTION__, __LINE__, prologdata, sz, -1);
      memcpy(prologdata, elfsh_readmem(got), got->shdr->sh_size);
      
      if (elfsh_insert_mapped_section(file, altgot, hdr, prologdata, 
				      ELFSH_DATA_INJECTION, mod) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  ".alt.got insertion failed", -1);

      altgot = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTGOT, 
				      NULL, NULL, NULL);
      if (altgot == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  ".alt.got insertion failed", -1);
      file->secthash[ELFSH_SECTION_ALTGOT] = altgot;
      altgot->curend = got->shdr->sh_size;
      memset(elfsh_readmem(altgot) + got->shdr->sh_size, 0x00, got->shdr->sh_size);
      altgot->shdr->sh_entsize = sizeof(eresi_Addr);
    }
  

  /* Insert EXTPLT in order to be able to resolve non present symbols */
  if (FILE_IS_IA32(file))
    {
      extplt_size = plt->shdr->sh_size * 2;
      extplt = elfsh_create_section(ELFSH_SECTION_NAME_EXTPLT);
      hdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR,
			      0, 0, extplt_size, 0, 0, 0, 0);

      XALLOC(__FILE__, __FUNCTION__, __LINE__, prologdata, plt->shdr->sh_size, -1);
      memcpy(prologdata, elfsh_readmem(plt), plt->shdr->sh_size);

      if (elfsh_insert_mapped_section(file, extplt, hdr, prologdata, 
				      mode, mod) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  ".ext.plt insertion failed", -1);
      extplt = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_EXTPLT, 
					 NULL, NULL, NULL);
      if (extplt == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  ".ext.plt insertion failed", -1);
      file->secthash[ELFSH_SECTION_EXTPLT] = extplt;
      extplt->curend = elfsh_get_first_pltentsz(file);
    }



  /* Loop on .plt and inject 'old_symnam' symbols */
  for (off = 0; off < plt->shdr->sh_size; off += entsz)
    {

      /* SPARC does not have ALTGOT */
      if (FILE_IS_MIPS(file) || FILE_IS_ALPHA64(file) || FILE_IS_IA32(file))
	diff = (uint32_t) altgot->shdr->sh_addr - got->shdr->sh_addr;
      else
	diff = 0;

      /* Special case for the first plt entry */
      if (off == 0 && elfsh_altplt_firstent(enew, &off, symtab, file, extplt, plt, diff) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "ALTPLT on first entry failed", -1);
      else if (off == 0)
	continue;
      
      /* Get the existing symbol name for this plt entry ... */
      sym = elfsh_get_sym_by_value(elfsh_readmem(dynsym),
				   dynsym->shdr->sh_size / sizeof(elfsh_Sym),
				   plt->shdr->sh_addr + off, 
				   NULL, ELFSH_EXACTSYM);

      /* New versions of ld do not fill the vaddr of dynamic symbols,
	 do it ourself. Do not insert old symbol in emergency cases */
      if (sym == NULL)
      {
	  if ((sym = elfsh_restore_dynsym(file, plt, off, dynsym)) == NULL)
	      continue;

	  name = elfsh_get_dynsymbol_name(file, sym);
	  
	  /* __gmon_start__ should not be resolved 
	     if it was not already done by gcc */
	  if (name && !strcmp(name, "__gmon_start__"))
	      sym->st_value = 0x0;
      }
      
      /* ... and we inject the 'old' occurence symbol pointing in 
	 .alt.plt (.plt on MIPS) */
      if (!FILE_IS_MIPS(file))
	addr = enew->shdr->sh_addr + off;
      else
	addr = plt->shdr->sh_addr + off;
      
#if   __BYTE_ORDER == __BIG_ENDIAN
      if (file->hdr->e_ident[EI_DATA] == ELFDATA2LSB) 
#elif __BYTE_ORDER == __LITTLE_ENDIAN 
      if (file->hdr->e_ident[EI_DATA] == ELFDATA2MSB)
#else
#error Unexpected __BYTE_ORDER !      
#endif 
	addr = swaplong(addr);
      
      /* Injection */
      name = elfsh_get_dynsymbol_name(file, sym);
      newsym = elfsh_create_symbol(addr, entsz, STT_FUNC, 0, 0, 0);
      snprintf(buf, BUFSIZ, "old_%s", name);
      if (elfsh_insert_symbol(symtab, &newsym, buf) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "old_* symbol injection failed", -1);
      
#if __DEBUG_COPYPLT__
      printf("[DEBUG_COPYPLT] Symbol at .plt + %u injected"
	     " succesfully (%s) \n", off, buf);
#endif

      /* On ALPHA, shift the relocation offset from .got to .alt.got to avoid 
	 hooks removing when calling back the original function. */
      if (FILE_IS_ALPHA64(file) && 
	  elfsh_shift_alpha_relocs(file, name, altgot, off) < 0)
	continue;

      /* Reencode the PLT entry to use the alternative GOT */
      /* This condition is for compatibility with other archs where EXTPLT
	 is not yet supported. For those we do not enter the hook */
      if (FILE_IS_IA32(file))
	{
	  diff = (eresi_Addr) altgot->shdr->sh_addr - got->shdr->sh_addr;
	  elfsh_encodeplt(file, plt, diff, off);
	  if (file->hdr->e_type == ET_DYN)
	    elfsh_encodeplt(file, file->secthash[ELFSH_SECTION_ALTPLT], 
			    diff, off);
	  diff = (eresi_Addr) altgot->shdr->sh_addr - got->shdr->sh_addr + 
	    got->shdr->sh_size;
	  elfsh_encodeplt(file, extplt, diff, off);
	}
    }

  /* Activate ALTGOT */
  if (elfsh_redirect_pltgot(file, altgot, got, plt, enew) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "PLTGOT redirection failed", -1);

  /* Activate EXTPLT */
  if (elfsh_extplt_mirror_sections(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Section mirroring failed", -1);

#if	__DEBUG_COPYPLT__
  printf("[DEBUG_COPYPLT] Section Mirrored Successfully ! \n");
#endif


  /* Everything is 0k4y */
  if (elfsh_sync_sorted_symtab(symtab) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "symtab synchronisation failed", -1);
  if (elfsh_sync_sorted_symtab(dynsym) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "dynsym synchronisation failed", -1);
  elfsh_sync_sectnames(file);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/** 
 * @brief On MIPS there is no .plt section : call to libraries are done
 * using an indirect jump on .got value directly from .text. If 
 * we want to be able to call the original function from the hook 
 * function, we need to create a plt-like section and mirror the
 * Global Offset Table (.got). 
 *
 * By default, .got points in some code stub standing in .text. This
 * code stubs looks much like a PLT. Indeed, there is a kind of PLT 
 * on MIPS but it is embedded in the .text section. What we do in this 
 * function is to disambiguize .plt and .text, so that we can apply 
 * ALTPLT technique as well on MIPS.
 *
 * Unlike on ALPHA, we cannot move relocations from .got to .alt.got
 * since MIPS binaries have no relocation tables in ET_EXEC objects.
 * What we do instead is changing the processor specific DT_PLTGOT 
 * entry in the .dynamic section and continue to rely on the ALTPLT 
 * technique (call's the original functions using the injected 'old_' 
 * symbol) just like ALTPLT redirection on other architectures. -mm
 *
 * @param file Host file.
 * @return Success (0) or Error (-1).
 */
int		elfsh_build_plt(elfshobj_t *file)
{
  elfshsect_t	*text;
  elfsh_SAddr	off;
  char		buff[16] = {0x00};
  eresi_Addr	pltaddr = 0;
  eresi_Addr	pltend  = 0;
  elfsh_Shdr	start;
  elfsh_Shdr	plt;
  elfshsect_t	*enew;
  eresi_Addr	lsize;
  unsigned int	size;
  char		*data;
  char		*tdata;
  unsigned int	idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First checks */
  text = elfsh_get_parent_section(file, elfsh_get_entrypoint(file->hdr), &off);
  if (!text)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find parent section from entry point", -1);
  if (!elfsh_get_anonymous_section(file, text))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get an anonymous section", -1);

  
  /* 
  ** Find the embedded plt by searching the nop;nop;nop;nop; signature 
  ** that delimit the beginning and the end of plt. This is MIPS specific
  ** since only MIPS needs this.
  */
  tdata = elfsh_readmem(text);
  for (off = 0; off < text->shdr->sh_size; off += 4)
    if (!memcmp(tdata + off, buff, sizeof(buff)))
      {
	pltaddr = text->shdr->sh_addr + off + 16;
	for (off += 16; off < text->shdr->sh_size; off += 4)
	  if (!memcmp(tdata + off, buff, 16))
	    {
	      pltend = text->shdr->sh_addr + off + 16;
	      goto found;
	    }
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Cannot find PLT end", -1);
      }

 found:
  idx = text->index;
  if (!pltaddr) 
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find PLT start", -1);

  /* 
  ** Now create additional section header table entries so that we reduce the
  ** MIPS model to our standard ALTPLT/ALTGOT model.
  **
  ** Do not use the elfsh_insert_*_section() since we want no address space 
  ** shifting.
  */

  /* .start */
  lsize = pltaddr - text->shdr->sh_addr;
  size  = (unsigned int) lsize;
  start = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC,
			    text->shdr->sh_addr, text->shdr->sh_offset,
			    size, 0, 0, 0, 0);
  enew = elfsh_create_section(ELFSH_SECTION_NAME_START);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,data, size, -1);
  memcpy(data, tdata, size);
  elfsh_insert_shdr(file, start, idx, enew->name, 0);
  elfsh_add_section(file, enew, idx, data, ELFSH_SHIFTING_MIPSPLT);
  file->secthash[ELFSH_SECTION_MIPSTART] = enew;

  /* .plt */
  lsize = pltend - pltaddr;
  size  = (unsigned int) lsize;
  plt   = elfsh_create_shdr(0, SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC,
			    start.sh_addr + start.sh_size, 
			    start.sh_offset + start.sh_size,
			    size, 0, 0, 0, 0);
  enew = elfsh_create_section(ELFSH_SECTION_NAME_PLT);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,data, size, -1);
  memcpy(data, tdata + start.sh_size, size);
  elfsh_insert_shdr(file, plt, idx + 1, enew->name, 0);
  elfsh_add_section(file, enew, idx + 1, data, ELFSH_SHIFTING_MIPSPLT);

  /* Shift .text data, sh_offset, sh_addr, and sh_size correctly */
  text->shdr->sh_offset += (start.sh_size + plt.sh_size);
  text->shdr->sh_addr   += (start.sh_size + plt.sh_size);
  memmove(tdata, 
	  tdata + start.sh_size + plt.sh_size, 
	  text->shdr->sh_size - (start.sh_size + plt.sh_size));
  text->shdr->sh_size   -= (start.sh_size + plt.sh_size);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * @brief Main function performing ALTPLT, ALTGOT, and EXTPLT techniques.
 * @param file The host file.
 * @param modulo Always inject sections with a size being a multiple of mod.
 * @return Success (0) or Error (-1).
 */
int		elfsh_copy_plt(elfshobj_t *file, u_int modulo)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Some sanity checks */
  if (elfsh_static_file(file))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (FILE_IS_MIPS(file) && 
      elfsh_dynamic_file(file) && 
      (elfsh_build_plt(file) < 0))
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			"Unable to build MIPS plt", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     elfsh_relink_plt(file, modulo));
}
