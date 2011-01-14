/**
* @file libaspect/include/libaspect.h
 **
 ** @brief The header file for modular objects in the framework
 **
 ** Started Dec 22 2006 02:57:03 jfv
 **
 **
 ** $Id$
 **
 */
#if !defined(__ASPECT_H__)
 #define __ASPECT_H__ 1

#ifndef __KERNEL__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <assert.h>

/* Debug flags */
#define		__DEBUG_LIST__	1

#ifdef  __BEOS__
 #include <image.h>
 #include <bsd_mem.h>
#else
#if !defined(__USE_GNU)
 #define __USE_GNU
#endif
 #include <dlfcn.h>
#endif

#endif /* __KERNEL__ */

/* Endianess definitions */
#if defined(__linux__) || defined (__BEOS__)

#ifndef __KERNEL__
 #include <endian.h>
#else
 #define __LITTLE_ENDIAN 1234
 #define __BIG_ENDIAN    4321
 #define __PDP_ENDIAN    3412
#endif

#elif defined(__FreeBSD__) || defined (__OpenBSD__) || defined(__NetBSD__)

#include <machine/endian.h>
 #define __LITTLE_ENDIAN _LITTLE_ENDIAN
 #define __BIG_ENDIAN    _BIG_ENDIAN

#if defined(__FreeBSD__)
 #include <elf.h>
 #include <sys/elf_common.h>
 #define __WORDSIZE      __ELF_WORD_SIZE
#endif
 #if __FreeBSD__ < 5
  #define __BYTE_ORDER   __LITTLE_ENDIAN
 #else
  #define __BYTE_ORDER   BYTE_ORDER
 #endif

#elif defined(sun)

 #define __LITTLE_ENDIAN 1234
 #define __BIG_ENDIAN	 4321

#if !defined(__i386)
 #define __BYTE_ORDER    __BIG_ENDIAN
#else
 #define __BYTE_ORDER    __LITTLE_ENDIAN
#endif

#elif defined(HPUX)

 #include <arpa/nameser_compat.h>
 #undef ADD
 #define __LITTLE_ENDIAN LITTLE_ENDIAN
 #define __BIG_ENDIAN    BIG_ENDIAN
 #define __BYTE_ORDER    BYTE_ORDER

#elif defined(IRIX)

 #include <standards.h>
 #include <sys/endian.h>
 #define __LITTLE_ENDIAN LITTLE_ENDIAN
 #define __BIG_ENDIAN    BIG_ENDIAN
 #define __BYTE_ORDER    BIG_ENDIAN

#endif

#ifndef __RELEASE__
#define ASSERT(_x)      assert(_x)
#define NOT_REACHED()   ASSERT(FALSE)
#else
#define ASSERT(_x)      
#define NOT_REACHED()   
#endif
#define NOT_USED(_x)    _x = _x

#ifndef swap32
#define swap32(x)						\
	((uint32_t)(						\
	(((uint32_t)(x) & (uint32_t) 0x000000ffU) << 24) |	\
	(((uint32_t)(x) & (uint32_t) 0x0000ff00U) <<  8) |	\
	(((uint32_t)(x) & (uint32_t) 0x00ff0000U) >>  8) |	\
	(((uint32_t)(x) & (uint32_t) 0xff000000U) >> 24) ))
#endif

#ifndef swap16
#define swap16(x)							\
	((unsigned short)(						\
	(((unsigned short)(x) & (unsigned short) 0x00ffU) << 8) |	\
	(((unsigned short)(x) & (unsigned short) 0xff00U) >> 8) ))
#endif

#ifndef swap64
#define	swap64(x)						    \
	((uint64_t)(						    \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x00000000000000ffULL) << 56) | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x000000000000ff00ULL) << 40) | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x0000000000ff0000ULL) << 24) | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x00000000ff000000ULL) << 8)  | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x000000ff00000000ULL) >> 8)  | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x0000ff0000000000ULL) >> 24) | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0x00ff000000000000ULL) >> 40) | \
	((uint64_t)(((uint64_t)x) & (uint64_t) 0xff00000000000000ULL) >> 56)))
#endif

/* The size of an address depends on if we analyse 32bits or 64bits objects */
#if defined(ERESI32)
 typedef uint32_t	eresi_Addr;
 typedef uint32_t	eresi_Off;

#define	XFMT		"0x%08X"
#define	AFMT		"%08X"
#define UFMT		"%08u"
#define DFMT		"%08d"
#define	RDFMT		"%d"
#define RXFMT		"0x%X"

