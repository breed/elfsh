/**
 * @file asm_operand_fetch.c
 * $Id: asm_operand_fetch_register.c,v 1.3 2007-06-27 11:25:12 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 *
 *
 */
/**
 * Decode data for operand type ASM_OTYPE_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

int     asm_operand_fetch_register(asm_operand *operand, u_char *opcode, 
				   int otype, asm_instr *ins)
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  operand->type = ASM_OTYPE_REGISTER;
  operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(ins->proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->baser = modrm->m;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (0);
}
