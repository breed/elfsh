/*
** readlnheap.c for elfsh
** 
** Handle the readline malloc/free to avoid messing with the alloc proxy
**
** Started on  Sat Jul 1 10:10:53 2006 mayhem
*/
#include "e2dbg.h"



/* When inside e2dbg and not elfsh, readline should allocate with the internal malloc */
void	*vm_readline_malloc(unsigned int sz)
{
  return (aproxy_malloc(sz));
}

/* When inside e2dbg and not elfsh, readline should free with the real malloc */
void	vm_readline_free(void *ptr)
{
  aproxy_free(ptr);
}