#elif defined(ERESI64)
 typedef uint64_t	eresi_Addr;
 typedef uint64_t	eresi_Off;
#if __WORDSIZE == 32
 #define XFMT		"0x%016llX"
 #define AFMT		"%016llX"
 #define UFMT		"%016llu"
 #define DFMT		"%016lld"
 #define RDFMT		"%lld"
 #define RXFMT		"0x%llX"
#elif __WORDSIZE == 64
 #define XFMT		"0x%016lX"
 #define AFMT		"%016lX"
 #define UFMT		"%016lu"
 #define DFMT		"%016ld"
 #define RDFMT		"%ld"
 #define RXFMT		"0x%lX"
#else
 #error "__WORDSIZE not defined"
#endif

#else
 #error "You must define either ERESI32 or ERESI64 built"
#endif

/* Define Boolean types */
typedef  uint8_t        Bool;
#define  TRUE           1
#define  FALSE          0

/* Typedef the registers, linear addresses, virtual, etc for better
   understanding. */
typedef  uint32_t       ureg32;
typedef  uint64_t       ureg64;

typedef  uint32_t       la32;
typedef  uint64_t       la64;

typedef  uint32_t       va32;
typedef  uint64_t       va64;


/* Include this here since it contains an allocation too */
#ifndef __KERNEL__
 #include "aproxy.h"
#endif
#include "libaspect-hash.h"
#include "libaspect-list.h"
#include "libaspect-btree.h"
#include "libaspect-profiler.h"

/* Those types are built-in */
#define         ASPECT_TYPE_UNKNOW       0  /*!< Unknown           */
#define         ASPECT_TYPE_RAW          1  /*!< Raw               */
#define		ASPECT_TYPE_BYTE	 2  /*!< Byte	           */
#define         ASPECT_TYPE_STR          3  /*!< String            */
#define         ASPECT_TYPE_SHORT        4  /*!< 2 bytes           */
#define         ASPECT_TYPE_INT          5  /*!< 4 bytes           */
#define         ASPECT_TYPE_LONG         6  /*!< 4 or 8 bytes      */
#define         ASPECT_TYPE_DADDR        7  /*!< 4 or 8 bytes      */
#define         ASPECT_TYPE_CADDR        8  /*!< 4 or 8 bytes      */
#define		ASPECT_TYPE_BIT		 9  /*!< just 1 bit        */
#define		ASPECT_TYPE_OID		10  /*!< object identifier */
#define		ASPECT_TYPE_CORENUM	11  /*!< Core types number */

/* Vectors, tables and hash are parts of "simple types"
   Note: you cannot copy them or look inside them without foreach */
#define	        ASPECT_TYPE_VECT	ASPECT_TYPE_CORENUM         /*!< Vector type	   */
#define		ASPECT_TYPE_HASH	(ASPECT_TYPE_CORENUM + 1)   /*!< Hash table type   */
#define		ASPECT_TYPE_LIST	(ASPECT_TYPE_CORENUM + 2)   /*!< List type         */
#define		ASPECT_TYPE_SIMPLENUM	(ASPECT_TYPE_CORENUM + 3)   /*!< SIMPLE TYPES NUMBER */

/* Now come complex types which are part of the base types */
#define		ASPECT_TYPE_EXPR	ASPECT_TYPE_SIMPLENUM       /*!< Expression type   */
#define		ASPECT_TYPE_BLOC	(ASPECT_TYPE_SIMPLENUM + 1) /*!< Block type        */
#define		ASPECT_TYPE_FUNC	(ASPECT_TYPE_SIMPLENUM + 2) /*!< Function type     */
#define		ASPECT_TYPE_LINK	(ASPECT_TYPE_SIMPLENUM + 3)
#define         ASPECT_TYPE_BASENUM     (ASPECT_TYPE_SIMPLENUM + 4) /*!< BASE TYPES NUMBER */

/* Type names */
#define		ASPECT_TYPENAME_UNKNOW	"unknown"
#define		ASPECT_TYPENAME_RAW	"raw"     
#define		ASPECT_TYPENAME_BYTE	"byte"    
#define		ASPECT_TYPENAME_BIT	"bit"
#define		ASPECT_TYPENAME_STR	"string"  
#define		ASPECT_TYPENAME_SHORT	"short"   
#define		ASPECT_TYPENAME_INT	"int"     
#define		ASPECT_TYPENAME_LONG	"long"    
#define		ASPECT_TYPENAME_DADDR	"daddr"   
#define		ASPECT_TYPENAME_CADDR	"caddr" 
#define		ASPECT_TYPENAME_OID	"oid"
  
