/**
* @file libasm/src/arch/mips/handlers/asm_mips_movcf_d.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_movcf_d(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_reg temp;
   u_int converted = 0;
   
   memcpy((char *)&converted,buf,sizeof(converted));

   if ((converted >> 16) & 0x1)
      ins->instr = ASM_MIPS_MOVT_D;
   else
      ins->instr = ASM_MIPS_MOVF_D;
   ins->type = ASM_TYPE_ARITH | ASM_TYPE_ARCH | ASM_TYPE_ASSIGN;
   mips_convert_format_r(&temp, buf);
   ins->op[0].regset = ASM_MIPS_REG_FPU;
   ins->op[0].baser = temp.sa;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].regset = ASM_MIPS_REG_FPU;
   ins->op[1].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].imm = temp.rt >> 2;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);      

   /* Exception: Reserved Instruction, Coprocessor Unusable */
   /* FPU Exceptions: Unimplemented Operation */

   return 4;
}
