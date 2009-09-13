/**
* @file libasm/src/arch/mips/operand_handlers/asm_mips_operand_regbase.c
** @ingroup MIPS_operands
*/
/*
 * - Adam 'pi3' Zabrocki
 *
 */

#include <libasm.h>

void    asm_mips_operand_regbase(asm_operand *op, u_char *opcode, int otype,
                                          asm_instr *ins)
{
   op->type = ASM_MIPS_OTYPE_REGBASE;
//   memcpy(&op->scale,opcode,4);
}
