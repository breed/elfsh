/*
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_cld" opcode="0xfc"/>
*/

int op_cld(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_CLD;
  new->type = ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_DF;
  return (new->len);
}
