/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_illtrap.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_illtrap(asm_instr * ins, u_char * buf, u_int len,
		  asm_processor * proc)
{
  struct s_decode_branch opcode;
  sparc_convert_branch(&opcode, buf);
  
  ins->type = ASM_TYPE_INT | ASM_TYPE_STOP;

  ins->instr = ASM_SP_ILLTRAP;
  ins->nb_op = 1;
  ins->op[0].imm = opcode.immediate;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_IMMEDIATE, ins);

  return 4;
}
