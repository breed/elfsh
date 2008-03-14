/**
 * @file reloc.c
 * @ingroup libelfsh
** reloc.c for elfsh
** 
** Started on  Tue Feb 27 14:34:50 2001 jfv
** 
**
** $Id: reloc.c,v 1.6 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * Contain the last requested relocation type
 * This ugly flag may be sensible to race conditions if 2 users use
 * it at the same time. However it is necessary for discriminating
 *  elfsh_Rel and elfsh_Rela pointers in scripting handlers since
 *  the handlers are shared with other ELF objects types, we lack
 *  a bit of flexibility here.
 */
static char	isrel = 1;


/**
 * Create relocation entry 
 * @param type
 * @param sym
 * @param off
 * @return
 */
elfsh_Rel	elfsh_create_relent(elfsh_Addr type, elfsh_Addr sym, 
				    elfsh_Addr off)
{
  elfsh_Rel	r;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_set_reltype(&r, type);
  elfsh_set_relsym(&r, sym);
  elfsh_set_reloffset(&r, off);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (r));
}

/**
 * Create relocation entry 
 * @param type
 * @param sym
 * @param off
 * @param add
 * @return
 */
elfsh_Rela	elfsh_create_relaent(elfsh_Addr type, elfsh_Addr sym, 
				     elfsh_Addr off, elfsh_Addr add)
{
  elfsh_Rela	r;
  elfsh_Rela	*enta;
  elfsh_Rel	*ent;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  enta = &r;
  ent  = (elfsh_Rel *) enta;
  elfsh_set_reltype(ent, type);
  elfsh_set_relsym(ent, sym);
  elfsh_set_reloffset(ent, off);
  elfsh_set_reladdend(&r, add);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (r));
}

/**
 * Return the relocation type 
 * @param r
 * @return
 */
u_int	elfsh_get_reltype(elfsh_Rel *r)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_R_TYPE(r->r_info)));
}

/**
 * Return the relocation type 
 * @param r
 * @param type
 * @return
 */
u_int	elfsh_set_reltype(elfsh_Rel *r, elfsh_Addr type)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  r->r_info = ELFSH_R_INFO(ELFSH_R_SYM(r->r_info), type);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Return the relocation symbol index 
 * @param r
 * @return
 */
u_int	elfsh_get_relsym(elfsh_Rel *r)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_R_SYM(r->r_info)));
}

/**
 * Change the relocation symbol index 
 * @param r
 * @param sym
 * @return
 */
u_int	elfsh_set_relsym(elfsh_Rel *r, elfsh_Addr sym)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  r->r_info = ELFSH_R_INFO(sym, ELFSH_R_TYPE(r->r_info));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the relocation offset 
 * @param r
 * @return
 */
elfsh_Addr	elfsh_get_reloffset(elfsh_Rel *r)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (r->r_offset));
}

/**
 * Change the relocation offset for this entry 
 * @param r
 * @param off
 * @return
 */
int		elfsh_set_reloffset(elfsh_Rel *r, elfsh_Addr off)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  r->r_offset = off;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the add-end 
 * @param r
 * @return
 */
elfsh_Sword     elfsh_get_reladdend(elfsh_Rela *r)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

   if (r == NULL)
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid NULL parameter", -1);
   PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (r->r_addend));
}

/**
 * Change the add-end 
 * @param r
 * @param val
 * @return
 */
