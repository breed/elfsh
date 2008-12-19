/**
 * @file asm_operand_fetch_address.c
 * @ingroup operand_handler
 * $Id: asm_operand_fetch_address.c,v 1.4 2007/10/14 00:01:41 heroine Exp $
 */

#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch operands of 
 *
 *
 */
/**
 * @ingroup operand_handler
 * Decode data for operand type ASM_OTYPE_YDEST
 * @param operand Pointer to operand structure to fill.
 * @param opcode Pointer to operand data
 * @param otype
 * @param ins Pointer to instruction structure.
 * @return Operand length
 */
#if WIP
int     asm_operand_fetch_address(asm_operand *operand, u_char *opcode, int otype, 
				asm_instr *ins, int opt)
#else
int     asm_operand_fetch_address(asm_operand *operand, u_char *opcode, 
				  int otype, 
                                  asm_instr *ins)
#endif
{
  u_int	len;

  operand->type = ASM_OTYPE_ADDRESS;
  operand->content = ASM_OP_VALUE;
  operand->ptr = opcode;
  operand->imm = 0;
  len = asm_proc_opsize(ins->proc) ? 2 : 4;
  operand->len = len;
  memcpy(&operand->imm, opcode, len);
  if (len == 2)
    operand->imm = operand->imm & 0xFFFF;
  return (len);
}
