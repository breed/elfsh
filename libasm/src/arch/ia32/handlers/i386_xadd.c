/**
* @file libasm/src/arch/ia32/handlers/i386_xadd.c
 *
 * @ingroup IA32_instrs
 *  $Id$
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for the opcode FF C1
 <i386 func="" opcode="0xc1"/>
*/

int i386_xadd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  struct s_modrm        *modrm;
  modrm = (struct s_modrm *) opcode;
  new->instr = ASM_XADD;
  new->len += 1;
#if LIBASM_USE_OPERAND_VECTOR
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_ENCODED,				new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,				new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,				new);
#endif
#else
  new->op[0].type = ASM_OTYPE_GENERAL;
  new->op[1].type = ASM_OTYPE_GENERAL;
  operand_rmv_rv(new, opcode + 1, len - 1, proc);
#endif
  return (new->len);
}
