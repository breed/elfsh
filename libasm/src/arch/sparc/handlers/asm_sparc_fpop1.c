/**
* @file libasm/src/arch/sparc/handlers/asm_sparc_fpop1.c
** @ingroup SPARC_instrs
*/
/*
**
** $Id$
**
*/
#include "libasm.h"

int
asm_sparc_fpop1(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;

  sparc_convert_format3(&opcode, buf);
  inter = proc->internals;  
  ins->instr = inter->fpop1_table[opcode.opf];

  if (opcode.opf < 0x40 || opcode.opf >= 0x70) {
    ins->type = ASM_TYPE_ASSIGN;
    ins->nb_op = 2;
  }
  else { /* 0x40 < opf < 0x69 - add, sub, mul, div */
    ins->type = ASM_TYPE_ARITH;

    /* FIXME: what is the value to switch on here ? */
    /*
    switch ()
      {
      case:
	ins->arith = ASM_ARITH_ADD;        
	break;
      case:
	ins->arith = ASM_ARITH_SUB;        
	break;
      case:
	ins->arith = ASM_ARITH_MUL;        
	break;
      case:
	ins->arith = ASM_ARITH_DIV;        
	break;
      }
    */

    ins->nb_op = 3;
    ins->op[2].baser = opcode.rs1;
    asm_sparc_op_fetch(&ins->op[2], buf, ASM_SP_OTYPE_FREGISTER, ins);
  }

  ins->op[0].baser = opcode.rd;
  asm_sparc_op_fetch(&ins->op[0], buf, ASM_SP_OTYPE_FREGISTER, ins);
  ins->op[1].baser = opcode.rs2;
  asm_sparc_op_fetch(&ins->op[1], buf, ASM_SP_OTYPE_FREGISTER, ins);

  return 4;

}
