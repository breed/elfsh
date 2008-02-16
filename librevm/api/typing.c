/**
* @file typing.c
* 
* Various API for manipulating types
*
* Started on Sun Jan 9 07:23:58 2007 jfv
* $Id: typing.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "revm.h"


/**
 * Copy a designed type 
 * @param from Source
 * @param to Destination
 * @return 0 on success, or < 0 on error.
 */
int		revm_type_copy(char *from, char *to)
{
  aspectype_t	*tocopy, *newtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  tocopy = (aspectype_t *) hash_get(&types_hash, from);
  if (!tocopy)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Type not found", -1);

  newtype = (aspectype_t *) hash_get(&types_hash, to);

  if (newtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Type destination already exist", -1);    

  newtype = aspect_type_copy(tocopy, tocopy->off, 
			     tocopy->isptr, tocopy->dimnbr,
			     strdup(to), tocopy->elemnbr);
  if (!newtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to create a new type", -2);

  hash_add(&types_hash, newtype->fieldname, (void *) newtype);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Init the global type table 
 *
 * @param name
 * @return
 */
int		revm_type_hashcreate(char *name)
{
  char		hashname[BUFSIZ];
  hash_t	*newhash;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Create the hash table for objects of that type */
  snprintf(hashname, sizeof(hashname), "type_%s", 
	   name);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, newhash, sizeof(hash_t), -1);
  hash_init(newhash, strdup(hashname), 11, ASPECT_TYPE_UNKNOW);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Print a specific type 
 * 
 * @param type
 * @param mode
 * @return
 */
int		revm_type_print(char *type, char mode)
{
  aspectype_t	*cur;
  aspectype_t	*child;
  char		buf[BUFSIZ];
  char		prefix[128];
  int		len;
  char		*size;
  char		offset[128];
  int		idx;
  int		sz;
  char		*pad;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  cur = hash_get(&types_hash, type);
  if (!cur)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unknown type", 0);

  /* Set up things */
  revm_endline();

  /* Setup first part of the line */
  snprintf(prefix, sizeof(prefix), "%s%s%s", 
	   revm_colorfieldstr("{"),
	   revm_colornumber("%u", cur->size),
	   revm_colorfieldstr("}"));
  len = snprintf(buf, sizeof(buf), "  %s %s %-20s %s %-10s",
		 revm_colornumber("id:%-10u", cur->type),
		 revm_colorfieldstr("Type"),
		 revm_colortypestr_fmt("%-20s", type), 
		 revm_colorfieldstr("size"),
		 prefix);
  size = alloca(20);

  /* If the type is a structure */
  if (cur->childs)
    {
      len += snprintf(buf + len, sizeof(buf) - len, "%s",
		      revm_colorfieldstr(" = {"));
      
      /* Prepare the padding after each field name */
      sz = len - revm_color_size(buf) - 16; /* -16 is dirty: some bug in colors */
      pad = alloca(sz + 1);
      memset(pad, ' ', sz);
      pad[sz] = 0x00;

      /* For each child field type */
      for (child = cur->childs; child; child = child->next)
	{
	  
	  /* Compute the size field */
	  if (child->type == ASPECT_TYPE_RAW)
	    snprintf(size, sizeof(size), "%s%s%s", 
		     revm_colorfieldstr("["), 
		     revm_colornumber("%u", child->size), 
		     revm_colorfieldstr("]"));
	  else if (child->dimnbr && child->elemnbr)
	    {
	      for (sz = idx = 0; idx < child->dimnbr; idx++)
		sz += 20;
	      size = alloca(sz);
	      for (sz = idx = 0; idx < child->dimnbr; idx++)
		sz += sprintf(size + sz, "%s%s%s", 
			      revm_colorfieldstr("["),
			      revm_colornumber("%u", child->elemnbr[idx]),
			      revm_colorfieldstr("]"));
	    }
	  else
	    *size = 0x00;

	  /* Format the whole thing */
	  if (mode)
	    *offset = 0x00;
	  else
	    snprintf(offset, sizeof(offset), "%s%s", 
		     revm_colornumber("%u", child->off),
		     revm_colorfieldstr(":"));
	  len += snprintf(buf + len, sizeof(buf) - len, 
			  "%s%s%s%s%s%s%s", 
			  offset, 
			  revm_colorstr(child->fieldname),
			  revm_colorfieldstr(":"),
			  revm_colortypestr((child->isptr ? "*" : "")), 
			  revm_colortypestr(child->name), 
			  size,
			  revm_colorfieldstr((child->next ? ",\n" : "}\n\n")));
	  
	  /* Print field and next padding */
	  revm_output(buf);
	  revm_endline();
	  if (child->next)
	    revm_output(pad);
	  *buf = 0x00;
	  len = 0;
	}
    }

  /* Print non-structures types */
  else
    {
      revm_output(buf);
      revm_output("\n");
    }

  /* Return success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}


/**
 * Print types matching a regular expression 
 *
 * @param regex
 * @return
 */
int		revm_type_print_regex(char *regex)
{
  regex_t	rx;
  int		keynbr;
  char		**keys;
  int		index;
  int		match;
  char		buf[50];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (regcomp(&rx, regex, REG_EXTENDED) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed to compute regex", -1);

  keys = hash_get_keys(&types_hash, &keynbr);
  for (match = index = 0; index < keynbr; index++)
    {
      if (!regexec(&rx, keys[index], 0, 0, 0))
	{
	  revm_type_print(keys[index], 0);
	  match++;
	}
    }

  snprintf(buf, sizeof(buf), " [*] Matched %u types \n\n", match);
  revm_output(buf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}



/**
 * Print available types 
 *
 * @return
 */
int		revm_type_prints()
{
  int		keynbr;
  char		**keys;
  int		index;
  int		total;
  char		buf[BUFSIZ];


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!types_hash.elmnbr)
    {
      revm_output("\n  .:: No registered types\n\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  revm_output("\n  .:: Available meta-language types \n\n");
  keys = hash_get_keys(&types_hash, &keynbr);
  for (total = index = 0; index < keynbr; index++)
    total += revm_type_print(keys[index], 1);
  snprintf(buf, sizeof(buf), "\n  .:: Found %u registered types\n\n", total);
  revm_output(buf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
