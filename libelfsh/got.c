/**
* @file libelfsh/got.c
 * @ingroup libelfsh
** got.c for elfsh
** 
** Started on  Sun Jun 24 21:30:41 2001 jfv
** Last update Thu May 15 04:39:15 2003 jfv
**
** $Id$
**
*/
#include "libelfsh.h"




/**
 * Shift GOT on ET_DYN 
 *
 * @param file
 * @param size
 * @param name
 * @return
 */
int		elfsh_shift_got(elfshobj_t *file, 
				u_int size,
				char *name)
{
  elfshsect_t	*got;
  int		nbr;
  u_int		idx;
  eresi_Addr	*addr;


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
#if	__DEBUG_ETRELintoETDYN__
  printf("[DEBUG_ETRELintoETDYN] Shifting GOT from %u bytes \n", size);
#endif

  got = elfsh_get_section_by_name(file, name,
				  NULL, NULL, &nbr);
  if (!got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Cannot retreive GOT in ET_DYN", -1);
  nbr = nbr / sizeof(eresi_Addr);
  for (idx = 0; idx < nbr; idx++)
    {
      addr = elfsh_get_got_entry_by_index(got->data, idx);
      if (*addr)
	*addr += size;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/**
 * Change endianess of GOT 
 *
 * @param newsect
 * @return
 */
int		elfsh_endianize_got(elfshsect_t *newsect)
{
  eresi_Addr	*cur;  
  int		idx;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (!newsect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);
  
#if __BYTE_ORDER == __LITTLE_ENDIAN
  if (newsect->parent->hdr->e_ident[EI_DATA] == ELFDATA2MSB) {
#elif __BYTE_ORDER == __BIG_ENDIAN
  if (newsect->parent->hdr->e_ident[EI_DATA] == ELFDATA2LSB) {
#else
#error Unexpected __BYTE_ORDER !
#endif
    cur = elfsh_readmem(newsect);
    for (idx = 0; idx < newsect->shdr->sh_size / newsect->shdr->sh_entsize; idx++)
      cur[idx] = swaplong(cur[idx]);
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
  
  
  
/**
 * Read the global offset table in section .got 
 *
 * @param file
 * @param num
 * @return
 */
eresi_Addr     	*elfsh_get_got(elfshobj_t *file, int *num)
{
  elfshsect_t	*enew;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  else if (file->sectlist == NULL && elfsh_read_obj(file) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to read object", NULL);

  /* Read GOT */
  /* Now called .got.plt on new ld */
  enew = file->secthash[ELFSH_SECTION_GOT];
  if (enew == NULL)
    {
      enew = elfsh_get_gotsct(file);
      if (enew == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to get GOT by name", NULL);
    }
 
  /* Load GOT data */
  if (enew->data == NULL)
    {
      enew->data = elfsh_load_section(file, enew->shdr);
      if (enew->data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load GOT", NULL);
      file->secthash[ELFSH_SECTION_GOT] = enew;

      /* Arrange endianess if necessary */
      elfsh_endianize_got(enew);
    }

  /* Final things */
  if (num != NULL)
    *num = enew->shdr->sh_size / enew->shdr->sh_entsize;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_readmem(enew)));
}
 
 
 
 
/**
 * Return the real GOT section 
 *
 * @param file
 * @return
 */
elfshsect_t	*elfsh_get_gotsct(elfshobj_t *file)
{
   elfshsect_t *new;
     
   PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);    
     
   new = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_ALTGOT,
				   NULL, NULL, NULL);
   if (new)  
     PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
   new = file->secthash[ELFSH_SECTION_GOT];
   if (new)
     PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
   new = elfsh_get_section_by_name(file, 
	        		   ELFSH_SECTION_NAME_GOTPLT, 
				   NULL, NULL, NULL);
   if (new)
     PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
   new = elfsh_get_section_by_name(file, 
   			           ELFSH_SECTION_NAME_GOT, 
			     	   NULL, NULL, NULL);
   if (new)
     PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		"Unable to get GOT by name", NULL);
}
 

/**
 * Return the 'range'th got - seems ok 
 *
 * @param file
 * @param range
 * @param nbr
 * @return
 */
elfshsect_t     *elfsh_get_got_by_idx(elfshobj_t *file, 
				      eresi_Addr range, 
				      u_int	 *nbr)
{
  elfshsect_t	*got;
  elfshsect_t	*cursect;
  u_int		idx;
  char		*name;
  eresi_Addr	rank = 0;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == 0x0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "No file selected", NULL);
  
  if (file->sectlist == NULL && elfsh_get_sht(file, NULL) == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  for (idx = 0; idx < elfsh_get_shtnbr(file->hdr); idx++)
    {
      
      cursect = elfsh_get_section_by_index(file, idx, NULL, NULL);
      name = cursect->name;

      if (strstr(name, "got"))
	{
	  
	  got = elfsh_get_section_by_name(file, name,
					  NULL, NULL, NULL);
	  if (got == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to get GOT by name", NULL);	  
	  
	  /* Load GOT data */
	  if (got->data == NULL)
	    {

	      got->data = elfsh_load_section(file, got->shdr);
	     
	      if (got->data == NULL)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
				  "Unable to load got", NULL);
	      
	      // file->secthash[ELFSH_SECTION_GOT] = got;
	      
	      /* Arrange endianess if necessary */
	      elfsh_endianize_got(got);
	    }
	  
	  if (rank == range)
	    {
	      if (nbr != NULL)
		*nbr =  got->shdr->sh_size / got->shdr->sh_entsize;
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (got));
	    }

	  rank++;	  
	}
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Cannot reach requested range", NULL);
}
  



/**
 * Modify a GOT entry 
 *
 * @param file
 * @param i
 * @param n
 * @return
 */
int		elfsh_set_got_entry_by_index(elfshobj_t *file, 
					     int i, 
					     eresi_Addr n)
{
  int		nbr;
  eresi_Addr	*got;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  got = elfsh_get_got(file, &nbr);
  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get GOT", -1);
  if (i >= nbr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GOT index too big", -1);
  got[i] = n;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/**
 * Modify the got entry for the dynamic symbol 'n' 
 *
 * @param f
 * @param n
 * @param a
 * @return
 */
int		elfsh_set_got_entry_by_name(elfshobj_t *f, 
					    char *n, 
					    eresi_Addr a)
{
  eresi_Addr	*got;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (f == NULL || n == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  got = elfsh_get_got_entry_by_name(f, n);
  if (got == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to get GOT entry by name", -1);
  *got = a;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Return a entry giving its parent and its index 
 * @param got
 * @param index
 * @return
 */
eresi_Addr     	*elfsh_get_got_entry_by_index(eresi_Addr *got, 
					      eresi_Addr index)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (got == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (got + index));
}



/**
 * Return a entry giving its parent and its index 
 *
 * @param file
 * @param name
 * @return
 */
eresi_Addr     	*elfsh_get_got_entry_by_name(elfshobj_t *file, char *name)
{
  int		nbr;
  u_int		index;
  eresi_Addr	*got;
  elfsh_Sym	*sym;
  u_int		sz;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (file == NULL || name == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", NULL);
  got = elfsh_get_got(file, &nbr);
  sym = elfsh_get_dynsymbol_by_name(file, name);
  if (got == NULL || sym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find symbol", NULL);

  /* Get PLT info */
  if (!elfsh_is_pltentry(file, sym))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Symbol is not PLT entry", NULL);
  sz = elfsh_get_pltentsz(file);
  
  /* Find the entry */
  for (index = 0; index < nbr; index++)
    if (INTERVAL(sym->st_value, got[index], sym->st_value + sz))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (got + index));
    
  /* Not found */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "GOT entry not found", NULL);
}




/**
 * Return a entry giving its parent and 
 * its index : used as INTERNAL hash handler 
 *
 * @param got
 * @param vaddr
 * @return
 */
int		elfsh_set_got_entry(eresi_Addr *got, eresi_Addr vaddr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  *got = vaddr;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Return a entry value giving its parent and its 
 * index : used as INTERNAL hash handler 
 *
 * @param got
 * @return
 */
eresi_Addr		elfsh_get_got_entry(eresi_Addr *got)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*got));
}



/**
 * Return a GOT entry value 
 *
 * @param got
 * @return
 */
eresi_Addr		elfsh_get_got_val(eresi_Addr *got)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*got));
}



/**
 * Return a GOT entry address 
 *
 * @param got
 * @return
 */
eresi_Addr		elfsh_get_got_addr(eresi_Addr *got)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (*got));
}



/**
 * Change a GOT entry val 
 *
 * @param got
 * @param val
 * @return
 */
u_int			elfsh_set_got_val(eresi_Addr *got, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == got)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  *got = val;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * Change a GOT entry addr 
 *
 * @param got
 * @param val
 * @return
 */
u_int			elfsh_set_got_addr(eresi_Addr *got, eresi_Addr val)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  // XXX Not yet implemented ... and not going to be
  printf("[DEBUG] elfsh_set_got_addr\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
