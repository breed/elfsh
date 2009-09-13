/**
* @file libasm/src/arch/ia32/handlers/op_std.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_std" opcode="0xfd"/>
*/

int op_std(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_STD;
  new->type = ASM_TYPE_WRITEFLAG;
  new->flagswritten = ASM_FLAG_DF;
  return (new->len);
}