#define		ASPECT_TYPENAME_VECT	"vector" 
#define		ASPECT_TYPENAME_HASH	"hash"
#define		ASPECT_TYPENAME_LIST	"list"
#define		ASPECT_TYPENAME_EXPR	"expr"
#define		ASPECT_TYPENAME_BLOC	"bloc"
#define		ASPECT_TYPENAME_FUNC	"func"
#define		ASPECT_TYPENAME_LINK	"link"


/* Max pointer depth for a compound type */
#define		ASPECT_TYPE_MAXPTRDEPTH	1

/* Some general purpose macros */
#define	IS_VADDR(s)		(s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
#define	PRINTABLE(c)		(c >= 32 && c <= 126)

#define LOWORD(_dw)     ((_dw) & 0xffff)
#define HIWORD(_dw)     (((_dw) >> 16) & 0xffff)
#define LOBYTE(_w)      ((_w) & 0xff)
#define HIBYTE(_w)      (((_w) >> 8) & 0xff)

#define HIDWORD(_qw)    ((uint32)((_qw) >> 32))
#define LODWORD(_qw)    ((uint32)(_qw)) 
#define QWORD(_hi, _lo) ((((uint64)(_hi)) << 32) | ((uint32)(_lo)))

#define BYTE_IN_CHAR    2
#define WORD_IN_BYTE    2        
#define DWORD_IN_BYTE   4
#define QWORD_IN_BYTE   8
#define BYTE_IN_BIT     8
#define WORD_IN_BIT     16        
#define DWORD_IN_BIT    32
#define QWORD_IN_BIT    64

#define NEXT_CHAR(_x)   _x + 1

/* A structure for the type information */
typedef struct		s_info
{
  char			*name;
  u_int			size;
}			typeinfo_t;


/**
 * @brief Structure for type 
 */
typedef struct		s_type
{
  u_int			type;		/*!< @brief Plain or pointed type */

  u_char		isptr;		/*!< @brief the type is a pointer */
  u_int			size;		/*!< @brief Type full memsize     */
  u_int			off;		/*!< @brief Offset inside parent  */

  u_int			dimnbr;		/*!< @brief Nbr of array dimensions */
  u_int			*elemnbr;	/*!< @brief Nbr of elements per dim */
  char			*name;		/*!< @brief Type name             */
  char			*fieldname;	/*!< @brief Field name		 */

  /* We have a list of child, and a list
  ** of nexts in case we are ourselves 
  ** a child of a structure object */
  struct s_type		*childs;	/*!< @brief Curobj fields if any */
  struct s_type		*next;		/*!< @brief Next parent field */
}			aspectype_t;


/** 
 * @brief The structure of multidimensional vectors 
 */
typedef struct	s_vector
{
  void		*hook;		   /*!< @brief The vector data */
  void		*register_func;	   /*!< @brief Registration function */
  void		*default_func;	   /*!< @brief Default registered function */
  u_int		*arraydims;	   /*!< @brief Size of each dimension */
  char		**strdims;	   /*!< @brief Label for each dimension */
  u_int		arraysz;	   /*!< @brief Number of dimensions */
  u_int   	type;		   /*!< @brief Elements type in this vector */
}		vector_t;


/**
 * @brief Structure to define a generic container 
 */
typedef struct	s_container	
{
  unsigned int 	id;	  	  /* !< @brief unique id of this container */
  u_int		type;		  /* !< @brief Contained object type */
  u_int		nbrinlinks;	  /* !< @brief Number of -ondisk- input links */
  u_int		nbroutlinks;	  /* !< @brief Number of -ondisk- output links */
  list_t	*inlinks;	  /* !< @brief Input links for this container */
  list_t	*outlinks;	  /* !< @brief Output links for this container */
  void		*data;		  /* !< @brief Pointer to the contained object */
}		container_t;

/**
 * @brief Input and Output links for containers
 */
#define CONTAINER_LINK_IN		0
#define CONTAINER_LINK_OUT		1

/**
 * @brief Config related data types 
 */
#define		CONFIG_HASH_SIZE	256

/**
 * @brief Default names for config names 
 */
#define		CONFIG_NAME_SAFEMODE	"safemode"
#define		CONFIG_USE_ASMDEBUG	"asm.debug"
#define         CONFIG_CFGDEPTH         "mjr.cfg_depth"

/** 
 * @brief Config flags 
 */
