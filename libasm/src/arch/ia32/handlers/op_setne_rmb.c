/**
 * @file op_setne_rmb.c
 * @ingroup handlers_ia32
 * $Id: op_setne_rmb.c,v 1.6 2007-10-14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * Handler for instruction setne, opcode 0x0f 0x95
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction.
*/

int op_setne_rmb(asm_instr *new, u_char *opcode, u_int len, 
		 asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_SET_NOT_EQUAL;
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, 
				ASM_OTYPE_ENCODEDBYTE, new);
#endif
  return (new->len);
}
