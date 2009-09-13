/**
* @file libasm/src/arch/ia32/handlers/op_sti.c
 *
 * @ingroup IA32_instrs
 *
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/**
 * @todo: Set flags field.
 *
 *
  <instruction func="op_sti" opcode="0xfb"/>
*/

int op_sti(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->instr = ASM_STI;
  new->type = ASM_TYPE_WRITEFLAG;
  /* Should be VIF for CPL = 3 and IOPL < CPL */
  new->flagswritten = ASM_FLAG_IF;
  return (new->len);
}
