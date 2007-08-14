/*
** $Id: op_int_ib.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_int_ib" opcode="0xcd"/>
*/

int op_int_ib(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->instr = ASM_INT;
  new->type = ASM_TYPE_INT | ASM_TYPE_TOUCHSP;
  new->len += 1;

  new->len += asm_operand_fetch(&new->op1, opcode + 1,
                                ASM_OTYPE_IMMEDIATEBYTE, new);

  return (new->len);
}
