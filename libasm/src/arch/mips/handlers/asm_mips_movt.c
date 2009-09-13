/**
* @file libasm/src/arch/mips/handlers/asm_mips_movt.c
** @ingroup MIPS_instrs
*/
/* Adam 'pi3' Zabrocki */
/* Manuel Martin - 2007 */

#include <libasm.h>

int asm_mips_movt(asm_instr *ins, u_char *buf, u_int len,
                  asm_processor *proc)
{
   struct s_mips_decode_mov temp;

   ins->instr = ASM_MIPS_MOVT;
   ins->type = ASM_TYPE_ARCH | ASM_TYPE_ASSIGN;
   mips_convert_format_mov(&temp,buf);
   ins->op[0].baser = temp.rd;
   asm_mips_operand_fetch(&ins->op[0], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[1].baser = temp.rs;
   asm_mips_operand_fetch(&ins->op[1], buf, ASM_MIPS_OTYPE_REGISTER, ins);
   ins->op[2].imm = temp.cc;
   asm_mips_operand_fetch(&ins->op[2], buf, ASM_MIPS_OTYPE_IMMEDIATE, ins);

   /* Exceptions: Reserved Instruction, Coprocessor Unusable */

   return 4;
}
