/**
 * @defgroup libkernsh
 * @ingroup kernsh
 */
/**
* @file libkernsh/include/libkernsh.h
** @ingroup libkernsh
**
*/
#ifndef __LIBKERNSH_H__
 #define __LIBKERNSH_H__

#include "libaspect.h"
#include "libelfsh.h"
#include "libasm.h"

#include "libkernsh-virtm.h"

#if defined(__NetBSD__)
#include <kvm.h>
#include <nlist.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif

#if defined(__linux__)

#include <sys/ptrace.h>

#define SYSCALL0(num) asm volatile \
( "\n\tint $0x80" : "=a"(__ret) : "0"(num) );

#define SYSCALL1(num,arg0) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num), "b" (arg0) );

#define SYSCALL2(num,arg0,arg1) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1) );

#define SYSCALL3(num,arg0,arg1,arg2) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2) );

#define SYSCALL4(num,arg0,arg1,arg2,arg3) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2),"S"(arg3) );

#define SYSCALL5(num,arg0,arg1,arg2,arg3,arg4) asm volatile \
( "\n\tint $0x80": "=a"(__ret) : "0"(num),"b" (arg0),"c"(arg1),"d"(arg2),"S"(arg3),"D"(arg4) );

#endif

#define GET_32BIT_LSB_FIRST(cp) \
	(((unsigned long)(unsigned char)(cp)[0]) | \
	((unsigned long)(unsigned char)(cp)[1] << 8) | \
	((unsigned long)(unsigned char)(cp)[2] << 16) | \
	((unsigned long)(unsigned char)(cp)[3] << 24))

#define PUT_32BIT_LSB_FIRST(cp, value) do { \
	(cp)[0] = (value); \
	(cp)[1] = (value) >> 8; \
	(cp)[2] = (value) >> 16; \
	(cp)[3] = (value) >> 24; } while (0)

/* The four core functions - F1 is optimized somewhat */

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

enum
  {
    LIBKERNSH_ARCH_I386, 
    LIBKERNSH_ARCH_X86_64,
    LIBKERNSH_ARCHNUM
  } libkernsh_e_arch_type;

enum
  {
    LIBKERNSH_OS_LINUX_2_6,
    LIBKERNSH_OS_LINUX_2_4,
    LIBKERNSH_OS_NETBSD,
    LIBKERNSH_OS_FREEBSD,
    LIBKERNSH_OSNUM
  } libkernsh_e_os_type;

enum
  {
    LIBKERNSH_HASH_MD5,
    LIBKERNSH_HASH_SHA1
  } libkernsh_e_hash_type;

enum
  {
    LIBKERNSH_DEVICE_KMEM,
    LIBKERNSH_DEVICE_MEM,
    LIBKERNSH_DEVICE_KCORE,
    LIBKERNSH_DEVICE_KVIRTM,
    LIBKERNSH_DEVICENUM
  } libkernsh_e_device_type;

enum
  {
    LIBKERNSH_MEM_MODE,
    LIBKERNSH_STATIC_MODE
  } libkernsh_e_debug_type;

enum
  {
    LIBKERNSH_VMA_USERLAND,
    LIBKERNSH_VMA_KERNELLAND,
    LIBKERNSH_VMANUM
  } libkernsh_e_vma_type;

#define	__DEBUG_KERNSH__			0

