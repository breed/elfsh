/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_bicc.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_bicc(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_branch opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_branch(&opcode, buf);

  inter = proc->internals;

  ins->instr = inter->bcc_table[opcode.cond];

  if (ins->instr == ASM_SP_BA)
    ins->type = ASM_TYPE_BRANCH;
  else if (ins->instr == ASM_SP_BN)
    ins->type = ASM_TYPE_NOP;
  else
    ins->type = ASM_TYPE_BRANCH | ASM_TYPE_CONDCONTROL;

  ins->nb_op = 1;  
  ins->op[0].imm = opcode.imm;  
  ins->annul = opcode.a;
  ins->prediction = 1;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_DISPLACEMENT, ins);

  return 4;
}
