/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 <instruction func="op_cltd" opcode="0x99"/>
*/

int op_cltd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_ARITH;

  if (asm_proc_opsize(proc))
    new->instr = ASM_CWTD;
  else
  new->instr = ASM_CLTD;

  return (new->len);
}
