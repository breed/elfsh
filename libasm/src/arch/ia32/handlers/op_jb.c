/*
** $Id: op_jb.c,v 1.5 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_jb" opcode="0x72"/>
*/


int  op_jb(asm_instr *new, u_char *opcode, u_int len, 
           asm_processor *proc) 
{
  new->len += 1;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_BRANCH_U_LESS;
  new->ptr_instr = opcode;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_SHORTJUMP, 
                                new);

  return (new->len);
}
