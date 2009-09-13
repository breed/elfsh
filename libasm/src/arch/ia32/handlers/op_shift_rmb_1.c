/**
* @file libasm/src/arch/ia32/handlers/op_shift_rmb_1.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_shift_rmb_1" opcode="0xd0"/>
 */

int op_shift_rmb_1(asm_instr *new, u_char *opcode, u_int len, 
                   asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_SHR;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_PF | ASM_FLAG_ZF | ASM_FLAG_SF |
                        ASM_FLAG_CF | ASM_FLAG_OF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
                                new);
#endif
  return (new->len);
}
