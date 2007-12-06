/*
** expressions.c for librevm in ERESI
**
** Implementation of scripting declarations for meta-language variables
**
** Started on Jun 23 2007 23:39:51 jfv
** $Id: expressions.c,v 1.20 2007-12-06 20:12:11 may Exp $
*/
#include "revm.h"


/** Get the real value of the parameter string for further revmobj initialization */
/** This function set the label and strval fields of the newly created revmexpr_t */
static revmexpr_t *revm_expr_read(char **datavalue)
{
  revmexpr_t	*expr;
  char		*datastr;
  u_int		opening;
  u_int		closing;
  u_int		beginning;
  char		*namend;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  datastr = *datavalue;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, expr,
	 sizeof(revmexpr_t), NULL);

  /* Check if a variable is given as value */
  /* FIXME: this is not good ! strval contains the initial value and not
     the current value of the source variable */
  if (*datastr == REVM_VAR_PREFIX)
    {
      expr = revm_expr_get(datastr);
      if (expr)
	{

#if __DEBUG_EXPRS__
	  fprintf(stderr, " [D] FOUND REVMEXPR = %s :: %s (recursing!) \n", 
		  expr->label, expr->strval);
#endif

	  datastr = strdup(expr->strval);
	  expr = revm_expr_read(&datastr);
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
	}
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to get root field or type name", NULL);
    }

  /* First get the field (or top-level type) name */
  expr->label = datastr;
  while (*datastr && *datastr != '(' && *datastr != ':')
    datastr++;
  if (!*datastr)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to get root field or type name", NULL);
    }

  /* Implicit field/type name */
  else if (expr->label == datastr)
    expr->label = NULL;

  /* Eat as many '(' or a ':' */
  namend = datastr;
  for (opening = 0; *datastr == '('; datastr++)
    beginning = ++opening;
  if (!opening && *datastr == ':')
    *datastr++ = 0x00;
  if (!*datastr)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid field syntax", NULL);
    }
  
  /* Get the nesting level */
  expr->strval = datastr;
  for (closing = 0; *datastr; datastr++)
    switch (*datastr)
      {
      case '(':
	opening++;
	break;
      case ')':
	closing++;
	if (closing > opening)
	  {
	    XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Invalid closing brace in field value", NULL);
	  }
	break;
      case ',':
	if (opening == closing)
	  {
	    *datavalue = datastr + 1; 
	    for (*datastr-- = 0x00; *datastr == ')' && beginning-- > 0; 
		 *datastr-- = 0x00)
 	      *namend++ = 0x00;

#if __DEBUG_EXPRS__
	    fprintf(stderr, " [D] NEW REVMEXPR = %s :: %s \n", 
		    expr->label, expr->strval);
#endif

	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
	  }
	break;
      default:
	break;
      }

  /* Well-bracketed end of string : OK */
  if (opening == closing)
    {
      for (*datavalue = datastr--; *datastr == ')' && beginning-- > 0; 
	   *datastr-- = 0x00)
	*namend++ = 0x00;
      
#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] NEW REVMEXPR =  %s ::: %s \n", 
	      expr->label, expr->strval);
#endif

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
    }
  
  /* Default error */
  XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Malformed parent field value", NULL);
}


/* Initialize a (potentially record) ERESI variable */
static revmexpr_t	*revm_expr_init(char		*curpath, 
					revmexpr_t	*curexpr,
					aspectype_t	*curtype, 
    					void		*srcdata,
					char		*datavalue)
{
  static u_int	toplevel = 1;
  static u_int	pathsize = 0;
  char		pathbuf[BUFSIZ + 1] = {0x00};
  char		*recpath;
  revmexpr_t	*newexpr, *rootexpr, *prevexpr;
  void		*childata;
  aspectype_t	*childtype;
  revmexpr_t	*curdata;
  u_int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!curexpr)
    toplevel = 1;
  newexpr = rootexpr = prevexpr = NULL;

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Entering revm_expr_init with toplevel = %u\n", toplevel);
#endif

  /* Preliminary processing */
  if (!curexpr)
    {
      strncpy(pathbuf, curpath, BUFSIZ);
      recpath = pathbuf;
      pathsize = strlen(curpath);
    }
  else
    recpath = curpath;

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Current expr path(%s) \n", recpath);
#endif

  /* Construct the expression until end of ascii string */
  while (*datavalue)
    {
      
#if __DEBUG_EXPRS__
      fprintf(stderr, " [D] Current datavalue = (%s) \n", datavalue);
#endif

      /* Read next typed expression in string */
      newexpr = revm_expr_read(&datavalue);
      if (newexpr == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to read expression value", NULL);	  

      /* If we are at the root expression, type might have been implicit */
      if (toplevel && newexpr->label)
	{

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] Top level type with explicit type label = (%s) strval = (%s) \n", 
		  newexpr->label, newexpr->strval);
#endif

	  if (strcmp(newexpr->label, curtype->name))
	    {
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Type checking failed during expression initialization", NULL);	  
	    }
	  datavalue = newexpr->strval;
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	  toplevel = 0;
	  continue;
	}
      else if (!curexpr && !newexpr->label)
	{
	  toplevel = 0;
	  datavalue = newexpr->strval;

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] No explicit top-level type, continuing with datavalue = %s \n", 
		  datavalue);