#define LIBKERNSH_CONFIG_WITHOUT_KERNEL	"libkernsh.without_kernel"
#define LIBKERNSH_CONFIG_DEVICE		"libkernsh.device"
#define LIBKERNSH_CONFIG_MODE			"libkernsh.mode"
#define LIBKERNSH_CONFIG_SYSTEMMAP		"libkernsh.systemmap"
#define LIBKERNSH_CONFIG_MMAP			"libkernsh.mmap"
#define LIBKERNSH_CONFIG_MMAP_SIZE		"libkernsh.mmapsize"
#define LIBKERNSH_CONFIG_KERNEL	        "libkernsh.kernel"
#define LIBKERNSH_CONFIG_GZIP			"libkernsh.gzipcmd"
#define LIBKERNSH_CONFIG_OBJCOPY		"libkernsh.objcopycmd"
#define LIBKERNSH_CONFIG_LD			"libkernsh.ldcmd"
#define LIBKERNSH_CONFIG_KERNELGZ		"libkernsh.kernelgz"
#define LIBKERNSH_CONFIG_KERNELELF		"libkernsh.kernelelf"
#define LIBKERNSH_CONFIG_USE_KERNEL		"libkernsh.use_kernel"
#define LIBKERNSH_CONFIG_STORAGE_PATH		"libkernsh.storagepath"
#define LIBKERNSH_CONFIG_NB_SYSCALLS		"libkernsh.nbsyscalls"
#define LIBKERNSH_CONFIG_NIL_SYSCALL		"libkernsh.nilsyscall"
#define LIBKERNSH_CONFIG_KERNEL_START		"libkernsh.kernel_start"
#define LIBKERNSH_CONFIG_KERNEL_END		"libkernsh.kernel_end"
#define LIBKERNSH_CONFIG_ALLOC		"libkernsh.alloc"
#define LIBKERNSH_CONFIG_KLOAD		"libkernsh.kload"
#define LIBKERNSH_CONFIG_KUNLOAD		"libkernsh.kunload"
#define LIBKERNSH_CONFIG_FENDSIZE		"libkernsh.fendsize"
#define LIBKERNSH_CONFIG_USEVM		"libkernsh.usevm"
#define LIBKERNSH_CONFIG_SPECIFY_IDTBASE	"libkernsh.specify_idtbase"
#define LIBKERNSH_CONFIG_SPECIFY_IDTLIMIT	"libkernsh.specify_idtlimit"
#define LIBKERNSH_CONFIG_SPECIFY_GDTBASE	"libkernsh.specify_gdtbase"
#define LIBKERNSH_CONFIG_SPECIFY_GDTLIMIT	"libkernsh.specify_gdtlimit"
#define LIBKERNSH_CONFIG_SPECIFY_SYSTEMCALL	"libkernsh.specify_systemcall"
#define LIBKERNSH_CONFIG_SPECIFY_SCT		"libkernsh.specify_sct"
#define LIBKERNSH_CONFIG_VIRTM		"libkernsh.virtm"
#define LIBKERNSH_CONFIG_VIRTM_NIL_SYSCALL	"libkernsh.virtm_nil_syscall"
#define LIBKERNSH_CONFIG_HASH			"libkernsh.hash"
#define LIBKERNSH_CONFIG_VMA			"libkernsh.vma"
#define LIBKERNSH_CONFIG_VMA_PREFIX		"libkernsh.vma_prefix"
#define LIBKERNSH_CONFIG_DUMP_VMA_PREFIX	"libkernsh.dump_vma_prefix"


#define LIBKERNSH_DEFAULT_LINUX_KERNEL		"/boot/vmlinuz"
#define LIBKERNSH_DEFAULT_LINUX_MAP		"/boot/System.map"
#define LIBKERNSH_DEFAULT_LINUX_KERNELGZ	"vmlinuz.gz"
#define LIBKERNSH_DEFAULT_LINUX_KERNELELF	"vmlinux"
#define LIBKERNSH_DEFAULT_LINUX_NB_SYSCALLS	320
#define LIBKERNSH_DEFAULT_LINUX_NIL_SYSCALL	17
#define LIBKERNSH_DEFAULT_LINUX_VIRTM_NIL_SYSCALL	31
#define LIBKERNSH_DEFAULT_LINUX_MMAP_SIZE	0x3e800000
#define LIBKERNSH_DEFAULT_LINUX_INSMOD		"/sbin/insmod"
#define LIBKERNSH_DEFAULT_LINUX_RMMOD		"/sbin/rmmod"
#define LIBKERNSH_DEFAULT_STORAGE_PATH		"/tmp/"
#define LIBKERNSH_DEFAULT_GZIP			"/bin/gzip"
#define LIBKERNSH_DEFAULT_OBJCOPY		"/usr/bin/objcopy"
#define LIBKERNSH_DEFAULT_LD			"/usr/bin/ld"
#define LIBKERNSH_DEFAULT_FENDSIZE		0x1000
#define LIBKERNSH_STRING_DEVICE_MEM		"/dev/mem"
#define LIBKERNSH_STRING_DEVICE_KMEM		"/dev/kmem"
#define LIBKERNSH_STRING_DEVICE_KCORE		"/proc/kcore"
#define LIBKERNSH_STRING_DEVICE_KVIRTM		"kvirtm"
#define LIBKERNSH_STRING_REL_GNU		".rel.gnu.linkonce.this_module"
#define LIBKERNSH_DEFAULT_VMA_PREFIX		"vma"
#define LIBKERNSH_DEFAULT_DUMP_VMA_PREFIX	"kernsh_dump"

