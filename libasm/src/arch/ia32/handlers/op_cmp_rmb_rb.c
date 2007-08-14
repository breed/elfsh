/*
** $Id: op_cmp_rmb_rb.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cmp_rmb_rb" opcode="0x38"/>
*/

int op_cmp_rmb_rb(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc) 
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_CMP;

  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERALBYTE, 
                                new);

  return (new->len);
}
