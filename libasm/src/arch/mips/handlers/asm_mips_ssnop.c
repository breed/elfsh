/**
* @file libasm/src/arch/mips/handlers/asm_mips_ssnop.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_ssnop(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_SSNOP;
   ins->type = ASM_TYPE_NOP;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_NOOP, ins);

   /* Exceptions: None */

   return 4;
}
