/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_prefix_gs" opcode="0x65"/>
 */

int     op_prefix_gs(asm_instr *new, u_char *opcode, u_int len,
		     asm_processor *proc)
{
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->prefix |= ASM_PREFIX_GS;
  new->len += 1;
  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