#define LIBKERNSH_HASH_MD5_SIZE			16

#define LIBKERNSH_I386_LINUX_START		0xc0000000      
#define LIBKERNSH_I386_LINUX_END	      	0xc1000000

#define LIBKERNSH_PROC_ENTRY_SIZE		1024

#define LIBKERNSH_PAGE_I386_LINUX_OFFSET	0xc0000000

#define GFP_KERNEL				208
#define NAMESIZ					64

#define LIBKERNSH_VECTOR_NAME_OPENMEM			"openmem"
#define LIBKERNSH_VECTOR_NAME_CLOSEMEM			"closemem"
#define LIBKERNSH_VECTOR_NAME_SCT			"sct"
#define LIBKERNSH_VECTOR_NAME_IDT			"idt"
#define LIBKERNSH_VECTOR_NAME_GDT			"gdt"
#define LIBKERNSH_VECTOR_NAME_INFO			"info"
#define LIBKERNSH_VECTOR_NAME_DECOMPKERNEL		"decompkernel"
#define LIBKERNSH_VECTOR_NAME_ADDRBYNAME		"addr_by_name"
#define LIBKERNSH_VECTOR_NAME_NAMEBYADDR		"name_by_addr"
#define LIBKERNSH_VECTOR_NAME_ALLOCCONTIGUOUS		"alloc_contiguous"
#define LIBKERNSH_VECTOR_NAME_ALLOCNONCONTIGUOUS	"alloc_noncontiguous"
#define LIBKERNSH_VECTOR_NAME_FREECONTIGUOUS		"free_contiguous"
#define LIBKERNSH_VECTOR_NAME_FREENONCONTIGUOUS		"free_noncontiguous"
#define LIBKERNSH_VECTOR_NAME_AUTOTYPES			"autotypes"
#define LIBKERNSH_VECTOR_NAME_RELINKMODULE		"relink_module"
#define LIBKERNSH_VECTOR_NAME_INFECTMODULE		"infect_module"
#define LIBKERNSH_VECTOR_NAME_KLOADMODULE		"kload_module"
#define LIBKERNSH_VECTOR_NAME_KUNLOADMODULE		"kunload_module"
#define LIBKERNSH_VECTOR_NAME_CALLSC			"call_syscall"
#define LIBKERNSH_VECTOR_NAME_KVIRTMREADVIRTM		"kvirtm_read_virtm"
#define LIBKERNSH_VECTOR_NAME_KVIRTMREADMEM		"kvirtm_read_mem"
#define LIBKERNSH_VECTOR_NAME_KVIRTMWRITEVIRTM		"kvirtm_write_virtm"
#define LIBKERNSH_VECTOR_NAME_KVIRTMWRITEMEM		"kvirtm_write_mem"
#define LIBKERNSH_VECTOR_NAME_KVIRTMTASKPID		"kvirtm_task_pid"
#define LIBKERNSH_VECTOR_NAME_KVIRTMDUMPELF		"kvirtm_dump_elf"
#define LIBKERNSH_VECTOR_NAME_KDUMPGETVMA		"kdump_get_vma"
#define LIBKERNSH_VECTOR_NAME_KDUMPVMA			"kdump_vma"


/* Idtr segment struct */
struct 
{
  unsigned short limit;
  unsigned long base;
} __attribute__((packed)) idtr;

