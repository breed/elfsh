/**
* @file libasm/src/arch/arm/handlers/asm_arm_bx.c
** @ingroup ARM_instrs
*/
#include "libasm.h"

int     asm_arm_bx(asm_instr * ins, u_char * buf, u_int len,
                    asm_processor * proc)
{
  struct s_arm_decode_branch2 opcode;
  struct s_asm_proc_arm *inter;

  LIBASM_PROFILE_FIN();

  inter = proc->internals;
  arm_convert_branch2(&opcode, buf);

  arm_decode_condition(ins, opcode.cond);

  ins->instr = inter->branch2_table[(opcode.op << 4) | opcode.cond];

  ins->name = ins->proc->instr_table[ins->instr];

  if (opcode.rm == ASM_ARM_REG_R14)
    /* This is a return */
    MASSIGNTYPE(ins, ASM_TYPE_RETPROC);
  else
    MASSIGNTYPE(ins, ASM_TYPE_BRANCH);
  MASSIGNTYPE(ins, ASM_TYPE_INDCONTROL | ASM_TYPE_ARCH); /* Change to Thumb/ARM */

  ins->nb_op = 1;

  /* Decode operands */
  ins->op[0].baser = opcode.rm;
  asm_arm_op_fetch(&ins->op[0], buf, ASM_ARM_OTYPE_REGISTER, ins);

  LIBASM_PROFILE_FOUT(4);
}