#endif

	  continue;
	}
      else
	toplevel = 0;


      /* Else we check if the field has children */
      childtype = aspect_type_get_child(curtype, newexpr->label);
      if (!childtype)
	{
	  XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	  pathsize = 0;
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		       "Invalid child structure for variable", NULL);
	}
      newexpr->type = childtype;

      /* Duplicate names cause they are on the stack now */
      newexpr->label = (char *) strdup(newexpr->label);
      newexpr->strval = (char *) strdup(newexpr->strval);

      /* Non-terminal case : we will need to recurse */
      if (childtype->childs && !childtype->dimnbr)
	{

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] NOW RECORD field, recursing ! \n");
#endif

	  childata = (char *) srcdata + childtype->off;
	  len = snprintf(recpath + pathsize, BUFSIZ - pathsize,			
			 ".%s", childtype->fieldname);
	  revm_inform_type_addr(childtype->name, recpath, 
				(elfsh_Addr) childata, newexpr, 0, 0);
	  pathsize += len;

	  /* Insert child where necessary */ 
	  if (!revm_expr_init(recpath, newexpr, childtype, 
			      childata, newexpr->strval))
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Invalid child tree for variable", NULL);
	    }   
	    
	  pathsize -= len;
	  bzero(pathbuf + pathsize, len);
	}
      
       /* Terminal case : no recursion */
      else
	{

#if __DEBUG_EXPRS_MORE__
	  fprintf(stderr, " [D] NOW Terminal field, setting its value\n");
#endif

	  /* Handle RAW terminal field */
	  if (childtype->type == ASPECT_TYPE_RAW)			       
	    {
	      //FIXME: Call hexa converter curval.datastr and set field
	      fprintf(stderr, " [E] Raw object initialization yet unsupported.\n");
	      continue;
	    }

	  /* Lookup scalar value and assign it to the field */
	  newexpr->value = revm_object_lookup_real(curtype, recpath, 
						   childtype->fieldname, 0);	
	  curdata  = revm_lookup_param(newexpr->strval);
	  if (!newexpr->value || !curdata)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Unable to lookup src or dst object", NULL);
	    }

	  /* Convert source data to recipient type and set it */
	  if (newexpr->type->type != curdata->type->type)
	    revm_convert_object(curdata, newexpr->type->type);
	  if (revm_object_set(newexpr, curdata) < 0)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
	      pathsize = 0;
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Failed to set destination object", NULL);
	    }

	  /* Destroy the temporary variable we have created for the right-hand-side value */
	  if (revm_variable_istemp(curdata))
	    revm_expr_destroy(curdata->label);

	  /* Handle terminal Array fields */
	  if (childtype->dimnbr && childtype->elemnbr)			
	    {
	      //FIXME: Use child->elemnbr[idx] foreach size of dim (Use previous code in loop)
	      fprintf(stderr, 
		      " [E] Arrays objects initialization unsupported\n");
	      continue;
	    }

	  /* Inform the runtime system about this terminal field */
	  childata = (char *) srcdata + childtype->off;			
	  len = snprintf(recpath + pathsize, BUFSIZ - pathsize,		
			 ".%s", childtype->fieldname);			
	  revm_inform_type_addr(childtype->name, recpath,		
				(elfsh_Addr) childata, newexpr, 0, 0); 
	  bzero(recpath + pathsize, len);				
	}

      /* Link next field of current structure */
      if (curexpr)
	{
	  rootexpr = curexpr;
	  if (prevexpr)
	    {
	      prevexpr->next = newexpr;
	      prevexpr = newexpr;
	    }
	  else
	    {
	      curexpr->childs = newexpr;
	      prevexpr = newexpr;
	    }
	}
      else
	{
	  if (prevexpr)
	    {
	      prevexpr->next = newexpr;
	      prevexpr = newexpr;
	    }
	  else
	    rootexpr = prevexpr = newexpr;
	}

    }

  /* Return success or error */
  if (!rootexpr)
    {
      if (newexpr)
	XFREE(__FILE__, __FUNCTION__, __LINE__, newexpr);
      pathsize = 0;
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Unable to find a root expression", NULL);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, rootexpr);
}



