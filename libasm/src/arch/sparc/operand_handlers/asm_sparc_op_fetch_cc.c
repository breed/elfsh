/**
* @file libasm/src/arch/sparc/operand_handlers/asm_sparc_op_fetch_cc.c
** @ingroup SPARC_operands
*/
#include <libasm.h>
#include <libasm-sparc.h>

/**
 * Decode data for operand content ASM_SP_OTYPE_CC
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to opcode data.
 * @param otype Operand content.
 * @param ins Pointer to instruction structure.
 *
 * @return Operand length (currently unused in libasm-sparc)
 */

int asm_sparc_op_fetch_cc(asm_operand *operand, u_char *opcode, 
                 				   int otype, asm_instr *ins)
{
  operand->content = ASM_SP_OTYPE_CC;
  operand->type    = ASM_OPTYPE_REG;
  return (0);
}
