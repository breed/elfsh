/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
 * Handler for the aam instruction, opcode 0xd4
 * @param new Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of disassembled instruction.
*/

int op_aam(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AAM;
  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_SF | ASM_FLAG_ZF | ASM_FLAG_PF;
  return (new->len);
}