/* Interrupt descriptor */
struct 
{
  unsigned short off1;
  unsigned short sel;
  unsigned char none,flags;
  unsigned short off2;
} __attribute__ ((packed)) idt;

/* gdtr */
struct 
{
  unsigned short limit;
  unsigned long base;
} __attribute__ ((packed)) gdtr;


/**
 * @brief Kmalloc struct
 */
typedef struct	s_libkernshkma 
{
  eresi_Addr	(*kexec)(size_t, int);		    /* Execution of kmalloc */
  int		size;				    /* Size to alloc */
  int		flags;				    /* Flags, by default GFP_KERNEL */
  unsigned long mem;				    /* The address where the allocation has been done */
}		libkernshkma_t;


/**
 * @brief Kfree struct 
 */
typedef struct	s_libkernshkfr 
{
  unsigned long (*kexec)(const void *);     /* Execution of kfree */
  const void	*address;		    /* Address to be free */
}		libkernshkfr_t;


/**
 * @brief Vmalloc struct 
 */
typedef struct	s_libkernshvma 
{
  unsigned long (*kexec)(size_t);	/* Execution of vmalloc */
  int		size;			/* Size to alloc */
  unsigned long mem;			/* The address where the allocation has been done */
}		libkernshvma_t;


/**
 * @brief Vfree struct 
 */
typedef struct	s_libkernshvfr 
{
  unsigned long (*kexec)(void *);	/* Execution of vfree */
  void		*address;		/* Address to be free */
}		libkernshvfr_t;

/**
 * @brief Syscall struct 
 */
typedef struct	s_libkernshsyscall
{
  eresi_Addr	addr;
  char		name[NAMESIZ];
}		libkernshsyscall_t;

/**
 * @brief Interrupt struct 
 */
typedef struct	s_libkernshint
{
  eresi_Addr	addr;
  char		name[NAMESIZ];
}		libkernshint_t;

/**
 * @brief Gdt segment struct 
 */
typedef struct	s_libkernshsgdt
{
  eresi_Addr	addr;
  eresi_Addr	deb;
  eresi_Addr	fin;
}		libkernshsgdt_t;

/** 
 * @brief md5 context
 */
typedef struct s_libkernshmd5context
{
  unsigned long buf[4];
  unsigned long bits[2];
  unsigned char in[64];
}		libkernshmd5context_t;


/**
 * @brief dec task
 */
typedef struct	s_dectask
{
  int		dec_list;
  int		dec_pid;
  int		dec_uid;
}		dectask_t;

/**
 * @brief auto task
 */  
typedef struct	s_autotask
{
  int		offset_name;
  int		offset_list;
  int		offset_next;
  int		offset_pid;
  int		offset_uid;
  dectask_t	dectask;
}		autotask_t;

/**
 * @brief World kernsh struct 
 */
typedef struct	s_libkernshworld
{
  u_int		arch;			/* Arch type */
  u_int		os;			/* Os type */
  u_int		device;			/* Device type */
  int		open;			/* 0 => memory close, 1 => memory open */
  int		open_static;		/* 0 => static kernel close, 1 => open */
  char		*release;		/* Release name */
  int		fd;			/* !< @brief File descriptor for the memory */
  int		fdmode;			/* Mode to open memory (read/write) */
  int		protmode;		/* Protection mode to mmap */
  int		flagsmode;		/* Flags mode to mmap */
  int		mmap;			/* 0 => memory not mmap, 1 => memory mmap */
  int		mmap_size;		/* Size of the mmap */
  void		*ptr;			/* Pointer of the mmap */
  eresi_Addr	kernel_start;		/* Address of kernel start */
  eresi_Addr	kernel_end;		/* Address of kernel end */
  int		mem;			/* Static/Mem mode */
  int		physical;		/* 0 => virtual memory address, 1 => physical memory addr */
  eresi_Addr	idt_base;		/* Address of idt table */
  u_short	idt_limit;		/* Length */
  eresi_Addr	sct;			/* Address of syscall table */
  eresi_Addr	system_call;
  eresi_Addr	gdt_base;		/* Address of the gdt table */
  u_short	gdt_limit;		/* Lenght */  
  autotask_t	typetask;		/* Make type for task_struct */
  asm_processor	proc;			/* To play with libasm of course */
  elfshobj_t	*root;			/* Pointer to the kernel's elfshobj_t*/
}		libkernshworld_t;

