/**
 * @file op_cmpsb.c
 * @brief Handler for instruction cmpsb opcode 0xa6
 * @ingroup handlers_ia32
 * $Id: op_cmpsb.c,v 1.8 2007-10-14 00:01:41 heroine Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/**
 * @brief Handler for instruction cmpsb opcode 0xa6
 * @param instr Pointer to instruction structure.
 * @param opcode Pointer to data to disassemble.
 * @param len Length of data to disassemble.
 * @param proc Pointer to processor structure.
 * @return Length of instruction .
 */

int op_cmpsb(asm_instr *instr, u_char *opcode, u_int len, asm_processor *proc)
{
  instr->instr = ASM_CMPSB;
  instr->len += 1;
  instr->ptr_instr = opcode;

  instr->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  instr->flagswritten = ASM_FLAG_AF | ASM_FLAG_CF | ASM_FLAG_PF |
                        ASM_FLAG_OF | ASM_FLAG_SF | ASM_FLAG_ZF;

#if WIP
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_OTYPE_XSRC, instr, 0);
#else
  instr->len += asm_operand_fetch(&instr->op[0], opcode + 1, ASM_OTYPE_XSRC, instr);
#endif
#if WIP
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_OTYPE_YDEST, instr, 0);
#else
  instr->len += asm_operand_fetch(&instr->op[1], opcode + 1, ASM_OTYPE_YDEST, instr);
#endif

  return (instr->len);
}
