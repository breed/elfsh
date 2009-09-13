/**
* @file libasm/src/arch/ia32/handlers/op_repnz.c
 *
 * @ingroup IA32_instrs
** $Id$
**
*/
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_repnz" opcode="0xf2"/>
*/

int op_repnz(asm_instr *new, u_char *opcode, u_int len, asm_processor *proc)
{
  if (!new->ptr_prefix)
    new->ptr_prefix = opcode;
  new->len += 1;
  new->prefix = ASM_PREFIX_REPNE;

  return (proc->fetch(new, opcode + 1, len - 1, proc));
}
