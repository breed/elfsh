/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jne" opcode="0x75"/>
*/


int  op_jne(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_BRANCH | ASM_TYPE_CONDCONTROL;
  new->instr = ASM_BRANCH_NOT_EQUAL;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_SHORTJUMP,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_SHORTJUMP,                                new);
#endif

  return (new->len);
}
