/**
* @file libasm/src/arch/ia32/operand_handlers/asm_operand_fetch_immediateword.c
 *
 * @ingroup IA32_operands
 * $Id$
 */

#include <libasm.h>
#include <libasm-int.h>

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
int     asm_operand_fetch_immediateword(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_immediateword(asm_operand *operand, u_char *opcode, 
					int otype, asm_instr *ins)
#endif
{ 
  operand->type = ASM_OTYPE_IMMEDIATE;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  operand->len = 2;
  memcpy(&operand->imm, opcode, 2);
  return (2);
}
