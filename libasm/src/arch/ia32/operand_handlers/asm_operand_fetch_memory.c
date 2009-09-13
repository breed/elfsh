/**
* @file libasm/src/arch/ia32/operand_handlers/asm_operand_fetch_memory.c
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>
/**
 *
 *
 */
/**
 * Decode data for operand type ASM_OTYPE_YDEST
 *
 * @ingroup IA32_operands
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_memory(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_memory(asm_operand *operand, u_char *opcode, 
				 int otype, asm_instr *ins)
#endif
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_MEMORY;

  operand->content = ASM_OP_BASE;
  operand->regset = ASM_REGSET_R32;
  operand->baser = modrm->r;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);

  return (0);
}

