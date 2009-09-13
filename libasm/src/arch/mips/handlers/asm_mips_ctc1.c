/**
* @file libasm/src/arch/mips/handlers/asm_mips_ctc1.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_ctc1(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_cop2 temp;

   ins->instr = ASM_MIPS_CTC1;
   ins->type = ASM_TYPE_ARCH | ASM_TYPE_ASSIGN;
   mips_convert_format_cop2(&temp,buf);
   ins->op[0].baser = temp.rt;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);

   /* Exceptions: Coprocessor Unusable, Reserved instruction */
   /* FPU Exceptions: Unimplemented Operation, Invalid Operation, Division-by-zero, Inexact, Overflow, Underflow */

   return 4;
}
