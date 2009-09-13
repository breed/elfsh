/**
* @file libasm/src/arch/mips/handlers/asm_mips_cop2.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_cop2(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   ins->instr = ASM_MIPS_COP2;
   ins->type = ASM_TYPE_ARCH;

   /* Exceptions: Coprocessor Unusable, Reserved instruction */

   return 4;
}
