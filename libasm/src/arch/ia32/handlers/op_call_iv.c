/*
** $Id: op_call_iv.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_call_iv" opcode="0xe8"/>
*/

int op_call_iv(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->instr = ASM_CALL;
  new->type = ASM_TYPE_CALLPROC | ASM_TYPE_TOUCHSP;
  new->len += 1;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_JUMP, new);

  return (new->len);
}