/* Compare or Set source and destination */
/* OP = REVM_OP_SET or REVM_OP_MATCH */
static int		revm_expr_handle(revmexpr_t	*dest, 
					 revmexpr_t	*source, 
					 u_char		op)
{
  int			ret;
  elfsh_Addr		cmpval;
  revmexpr_t		*cursource;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  if (!dest || !source)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  if (op != REVM_OP_SET && op != REVM_OP_MATCH)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid requested operation", -1);

  /* Recursive comparison or setting */
  for (cmpval = 0; dest; dest = dest->next)
    {

      /* Find this field in the current hierarchy list of fields */
      for (cursource = source; cursource; cursource = cursource->next)
	if (!strcmp(dest->label, cursource->label))
	  break;
      if (!cursource)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot find expression field", -1);
      
      if (cursource->childs)
	{
	  ret = revm_expr_handle(dest->childs, cursource->childs, op);
	  if (ret != 0)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
	  continue;
	}

      /* FIXME-XXX: set/match in case of array ! */
      /* FIXME-XXX: set/match in case of raw type ! */
      
      /* Now realize the operation depending if we are a leaf or not */
      switch (op)
	{
	case REVM_OP_SET:
	  if ((ret = revm_object_set(dest, cursource)) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to set expression field", ret);
	  break;
	case REVM_OP_MATCH:	  
	  if ((ret = revm_object_compare(dest, cursource, &cmpval)) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Unable to compare expression fields", ret);
	  if (cmpval)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
	  break;
	}
    }
  
  /* Final checks and result */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Get (and optionally print) the tree of an expression */
static int	revm_expr_printrec(revmexpr_t *expr, u_int taboff, u_int typeoff, u_int iter)
{
  aspectype_t	*curtype;
  char		buf[BUFSIZ];
  int		len;
  char		*size;
  char		*typename;
  char		offset[128];
  int		idx;
  int		sz;
  char		*pad, *pad2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (; expr; expr = (iter ? expr->next : NULL))
    {
      
      /* Get type of the first real top level expression */
      curtype = expr->type;
      typename = curtype->name;
      
      /* Some printing header */
      revm_endline();
      len = snprintf(buf, sizeof(buf), "%s%*s %*s", 
		     (curtype->isptr ? revm_colorwarn("    *") : ""),
		     (iter ? 18 - (curtype->isptr * 4) : 0), 
		     revm_colorfieldstr(typename), 
		     (iter ? 18 : 0), 
		     revm_colortypestr_fmt("%10s", expr->label));
      
      sz = (taboff < 21 ? 0 : len - revm_color_size(buf) - 20);
      pad = alloca(taboff + sz + 1);
      memset(pad, ' ', taboff + sz);
      pad[taboff + sz] = 0x00;
      
      /* If the expr is a structure */
      if (expr->childs)
	{
	  revm_output(buf);
	  revm_output(revm_colorwarn(" {"));
	  revm_endline();
	  revm_expr_printrec(expr->childs, 
			     taboff + len - revm_color_size(buf) - 7, 
			     typeoff, iter);
	  revm_output(revm_colorwarn("}"));
	  if (expr->next)
	    {
	      revm_output(",\n");
	      pad2 = alloca(taboff + 1);
	      memset(pad2, ' ', taboff);
	      pad2[taboff] = 0x00;
	      revm_output(pad2);
	    }

	  /* Do not add offset if we are in union */
	  if (expr->next && expr->next->type->off != curtype->off)
	    typeoff += curtype->size;
	  continue;
	}
      
      // FIXME-XXX: Print up to 10 bytes
      if (curtype->type == ASPECT_TYPE_RAW)
	{
	  size = alloca(100);
	  snprintf(size, 100, "%s%s%s", 
		   revm_colorwarn("["), 
		   revm_colornumber("%u", curtype->size), 
		   revm_colorwarn("] = {...} "));
	}
      
      // FIXME-XXX: Print up 10 elements of array ...
      else if (curtype->dimnbr && curtype->elemnbr)
	{
	  for (sz = idx = 0; idx < curtype->dimnbr; idx++)
	    sz += 40;
	  size = alloca(sz + 50);
	  for (sz = idx = 0; idx < curtype->dimnbr; idx++)
	    sz += sprintf(size + sz, "%s%s%s", 
			  revm_colorwarn("["),
			  revm_colornumber("%u", curtype->elemnbr[idx]),
			  revm_colorwarn("]"));
	  sz += sprintf(size + sz, "%s", revm_colorwarn(" = {...} "));
	}
      else
	{
	  size = alloca(50);
	  snprintf(size, 50, "%s", revm_colorwarn(" = "));
	}

      /* Format the offset */
      snprintf(offset, sizeof(offset), "@ off(%s)", 
	       revm_colornumber("%u", typeoff));
      
      /* Print everything in order */
      revm_output(buf);
      revm_output(size);
      if (expr->value)
	revm_object_print(expr->value);
      revm_output(offset);

      /* Next field ! */
      if (iter && expr->next)
	{
	  revm_output(revm_colorwarn(",\n"));
	  revm_output(pad);      

	  /* Do not add size if we are in a union */
	  if (expr->next->type->off != curtype->off)
	    typeoff += curtype->size;
	}
      revm_endline();
    }
      
  /* Return success  */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Recursive copy of an expression */
static int		revm_expr_copyrec(aspectype_t	*parentype,
					  revmexpr_t	*dest, 
					  revmexpr_t	*source,
					  char		*newname,
					  u_int		namelen,
					  u_int		nameoff,
					  char		*data)
{
  int			ret;
  aspectype_t		*type;
  char			*childata;
  u_int			len;

  /* Preliminary checks */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dest || !source)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);

  /* Recursive comparison or setting */
  while (source)
    {
      memcpy(dest, source, sizeof(revmexpr_t));
      if (source->label)
	dest->label = strdup(source->label);
      if (source->strval)
	dest->strval = strdup(source->strval);
      
      /* Copy children structure */
      if (source->childs)
	{
	  type = source->childs->type;
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->childs, sizeof(revmexpr_t), -1);
	  len = snprintf(newname + nameoff, namelen - nameoff, ".%s", source->label);
	  childata = data + type->off;
	  revm_inform_type_addr(type->name, strdup(newname), (elfsh_Addr) childata, dest->childs, 0, 0);
	  ret = revm_expr_copyrec(source->type, dest->childs, source->childs, newname, 
				  namelen, nameoff + len, childata);
	  if (ret != 0)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
	  bzero(newname + nameoff, len);
	}

      /* FIXME-XXX: copy in case of array ! */
      /* FIXME-XXX: copy in case of raw type ! */

      /* Copy terminal field */
      else if (source->value)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->value, sizeof(revmobj_t), -1);
	  type = source->value->otype;
	  dest->value = revm_object_lookup_real(parentype, newname, source->label, 0);
	  len = snprintf(newname + nameoff, namelen - nameoff, ".%s", source->label);
	  childata = data + type->off;
	  revm_inform_type_addr(type->name, strdup(newname), (elfsh_Addr) childata, dest, 0, 0);
	  bzero(newname + nameoff, len);
	  if (revm_object_set(dest, source) < 0)
	    {
	      XFREE(__FILE__, __FUNCTION__, __LINE__, dest);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			   "Failed to copy terminal object", -1);
	    }
	}

      /* Allocate next element */
      source = source->next;
      if (source)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->next, sizeof(revmexpr_t), -1);
	  dest = dest->next;
	}
    }
  
  /* Final checks and result */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Copy an expression (set $e1 $e2) */
