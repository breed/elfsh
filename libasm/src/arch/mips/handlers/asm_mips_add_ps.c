/**
* @file libasm/src/arch/mips/handlers/asm_mips_add_ps.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_add_ps(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;

   ins->instr = ASM_MIPS_ADD_PS;
   ins->type = ASM_TYPE_ARITH | ASM_TYPE_ARCH;
   mips_convert_format_r(&temp, buf);
   ins->op[0].regset = ASM_MIPS_REG_FPU;
   ins->op[0].baser = temp.sa;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].regset = ASM_MIPS_REG_FPU;
   ins->op[1].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].regset = ASM_MIPS_REG_FPU;
   ins->op[2].baser = temp.rt;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exception: Reserved Instruction, Coprocessor Unusable */
   /* FPU Exceptions: Inexact, Unimplemented Operation, Invalid Operation, Overflow, Underflow */

   return 4;
}
