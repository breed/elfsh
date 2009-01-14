/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  Opcode :              0x05
  Instruction :         ADD
*/

int op_add_eax_iv(asm_instr *new, u_char *opcode, u_int len,
		  asm_processor *proc)
{

  new->instr = ASM_ADD;
  new->ptr_instr = opcode;
  new->len += 1;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_ZF | ASM_FLAG_PF |
                      ASM_FLAG_OF | ASM_FLAG_AF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
  new->op[0].content = ASM_OP_FIXED | ASM_OP_BASE;
  new->op[0].len = 0;
  new->op[0].baser = ASM_REG_EAX;
  new->op[0].regset = asm_proc_opsize(proc) ? ASM_REGSET_R16 :
    ASM_REGSET_R32;
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATE,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_IMMEDIATE,                                new);
#endif

  return (new->len);
}