#define		CONFIG_SAFEMODE_OFF	0
#define		CONFIG_SAFEMODE_ON	1
#define         CONFIG_CFGDEPTH_DEFAULT 1

/**
 * @brief libasm related config 
 */
#define		CONFIG_ASM_ENDIAN_FLAG		"libasm.endian.flag"
#define		CONFIG_ASM_LITTLE_ENDIAN	0
#define		CONFIG_ASM_BIG_ENDIAN		1

#define		CONFIG_ASM_ATT_MARGIN_DEFAULT	14
#define		CONFIG_ASM_ATT_MARGIN_FLAG	"libasm.output.att.margin"

#define		CONFIG_ASM_SYNTHINSTRS_DEFAULT	1
#define		CONFIG_ASM_SYNTHINSTRS		"libasm.output.synth.instrs"

/* A configuration item */
typedef struct	s_config_item
{
  char		*name;
#define		CONFIG_TYPE_INT	0
#define		CONFIG_TYPE_STR	1
  u_int		type;			  /*!< int will use val, str *data */
  
  /* RO/RW - it's relevant to higher api
   like revm_ allows direct updates to those values
   when RW is set and enforces usage of revm_api
   when RO is set (see profile) */
  u_int		val;			  /*!< For int values 0-off/1-on ... */
#define		CONFIG_MODE_RW	0
#define		CONFIG_MODE_RO	1
  u_int		mode;			  
  void		*data;
}		configitem_t;



/**
 * The library has its world itself 
 * There stands all the flags that are used by libelfsh 
 */
typedef struct	s_aspectworld
{
  hash_t	config_hash;		/*!< Configuration */
  u_char	kernel_mode;		/*!< Kernel residence */

#define		PROFILE_NONE	0
#define		PROFILE_WARN    (1 << 0)
#define		PROFILE_FUNCS   (1 << 1)
#define		PROFILE_ALLOC   (1 << 2)
#define		PROFILE_DEBUG	(1 << 3)
  u_char	proflevel;		/*!< Profiling switch */
  u_char	profstarted;		/*!< Profiling started ? */
  int           (*profile)(char *);	/*!< Profiling output func */
  int           (*profile_err)(char *);	/*!< Profiling error func */
  
  /* Libui pointers */
  void	     	(*endline)();

  /* Simple */
  char		*(*colorinstr)(char *text);
  char        	*(*colorstr)(char *t);
  char         	*(*colorfieldstr)(char *t);
  char         	*(*colortypestr)(char *t);
  char		*(*colorend)(char *text); 
  char		*(*colorwarn)(char *text);
  char		*(*colorfunction)(char *text);
  char		*(*colorfilename)(char *text);
  /* Advanced */
  char 		*(*coloradv)(char *ty, char *p, char *te);
  char		*(*colorinstr_fmt)(char* p, char *t);
  char          *(*coloraddress)(char *p, eresi_Addr a);
  char          *(*colornumber)(char *p, eresi_Off n);
  char          *(*colorstr_fmt)(char *p, char *t);
  char          *(*colorfieldstr_fmt)(char *p, char *t);
  char          *(*colortypestr_fmt)(char *p, char *t);
  char		*(*colorwarn_fmt)(char *pattern, char *text);
}		aspectworld_t;

/* Make it accessible from all files */
extern aspectworld_t	aspectworld;

/* Low-level constructor-time related functions */
int		aspect_init();
void		aspect_called_ctors_inc();
int		aspect_called_ctors_finished();
int		kernsh_is_present();
void		kernsh_present_set();
void		kedbg_present_set();
int		kedbg_is_present();
void		e2dbg_presence_set();
void		e2dbg_presence_reset();
u_char		e2dbg_presence_get();
u_char		e2dbg_kpresence_get();
void		e2dbg_kpresence_set(u_char pres);


/* Retreive pointer on the vector hash table */
hash_t*		aspect_vecthash_get();

/* Retreive a vector from the vectors hash table */
vector_t*	aspect_vector_get(char *name);

/* Add the function pointer to the hook at requested coordonates */
void		aspect_vectors_insert(vector_t *vect, 
				      unsigned int *dim, 
				      unsigned long fct);

/* Get the hook from the dimension array */
void*		aspect_vectors_select(vector_t *vect, 
				      unsigned int *dim);

/* Project each dimension and get the requested data pointer */
void            *aspect_vectors_selectptr(vector_t *vect, 
					  unsigned int *dim);

