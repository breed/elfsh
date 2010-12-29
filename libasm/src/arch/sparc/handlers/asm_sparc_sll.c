/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_sll.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_sll(asm_instr * ins, u_char * buf, u_int len,
	      asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->arith = ASM_ARITH_SL;        
  ins->type = ASM_TYPE_ARITH;

  ins->instr = inter->shift_table[opcode.op3 - 0x25 +
                                 ((opcode.imm & 0x1000) ? 1 : 0)];

  ins->nb_op = 3;
  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_REGISTER, ins);
  ins->op[2].baser = opcode.rs1;
  asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_REGISTER, ins);

  if (opcode.i == 0) {
    ins->op[1].baser = opcode.rs2;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_REGISTER, ins);
  }
  else {
    ins->op[1].imm = opcode.shcnt;
    asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_IMMEDIATE, ins);
  }

  return 4;
}
