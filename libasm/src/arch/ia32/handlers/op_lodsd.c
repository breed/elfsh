/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_lodsd" opcode="0xad"/>
*/

int op_lodsd(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->instr = ASM_LODSD;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_LOAD;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_YDEST, new);
#endif
#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_XSRC, new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode, ASM_OTYPE_XSRC, new);
#endif

  return (new->len);
}