extern libkernshworld_t     libkernshworld; /* Global libkernsh struct */


/* Init lib */
int		kernsh_init_ia32(char *, char *);
int		kernsh_del_ia32();

/* Information about kernel */
int		kernsh_info();
int		kernsh_info_linux();
int		kernsh_info_netbsd();
int		kernsh_info_freebsd();
elfshobj_t	*kernsh_load_file(char *);
void		kernsh_unload_file(elfshobj_t *);

/* Raw mode */
int		kernsh_raw_write(elfshobj_t *, u_int, void *, int);
int		kernsh_raw_read(elfshobj_t *,  u_int, void *, int);
void		*kernsh_get_raw(elfshsect_t *);
void		*kernsh_get_raw_by_addr(elfshobj_t *, eresi_Addr, void*, u_int);

/* Default vector handlers */
int kernsh_openmem_default();
int kernsh_closemem_default();
int kernsh_sct_default();
int kernsh_callsc_default();
int kernsh_idt_default();
int kernsh_gdt_default();
int kernsh_info_default();
int kernsh_decompkernel_default();
int kernsh_loadkernel_default();
int kernsh_autotypes_default();
int kernsh_symbs_default();
int kernsh_alloc_contiguous_default();
int kernsh_alloc_noncontiguous_default();
int kernsh_free_contiguous_default();
int kernsh_free_noncontiguous_default();
int kernsh_relink_module_default();
int kernsh_infect_module_default();
int kernsh_kload_module_default();
int kernsh_kunload_module_default();
int kernsh_kvirtm_read_virtm_default();
int kernsh_kvirtm_read_mem_default();
int kernsh_kvirtm_write_virtm_default();
int kernsh_kvirtm_write_mem_default();
int kernsh_kvirtm_task_pid_default();
int kernsh_kdump_get_vma_default();
int kernsh_kdump_vma_default();

/* Init vectors */
int	kernsh_init_vectors();
int	kernsh_register_vectors();
int	kernsh_register_openmem(u_int, u_int, u_int, void *);
int	kernsh_register_closemem(u_int, u_int, u_int, void *);
int	kernsh_register_sct(u_int, u_int, void *);
int	kernsh_register_callsc(u_int, void *);
int	kernsh_register_idt(u_int, u_int, void *);
int	kernsh_register_gdt(u_int, u_int, void *);
int	kernsh_register_info(u_int, u_int, void *);
int	kernsh_register_decompkernel(u_int, void *);
int	kernsh_register_symbs_abn(u_int, u_int, void *);
int	kernsh_register_symbs_nba(u_int, u_int, void *);
int	kernsh_register_alloc_contiguous(u_int, void *);
int	kernsh_register_alloc_noncontiguous(u_int, void *);
int	kernsh_register_free_contiguous(u_int, void *);
int	kernsh_register_free_noncontiguous(u_int, void *);
int	kernsh_register_autotypes(u_int, u_int, void *);
int	kernsh_register_relink(u_int, void *);
int	kernsh_register_infect(u_int, void *);
int	kernsh_register_kload(u_int, void *);
int	kernsh_register_kunload(u_int, void *);
int	kernsh_register_kvirtm_read_virtm(u_int, u_int, void *);
int	kernsh_register_kvirtm_read_mem(u_int, u_int, void *);
int	kernsh_register_kvirtm_write_virtm(u_int, u_int, void *);
int	kernsh_register_kvirtm_write_mem(u_int, u_int, void *);
int	kernsh_register_kvirtm_task_pid(u_int, u_int, void *);
int	kernsh_register_kdump_get_vma(u_int, u_int, void *);
int	kernsh_register_kdump_vma(u_int, u_int, void *);