int     elfsh_set_reladdend(elfsh_Rela *r, elfsh_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (r == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  r->r_addend = (elfsh_Sword) val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Change endianess of relocation tables 
 * @param s
 * @return
 */
int		elfsh_endianize_relocs(elfshsect_t *s)
{
  u_int		idx;
  elfsh_Rel	*rel;
  elfsh_Rela	*rela;
  u_int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!s)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", -1);

  sz = s->shdr->sh_size / (IS_REL(s) ? 
			   sizeof(elfsh_Rel) : 
			   sizeof(elfsh_Rela));;

  /* Arrange endianess if necessary */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (s->parent->hdr->e_ident[EI_DATA] == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (s->parent->hdr->e_ident[EI_DATA] == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    rel = (void *) elfsh_get_raw(s);
    rela = (void *) rel;

    for (idx = 0; idx < sz; idx++)
      if (s->shdr->sh_type == SHT_REL)
	{
	  rel[idx].r_offset = swaplong(rel[idx].r_offset);
	  rel[idx].r_info   = swaplong(rel[idx].r_info);
	}
      else if (s->shdr->sh_type == SHT_RELA)
	{
	  rela[idx].r_offset = swaplong(rela[idx].r_offset);
	  rela[idx].r_info   = swaplong(rela[idx].r_info);
	  rela[idx].r_addend = swaplong(rela[idx].r_addend);
	}
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Return the 'range'th relocation table 
 * @param file
 * @param range
 * @param nbr
 * @return
 */
elfshsect_t	*elfsh_get_reloc(elfshobj_t *file, 
				 elfsh_Addr range, 
				 u_int *nbr)
{
  elfshsect_t	*s;
  u_int		type;
  u_int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file->sectlist == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  /* Read section */
  type = (IS_REL(file->sectlist) ? SHT_REL : SHT_RELA);
  s = elfsh_get_section_by_type(file, type,
				range, NULL,
				NULL, NULL);
  if (s == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get reloc section", NULL);

  /* Return what's needed */
  sz = s->shdr->sh_size / (s->shdr->sh_type == SHT_REL ? 
			   sizeof(elfsh_Rel) : 
			   sizeof(elfsh_Rela));;
  if (nbr != NULL)
    *nbr = sz;

  /* Load section data */
  if (s->data == NULL)
    {
      s->data = elfsh_load_section(file, s->shdr);
      if (s->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load reloc data", NULL);
      elfsh_endianize_relocs(s);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (s));
}


/**
 * Insert a symbol in the given symbol table 
 * @param sect
 * @param rel
 * @return
 */
int		elfsh_insert_relent(elfshsect_t *sect, elfsh_Rel *rel)

{
  int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (sect == NULL || sect->shdr == NULL || rel == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL paramater", -1);
  if (sect->shdr->sh_type != SHT_REL && sect->shdr->sh_type != SHT_RELA)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Input section is not REL/RELA", -1);

  /* Insert entry in relocation table */
   if (sect->shdr->sh_type == SHT_REL)
     index = elfsh_append_data_to_section(sect, rel, sizeof(elfsh_Rel));
   else
     index = elfsh_append_data_to_section(sect, rel, sizeof(elfsh_Rela));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (index));
}



/**
 * Return the symbol name associated with the relocation entry 
 * @param file
 * @param r
 * @return
 */
char		*elfsh_get_symname_from_reloc(elfshobj_t *file,
					      elfsh_Rel	 *r)
{
  elfsh_Sym	*sym;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanitize */
  sym = elfsh_get_symbol_from_reloc(file, r);
  if (sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find symbol", NULL);

  /* The object is relocatable : find symbol in .symtab */
  if (file->hdr->e_type == ET_REL)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_symbol_name(file, sym)));
  
  /* else find symbol in .dynsym */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_dynsymbol_name(file, sym)));
}



/**
 * Return the Symbol associated with the relocation entry 
 * @param file
 * @param r
 * @return
 */
elfsh_Sym	*elfsh_get_symbol_from_reloc(elfshobj_t *file,
					     elfsh_Rel	*r)
{
  elfsh_Sym	*sym;
  u_int		tmp;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (NULL == file || NULL == r)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  tmp = ELFSH_R_SYM(r->r_info);

  /* The object is relocatable : find symbol in .symtab */
  if (file->hdr->e_type == ET_REL)
    {
      if (NULL == file->secthash[ELFSH_SECTION_SYMTAB])
	if (NULL == elfsh_get_symtab(file, NULL))
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unable to find SYMTAB", NULL);

      sym = (elfsh_Sym *) file->secthash[ELFSH_SECTION_SYMTAB]->data;
      sym += tmp; //(tmp * ELFSH_SYMTAB_ENTRY_SIZE));
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym));
    }

  /* else find symbol in .dynsym */
  if (NULL == file->secthash[ELFSH_SECTION_DYNSYM] &&
      NULL == elfsh_get_dynsymtab(file, NULL))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find DYNSYM", NULL);

  sym = elfsh_get_raw(file->secthash[ELFSH_SECTION_DYNSYM]) + 
    (tmp * ELFSH_SYMTAB_ENTRY_SIZE);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sym));
}



/** 
 * Ugly hook, but necessary for keeping track of the reloc element type
 * since elfsh_get_relent_by_index cannot do it since it shares a function
 * pointer with other elf objects in the scripting engine. -m
 * @param b
 */
void		elfsh_setrel(char b) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 
  isrel = b; 

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * Used as internal handler for elfsh hashes 
 * @param table
 * @param index
 * @return
 */
