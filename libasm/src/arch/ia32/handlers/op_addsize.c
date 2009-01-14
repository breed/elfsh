/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_addsize" opcode="0x67"/>
 */

int     op_addsize(asm_instr *new, u_char *opcode, u_int len,
                   asm_processor *proc)
{
  asm_i386_processor    *i386p;

  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;

  i386p = (asm_i386_processor *) proc;
  new->prefix |= ASM_PREFIX_ADDSIZE;

  i386p->internals->addsize = !i386p->internals->addsize;
  len = proc->fetch(new, opcode + 1, len - 1, proc);
  i386p->internals->addsize = !i386p->internals->addsize;

  return (len);
}