/* Memory */
int	kernsh_openmem();
int	kernsh_openmem_kmem_linux_2_6();
int	kernsh_openmem_kmem_linux_2_4();
int	kernsh_openmem_mem_linux();
int	kernsh_openmem_kcore_linux_2_6();
int	kernsh_openmem_netbsd();

int	kernsh_closemem();
int	kernsh_closemem_kmem_linux_2_6();
int	kernsh_closemem_kmem_linux_2_4();
int	kernsh_closemem_mem_linux();
int	kernsh_closemem_kcore_linux_2_6();
int	kernsh_closemem_netbsd();

void	*kernsh_readmema_kmem_linux_2_6(elfshobj_t *file, eresi_Addr, void *, int);
void	*kernsh_readmema_kmem_linux_2_4(elfshobj_t *file, eresi_Addr, void *, int);
void	*kernsh_readmema_mem_linux(elfshobj_t *file, eresi_Addr, void *, int);
void	*kernsh_readmema_kcore_linux_2_6(elfshobj_t *file, eresi_Addr, void *, int);
void	*kernsh_readmema_netbsd(elfshobj_t *file, eresi_Addr, void *, int);

int	kernsh_writemem_kmem_linux_2_6(elfshobj_t *file, eresi_Addr, void *, int);
int	kernsh_writemem_kmem_linux_2_4(elfshobj_t *file, eresi_Addr, void *, int);
int	kernsh_writemem_mem_linux(elfshobj_t *file, eresi_Addr, void *, int);
int	kernsh_writemem_kcore_linux_2_6(elfshobj_t *file, eresi_Addr, void *, int);
int	kernsh_writemem_netbsd(elfshobj_t *file, eresi_Addr, void *, int);

/* Syscalls table */
int	kernsh_sct(list_t *);
int	kernsh_sct_linux(list_t *);
int	kernsh_sct_netbsd(list_t *);
int	kernsh_sct_freebsd(list_t *);

int	kernsh_syscall(int, int, unsigned int []);
int	kernsh_syscall_linux(int, int, unsigned int []);

/* IDT */
int	kernsh_idt(list_t *);
int	kernsh_idt_linux(list_t *);
int	kernsh_idt_netbsd(list_t *);
int	kernsh_idt_freebsd(list_t *);

/* GDT */
int	kernsh_gdt(list_t *);
int	kernsh_gdt_linux(list_t *);
int	kernsh_gdt_netbsd(list_t *);
int	kernsh_gdt_freebsd(list_t *);

/* Symbols */
int	kernsh_get_addr_by_name(char *, eresi_Addr*, size_t);
int	kernsh_get_name_by_addr(eresi_Addr, char *, size_t);

int	kernsh_get_addr_by_name_linux_2_6(char *, eresi_Addr*, size_t);
int	kernsh_get_name_by_addr_linux_2_6(eresi_Addr, char *, size_t);
int	kernsh_get_addr_by_name_linux_2_4(char *, eresi_Addr*, size_t);
int	kernsh_get_name_by_addr_linux_2_4(eresi_Addr, char *, size_t);

int	kernsh_walk_kstrtab(const char *, eresi_Addr*, size_t);
int	kernsh_get_kernel_syms(char *, eresi_Addr*, size_t);

/* Alloc-Free */
eresi_Addr kernsh_alloc(elfshobj_t *file, size_t size, int prot);
int	kernsh_free(eresi_Addr);

int	kernsh_alloc_contiguous(size_t, eresi_Addr*);
int	kernsh_free_contiguous(eresi_Addr);
int	kernsh_alloc_noncontiguous(size_t, eresi_Addr*);
int	kernsh_free_noncontiguous(eresi_Addr);

int	kernsh_alloc_contiguous_linux(size_t, eresi_Addr*);
int	kernsh_free_contiguous_linux(eresi_Addr);
int	kernsh_alloc_noncontiguous_linux(size_t, eresi_Addr*);
int	kernsh_free_noncontiguous_linux(eresi_Addr);

/* Module */
int	kernsh_kload_module(char *);
int	kernsh_kload_module_linux(char *);