elfsh_Rel	*elfsh_get_relent_by_index(elfsh_Rel *table, elfsh_Addr index)
{
  elfsh_Rela	*atable;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!table)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid NULL parameter", NULL);
  atable = (elfsh_Rela *) table;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (isrel ? table + index : (elfsh_Rel*) ((elfsh_Rela*) atable + index)));
}



/**
 * Used as internal handler for elfsh hashes 
 * @param file
 * @param name
 * @return
 */
elfsh_Rel	*elfsh_get_relent_by_name(elfshobj_t *file, char *name)
{
  elfshsect_t	*sect;
  elfsh_Rel	*cur;
  u_int		range;
  u_int		idx;
  u_int		num;
  char		*curnam;
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);

  /* Find relocation entry by name */
  sect = elfsh_get_reloc(file, 0, &num);
  for (range = 1; NULL != sect; range++)
    {
      for (idx = 0; idx < num; idx++)
	{
	  data = elfsh_get_raw(sect);
	  cur = (sect->shdr->sh_type == SHT_RELA ?
		 (void *) ((elfsh_Rela *) data + idx) :
		 (void *) ((elfsh_Rel  *) data + idx));
	  curnam = elfsh_get_symname_from_reloc(file, cur);
	  if (curnam != NULL && !strcmp(curnam, name))
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
	}
      sect = elfsh_get_reloc(file, range, &num);
    }

  /* Not found */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Relentry not found", NULL);
}



/**
 * Create relocation table for section 
 * @param sect
 * @return
 */
elfshrel_t	*elfsh_find_rel(elfshsect_t *sect)
{
  elfshsect_t	*target;
  elfshrel_t	*rel;
  int		*dword;
  char		*str;
  elfsh_Addr   	vaddr;
  u_int		index;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  else if (elfsh_get_raw(sect) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section empty", NULL);
  else if (sect->shdr->sh_addr == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Section unmapped", NULL);
  else if (sect->rel)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->rel));

  /* These sections must not be relocated, but passed to relative */
  if (sect->shdr->sh_type == SHT_DYNSYM             ||
      sect->shdr->sh_type == SHT_REL                ||
      sect->shdr->sh_type == SHT_RELA               ||
      sect->shdr->sh_type == SHT_DYNAMIC            ||
      !strcmp(sect->name, ELFSH_SECTION_NAME_GOT)   ||
      !strcmp(sect->name, ELFSH_SECTION_NAME_CTORS) ||
      !strcmp(sect->name, ELFSH_SECTION_NAME_DTORS))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Use different relocation code for this section", NULL);

  /* Free existing rel[] entries if existing (section data probably changed) */
  sect->srcref = sect->dstref = 0;

  /* Read the actual section and find valid references */
  str = elfsh_get_raw(sect);
  for (dword = (int *) str;
       ((char *) dword + 4) <= ((char *) elfsh_get_raw(sect) + 
				sect->shdr->sh_size);
       dword = (int *) str)
    {
      target = elfsh_get_parent_section(sect->parent, *dword, NULL);
      if (target != NULL)
	{
	  sect->srcref++;
	  target->dstref++;
	}
      str++;
    }

  /* allocate the array */
  if (sect->srcref == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No need to relocate section", NULL);

  XALLOC(__FILE__, __FUNCTION__, __LINE__,rel, sect->srcref * sizeof(elfshrel_t), NULL);

  /* Read the actual section again and create section rel table */
  str = elfsh_get_raw(sect);
  for (index = 0, dword = (int *) str;
       ((char *) dword + 4) <= ((char *) elfsh_get_raw(sect) +
				sect->shdr->sh_size);
       dword = (int *) str)
    {
      target = elfsh_get_parent_section(sect->parent, *dword, NULL);
      if (target != NULL)
	{
	  vaddr = sect->shdr->sh_addr + 
	    (elfsh_Addr) ((char *) str - (char *) elfsh_get_raw(sect));
	  rel[index].idx_src = sect->index;
	  rel[index].off_src = vaddr - sect->shdr->sh_addr;
	  rel[index].idx_dst = target->index;
	  rel[index].off_dst = *dword - target->shdr->sh_addr;
	  rel[index].type    = ELFSH_RELOC_SECTBASE;
	  index++;
	}
      str++;
    }

  /* Return the array */
  sect->rel = rel;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (sect->rel));
}
