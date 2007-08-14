/**
 * @file op_mov_rmb_rb.c
 * @ingroup handlers_ia32
 * $Id: op_mov_rmb_rb.c,v 1.6 2007-08-14 06:52:55 strauss Exp $
 *
 */
#include <libasm.h>
#include <libasm-int.h>

/*
  <instruction func="op_mov_rmb_rb" opcode="0x88"/>
*/

int op_mov_rmb_rb(asm_instr *new, u_char *opcode, u_int len, 
                  asm_processor *proc) 
{
  new->len += 1;
  new->ptr_instr = opcode;
  new->type = ASM_TYPE_ASSIGN;
  new->instr = ASM_MOV;

  new->len += asm_operand_fetch(&new->op1, opcode + 1, ASM_OTYPE_ENCODEDBYTE, 
                                new);
  new->len += asm_operand_fetch(&new->op2, opcode + 1, ASM_OTYPE_GENERALBYTE, 
                                new);

  return (new->len);
}