int	kernsh_kunload_module(char *);
int	kernsh_kunload_module_linux(char *);

int	kernsh_relink_module(char *, char *, char *);
int	kernsh_relink_module_linux(char *, char *, char *);

int	kernsh_infect_module(char *, char *, char *);
int	kernsh_infect_module_linux_2_6(char *, elfshobj_t *, char *, char *);
int	kernsh_infect_module_linux_2_4(char *, elfshobj_t *, char *, char *);

/* MD5 */
unsigned char *kernsh_hash(unsigned long, int, int *);
int	kernsh_md5init(libkernshmd5context_t *);
int	kernsh_md5update(libkernshmd5context_t *, unsigned char *, unsigned);
int	kernsh_md5final(unsigned char [16], libkernshmd5context_t *);
int	kernsh_md5transform(unsigned long [4], const unsigned char [64]);
int	kernsh_md5dump(unsigned char *, int, unsigned char [BUFSIZ]);
int	kernsh_md5(eresi_Addr, int, unsigned char [BUFSIZ]);

/* Auto Types */
int kernsh_autotypes();
int kernsh_autotypes_linux_2_6();
int kernsh_autotypes_linux_2_4();
int kernsh_autotypes_netbsd();
int kernsh_autotypes_freebsd();

int kernsh_autotask_linux_2_6();
int kernsh_autotask_linux_2_4();
int kernsh_autotask_netbsd();
int kernsh_autotask_freebsd();

int kernsh_autotask_offsetname_linux_2_6(char *, size_t);
int kernsh_autotask_offsetlist_linux_2_6(char *, size_t);

/* Kernel Decompression */
int kernsh_decompkernel();
int kernsh_decompkernel_linux();

/* Kvirtm */
int kernsh_kvirtm_read_virtm(pid_t, eresi_Addr, char *, int);
int kernsh_kvirtm_read_virtm_proc_linux(pid_t, eresi_Addr, char *, int);
int kernsh_kvirtm_read_virtm_syscall_linux(pid_t, eresi_Addr, char *, int);

int kernsh_kvirtm_write_virtm(pid_t, eresi_Addr, char *, int);
int kernsh_kvirtm_write_virtm_proc_linux(pid_t, eresi_Addr, char *, int);
int kernsh_kvirtm_write_virtm_syscall_linux(pid_t, eresi_Addr, char *, int);

int kernsh_kvirtm_openmem();
int kernsh_kvirtm_closemem();

void *kernsh_kvirtm_readmema(elfshobj_t *kern, eresi_Addr, char *, int);
int kernsh_kvirtm_readmem_proc_linux(eresi_Addr, char *, int);
int kernsh_kvirtm_readmem_syscall_linux(eresi_Addr, char *, int);

int kernsh_kvirtm_writemem(elfshobj_t *kern, eresi_Addr, char *, int);
int kernsh_kvirtm_writemem_proc_linux(eresi_Addr, char *, int);
int kernsh_kvirtm_writemem_syscall_linux(eresi_Addr, char *, int);

int kernsh_kvirtm_task_pid(pid_t, list_t *);
int kernsh_kvirtm_task_pid_proc_linux(pid_t, list_t *);
int kernsh_kvirtm_task_pid_syscall_linux(pid_t, list_t *);

/* Dump */
int	kernsh_dump_kvirtm_elf(pid_t, char *);
list_t *kernsh_kdump_get_vma(pid_t);
int	kernsh_kdump_get_vma_userland_linux(pid_t, list_t *);

int	kernsh_kdump_vma(pid_t);
int	kernsh_kdump_vma_userland_linux(pid_t, hash_t *);
int	kernsh_kdump_vma_kernelland_linux(pid_t, hash_t *);

/* Misc */
void	*kernsh_find_pattern(const void *, int, const void *, int);
int	kernsh_resolve_systemmap(eresi_Addr, char *, size_t);
int	kernsh_rresolve_systemmap(const char *, eresi_Addr*, size_t);
int	kernsh_find_end(eresi_Addr);

#endif
