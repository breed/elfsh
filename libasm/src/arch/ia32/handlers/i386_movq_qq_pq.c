/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <i386 func="i386_movq_qq_pq" opcode="0x7f"/>
 */

int     i386_movq_qq_pq(asm_instr *new, u_char *opcode, u_int len,
			asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_MOVQ;

  #if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_PMMX, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_PMMX, new);
#endif
#else

  new->op[0].type = ASM_OTYPE_QMMX;
  new->op[0].size = ASM_OSIZE_QWORD;
  new->op[1].type = ASM_OTYPE_PMMX;
  new->op[1].size = ASM_OSIZE_QWORD;

  operand_rmv_rv(new, opcode + 1, len - 1, proc);
  new->op[1].regset = ASM_REGSET_MM;
  #endif
  return (new->len);
}