revmexpr_t	*revm_expr_copy(revmexpr_t *source, char *dstname)
{
  revmexpr_t	*dest;
  aspectype_t	*type;
  int		ret;
  char		newname[BUFSIZ] = {0x00};
  int		curoff;
  char		*copydata;
  hash_t	*thash;
  revmannot_t	*annot;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First copy the data */
  type = source->type;
  XALLOC(__FILE__, __FUNCTION__, __LINE__, copydata, type->size, NULL);
  snprintf(newname, sizeof(newname), "type_%s", type->name);
  thash = hash_find(newname);
  annot = hash_get(thash, source->label);

  /* Constants are not annotated, we might not want to do anything here */
  if (annot)
    memcpy(copydata, (char *) annot->addr, type->size);

  /* Create a temporary variable if necessary */
  if (*dstname != REVM_VAR_PREFIX || 
      !strncmp(dstname, "$hash", 5) || 
      !strncmp(dstname, "$list", 5))
    dstname = revm_tmpvar_create();
  else
    dstname = strdup(dstname);

  /* Then copy the meta-data */
  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest, sizeof(revmexpr_t), NULL);
  strncpy(newname, dstname, sizeof(newname));
  curoff = strlen(newname);
  if (source->strval)
    dest->strval = strdup(source->strval);
  dest->label    = strdup(newname);
  dest->type     = type;
  if (!revm_inform_type_addr(type->name, dstname, (elfsh_Addr) copydata, dest, 0, 0))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to inform copy expression", NULL);

  /* Some (structure) expressions have an extra top-level type, copy was done before the recursion */
  if (source->childs) 
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->childs, sizeof(revmexpr_t), NULL);
      ret = revm_expr_copyrec(dest->type, dest->childs, source->childs, 
			      newname, BUFSIZ, curoff, copydata);
    }

  /* Simple object copy is easier */
  else if (aspect_type_simple(type->type) && !type->next)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, dest->value, sizeof(revmobj_t), NULL);
      dest->value->otype = dest->type;
      dest->value->immed = 1;
      dest->value->perm  = 1;
      ret = revm_object_set(dest, source);
    }

  /* Default case : complex types without top-level */
  else
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to copy expression in impossible default case", NULL);

  /* Return error or success */
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to copy expression", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}


