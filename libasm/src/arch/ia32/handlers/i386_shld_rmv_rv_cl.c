/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_shld_rmv_rv_cl" opcode="0xa5"/>
*/

int i386_shld_rmv_rv_cl(asm_instr *new, u_char *opcode, u_int len,
			asm_processor *proc)
{
  new->instr = ASM_SHRD;
  new->len += 1;

#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_GENERAL,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[2], opcode + 1, ASM_OTYPE_FIXED, new, 
				asm_fixed_pack(0, ASM_OP_BASE, ASM_REG_EAX,
					       asm_proc_is_protected(proc) ?
					       ASM_REGSET_R32 : ASM_REGSET_R16));

#else
  new->len += asm_operand_fetch(&new->op[2], opcode + 1, ASM_OTYPE_FIXED, new);
#endif
  new->op[2].content = ASM_OP_BASE;
  new->op[2].regset = ASM_REGSET_R8;
  new->op[2].ptr = opcode;
  new->op[2].len = 0;
  new->op[2].baser = ASM_REG_CL;

#else
  new->op[0].type = ASM_OTYPE_ENCODED;
  new->op[0].size = ASM_OSIZE_VECTOR;
  new->op[1].type = ASM_OTYPE_GENERAL;
  new->op[1].size = ASM_OSIZE_VECTOR;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  new->op[2].type = ASM_OTYPE_FIXED;
  new->op[2].content = ASM_OP_BASE;
  new->op[2].regset = ASM_REGSET_R8;
  new->op[2].ptr = opcode;
  new->op[2].len = 0;
  new->op[2].baser = ASM_REG_CL;
#endif
  return (new->len);
}
