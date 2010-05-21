/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_bpcc.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_bpcc(asm_instr * ins, u_char * buf, u_int len,
	       asm_processor * proc)
{
  struct s_decode_pbranch opcodep;
  struct s_asm_proc_sparc *inter;
  sparc_convert_pbranch(&opcodep, buf);

  inter = proc->internals;

  ins->instr = inter->bcc_table[opcodep.cond];

  if (ins->instr == ASM_SP_BA)
    ins->type = ASM_TYPE_BRANCH;
  else if (ins->instr == ASM_SP_BN)
    ins->type = ASM_TYPE_NOP;
  else
    ins->type = ASM_TYPE_BRANCH | ASM_TYPE_CONDCONTROL;

  ins->nb_op = 2;  
  ins->op[0].imm = opcodep.imm;  
  ins->op[1].baser = opcodep.cc + 4;
  ins->annul = opcodep.a;
  ins->prediction = opcodep.p;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_DISPLACEMENT, ins);
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_CC, ins);

  if (ins->instr == ASM_SP_BN &&
      ins->annul && ins->prediction && ins->op[1].baser == ASM_SP_XCC) {

    ins->instr = ASM_SP_IPREFETCH;
    ins->nb_op = 1;
    ins->type = ASM_TYPE_NONE;
  }

  return 4;
}
