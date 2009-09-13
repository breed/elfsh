/**
 * @defgroup usermode Usermode running code.
 * @ingroup libkernsh
 */
/*
* @file libkernsh/user/kernsh.c
* @ingroup usermode
* @brief initialisation, get_raw and mode switch
**
*/
#include "libkernsh.h"

libkernshworld_t libkernshworld;

/**
 * @brief Initialise kernsh for ia32
 * @param os Operating System
 * @param release Release of the OS
 * @return 0 on success, -1 on error 
 */
int kernsh_init_ia32(char *os, char *release)
{
  char buffer[256];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(&libkernshworld, '\0', sizeof(libkernshworld_t));

  libkernshworld.arch = LIBKERNSH_ARCH_I386;
  
  /* By default we use static kernel */
  elfsh_set_static_mode();
  
  config_add_item(LIBKERNSH_CONFIG_WITHOUT_KERNEL,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);

  config_add_item(LIBKERNSH_CONFIG_USE_KERNEL,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);

  config_add_item(LIBKERNSH_CONFIG_ALLOC,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);

  config_add_item(LIBKERNSH_CONFIG_GZIP,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) LIBKERNSH_DEFAULT_GZIP);
  
  config_add_item(LIBKERNSH_CONFIG_OBJCOPY,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) LIBKERNSH_DEFAULT_OBJCOPY);

  config_add_item(LIBKERNSH_CONFIG_LD,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) LIBKERNSH_DEFAULT_LD);

  config_add_item(LIBKERNSH_CONFIG_FENDSIZE,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) LIBKERNSH_DEFAULT_FENDSIZE);
  
  config_add_item(LIBKERNSH_CONFIG_USEVM,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) 0);

  config_add_item(LIBKERNSH_CONFIG_SPECIFY_IDTBASE,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) "0x0");

  config_add_item(LIBKERNSH_CONFIG_SPECIFY_IDTLIMIT,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) "0x0");

  config_add_item(LIBKERNSH_CONFIG_SPECIFY_GDTBASE,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) "0x0");

  config_add_item(LIBKERNSH_CONFIG_SPECIFY_GDTLIMIT,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) "0x0");

#if defined(__linux__)
 config_add_item(LIBKERNSH_CONFIG_SPECIFY_SYSTEMCALL,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) "0x0");
#endif

  config_add_item(LIBKERNSH_CONFIG_SPECIFY_SCT,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) "0x0");

  config_add_item(LIBKERNSH_CONFIG_VIRTM,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) LIBKERNSH_KERNEL_MODE);

  config_add_item(LIBKERNSH_CONFIG_HASH,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) LIBKERNSH_HASH_MD5);

  config_add_item(LIBKERNSH_CONFIG_VMA,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) LIBKERNSH_VMA_USERLAND);

  config_add_item(LIBKERNSH_CONFIG_VMA_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) LIBKERNSH_DEFAULT_VMA_PREFIX);

  config_add_item(LIBKERNSH_CONFIG_DUMP_VMA_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  (char *) LIBKERNSH_DEFAULT_DUMP_VMA_PREFIX);
  
  /* We are on Linux ! */
  if (!strcmp(os, "Linux"))
    {
      if (!strncmp(release, "2.6", 3))
	{
	  libkernshworld.os = LIBKERNSH_OS_LINUX_2_6;
	}
      else if (!strncmp(release, "2.4", 3))
	{
	  libkernshworld.os = LIBKERNSH_OS_LINUX_2_4;
	}
      else 
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Release not supported", -1);
	}
 
      config_add_item(LIBKERNSH_CONFIG_DEVICE,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *) LIBKERNSH_STRING_DEVICE_MEM);
      
      config_add_item(LIBKERNSH_CONFIG_MODE,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *)"write");

      config_add_item(LIBKERNSH_CONFIG_MMAP,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) 1);
      
      config_add_item(LIBKERNSH_CONFIG_MMAP_SIZE,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_MMAP_SIZE);

      memset(buffer, '\0', sizeof(buffer));
      snprintf(buffer, sizeof(buffer), "%s-%s", LIBKERNSH_DEFAULT_LINUX_KERNEL, release);
      
      config_add_item(LIBKERNSH_CONFIG_KERNEL,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) buffer);
      
      config_add_item(LIBKERNSH_CONFIG_KERNELGZ,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_KERNELGZ);
      
      config_add_item(LIBKERNSH_CONFIG_KERNELELF,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_KERNELELF);

      config_add_item(LIBKERNSH_CONFIG_STORAGE_PATH,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_STORAGE_PATH);
      
      memset(buffer, '\0', sizeof(buffer));
      snprintf(buffer, sizeof(buffer), "%s-%s", LIBKERNSH_DEFAULT_LINUX_MAP, release);
  
      config_add_item(LIBKERNSH_CONFIG_SYSTEMMAP,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (char *) buffer);

      config_add_item(LIBKERNSH_CONFIG_NB_SYSCALLS,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_NB_SYSCALLS);

      config_add_item(LIBKERNSH_CONFIG_NIL_SYSCALL,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_NIL_SYSCALL);
      
      config_add_item(LIBKERNSH_CONFIG_VIRTM_NIL_SYSCALL,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_VIRTM_NIL_SYSCALL);
      
      config_add_item(LIBKERNSH_CONFIG_KERNEL_START,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_I386_LINUX_START);
      
      config_add_item(LIBKERNSH_CONFIG_KERNEL_END,
		      CONFIG_TYPE_INT,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_I386_LINUX_END);

      config_add_item(LIBKERNSH_CONFIG_KLOAD,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_INSMOD);

      config_add_item(LIBKERNSH_CONFIG_KUNLOAD,
		      CONFIG_TYPE_STR,
		      CONFIG_MODE_RW,
		      (void *) LIBKERNSH_DEFAULT_LINUX_RMMOD);

    }
  else
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Os not supported", -1);
    }
  
  XALLOC(__FILE__, 
	 __FUNCTION__,
	 __LINE__, 
	 libkernshworld.release, 
	 strlen(release) + 1, 
	 -1);
  
  if (memcpy(libkernshworld.release, release, strlen(release)) == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Memcpy failed", -1);
    }