/* Create an expression from an object */
revmexpr_t	*revm_expr_create_from_object(revmobj_t *copyme, char *name)
{
  revmexpr_t	*dest;
  aspectype_t	*type;
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  dest = hash_get(&exprs_hash, name);
  if (dest)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);

  /* Create a new temporary variable name if necessary */
  if (*name != REVM_VAR_PREFIX || 
      !strncmp(name, "$hash", 5) || 
      !strncmp(name, "$list", 5))
    name = revm_tmpvar_create();
  else
    name = strdup(name);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, dest, sizeof(revmexpr_t), NULL);
  dest->type     = copyme->otype;
  type           = dest->type;
  dest->label    = name;
  if (!copyme->otype || (aspect_type_simple(copyme->otype->type) && !copyme->otype->next))
    dest->value  = copyme;
  else
    {
      data = (void *) copyme->get_obj(copyme->parent);
      if (!data)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to dereference object", NULL);
      if (!revm_inform_type_addr(type->name, name, (elfsh_Addr) data, dest, 0, 1))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to create expression from complex object", NULL);
    }

  hash_add(&exprs_hash    , (char *) strdup(name), (void *) dest);

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] Create_Expr_From_Object %s added with type = %s \n", 
	  name, (type ? type->name : "UNKNOWN TYPE"));
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dest);
}
  
  
/* Print an annotated expression */
int		revm_expr_print(char *pathname)
{
  revmexpr_t	*expr;
  int		ret;
  char		buf[BUFSIZ];
  int		iter;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  expr = hash_get(&exprs_hash, pathname);
  if (!pathname || *pathname != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  if (!expr || !expr->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expression name", -1);

  /* Make sure we only print the subrecord if requested */
  if (expr->childs)
    expr = expr->childs;

  /* Guess if we need to iterate printing or not */
  iter = (aspect_type_simple(expr->type->type) && !expr->next ? 0 : 1);

  /* If we are printing a simple type or a subtype expression, we need to print a top level */
  if (!iter || expr->next)
    {
      snprintf(buf, BUFSIZ, "  %-20s %s", revm_colorfunction(pathname), revm_colorwarn("= {"));
      revm_output(buf);
      revm_endline();
    }
  revm_expr_printrec(expr, (!iter || !expr->next ? strlen(pathname) + 6 : 1), 0, iter);
  if (!iter || expr->next)
    revm_output(revm_colorwarn("}"));
  revm_endline();
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Get an expression from its name */
revmexpr_t	*revm_expr_get(char *pathname)
{
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!pathname || *pathname != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", NULL);    
  expr = hash_get(&exprs_hash, pathname);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid expression name", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}

/* Set an expression to the value of another (only if compatible) */
int		revm_expr_set_by_name(char *dest, char *source)
{
  revmexpr_t	*adst;
  revmexpr_t	*asrc;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dest || !source || *dest != REVM_VAR_PREFIX || *source != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  adst = revm_expr_get(dest);
  asrc = revm_expr_get(source);
  if (!adst || !asrc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);

  /* Return result */
  ret = revm_expr_set(adst, asrc);
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to set expression by name", ret);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Set an expression to the value of another (only if compatible) */
int		revm_expr_set(revmexpr_t *adst, revmexpr_t *asrc)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!adst || !asrc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);    
  
  /* Take care of top-level structure types */
  if (asrc->childs)
    asrc = asrc->childs;
  if (adst->childs)
    adst = adst->childs;

  /* Necessary for assignment of scalar values */
  if (adst->value && asrc->value && !asrc->next)
    {
      /* See if object conversion is necessary and/or possible */
      if (revm_nextconds_atomics(adst, asrc) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to set scalar variable", -1);
      if (revm_object_set(adst, asrc) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to set scalar variable", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Filter the base error case */
  else if (adst->value && !asrc->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot set scalar with complex value", ret);
  else if (!adst->value && asrc->value)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot set structure with scalar value", ret);

  /* Set a whole structure or a substructure */
  else
    ret = revm_expr_handle(adst, asrc, REVM_OP_SET);

  /* Result */
  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to set expression", ret);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Compare 2 typed expressions */
int		revm_expr_compare_by_name(char *original, char *candidate, elfsh_Addr *val)
{
  revmexpr_t	*candid;
  revmexpr_t	*orig;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candidate || !original || *candidate != REVM_VAR_PREFIX || *original != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  candid = revm_expr_get(candidate);
  orig   = revm_expr_get(original);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);

  ret = revm_expr_compare(orig, candid, val);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Compare 2 typed expressions */
int		revm_expr_compare(revmexpr_t *orig, revmexpr_t *candid, elfsh_Addr *val)
{
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Take care of top-level structure types */
  if (orig->childs)
    orig = orig->childs;
  if (candid->childs)
    candid = candid->childs;

  /* Comparison between simple objects */
  /* Necessary for comparison with constants without a tree/field structure */
  if (orig->value && candid->value && !orig->next && !candid->next)
    {
      if (revm_object_compare(orig, candid, val) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid name for expression", -1);    
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /* Filter the base error case */
  else if ((orig->value && !candid->value) || (!orig->value && candid->value))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot compare scalar with complex expression", ret);

  /* Comparison between whole structures */
  /* A comparison is an symmetric matching of the 2 objects */
  else
    {
      ret = revm_expr_handle(candid, orig, REVM_OP_MATCH);
      if (!ret)
	ret = revm_expr_handle(orig, candid, REVM_OP_MATCH);
    }

  /* Return result */
  *val = ret;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Match or not 2 typed expressions */
int		revm_expr_match_by_name(char *original, char *candidate)
{
  revmexpr_t	*candid;
  revmexpr_t	*orig;
  int		ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candidate || !original || *candidate != REVM_VAR_PREFIX || *original != REVM_VAR_PREFIX)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", -1);    
  candid = revm_expr_get(candidate);
  orig   = revm_expr_get(original);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid input parameters", -1);
  ret = revm_expr_match(candid, orig);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Match or not 2 typed expressions */
int		revm_expr_match(revmexpr_t *candid, revmexpr_t *orig)
{
  int		ret;
  elfsh_Addr	*val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!candid || !orig)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameters", -1);    

  /* Take care of top-level structure types */
  if (!orig->next && orig->childs)
    orig = orig->childs;
  if (!candid->next && candid->childs)
    candid = candid->childs;

  /* For simple objects */
  if (orig->value && candid->value && !orig->next && !candid->next)
    {
      if (revm_object_compare(orig, candid, val) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid name for expression", -1);    
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  
  /* Comparison between whole structures */
  /* A comparison is an symmetric matching of the 2 objects */
  else if ((orig->value && !candid->value) || (!orig->value && candid->value))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot match scalar with complex expression", ret);

  /* Comparison between substructures */
  else
    ret = revm_expr_handle(candid, orig, REVM_OP_MATCH);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/* Create a new revm expression */
revmexpr_t	*revm_expr_create(aspectype_t	*datatype,
				  char		*dataname,
				  char		*datavalue) 
{
  revmexpr_t	*expr;
  char		*data;
  char		*realname;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!dataname)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid name for expression", NULL);    
  if (!datatype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid type for expression", NULL);    

  /* Create a new temporary if necessary */
  if (*dataname != REVM_VAR_PREFIX || 
      !strncmp(dataname, "$hash", 5) || 
      !strncmp(dataname, "$list", 5))
    dataname = revm_tmpvar_create();
  else
    dataname = strdup(dataname);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, data, datatype->size, NULL);
  realname = dataname;

  revm_inform_type_addr(datatype->name, realname, (elfsh_Addr) data, NULL, 0, 0);

  if (!datatype->next && datatype->childs)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__, expr, sizeof(revmexpr_t), NULL);
      expr->strval = strdup(datavalue);
      expr->label  = dataname;
      expr->type   = datatype;
      expr->childs = revm_expr_init(dataname, NULL, datatype, data, datavalue);
    }
  else
    expr = revm_expr_init(dataname, NULL, datatype, data, datavalue);
  
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to create REVMEXPR", NULL);    

  revm_inform_type_addr(datatype->name, realname, (elfsh_Addr) data, expr, 0, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}


/* Simply create an expression from an revmobj_t */
revmexpr_t	*revm_simple_expr_create(aspectype_t *datatype, char *name, char *value)
{
  revmexpr_t	*expr;
  revmobj_t	*obj;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, expr, sizeof(revmexpr_t), NULL);
  if (value)
    obj = revm_lookup_immed(value);
  else
    obj = revm_object_lookup_real(datatype, name, NULL, 0);
  if (!obj)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to create REVMEXPR", NULL);
  
  if (*name != REVM_VAR_PREFIX || !strncmp(name, "$hash", 5) || !strncmp(name, "$list", 5))
    name = revm_tmpvar_create();
  else
    name = strdup(name);

  expr->type   = datatype;
  expr->strval = (value ? strdup(value) : NULL);
  expr->value  = obj;
  expr->label  = name;
  hash_add(&exprs_hash    , (char *) strdup(name), (void *) expr);

#if __DEBUG_EXPRS__
  fprintf(stderr, " [D] SimpleExpr %s added with type = %s \n", 
	  name, (datatype ? datatype->name : "UNKNOWN TYPE"));
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, expr);
}




/* Read the requested type for an expression in ascii form */
aspectype_t	*revm_exprtype_get(char *exprvalue)
{
  aspectype_t	*type;
  char		*typename;
  u_int		typenamelen;
  char		*curexprvalue;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!exprvalue)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", NULL);
  type = NULL;
  for (curexprvalue = exprvalue, typenamelen = 0; *curexprvalue != '('; typenamelen++)
    curexprvalue++;
  typename = alloca(typenamelen + 1);
  strcpy(typename, exprvalue);
  type = aspect_type_get_by_name(typename);
  if (!type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expression type", NULL);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, type);
}



/* Destroy an expression and remove it from the hash table */
int		revm_expr_destroy(char *e)
{
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!e)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid NULL parameter", -1);

#if __DEBUG_EXPRS__
  printf("\n [D] DestroyExpr %s \n", e);
#endif

  expr = hash_get(&exprs_hash, e);
  if (!expr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unknown expr parameter", -1);
  hash_del(&exprs_hash, e);

  // XXX: must free recursively ! memory leak here
  if (expr->value)
    //revm_destroy_object(expr->value); // this was not copied/reallocated correctly somewhere
    XFREE(__FILE__, __FUNCTION__, __LINE__, expr->value);

  XFREE(__FILE__, __FUNCTION__, __LINE__, expr);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