/* Add a new vector */
int		aspect_register_vector(char *, void*, 
				       unsigned int*,
				       char **, unsigned int, 
				       unsigned int);

/* Container related functions */
container_t	*container_create(u_int		type, 
				  void		*data, 
				  list_t	*in, 
				  list_t	*out, 
				  u_int		uniqid);
int		container_linklists_create(container_t *container,
					   u_int	linktype,
					   u_int	uniqid);

/* Type related functions */
int		aspect_type_simple(int typeid);
char		*aspect_type_get(u_int type);
u_int		aspect_typesize_get(u_int type);
int		aspect_basetypes_create();
int		aspect_type_find_union_size(aspectype_t *type);
int		aspect_type_register_real(char *label, 
					  aspectype_t *ntype);
aspectype_t	*aspect_type_create(u_char isunion,
				    char *label, 
				    char **fields, 
				    u_int fieldnbr);
int		aspect_type_register(u_char isunion,
				     char *label, 
				     char **fields, 
				     u_int fieldnbr);
aspectype_t	*aspect_type_copy(aspectype_t	*type, 
				  unsigned int	off, 
				  u_char	isptr, 
				  u_int		elemnbr, 
				  char		*fieldname,
				  u_int		*dims);
aspectype_t	*aspect_type_copy_by_name(aspectype_t *type, char *name, hash_t *fields, u_int, u_int);
int		aspect_basetype_register(char *name, u_int size);
typeinfo_t	*aspect_basetype_get(unsigned int *nbr);
aspectype_t	*aspect_type_get_by_name(char *name);
aspectype_t	*aspect_type_get_by_id(unsigned int id);
aspectype_t	*aspect_type_get_child(aspectype_t *parent, char *name);
char		*aspect_typename_get(u_int typeid);

/* profile.c : Profiler related functions */
void		profiler_reset(u_int sel);
int		profiler_print(char *file, char *func, 
			       u_int line, char *msg);
void		profiler_err(char *file, char *func, 
			     u_int line, char *msg);
void		profiler_out(char *file, char *func, u_int line);

void		profiler_error();
void		profiler_incdepth();
void		profiler_decdepth();
void		profiler_updir();
int		profiler_enabled();
int		profiler_is_enabled(u_char mask);
u_char		profiler_started();
void		profiler_error_reset();
int		profiler_enable_err();
int		profiler_enable_out();
int		profiler_enable_alloc();
int		profiler_enable_debug();
int		profiler_disable_err();
int		profiler_disable_out();
int		profiler_disable_alloc();
int		profiler_disable_debug();
int		profiler_disable_all();
int		profiler_enable_all();

void		profiler_install(int (*profile)(char *), 
				 int (*profile_err)(char *));

/* The allocation profiler */
void			profiler_alloc_warnprint(char *s, int fat, int i);
void			profiler_alloc_warning(u_char warntype);
void			profile_alloc_shift();
int			profiler_alloc_update(char *file, char *func, 
					      u_int line, u_long addr, 
					      u_char atype, u_char otype);
profallocentry_t	*profiler_alloc_find(u_char direction,
					     u_long addr, 
					     u_char optype);

/* Color related handlers for the profiler */
void		profiler_setcolor(void (*endline)(), 
				  char *(*colorinstr)(char *text),
				  char *(*colorstr)(char *t), 
				  char *(*colorfieldstr)(char *t),
				  char *(*colortypestr)(char *t), 
				  char *(*colorend)(char *text),
				  char *(*colorwarn)(char *text), 
				  char *(*colorfunction)(char *text),
				  char *(*colorfilename)(char *text));
void		profiler_setmorecolor(char *(*coloradv)(char *ty, 
							char *p, 
							char *te),
				      char *(*colorinstr_fmt)(char* p, 
							      char *t),
				      char *(*coloraddress)(char *p,
							    eresi_Addr a),
				      char *(*colornumber)(char *p, 
							   eresi_Off n),
				      char *(*colorstr_fmt)(char *p, 
							    char *t),
				      char *(*colorfieldstr_fmt)(char *p,
								 char *t),
				      char *(*colortypestr_fmt)(char *p,
								char *t),
				      char *(*colorwarn_fmt)(char *pat,
							     char *text));

/* config.c : Configuration related fonctions */
void		config_init();
void		config_add_item(char *name, u_int type, 
				u_int mode, void *dat);
void		config_update_key(char *name, void *data);
void		*config_get_data(char *name);
void 		config_safemode_set();
void		config_safemode_reset();
int		config_safemode();

#endif
