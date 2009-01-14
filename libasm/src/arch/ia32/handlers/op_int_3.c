/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_int_3" opcode="0xcc"/>
*/

int op_int_3(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->ptr_instr = opcode;
  new->len += 1;
  new->instr = ASM_INT3;
  new->type = ASM_TYPE_INT | ASM_TYPE_TOUCHSP;
  return (new->len);
}
