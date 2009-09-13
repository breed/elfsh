/**
* @file libelfsh/map.c
 * @ingroup libelfsh
** map.c for libelfsh
** 
** Started on  Sat Mar  2 20:47:36 2002 jfv
** 
**
** $Id$
**
*/
#include "libelfsh.h"


/**
 * Fixup the binary, inject symbols and sort SHT 
 * @param file
 * @return
 */
void		      elfsh_fixup(elfshobj_t *file)
{
  elfsh_Shdr	*got;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->hdr->e_type == ET_REL || elfsh_static_file(file))
    elfsh_sort_sht(file);

  /* .got sht entsize fixup */
  got = elfsh_get_sht_entry_by_name(file, ELFSH_SECTION_NAME_GOT);
  if (got != NULL && got->sh_entsize == 0)
    got->sh_entsize = sizeof(eresi_Addr);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Load all the part of the binary.
 * This function should not be used by e2dbg 
 * @param file
 * @return
 */
int		        elfsh_read_obj(elfshobj_t *file)
{
  elfshsect_t		*actual;
  int			index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->read)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  if (file->sht == NULL && NULL == elfsh_get_sht(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
                 "Unable to grab SHT", -1);
  if (NULL == elfsh_get_pht(file, NULL) && file->hdr->e_type != ET_REL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
                 "Unable to grab PHT", -1);

#if __DEBUG_MAP__
  puts("[DEBUG:read_obj] Loading all known typed sections\n");
#endif

  /* Fill multiple relocation sections */
  for (index = 0; NULL != 
       (actual = elfsh_get_reloc(file, index, NULL)); 
       index++);

  /*
  ** Load sections placed after symtab
  ** Added for Solaris
  */
  elfsh_get_comments(file);
  elfsh_get_dwarf(file);
  elfsh_get_stab(file, NULL);
  
  if (file->hdr->e_type == ET_CORE) 
    {
      elfsh_get_core_notes(file);
      goto out;
    }


  /*
   ** We cannot use simply elfsh_get_anonymous_section() here
   ** because the object's section hash ptrs would not be filled.
   */
  elfsh_get_symtab(file, NULL);

  /* Fixup stuffs in the SHT */
  elfsh_fixup(file);

  elfsh_get_dynsymtab(file, NULL);
  elfsh_get_stab(file, NULL);
  elfsh_get_dynamic(file, NULL);
  elfsh_get_ctors(file, NULL);
  elfsh_get_dtors(file, NULL);
  elfsh_get_got(file, NULL);
  elfsh_get_interp(file);

  elfsh_get_versymtab(file, NULL);
  elfsh_get_verneedtab(file, NULL);
  elfsh_get_verdeftab(file, NULL);
  elfsh_get_hashtable(file, NULL);

  //elfsh_get_comments(file);
  elfsh_get_plt(file, NULL);

  /* Fill the multiple notes sections */
  for (index = 0; NULL != elfsh_get_notes(file, index); index++);

  /* Loop on the section header table and load all unknown-typed sections */
  for (actual = file->sectlist; actual; actual = actual->next)
  {
    /* Fix first section size */
    if (actual->shdr->sh_size == 0 && actual->next &&
        actual->next->shdr->sh_offset != actual->shdr->sh_offset &&
	actual->next->shdr->sh_addr   != actual->shdr->sh_addr)
      actual->shdr->sh_size =
        actual->next->shdr->sh_offset - actual->shdr->sh_offset;

    /* If the section data has to be loaded, load it */
    /* In case of bss, only load if BSS data is inserted in the file */
    if (actual->data == NULL && actual->shdr->sh_size)
    {
      if ((actual->shdr->sh_type == SHT_NOBITS && 
           actual->shdr->sh_offset == actual->next->shdr->sh_offset) ||
          (actual->next != NULL && actual->next->shdr->sh_offset == actual->shdr->sh_offset))
        continue;

#if __DEBUG_MAP__
      printf("[LIBELFSH] Loading anonymous  section %15s \n",
             elfsh_get_section_name(file, actual));
#endif
      elfsh_get_anonymous_section(file, actual);
    }
  }

  /* Fixup various symbols like dynamic ones that are NULL */
  /* Non fatal error */
  if (file->secthash[ELFSH_SECTION_DYNSYM])
    elfsh_fixup_dynsymtab(file->secthash[ELFSH_SECTION_DYNSYM]);

out:
  /* We close the file descriptor after file mapping so we can open more files */
  if (file->fd >= 0) {
#if __DEBUG_MAP__
    printf("[LIBELFSH] Closing descriptor %d \n",
           file->fd);
#endif

    XCLOSE(file->fd, -1);
    /* neutralize file descriptor */
    file->fd = -1;
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Called from elfsh_fixup_symtab
 * When trying to inject part of the libc, some bss symbols have a wrong sctndx 
 * @param symtab
 * @return
 */
elfshsect_t   *elfsh_fixup_sctndx(elfshsect_t *symtab)
{
  int	        index;
  elfsh_Sym     *sym;
  elfsh_SAddr	offset;
  elfsh_Shdr	*shdr;
  elfshsect_t	*sct;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //return (symtab); // XXX

  sym = symtab->data;
  shdr = symtab->parent->sht + symtab->index;

  for (index = 0; index < shdr->sh_size / sizeof(elfsh_Sym); index++)
  {
    if (elfsh_get_symbol_link(sym + index) != SHN_COMMON)
    {
      if (elfsh_get_symbol_type(sym + index) == STT_SECTION)
        continue;

      sct = elfsh_get_parent_section(symtab->parent, 
                                     elfsh_get_symbol_value(sym + index), 
                                     &offset);

      if (sct == NULL)
      {
        sct = elfsh_get_section_by_index(symtab->parent, 
                                         elfsh_get_symbol_link(sym + index), 
                                         NULL, 
                                         NULL);
        if (sct && elfsh_get_section_type(sct->shdr) == SHT_NOBITS)
        {

#if	__DEBUG_MAP__
          printf(" [*] Symbol [%s] sctndx changed from %u to SHN_COMMON\n", 
                 elfsh_get_symbol_name(symtab->parent, sym + index), 
                 elfsh_get_symbol_link(sym + index));
#endif

          elfsh_set_symbol_link(sym + index, SHN_COMMON);
          continue;
        }

      }

      if (sct && elfsh_get_section_type(sct->shdr) == SHT_NOBITS)
      {
        elfsh_set_symbol_link(sym + index, SHN_COMMON);
#if	__DEBUG_MAP__		  
        printf(" [*] Symbol [%s] sctndx changed to SHN_COMMON\n", 
               elfsh_get_symbol_name(symtab->parent, sym + index));
#endif
      }
    }
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, symtab);
}

/**
 * Map in memory all ressources for this file 
 * @param name
 * @return
 */
elfshobj_t	  *elfsh_map_obj(char *name)
{
  elfshobj_t	*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  file = elfsh_load_obj(name);
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
                 "Unable to load object", NULL);

  file->rights = O_RDWR;

  elfsh_read_obj(file);

  hash_init(&file->redir_hash, "redirections", 51, ASPECT_TYPE_UNKNOW);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (file));
}
