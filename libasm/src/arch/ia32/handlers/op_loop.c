/*
** $Id: op_loop.c,v 1.5 2007/10/14 00:01:41 heroine Exp $
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_loop" opcode="0xe2"/>
 */

int op_loop(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_CONDBRANCH;
  new->instr = ASM_LOOP;

#if WIP
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_SHORTJUMP,                                new, 0);
#else
  new->len += asm_operand_fetch(&new->op[0], opcode + 1, ASM_OTYPE_SHORTJUMP,                                new);
#endif

  return (new->len);
}
