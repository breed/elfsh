/**
 * @defgroup librevm_hdr librevm Headers
 */
/**
** @file revm.h
** @ingroup librevm_hdr
** Started on  Thu Feb 22 07:19:04 2001 jfv
** 
** Moved from elfsh to librevm on January 2007 -may
**
**
** $Id$: revm-objects.h,v 1.9 2007-11-30 10:13:54 may Exp $
**
*/
#ifndef __REVM_OBJECTS_H_
 #define __REVM_OBJECTS_H_


/** 
 * Meta-object : Abstract object description when reflected 
 */
typedef	struct		s_revm_object
{

  /* Access handlers */
  eresi_Addr          (*get_obj)(void *parent);
  int	              (*set_obj)(void *parent, eresi_Addr value);
  char                *(*get_name)(void*, void *obj);
  int                 (*set_name)(void*, void *, char *);
  char                *(*get_data)(void*, u_int off, u_int);
  int                 (*set_data)(void*, u_int, char *, u_int, u_int);

  /* Attributes */
  elfshobj_t          *root;          /* Root parent */
  void                *parent;        /* Direct parent */
  u_int               off;            /* Optional byte offset */
  u_int               size;           /* Size of the immediate string */
  u_int               sizelem;        /* Size of element for OBJRAW */
  u_char              immed;          /* Immediate binary flag */
  u_char              perm;	      /* TRUE if obj is a script variable */
  aspectype_t	      *otype;	     /* The object type identifier */

  /* Only when describing elements of hash tables or list */
#define		      CONT_UNKNOW	0
#define		      CONT_HASH		1
#define		      CONT_LIST		2
  u_char	      contype;		/* Container type */
  char		      *hname;		/* Name of parent container */
  char		      *kname;		/* Name of element in container */

  /* Immediate value if immed flag is set */
  union               immval
  {
    u_char            byte;
    u_short           half;
    u_int             word;
    eresi_Addr        ent;
    char              *str;
  }                   immed_val;
}                     revmobj_t;


typedef struct s_revm_annotation revmannot_t;

/**
 * @brief Structure for an expression in ERESI : A tree of revmobj_t 
 */
typedef struct		s_revm_expr
{
  char			*label;		/*! Relative current field name */
  char			*strval;	/*! ASCII form of current expression object */
  aspectype_t		*type;		/*! Expression type */
  revmobj_t		*value;		/*! Expression field value */
  revmannot_t		*annot;		/*! Annotation on this expression */
  struct s_revm_expr	*parent;	/*! Parent expression */
  struct s_revm_expr	*childs;	/*! Child expressions list, if non-terminal */
  struct s_revm_expr	*next;		/*! Next expression if curexpr is in record */
}			revmexpr_t;


/**
 * @brief Generic structure for objects program annotations in ERESI 
 */
struct			s_revm_annotation
{
  u_char		inhash;		/*! Is this expression somewhere in hash or list ? */
#define EDFMT_SCOPE_UNK    0
#define EDFMT_SCOPE_GLOBAL 1
#define EDFMT_SCOPE_FUNC   2
  u_char 		scope;		/*! XXX: Unused for now ? */
  eresi_Addr 		addr; 		/*! Global address */
  u_int			typenum;	/*! Type id associated to this annotation */
  u_int 		reg;   		/*! Function reg id base (stack) */
  int 			relvalue;     	/*! Relative value based on reg */
  int			nameoff;	/*! Name offset in string table */
					/* XXX: should be a symbol instead */
  revmexpr_t		*expr;		/* Annotated expression, if already constructed */
};


/**********************************************************************************/
/****** Those 2 objects are reminicescence of the old object system of elfsh ******/
/**** Need to disapear to use only revmannot/revmobj/aspectype instead of those *****/
/**************** This only affects ERESI types of ELF structures ******************/
/**********************************************************************************/


/**
 * @brief REVM Level 2 (= child of L1) object structure 
 */
typedef struct	s_L2handler
{

  /* For fields */
  eresi_Addr	(*get_obj)(void *obj);                          /*! Read object */
  int		(*set_obj)(void *par, eresi_Addr arg);          /*! Write object */
  
  /* For names */
  char          *(*get_name)(elfshobj_t *, void *obj);          /*! Get name */
  int	        (*set_name)(elfshobj_t *, void *, char *);      /*! Set name */
  
  /* For sections data */
  char		*(*get_data)(elfshsect_t *, u_int, u_int);	/*! Read data */
  int		(*set_data)(elfshsect_t*, u_int, char*, u_int, u_int); /* Write data */
  
  u_char        type;                                           /*! Object type */
}               revmL2_t;



/**
 * @brief ELFsh Level 1 object (= parent object) structure 
 */
typedef struct	s_L1handler
{
  hash_t	*l2list;					/*! A ptr on the child L2 hashtable */
  u_int		elem_size;					/*! Size of one element of this object */
  
  /* Handlers */
  void		*(*get_obj)(void *container, void *retarg);	/*! Read object and return size */
  void		*(*get_obj_idx)(void *f, eresi_Addr i, void *a); /*! Read handler for arrays objects */
  void		*(*get_obj_nam)(void *f, char *name);		/*! Read handler by name */
  void		*(*get_entptr)(void *f, eresi_Addr idx);	/*! Get address */
  eresi_Addr   	(*get_entval)(void *ptr);			/*! Get value */
  eresi_Addr   	(*set_entval)(void *ptr, eresi_Addr vaddr);     /*! Set value */
}		revmL1_t;



#endif