#if __DEBUG_KERNSH__
  printf("RELEASE %s\n", libkernshworld.release);
#endif

  kernsh_init_vectors();
  kernsh_register_vectors();
  asm_init_arch(&libkernshworld.proc, ASM_PROC_IA32);
  kernsh_present_set();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Delete kernsh for ia32
 * @return 0 on success, -1 on error 
 */
int kernsh_del_ia32()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XFREE(__FILE__, __FUNCTION__, __LINE__, libkernshworld.release);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Called in elfsh_readmem to interact with the memory
 * @param sect The section
 */
void	*kernsh_get_raw(elfshsect_t *sect)
{
  void *dataptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("kernsh_get_raw\n");
#endif

  if (libkernshworld.open && elfsh_is_runtime_mode() && libkernshworld.mmap)
    { 
      /* We use physical memory ? */
      if (libkernshworld.physical)
	{
#if __DEBUG_KERNSH__
	  printf("PTR 0x%lx RHDR BASE 0x%lx KERNEL_START 0x%lx SH_ADDR 0x%lx DATA 0x%lx 0x%lx\n",
		 (unsigned long) libkernshworld.ptr,
		 (unsigned long) sect->parent->rhdr.base, 
		 (unsigned long) libkernshworld.kernel_start,
		 (unsigned long) sect->shdr->sh_addr,
		 (unsigned long) sect->data,
		 (unsigned long) sect->phdr);
#endif  
	
	  dataptr = libkernshworld.ptr;
	  
	  if(elfsh_section_is_runtime(sect))
	    {
	      sect->parent->rhdr.base = 0;
	      dataptr += sect->shdr->sh_addr - libkernshworld.kernel_start;
	    }
	  else
	    {
	      sect->parent->rhdr.base = 
		libkernshworld.kernel_start - sect->shdr->sh_addr;
	    }
	  
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dataptr);
	}
    }
  else
    {
      if (libkernshworld.open)
	sect->parent->rhdr.base = 0;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/**
 * @brief Called in elfsh_readmema to interact with the memory
 * @param addr The address
 */
void	*kernsh_get_raw_by_addr(elfshobj_t *null, eresi_Addr addr, void *buf, u_int size)
{
  void	*dataptr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("kernsh_get_raw_by_addr\n");
#endif

  if (libkernshworld.open && elfsh_is_runtime_mode() && libkernshworld.mmap)
    {
      /* We use physical memory ? */
      if (libkernshworld.physical)
	dataptr = libkernshworld.ptr + addr - libkernshworld.kernel_start;
      else
	dataptr = libkernshworld.ptr + addr;
      if (buf && size)
	memcpy(buf, dataptr, size);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, dataptr);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
}

/**
 * @brief Get some information about the kernel
 * @return 0 on sucess, -1 on error
 */
int kernsh_info()
{
  int ret;
  u_int         dim[3];
  vector_t      *info;
  int          (*fct)();

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
    
  info = aspect_vector_get(LIBKERNSH_VECTOR_NAME_INFO);
  dim[0] = libkernshworld.arch;
  dim[1] = libkernshworld.os;
  
  fct = aspect_vectors_select(info, dim);
  
  ret = fct();
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Get some information about the linux kernel
 * @return 0 on sucess, -1 on error
 */
int kernsh_info_linux()
{
  unsigned long system_call;
  char buffer[256];
  char *p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if defined(__linux__)
  asm("sidt %0" : "=m" (idtr));
#endif
  
  libkernshworld.idt_base = idtr.base;
  libkernshworld.idt_limit = idtr.limit;

#if __DEBUG_KERNSH__
  printf("IDTR BASE 0x%lx LIMIT 0x%x\n", idtr.base,idtr.limit);
#endif

  elfsh_readmema(libkernshworld.root, idtr.base+(2*sizeof(unsigned long))*0x80, 
		 &idt, 
		 sizeof(idt));
  system_call = (idt.off2 << 16) | idt.off1;

#if defined(__linux__)
  libkernshworld.system_call = system_call;
#endif

#if __DEBUG_KERNSH__
  printf("idt80: flags = %d flags=%X sel=%X off1=%x off2=%X\n",
	 idt.flags,(unsigned)idt.flags,(unsigned)idt.sel,(unsigned)idt.off1, 
	 (unsigned)idt.off2);
  printf("SYSTEM_CALL : 0x%lx\n", system_call);
#endif

  elfsh_readmema(libkernshworld.root, system_call, buffer, 255);
  p = (char *)kernsh_find_pattern(buffer, 255, "\xff\x14\x85", 3);
      
  if (p == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Cannot get sct", -1);
    }
  
  libkernshworld.sct = *(unsigned long *)(p + 3);

#if __DEBUG_KERNSH__
  printf("Sys Call Table 0x%lx\n", libkernshworld.sct);
#endif


#if defined(__linux__)
  asm("sgdt %0" : "=m" (gdtr));
#endif

#if __DEBUG_KERNSH__
  printf("GDTR BASE 0x%lx LIMIT 0x%x\n", gdtr.base,gdtr.limit);
#endif

  libkernshworld.gdt_base = gdtr.base;
  libkernshworld.gdt_limit = gdtr.limit;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Get some information about the netbsd kernel
 * @return 0 on sucess, -1 on error
 */
int kernsh_info_netbsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Get some information about the freebsd kernel
 * @return 0 on sucess, -1 on error
 */
int kernsh_info_freebsd()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Load elf file
 * @param name The path of the file to load
 * @return 0 on sucess, -1 on error
 */
elfshobj_t *kernsh_load_file(char *name)
{
  elfshobj_t *ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ret = elfsh_load_obj(name);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Unload elf file
 * @param file  The elf object
 */
void kernsh_unload_file(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  elfsh_unload_obj(file);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * @brief Write raw in the kernel memory
 * @param file The elf object
 * @param foffset Offset to write memory
 * @param src_buff Write buf into memoryr
 * @param len Count bytes to write
 * @return len on success, -1 on error
 */
int kernsh_raw_write(elfshobj_t *file, u_int foffset, void *src_buff, int len)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (elfsh_is_runtime_mode())
    {
      elfsh_writemem(file, elfsh_get_vaddr_from_foffset(file, foffset), src_buff, len);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (len));
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Mode is wrong", -1);
}

/**
 * @brief Read raw kernel memory
 * @param file The elf object
 * @param foffset Offset to read memory
 * @param dest_buff Read memory into the buf
 * @param len Count bytes to read
 * @return len on success, -1 on error
 */
int kernsh_raw_read(elfshobj_t *file,  u_int foffset, void *dest_buff, int len)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (elfsh_is_runtime_mode())
    {
      elfsh_readmema(file, elfsh_get_vaddr_from_foffset(file, foffset), dest_buff, len);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, len);
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Mode is wrong", -1);
}
