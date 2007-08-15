/**
 * @file op_test_eax_iv.c
 * @ingroup handlers_ia32
 * $Id: op_test_eax_iv.c,v 1.5 2007-08-15 21:30:21 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 *
 * <instruction opcode="0xa9" func="op_test_eax_iv"/>
 */

int op_test_eax_iv(asm_instr *new, u_char *opcode, u_int len, 
                   asm_processor *proc)
{
  new->instr = ASM_TEST;
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_OF | ASM_FLAG_CF | ASM_FLAG_PF |
                          ASM_FLAG_SF | ASM_FLAG_ZF;

  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_FIXED, new);
  new->op1.content = ASM_OP_BASE;
  new->op1.regset = asm_proc_opsize(proc) ?
    ASM_REGSET_R16 : ASM_REGSET_R32;
  new->op1.baser = ASM_REG_AX;
  new->len += asm_operand_fetch(&new->op1, opcode, ASM_OTYPE_IMMEDIATE, new);

  return (new->len);
}
