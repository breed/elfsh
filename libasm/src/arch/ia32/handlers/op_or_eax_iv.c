/*
** $Id: op_or_eax_iv.c,v 1.6 2007-08-15 21:30:20 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x0d
  Instruction :         OR
  */

int op_or_eax_iv(asm_instr *new, u_char *opcode, u_int len,
                 asm_processor *proc) 
{
  new->instr = ASM_OR;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->ptr_instr = opcode;
  new->len += 1;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_OF | ASM_FLAG_PF |
                        ASM_FLAG_ZF | ASM_FLAG_SF;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.baser = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op2, opcode, ASM_OTYPE_IMMEDIATE, new);

  return (new->len);
}
