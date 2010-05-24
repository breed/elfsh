/**
* @file libasm/src/arch/arm/arm_convert.c
** @ingroup arm
*/
#include <libasm.h>

void	arm_convert_dataproc(struct s_arm_decode_dataproc *opcode, u_char *buf)
{

#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 26) & 0x03;
  opcode->i = (converted >> 25) & 0x01;
  opcode->op2 = (converted >> 21) & 0x0F;
  opcode->s = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->shifter_operand = converted & 0xFFF;

#else
  memcpy(opcode,buf,4);
#endif

  opcode->rm = (opcode->shifter_operand & 0x0F);
  opcode->rs = opcode->rotate_imm = ((opcode->shifter_operand >> 8) & 0x0F);
  opcode->immed = (opcode->shifter_operand) & 0x0FF;
  opcode->shift = ((opcode->shifter_operand >> 5) & 0x03);
  opcode->shift_imm = ((opcode->shifter_operand >> 7) & 0x01F);
  opcode->reg_shift = ((opcode->shifter_operand >> 4) & 0x01);

  /* MSR/MRS */
  opcode->r = (opcode->op2 >> 1) & 0x01;
  opcode->topsr = opcode->op2 & 0x01;
  opcode->field_mask = opcode->rn;
}

void	arm_convert_multiply(struct s_arm_decode_multiply *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none = (converted >> 24) & 0x03;
  opcode->op = (converted >> 21) & 0x07;
  opcode->s = (converted >> 20) & 0x01;
  opcode->r1 = (converted >> 16) & 0x0F;
  opcode->r2 = (converted >> 12) & 0x0F;
  opcode->r3 = (converted >> 8) & 0x0F;
  opcode->none2 = (converted >> 7) & 0x01;
  opcode->y = (converted >> 6) & 0x01;
  opcode->x = (converted >> 5) & 0x01;
  opcode->none3 = (converted >> 4) & 0x01;
  opcode->r4 = (converted & 0x0F);

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_ldst(struct s_arm_decode_ldst *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 26) & 0x03;
  opcode->reg_offset = (converted >> 25) & 0x01;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->b = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->imm_offset = converted & 0xFFF;

#else
  memcpy(opcode,buf,4);
#endif

  opcode->rm = opcode->imm_offset & 0x0F;
  opcode->shift_imm = (opcode->imm_offset >> 7) & 0x1F;
  opcode->shift = (opcode->imm_offset >> 5) & 0x03;
}

void	arm_convert_ldst_misc(struct s_arm_decode_ldst_misc *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 25) & 0x07;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->i = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->immedH = (converted >> 8) & 0x0F;
  opcode->none1 = (converted >> 7) & 0x01;
  opcode->s = (converted >> 6) & 0x01;
  opcode->h = (converted >> 5) & 0x01;
  opcode->none2 = (converted >> 4) & 0x01;
  opcode->immedL_rm = converted & 0x0F;
#else
  memcpy(opcode,buf,4);
#endif

}

void	arm_convert_ldst_mult(struct s_arm_decode_ldst_mult *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 25) & 0x07;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->s = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->reg_list = converted & 0xFFFF;

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_swap(struct s_arm_decode_ldst *opcode, u_char *buf)
{
  /* Many fields of the structure aren't used for swap */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int 	converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->b = (converted >> 22) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->imm_offset = converted & 0xFFF;

#else
  memcpy(opcode,buf,4);
#endif

  opcode->rm = opcode->imm_offset & 0x0F;
}

void    arm_convert_branch1(struct s_arm_decode_branch1 *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->op = (converted >> 25) & 0x07;
  opcode->l_h = (converted >> 24) & 0x01;
  opcode->signed_imm = converted & 0x00FFFFFF;

#else
  memcpy(opcode,buf,4);
#endif
}

void    arm_convert_branch2(struct s_arm_decode_branch2 *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none1 = (converted >> 6) & 0x03FFFFF;
  opcode->op = (converted >> 5) & 0x01;
  opcode->none2 = (converted >> 4) & 0x01;
  opcode->rm = converted & 0x0F;

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_bkpt(struct s_arm_decode_bkpt *opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->none1 = (converted >> 20) & 0x0FFF;
  opcode->immed1 = (converted >> 8) & 0x0FFF;
  opcode->none2 = (converted >> 4) & 0x0F;
  opcode->immed2 = converted & 0x0F;

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_coproc_dataproc(struct s_arm_decode_coproc_dataproc* opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none = (converted >> 24) & 0x0F;
  opcode->op1 = (converted >> 20) & 0x0F;
  opcode->crn = (converted >> 16) & 0x0F;
  opcode->crd = (converted >> 12) & 0x0F;
  opcode->cpnum = (converted >> 8) & 0x0F;
  opcode->op2 = (converted >> 5) & 0x07;
  opcode->zero = (converted >> 4) & 0x01;
  opcode->crm = converted & 0x0F;

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_coproc_mov(struct s_arm_decode_coproc_mov* opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none = (converted >> 24) & 0x0F;
  opcode->op1 = (converted >> 21) & 0x07;
  opcode->toarm = (converted >> 20) & 0x01;
  opcode->crn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->cpnum = (converted >> 8) & 0x0F;
  opcode->op2 = (converted >> 5) & 0x07;
  opcode->one = (converted >> 4) & 0x01;
  opcode->crm = converted & 0x0F;

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_coproc_mov2(struct s_arm_decode_coproc_mov2* opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none = (converted >> 21) & 0x07F;
  opcode->toarm = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->rd = (converted >> 12) & 0x0F;
  opcode->cpnum = (converted >> 8) & 0x0F;
  opcode->op = (converted >> 4) & 0x0F;
  opcode->crm = converted & 0x0F;

#else
  memcpy(opcode,buf,4);
#endif
}

void	arm_convert_coproc_ldst(struct s_arm_decode_coproc_ldst* opcode, u_char *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
  int   converted;

  memcpy(&converted,buf,4);

  opcode->cond = (converted >> 28) & 0x0F;
  opcode->none = (converted >> 25) & 0x07;
  opcode->p = (converted >> 24) & 0x01;
  opcode->u = (converted >> 23) & 0x01;
  opcode->n = (converted >> 22) & 0x01;
  opcode->w = (converted >> 21) & 0x01;
  opcode->l = (converted >> 20) & 0x01;
  opcode->rn = (converted >> 16) & 0x0F;
  opcode->crd = (converted >> 12) & 0x0F;
  opcode->cpnum = (converted >> 8) & 0x0F;
  opcode->offset = converted & 0x0FF;

#else
  memcpy(opcode,buf,4);
#endif
}
