/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_saved.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_saved(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_PROLOG;

  if (opcode.rd == 0)
    ins->instr = ASM_SP_SAVED;
  else if (opcode.rd == 1)
    ins->instr = ASM_SP_RESTORED;
  else
    ins->instr = ASM_SP_BAD;

  return 4;
}
