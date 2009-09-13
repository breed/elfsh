/**
* @file libasm/src/arch/ia32/handlers/op_scasb.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_scasb" opcode="0xae"/>
*/

int op_scasb(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc) {
  new->instr = ASM_SCASB;
  new->ptr_instr = opcode;
  new->len += 1;
  new->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new,
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       ASM_REGSET_R8));

  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_YDEST, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_FIXED, new);
  new->op[0].content = ASM_OP_BASE;
  new->op[0].regset = ASM_REGSET_R8;
  new->op[0].baser = ASM_REG_EAX;
  new->len += asm_operand_fetch(&new->op[1], opcode, ASM_OTYPE_YDEST, new);
#endif

  return (new->len);
}
