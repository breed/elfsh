/**
* @file libkernsh/kernel/arch/x86/linux/mem.c
** @ingroup kernelmode
**
*/
#include "libkernsh-kernel.h"
#include "libkernsh-kernel-linux.h"

/**
 * @brief Read kernel memory
 * @param addr Address to read memory
 * @param buffer Read memory into the buffer
 * @param len Count bytes to read
 * @param mode The mode to write into the buffer
 * @return len on success, -1 on error
 */
asmlinkage int kernsh_read_mem(unsigned long addr, char *buffer, int len, int mode)
{
  ssize_t read, sz;
  char *ptr; 
  unsigned long p = addr;

#if __DEBUG_LIBKERNSH_KERNEL__
  printk(KERN_ALERT "[+] kernsh_read_mem ENTER !!\n");
#endif

  if (!valid_phys_addr_range(p, len))
    return -EFAULT;

  read = 0;
  while (len > 0) 
    {
      /*
       * Handle first page in case it's not aligned
       */
      if (-p & (PAGE_SIZE - 1))
	sz = -p & (PAGE_SIZE - 1);
      else
	sz = PAGE_SIZE;
      
      sz = min_t(unsigned long, sz, len);
      
      ptr = __va(p);
      
#if __DEBUG_LIBKERNSH_KERNEL__
      printk(KERN_ALERT "SZ %d PTR 0x%lx\n", sz, (unsigned long)ptr);
#endif

      switch(mode)
	{
	case LIBKERNSH_KERNEL_MODE :
	  memcpy(buffer, ptr, sz);
	  break;
	case LIBKERNSH_USER_MODE :
	  if (copy_to_user(buffer, ptr, sz))
	    return -EFAULT;
	  break;
	}
      
      buffer += sz;
      p += sz;
      len -= sz;
      read += sz;
    }
  
#if __DEBUG_LIBKERNSH_KERNEL__
  printk(KERN_ALERT "[+] kernsh_read_mem EXIT !!\n");
#endif

  return read;
}

/**
 * @brief Write into kernel memory
 * @param addr Address to write buffer
 * @param buffer Write buffer into memory
 * @param len Count bytes to write
 * @param mode The mode to write into the buffer
 * @return len on success, -1 on error
 */
asmlinkage int kernsh_write_mem(unsigned long addr, const char *buffer, int len, int mode)
{
  unsigned long p = addr;
  ssize_t written, sz;
  unsigned long copied;
  void *ptr;
  
#if __DEBUG_LIBKERNSH_KERNEL__
  printk(KERN_ALERT "[+] kernsh_write_mem ENTER !!\n");
#endif

  if (!valid_phys_addr_range(p, len))
    return -EFAULT;
  
  written = 0;
  
  while (len > 0) 
    {
      /*
       * Handle first page in case it's not aligned
       */
      if (-p & (PAGE_SIZE - 1))
	sz = -p & (PAGE_SIZE - 1);
      else
	sz = PAGE_SIZE;
      
      sz = min_t(unsigned long, sz, len);
      
      /*
       * On ia64 if a page has been mapped somewhere as
       * uncached, then it must also be accessed uncached
       * by the kernel or data corruption may occur
       */
      ptr = __va(p);
      
      switch(mode)
	{
	case LIBKERNSH_KERNEL_MODE :
	  memcpy(ptr, buffer, sz);
	  copied = sz;
	  break;
	case LIBKERNSH_USER_MODE :
	  copied = copy_from_user(ptr, buffer, sz);
	  break;
	}
      
      if (copied) 
	{
	  written += sz - copied;
	  if (written)
	    break;
	  
	  return -EFAULT;
	}
      
      buffer += sz;
      p += sz;
      len -= sz;
      written += sz;
    }
  
#if __DEBUG_LIBKERNSH_KERNEL__
  printk(KERN_ALERT "[+] kernsh_write_mem EXIT !!\n");
#endif

  return written;
}
