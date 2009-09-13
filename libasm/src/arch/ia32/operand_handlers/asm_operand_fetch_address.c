/**
 * @defgroup IA32_operands Libasm IA32 operands handlers.
 * @ingroup ia32
 */
/**
* @file libasm/src/arch/ia32/operand_handlers/asm_operand_fetch_address.c
 * @ingroup IA32_operands
 * $Id$
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Fetch operands of 
 *
 *
 */
/**
 * @ingroup IA32_operands
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
