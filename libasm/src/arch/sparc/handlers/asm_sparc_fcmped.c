/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_fcmped.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_fcmped(asm_instr * ins, u_char * buf, u_int len,
		 asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf);

  inter = proc->internals;
  ins->instr = inter->op2_table[opcode.op3];
  
  ins->type = ASM_TYPE_COMPARISON | ASM_TYPE_WRITEFLAG;
  ins->flagswritten = ASM_SP_FLAG_FCC0 << opcode.cc;

  ins->instr = inter->fcmp_table[(opcode.opf & 0x1f) - 16];
  ins->nb_op = 3;
  ins->op[0].baser = ((opcode.rs2 & 1) << 5) | (opcode.rs2 & 0x1E);
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[1].baser = ((opcode.rs1 & 1) << 5) | (opcode.rs1 & 0x1E);
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[2].baser = opcode.cc;
  asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_CC, ins);
  
  return 4;
}
