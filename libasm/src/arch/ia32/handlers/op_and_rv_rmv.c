/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_and_rv_rmv" opcode="0x23"/>
*/

int op_and_rv_rmv(asm_instr *new, u_char *opcode, u_int len,
                  asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_AND;

  new->type = ASM_TYPE_ARITH | ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_CF | ASM_FLAG_ZF | ASM_FLAG_PF |
                      ASM_FLAG_OF | ASM_FLAG_SF;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_GENERAL,                                new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[1], opcode + 1, ASM_OTYPE_ENCODED,                                new);
#endif

  return (new->len);
}
