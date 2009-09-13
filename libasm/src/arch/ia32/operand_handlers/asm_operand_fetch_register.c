/**
* @file libasm/src/arch/ia32/operand_handlers/asm_operand_fetch_register.c
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Decode data for operand type ASM_OTYPE_YDEST
 *
 * @ingroup IA32_operands
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */

#if WIP
int     asm_operand_fetch_register(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_register(asm_operand *operand, u_char *opcode, 
				   int otype, asm_instr *ins)
#endif
{
  struct s_modrm        *modrm;

  modrm = (struct s_modrm *) opcode;
  asm_content_pack(operand, ASM_OP_BASE, ASM_OTYPE_REGISTER);
  //operand->type = ASM_OTYPE_REGISTER;
  //operand->content = ASM_OP_BASE;
  operand->regset = asm_proc_opsize(ins->proc) ? 
    ASM_REGSET_R16 : ASM_REGSET_R32;
  operand->baser = modrm->m;
  operand->sbaser = get_reg_intel(operand->baser, operand->regset);
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 0;
  return (0);
}
