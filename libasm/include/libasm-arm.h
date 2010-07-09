/**
 * @defgroup arm Libasm ARM support
 * @ingroup libasm
 */
/**
* @file libasm/include/libasm-arm.h
 * @ingroup arm
 */
#ifndef _LIBASM_ARM_H_
#define _LIBASM_ARM_H_

#include <libasm-arm-decode.h>

#define MGETBYTE(val, byte) ((val >> (byte*8)) & 0x00FF)
#define MGETNIBBLE(val, nibble) ((val >> (nibble*4)) & 0x000F)

#define MASSIGNTYPE(insptr, instype) ((insptr->type) |= (instype))
#define MUNASSIGNTYPE(insptr, instype) ((insptr->type) &= ~(instype))
#define MISTYPE(insptr, instype) ((insptr->type) & (instype))

/* Registration functions */
int asm_register_arm();

/* Resolve address */
void asm_resolve_arm(void *d, eresi_Addr val, char *buf, u_int len);
eresi_Addr asm_dest_resolve_arm(eresi_Addr addr, u_int disp, u_char half);

/* Output functions */
char *asm_arm_display_instr(asm_instr *instr, eresi_Addr addr);
char *asm_arm_get_op_name(asm_operand *op);
char *asm_arm_get_register(int reg);
char *asm_arm_get_psr_fields(u_int field_mask);
char *asm_arm_get_regset_suffix(int regset);
char *asm_arm_get_shift_type(u_int shift_type);
char *asm_arm_get_coprocessor(int coproc);
char *asm_arm_get_coprocessor_register(int reg);
void asm_arm_dump_operand(asm_instr *ins, int num, eresi_Addr addr, char *buf, u_int len);

/* Instruction handlers */
int asm_arm_illegal(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_undef(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_adc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_add(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_and(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_b(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_bic(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_bkpt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_bl(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_blx1(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_blx2(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_bx(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_cdp(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_clz(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_cmn(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_cmp(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_eor(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldm1(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldm2(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldm3(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrbt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrd(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrh(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrsb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrsh(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_ldrt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mcr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mcrr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mla(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mov(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mrc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mrrc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mrs(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_msr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mul(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_mvn(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_orr(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_pld(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_qadd(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_qdadd(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_qdsub(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_qsub(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_rsb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_rsc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_sbc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smlal(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smlalxy(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smlawy(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smlaxy(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smull(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smulwy(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_smulxy(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_stc(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_stm1(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_stm2(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_str(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strbt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strd(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strh(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_strt(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_sub(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_swi(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_swp(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_swpb(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_teq(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_tst(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_umlal(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);
int asm_arm_umull(asm_instr * ins, u_char * buf, u_int len, asm_processor * proc);

/* ARM operand handlers */
int asm_arm_op_fetch(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_immediate(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_register(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_reg_scaled(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_reg_offset(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_reg_list(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_disp(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_disp_half(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_coprocessor(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);
int asm_arm_op_fetch_coprocessor_register(asm_operand *operand, u_char *opcode, int otype, asm_instr *ins);

/* Decoding helper functions */
void arm_convert_dataproc(struct s_arm_decode_dataproc *opcode, u_char *buf);
void arm_convert_ldst(struct s_arm_decode_ldst *opcode, u_char *buf);
void arm_convert_ldst_misc(struct s_arm_decode_ldst_misc *opcode, u_char *buf);
void arm_convert_ldst_mult(struct s_arm_decode_ldst_mult *opcode, u_char *buf);
void arm_convert_swap(struct s_arm_decode_ldst *opcode, u_char *buf);
void arm_convert_multiply(struct s_arm_decode_multiply *opcode, u_char *buf);
void arm_convert_branch1(struct s_arm_decode_branch1 *opcode, u_char *buf);
void arm_convert_branch2(struct s_arm_decode_branch2 *opcode, u_char *buf);
void arm_convert_bkpt(struct s_arm_decode_bkpt *opcode, u_char *buf);
void arm_convert_coproc_dataproc(struct s_arm_decode_coproc_dataproc* opcode, u_char *buf);
void arm_convert_coproc_mov(struct s_arm_decode_coproc_mov* opcode, u_char *buf);
void arm_convert_coproc_mov2(struct s_arm_decode_coproc_mov2* opcode, u_char *buf);
void arm_convert_coproc_ldst(struct s_arm_decode_coproc_ldst* opcode, u_char *buf);


void arm_decode_condition(asm_instr *ins, u_char condition);
void arm_decode_dataproc_flagswritten(asm_instr *ins, struct s_arm_decode_dataproc *opcode);

void arm_decode_dataproc_shfop(asm_instr *ins, u_char *buf, u_int op_nr, struct s_arm_decode_dataproc *opcode);
void arm_decode_ldst_offop(asm_instr *ins, u_char *buf, u_int op_nr, struct s_arm_decode_ldst *opcode);
void arm_decode_ldst_misc_offop(asm_instr *ins, u_char *buf, u_int op_nr, struct s_arm_decode_ldst_misc *opcode);
void arm_decode_multiply_long(asm_instr *ins, u_char *buf, struct s_arm_decode_multiply *opcode);
void arm_decode_coproc_ldst_offop(asm_instr *ins, u_char *buf, u_int op_nr, struct s_arm_decode_coproc_ldst *opcode);

struct  s_asm_proc_arm
{
  int   *dataproc_table;
  int   *movpsr_table;
  int   *clz_table;
  int   *swi_table;
  int   *ldst_table;
  int   *ldst_mult_table;
  int   *ldst_misc_table;
  int   *swap_table;
  int   *multiply_table;
  int   *dsp_arith_table;
  int   *dsp_multiply_table;
  int   *branch1_table;
  int   *branch2_table;
  int   *coproc_dataproc_table;
  int   *coproc_mov_table;
  int   *coproc_mov2_table;
  int   *coproc_ldst_table;
};

enum e_arm_shift
  {
    ASM_ARM_SHIFT_LSL,
    ASM_ARM_SHIFT_LSR,
    ASM_ARM_SHIFT_ASR,
    ASM_ARM_SHIFT_ROR,
    ASM_ARM_SHIFT_RRX,

    ASM_ARM_SHIFT_NUM
  };

enum e_arm_addressing
  {
    ASM_ARM_ADDRESSING_OFFSET,
    ASM_ARM_ADDRESSING_PREINDEXED,
    ASM_ARM_ADDRESSING_POSTINDEXED,
    ASM_ARM_ADDRESSING_UNINDEXED, /* Coprocessor only */

    ASM_ARM_ADDRESSING_NUM
  };

enum e_arm_operand
  {
    ASM_ARM_OTYPE_REGISTER,
    ASM_ARM_OTYPE_IMMEDIATE,
    ASM_ARM_OTYPE_REG_SCALED,
    ASM_ARM_OTYPE_REG_OFFSET,
    ASM_ARM_OTYPE_REG_LIST,
    ASM_ARM_OTYPE_DISP,
    ASM_ARM_OTYPE_DISP_HALF,
    ASM_ARM_OTYPE_COPROC,
    ASM_ARM_OTYPE_COPROC_REGISTER,

    ASM_ARM_OTYPE_NUM
  };

enum e_arm_registers
  {
    ASM_ARM_REG_R0,
    ASM_ARM_REG_R1,
    ASM_ARM_REG_R2,
    ASM_ARM_REG_R3,
    ASM_ARM_REG_R4,
    ASM_ARM_REG_R5,
    ASM_ARM_REG_R6,
    ASM_ARM_REG_R7,
    ASM_ARM_REG_R8,
    ASM_ARM_REG_R9,
    ASM_ARM_REG_R10,
    ASM_ARM_REG_R11,
    ASM_ARM_REG_R12,
    ASM_ARM_REG_R13,
    ASM_ARM_REG_R14,
    ASM_ARM_REG_PC,
    ASM_ARM_REG_CPSR,
    ASM_ARM_REG_SPSR,

    ASM_ARM_REG_NUM
  };

enum e_arm_regset
  {
    ASM_ARM_REGSET_USR, /* User/System registers */
    ASM_ARM_REGSET_SVC, /* Supervisor registers */
    ASM_ARM_REGSET_ABT, /* Abort registers */
    ASM_ARM_REGSET_UND, /* Undefined registers */
    ASM_ARM_REGSET_IRQ, /* IRQ register */
    ASM_ARM_REGSET_FIQ, /* FIQ registers */

    ASM_ARM_REGSET_NUM
  };

/* ARM Flags - values correspond to the bit in the CPSR/SPSR register */
enum e_arm_flags
  {
    ASM_ARM_FLAG_NONE = 0x00,

    ASM_ARM_FLAG_N = 1 << 31, /* Negative */
    ASM_ARM_FLAG_Z = 1 << 30, /* Zero */
    ASM_ARM_FLAG_C = 1 << 29, /* Carry */
    ASM_ARM_FLAG_V = 1 << 28, /* Overflow */
    ASM_ARM_FLAG_Q = 1 << 27  /* Enhanced DSP Overflow (ARMv5E and superior) */
  };

enum e_arm_conditions
  {
    ASM_ARM_COND_EQ,
    ASM_ARM_COND_NE,
    ASM_ARM_COND_CS,
    ASM_ARM_COND_CC,
    ASM_ARM_COND_MI,
    ASM_ARM_COND_PL,
    ASM_ARM_COND_VS,
    ASM_ARM_COND_VC,
    ASM_ARM_COND_HI,
    ASM_ARM_COND_LS,
    ASM_ARM_COND_GE,
    ASM_ARM_COND_LT,
    ASM_ARM_COND_GT,
    ASM_ARM_COND_LE,
    ASM_ARM_COND_AL,
    ASM_ARM_COND_NV
  };

/* Coprocessor related */
enum e_arm_coprocessor
  {
    ASM_ARM_COPROC_P0,
    ASM_ARM_COPROC_P1,
    ASM_ARM_COPROC_P2,
    ASM_ARM_COPROC_P3,
    ASM_ARM_COPROC_P4,
    ASM_ARM_COPROC_P5,
    ASM_ARM_COPROC_P6,
    ASM_ARM_COPROC_P7,
    ASM_ARM_COPROC_P8,
    ASM_ARM_COPROC_P9,
    ASM_ARM_COPROC_P10,
    ASM_ARM_COPROC_P11,
    ASM_ARM_COPROC_P12,
    ASM_ARM_COPROC_P13,
    ASM_ARM_COPROC_P14,
    ASM_ARM_COPROC_P15,

    ASM_ARM_COPROC_NUM
  };

enum e_arm_coprocessor_registers
  {
    ASM_ARM_COPROC_REG_CR0,
    ASM_ARM_COPROC_REG_CR1,
    ASM_ARM_COPROC_REG_CR2,
    ASM_ARM_COPROC_REG_CR3,
    ASM_ARM_COPROC_REG_CR4,
    ASM_ARM_COPROC_REG_CR5,
    ASM_ARM_COPROC_REG_CR6,
    ASM_ARM_COPROC_REG_CR7,
    ASM_ARM_COPROC_REG_CR8,
    ASM_ARM_COPROC_REG_CR9,
    ASM_ARM_COPROC_REG_CR10,
    ASM_ARM_COPROC_REG_CR11,
    ASM_ARM_COPROC_REG_CR12,
    ASM_ARM_COPROC_REG_CR13,
    ASM_ARM_COPROC_REG_CR14,
    ASM_ARM_COPROC_REG_CR15,

    ASM_ARM_COPROC_REG_NUM
  };

/***
 * Instruction list
 * 
 * Last instruction must be ASM_ARM_BAD
 */

enum e_arm_instr
  {
    ASM_ARM_,

    /* ADC */
    ASM_ARM_ADCEQ,
    ASM_ARM_ADCEQS,
    ASM_ARM_ADCNE,
    ASM_ARM_ADCNES,
    ASM_ARM_ADCCS,
    ASM_ARM_ADCCSS,
    ASM_ARM_ADCCC,
    ASM_ARM_ADCCCS,
    ASM_ARM_ADCMI,
    ASM_ARM_ADCMIS,
    ASM_ARM_ADCPL,
    ASM_ARM_ADCPLS,
    ASM_ARM_ADCVS,
    ASM_ARM_ADCVSS,
    ASM_ARM_ADCVC,
    ASM_ARM_ADCVCS,
    ASM_ARM_ADCHI,
    ASM_ARM_ADCHIS,
    ASM_ARM_ADCLS,
    ASM_ARM_ADCLSS,
    ASM_ARM_ADCGE,
    ASM_ARM_ADCGES,
    ASM_ARM_ADCLT,
    ASM_ARM_ADCLTS,
    ASM_ARM_ADCGT,
    ASM_ARM_ADCGTS,
    ASM_ARM_ADCLE,
    ASM_ARM_ADCLES,
    ASM_ARM_ADC,
    ASM_ARM_ADCS,

    /* ADD */
    ASM_ARM_ADDEQ,
    ASM_ARM_ADDEQS,
    ASM_ARM_ADDNE,
    ASM_ARM_ADDNES,
    ASM_ARM_ADDCS,
    ASM_ARM_ADDCSS,
    ASM_ARM_ADDCC,
    ASM_ARM_ADDCCS,
    ASM_ARM_ADDMI,
    ASM_ARM_ADDMIS,
    ASM_ARM_ADDPL,
    ASM_ARM_ADDPLS,
    ASM_ARM_ADDVS,
    ASM_ARM_ADDVSS,
    ASM_ARM_ADDVC,
    ASM_ARM_ADDVCS,
    ASM_ARM_ADDHI,
    ASM_ARM_ADDHIS,
    ASM_ARM_ADDLS,
    ASM_ARM_ADDLSS,
    ASM_ARM_ADDGE,
    ASM_ARM_ADDGES,
    ASM_ARM_ADDLT,
    ASM_ARM_ADDLTS,
    ASM_ARM_ADDGT,
    ASM_ARM_ADDGTS,
    ASM_ARM_ADDLE,
    ASM_ARM_ADDLES,
    ASM_ARM_ADD,
    ASM_ARM_ADDS,

    /* AND */
    ASM_ARM_ANDEQ,
    ASM_ARM_ANDEQS,
    ASM_ARM_ANDNE,
    ASM_ARM_ANDNES,
    ASM_ARM_ANDCS,
    ASM_ARM_ANDCSS,
    ASM_ARM_ANDCC,
    ASM_ARM_ANDCCS,
    ASM_ARM_ANDMI,
    ASM_ARM_ANDMIS,
    ASM_ARM_ANDPL,
    ASM_ARM_ANDPLS,
    ASM_ARM_ANDVS,
    ASM_ARM_ANDVSS,
    ASM_ARM_ANDVC,
    ASM_ARM_ANDVCS,
    ASM_ARM_ANDHI,
    ASM_ARM_ANDHIS,
    ASM_ARM_ANDLS,
    ASM_ARM_ANDLSS,
    ASM_ARM_ANDGE,
    ASM_ARM_ANDGES,
    ASM_ARM_ANDLT,
    ASM_ARM_ANDLTS,
    ASM_ARM_ANDGT,
    ASM_ARM_ANDGTS,
    ASM_ARM_ANDLE,
    ASM_ARM_ANDLES,
    ASM_ARM_AND,
    ASM_ARM_ANDS,

    /* B */
    ASM_ARM_BEQ,
    ASM_ARM_BNE,
    ASM_ARM_BCS,
    ASM_ARM_BCC,
    ASM_ARM_BMI,
    ASM_ARM_BPL,
    ASM_ARM_BVS,
    ASM_ARM_BVC,
    ASM_ARM_BHI,
    ASM_ARM_BLS,
    ASM_ARM_BGE,
    ASM_ARM_BLT,
    ASM_ARM_BGT,
    ASM_ARM_BLE,
    ASM_ARM_B,

    /* BIC */
    ASM_ARM_BICEQ,
    ASM_ARM_BICEQS,
    ASM_ARM_BICNE,
    ASM_ARM_BICNES,
    ASM_ARM_BICCS,
    ASM_ARM_BICCSS,
    ASM_ARM_BICCC,
    ASM_ARM_BICCCS,
    ASM_ARM_BICMI,
    ASM_ARM_BICMIS,
    ASM_ARM_BICPL,
    ASM_ARM_BICPLS,
    ASM_ARM_BICVS,
    ASM_ARM_BICVSS,
    ASM_ARM_BICVC,
    ASM_ARM_BICVCS,
    ASM_ARM_BICHI,
    ASM_ARM_BICHIS,
    ASM_ARM_BICLS,
    ASM_ARM_BICLSS,
    ASM_ARM_BICGE,
    ASM_ARM_BICGES,
    ASM_ARM_BICLT,
    ASM_ARM_BICLTS,
    ASM_ARM_BICGT,
    ASM_ARM_BICGTS,
    ASM_ARM_BICLE,
    ASM_ARM_BICLES,
    ASM_ARM_BIC,
    ASM_ARM_BICS,

    ASM_ARM_BKPT, /* uncond TEQ */

    /* BL */
    ASM_ARM_BLEQ,
    ASM_ARM_BLNE,
    ASM_ARM_BLCS,
    ASM_ARM_BLCC,
    ASM_ARM_BLMI,
    ASM_ARM_BLPL,
    ASM_ARM_BLVS,
    ASM_ARM_BLVC,
    ASM_ARM_BLHI,
    ASM_ARM_BLLS,
    ASM_ARM_BLGE,
    ASM_ARM_BLLT,
    ASM_ARM_BLGT,
    ASM_ARM_BLLE,
    ASM_ARM_BL,

    /* BLX(1) */
    ASM_ARM_BLX1, /* blx(1): uncond b/bl */

    /* BLX(2) */
    ASM_ARM_BLX2EQ,
    ASM_ARM_BLX2NE,
    ASM_ARM_BLX2CS,
    ASM_ARM_BLX2CC,
    ASM_ARM_BLX2MI,
    ASM_ARM_BLX2PL,
    ASM_ARM_BLX2VS,
    ASM_ARM_BLX2VC,
    ASM_ARM_BLX2HI,
    ASM_ARM_BLX2LS,
    ASM_ARM_BLX2GE,
    ASM_ARM_BLX2LT,
    ASM_ARM_BLX2GT,
    ASM_ARM_BLX2LE,
    ASM_ARM_BLX2,

    /* BX */
    ASM_ARM_BXEQ,
    ASM_ARM_BXNE,
    ASM_ARM_BXCS,
    ASM_ARM_BXCC,
    ASM_ARM_BXMI,
    ASM_ARM_BXPL,
    ASM_ARM_BXVS,
    ASM_ARM_BXVC,
    ASM_ARM_BXHI,
    ASM_ARM_BXLS,
    ASM_ARM_BXGE,
    ASM_ARM_BXLT,
    ASM_ARM_BXGT,
    ASM_ARM_BXLE,
    ASM_ARM_BX,

    /* CDP/CDP2 */
    ASM_ARM_CDPEQ,
    ASM_ARM_CDPNE,
    ASM_ARM_CDPCS,
    ASM_ARM_CDPCC,
    ASM_ARM_CDPMI,
    ASM_ARM_CDPPL,
    ASM_ARM_CDPVS,
    ASM_ARM_CDPVC,
    ASM_ARM_CDPHI,
    ASM_ARM_CDPLS,
    ASM_ARM_CDPGE,
    ASM_ARM_CDPLT,
    ASM_ARM_CDPGT,
    ASM_ARM_CDPLE,
    ASM_ARM_CDP,
    ASM_ARM_CDP2,

    /* CLZ */
    ASM_ARM_CLZEQ,
    ASM_ARM_CLZNE,
    ASM_ARM_CLZCS,
    ASM_ARM_CLZCC,
    ASM_ARM_CLZMI,
    ASM_ARM_CLZPL,
    ASM_ARM_CLZVS,
    ASM_ARM_CLZVC,
    ASM_ARM_CLZHI,
    ASM_ARM_CLZLS,
    ASM_ARM_CLZGE,
    ASM_ARM_CLZLT,
    ASM_ARM_CLZGT,
    ASM_ARM_CLZLE,
    ASM_ARM_CLZ,

    /* CMN */
    ASM_ARM_CMNEQ,
    ASM_ARM_CMNNE,
    ASM_ARM_CMNCS,
    ASM_ARM_CMNCC,
    ASM_ARM_CMNMI,
    ASM_ARM_CMNPL,
    ASM_ARM_CMNVS,
    ASM_ARM_CMNVC,
    ASM_ARM_CMNHI,
    ASM_ARM_CMNLS,
    ASM_ARM_CMNGE,
    ASM_ARM_CMNLT,
    ASM_ARM_CMNGT,
    ASM_ARM_CMNLE,
    ASM_ARM_CMN,

    /* CMP */
    ASM_ARM_CMPEQ,
    ASM_ARM_CMPNE,
    ASM_ARM_CMPCS,
    ASM_ARM_CMPCC,
    ASM_ARM_CMPMI,
    ASM_ARM_CMPPL,
    ASM_ARM_CMPVS,
    ASM_ARM_CMPVC,
    ASM_ARM_CMPHI,
    ASM_ARM_CMPLS,
    ASM_ARM_CMPGE,
    ASM_ARM_CMPLT,
    ASM_ARM_CMPGT,
    ASM_ARM_CMPLE,
    ASM_ARM_CMP,

    /* EOR */
    ASM_ARM_EOREQ,
    ASM_ARM_EOREQS,
    ASM_ARM_EORNE,
    ASM_ARM_EORNES,
    ASM_ARM_EORCS,
    ASM_ARM_EORCSS,
    ASM_ARM_EORCC,
    ASM_ARM_EORCCS,
    ASM_ARM_EORMI,
    ASM_ARM_EORMIS,
    ASM_ARM_EORPL,
    ASM_ARM_EORPLS,
    ASM_ARM_EORVS,
    ASM_ARM_EORVSS,
    ASM_ARM_EORVC,
    ASM_ARM_EORVCS,
    ASM_ARM_EORHI,
    ASM_ARM_EORHIS,
    ASM_ARM_EORLS,
    ASM_ARM_EORLSS,
    ASM_ARM_EORGE,
    ASM_ARM_EORGES,
    ASM_ARM_EORLT,
    ASM_ARM_EORLTS,
    ASM_ARM_EORGT,
    ASM_ARM_EORGTS,
    ASM_ARM_EORLE,
    ASM_ARM_EORLES,
    ASM_ARM_EOR,
    ASM_ARM_EORS,

    /* LDC/LDC2 */
    ASM_ARM_LDCEQ,
    ASM_ARM_LDCEQL,
    ASM_ARM_LDCNE,
    ASM_ARM_LDCNEL,
    ASM_ARM_LDCCS,
    ASM_ARM_LDCCSL,
    ASM_ARM_LDCCC,
    ASM_ARM_LDCCCL,
    ASM_ARM_LDCMI,
    ASM_ARM_LDCMIL,
    ASM_ARM_LDCPL,
    ASM_ARM_LDCPLL,
    ASM_ARM_LDCVS,
    ASM_ARM_LDCVSL,
    ASM_ARM_LDCVC,
    ASM_ARM_LDCVCL,
    ASM_ARM_LDCHI,
    ASM_ARM_LDCHIL,
    ASM_ARM_LDCLS,
    ASM_ARM_LDCLSL,
    ASM_ARM_LDCGE,
    ASM_ARM_LDCGEL,
    ASM_ARM_LDCLT,
    ASM_ARM_LDCLTL,
    ASM_ARM_LDCGT,
    ASM_ARM_LDCGTL,
    ASM_ARM_LDCLE,
    ASM_ARM_LDCLEL,
    ASM_ARM_LDC,
    ASM_ARM_LDCL,
    ASM_ARM_LDC2,
    ASM_ARM_LDC2L,

    /* LDM */
    ASM_ARM_LDMEQIA,
    ASM_ARM_LDMEQIB,
    ASM_ARM_LDMEQDA,
    ASM_ARM_LDMEQDB,
    ASM_ARM_LDMNEIA,
    ASM_ARM_LDMNEIB,
    ASM_ARM_LDMNEDA,
    ASM_ARM_LDMNEDB,
    ASM_ARM_LDMCSIA,
    ASM_ARM_LDMCSIB,
    ASM_ARM_LDMCSDA,
    ASM_ARM_LDMCSDB,
    ASM_ARM_LDMCCIA,
    ASM_ARM_LDMCCIB,
    ASM_ARM_LDMCCDA,
    ASM_ARM_LDMCCDB,
    ASM_ARM_LDMMIIA,
    ASM_ARM_LDMMIIB,
    ASM_ARM_LDMMIDA,
    ASM_ARM_LDMMIDB,
    ASM_ARM_LDMPLIA,
    ASM_ARM_LDMPLIB,
    ASM_ARM_LDMPLDA,
    ASM_ARM_LDMPLDB,
    ASM_ARM_LDMVSIA,
    ASM_ARM_LDMVSIB,
    ASM_ARM_LDMVSDA,
    ASM_ARM_LDMVSDB,
    ASM_ARM_LDMVCIA,
    ASM_ARM_LDMVCIB,
    ASM_ARM_LDMVCDA,
    ASM_ARM_LDMVCDB,
    ASM_ARM_LDMHIIA,
    ASM_ARM_LDMHIIB,
    ASM_ARM_LDMHIDA,
    ASM_ARM_LDMHIDB,
    ASM_ARM_LDMLSIA,
    ASM_ARM_LDMLSIB,
    ASM_ARM_LDMLSDA,
    ASM_ARM_LDMLSDB,
    ASM_ARM_LDMGEIA,
    ASM_ARM_LDMGEIB,
    ASM_ARM_LDMGEDA,
    ASM_ARM_LDMGEDB,
    ASM_ARM_LDMLTIA,
    ASM_ARM_LDMLTIB,
    ASM_ARM_LDMLTDA,
    ASM_ARM_LDMLTDB,
    ASM_ARM_LDMGTIA,
    ASM_ARM_LDMGTIB,
    ASM_ARM_LDMGTDA,
    ASM_ARM_LDMGTDB,
    ASM_ARM_LDMLEIA,
    ASM_ARM_LDMLEIB,
    ASM_ARM_LDMLEDA,
    ASM_ARM_LDMLEDB,
    ASM_ARM_LDMIA,
    ASM_ARM_LDMIB,
    ASM_ARM_LDMDA,
    ASM_ARM_LDMDB,

    /* LDR */
    ASM_ARM_LDREQ,
    ASM_ARM_LDRNE,
    ASM_ARM_LDRCS,
    ASM_ARM_LDRCC,
    ASM_ARM_LDRMI,
    ASM_ARM_LDRPL,
    ASM_ARM_LDRVS,
    ASM_ARM_LDRVC,
    ASM_ARM_LDRHI,
    ASM_ARM_LDRLS,
    ASM_ARM_LDRGE,
    ASM_ARM_LDRLT,
    ASM_ARM_LDRGT,
    ASM_ARM_LDRLE,
    ASM_ARM_LDR,

    /* LDRB */
    ASM_ARM_LDREQB,
    ASM_ARM_LDRNEB,
    ASM_ARM_LDRCSB,
    ASM_ARM_LDRCCB,
    ASM_ARM_LDRMIB,
    ASM_ARM_LDRPLB,
    ASM_ARM_LDRVSB,
    ASM_ARM_LDRVCB,
    ASM_ARM_LDRHIB,
    ASM_ARM_LDRLSB,
    ASM_ARM_LDRGEB,
    ASM_ARM_LDRLTB,
    ASM_ARM_LDRGTB,
    ASM_ARM_LDRLEB,
    ASM_ARM_LDRB,

    /* LDRBT */
    ASM_ARM_LDREQBT,
    ASM_ARM_LDRNEBT,
    ASM_ARM_LDRCSBT,
    ASM_ARM_LDRCCBT,
    ASM_ARM_LDRMIBT,
    ASM_ARM_LDRPLBT,
    ASM_ARM_LDRVSBT,
    ASM_ARM_LDRVCBT,
    ASM_ARM_LDRHIBT,
    ASM_ARM_LDRLSBT,
    ASM_ARM_LDRGEBT,
    ASM_ARM_LDRLTBT,
    ASM_ARM_LDRGTBT,
    ASM_ARM_LDRLEBT,
    ASM_ARM_LDRBT,

    /* LDRD */
    ASM_ARM_LDREQD,
    ASM_ARM_LDRNED,
    ASM_ARM_LDRCSD,
    ASM_ARM_LDRCCD,
    ASM_ARM_LDRMID,
    ASM_ARM_LDRPLD,
    ASM_ARM_LDRVSD,
    ASM_ARM_LDRVCD,
    ASM_ARM_LDRHID,
    ASM_ARM_LDRLSD,
    ASM_ARM_LDRGED,
    ASM_ARM_LDRLTD,
    ASM_ARM_LDRGTD,
    ASM_ARM_LDRLED,
    ASM_ARM_LDRD,

    /* LDRH */
    ASM_ARM_LDREQH,
    ASM_ARM_LDRNEH,
    ASM_ARM_LDRCSH,
    ASM_ARM_LDRCCH,
    ASM_ARM_LDRMIH,
    ASM_ARM_LDRPLH,
    ASM_ARM_LDRVSH,
    ASM_ARM_LDRVCH,
    ASM_ARM_LDRHIH,
    ASM_ARM_LDRLSH,
    ASM_ARM_LDRGEH,
    ASM_ARM_LDRLTH,
    ASM_ARM_LDRGTH,
    ASM_ARM_LDRLEH,
    ASM_ARM_LDRH,

    /* LDRSB */
    ASM_ARM_LDREQSB,
    ASM_ARM_LDRNESB,
    ASM_ARM_LDRCSSB,
    ASM_ARM_LDRCCSB,
    ASM_ARM_LDRMISB,
    ASM_ARM_LDRPLSB,
    ASM_ARM_LDRVSSB,
    ASM_ARM_LDRVCSB,
    ASM_ARM_LDRHISB,
    ASM_ARM_LDRLSSB,
    ASM_ARM_LDRGESB,
    ASM_ARM_LDRLTSB,
    ASM_ARM_LDRGTSB,
    ASM_ARM_LDRLESB,
    ASM_ARM_LDRSB,

    /* LDRSH */
    ASM_ARM_LDREQSH,
    ASM_ARM_LDRNESH,
    ASM_ARM_LDRCSSH,
    ASM_ARM_LDRCCSH,
    ASM_ARM_LDRMISH,
    ASM_ARM_LDRPLSH,
    ASM_ARM_LDRVSSH,
    ASM_ARM_LDRVCSH,
    ASM_ARM_LDRHISH,
    ASM_ARM_LDRLSSH,
    ASM_ARM_LDRGESH,
    ASM_ARM_LDRLTSH,
    ASM_ARM_LDRGTSH,
    ASM_ARM_LDRLESH,
    ASM_ARM_LDRSH,

    /* LDRT */
    ASM_ARM_LDREQT,
    ASM_ARM_LDRNET,
    ASM_ARM_LDRCST,
    ASM_ARM_LDRCCT,
    ASM_ARM_LDRMIT,
    ASM_ARM_LDRPLT,
    ASM_ARM_LDRVST,
    ASM_ARM_LDRVCT,
    ASM_ARM_LDRHIT,
    ASM_ARM_LDRLST,
    ASM_ARM_LDRGET,
    ASM_ARM_LDRLTT,
    ASM_ARM_LDRGTT,
    ASM_ARM_LDRLET,
    ASM_ARM_LDRT,

    /* MCR/MCR2 */
    ASM_ARM_MCREQ,
    ASM_ARM_MCRNE,
    ASM_ARM_MCRCS,
    ASM_ARM_MCRCC,
    ASM_ARM_MCRMI,
    ASM_ARM_MCRPL,
    ASM_ARM_MCRVS,
    ASM_ARM_MCRVC,
    ASM_ARM_MCRHI,
    ASM_ARM_MCRLS,
    ASM_ARM_MCRGE,
    ASM_ARM_MCRLT,
    ASM_ARM_MCRGT,
    ASM_ARM_MCRLE,
    ASM_ARM_MCR,
    ASM_ARM_MCR2,

    /* MCRR */
    ASM_ARM_MCRREQ,
    ASM_ARM_MCRRNE,
    ASM_ARM_MCRRCS,
    ASM_ARM_MCRRCC,
    ASM_ARM_MCRRMI,
    ASM_ARM_MCRRPL,
    ASM_ARM_MCRRVS,
    ASM_ARM_MCRRVC,
    ASM_ARM_MCRRHI,
    ASM_ARM_MCRRLS,
    ASM_ARM_MCRRGE,
    ASM_ARM_MCRRLT,
    ASM_ARM_MCRRGT,
    ASM_ARM_MCRRLE,
    ASM_ARM_MCRR,

    /* MLA */
    ASM_ARM_MLAEQ,
    ASM_ARM_MLAEQS,
    ASM_ARM_MLANE,
    ASM_ARM_MLANES,
    ASM_ARM_MLACS,
    ASM_ARM_MLACSS,
    ASM_ARM_MLACC,
    ASM_ARM_MLACCS,
    ASM_ARM_MLAMI,
    ASM_ARM_MLAMIS,
    ASM_ARM_MLAPL,
    ASM_ARM_MLAPLS,
    ASM_ARM_MLAVS,
    ASM_ARM_MLAVSS,
    ASM_ARM_MLAVC,
    ASM_ARM_MLAVCS,
    ASM_ARM_MLAHI,
    ASM_ARM_MLAHIS,
    ASM_ARM_MLALS,
    ASM_ARM_MLALSS,
    ASM_ARM_MLAGE,
    ASM_ARM_MLAGES,
    ASM_ARM_MLALT,
    ASM_ARM_MLALTS,
    ASM_ARM_MLAGT,
    ASM_ARM_MLAGTS,
    ASM_ARM_MLALE,
    ASM_ARM_MLALES,
    ASM_ARM_MLA,
    ASM_ARM_MLAS,

    /* MOV */
    ASM_ARM_MOVEQ,
    ASM_ARM_MOVEQS,
    ASM_ARM_MOVNE,
    ASM_ARM_MOVNES,
    ASM_ARM_MOVCS,
    ASM_ARM_MOVCSS,
    ASM_ARM_MOVCC,
    ASM_ARM_MOVCCS,
    ASM_ARM_MOVMI,
    ASM_ARM_MOVMIS,
    ASM_ARM_MOVPL,
    ASM_ARM_MOVPLS,
    ASM_ARM_MOVVS,
    ASM_ARM_MOVVSS,
    ASM_ARM_MOVVC,
    ASM_ARM_MOVVCS,
    ASM_ARM_MOVHI,
    ASM_ARM_MOVHIS,
    ASM_ARM_MOVLS,
    ASM_ARM_MOVLSS,
    ASM_ARM_MOVGE,
    ASM_ARM_MOVGES,
    ASM_ARM_MOVLT,
    ASM_ARM_MOVLTS,
    ASM_ARM_MOVGT,
    ASM_ARM_MOVGTS,
    ASM_ARM_MOVLE,
    ASM_ARM_MOVLES,
    ASM_ARM_MOV,
    ASM_ARM_MOVS,

    /* MRC/MRC2 */
    ASM_ARM_MRCEQ,
    ASM_ARM_MRCNE,
    ASM_ARM_MRCCS,
    ASM_ARM_MRCCC,
    ASM_ARM_MRCMI,
    ASM_ARM_MRCPL,
    ASM_ARM_MRCVS,
    ASM_ARM_MRCVC,
    ASM_ARM_MRCHI,
    ASM_ARM_MRCLS,
    ASM_ARM_MRCGE,
    ASM_ARM_MRCLT,
    ASM_ARM_MRCGT,
    ASM_ARM_MRCLE,
    ASM_ARM_MRC,
    ASM_ARM_MRC2,

    /* MRRC */
    ASM_ARM_MRRCEQ,
    ASM_ARM_MRRCNE,
    ASM_ARM_MRRCCS,
    ASM_ARM_MRRCCC,
    ASM_ARM_MRRCMI,
    ASM_ARM_MRRCPL,
    ASM_ARM_MRRCVS,
    ASM_ARM_MRRCVC,
    ASM_ARM_MRRCHI,
    ASM_ARM_MRRCLS,
    ASM_ARM_MRRCGE,
    ASM_ARM_MRRCLT,
    ASM_ARM_MRRCGT,
    ASM_ARM_MRRCLE,
    ASM_ARM_MRRC,

    /* MRS */
    ASM_ARM_MRSEQ,
    ASM_ARM_MRSNE,
    ASM_ARM_MRSCS,
    ASM_ARM_MRSCC,
    ASM_ARM_MRSMI,
    ASM_ARM_MRSPL,
    ASM_ARM_MRSVS,
    ASM_ARM_MRSVC,
    ASM_ARM_MRSHI,
    ASM_ARM_MRSLS,
    ASM_ARM_MRSGE,
    ASM_ARM_MRSLT,
    ASM_ARM_MRSGT,
    ASM_ARM_MRSLE,
    ASM_ARM_MRS,

    /* MSR */
    ASM_ARM_MSREQ,
    ASM_ARM_MSRNE,
    ASM_ARM_MSRCS,
    ASM_ARM_MSRCC,
    ASM_ARM_MSRMI,
    ASM_ARM_MSRPL,
    ASM_ARM_MSRVS,
    ASM_ARM_MSRVC,
    ASM_ARM_MSRHI,
    ASM_ARM_MSRLS,
    ASM_ARM_MSRGE,
    ASM_ARM_MSRLT,
    ASM_ARM_MSRGT,
    ASM_ARM_MSRLE,
    ASM_ARM_MSR,

    /* MUL */
    ASM_ARM_MULEQ,
    ASM_ARM_MULEQS,
    ASM_ARM_MULNE,
    ASM_ARM_MULNES,
    ASM_ARM_MULCS,
    ASM_ARM_MULCSS,
    ASM_ARM_MULCC,
    ASM_ARM_MULCCS,
    ASM_ARM_MULMI,
    ASM_ARM_MULMIS,
    ASM_ARM_MULPL,
    ASM_ARM_MULPLS,
    ASM_ARM_MULVS,
    ASM_ARM_MULVSS,
    ASM_ARM_MULVC,
    ASM_ARM_MULVCS,
    ASM_ARM_MULHI,
    ASM_ARM_MULHIS,
    ASM_ARM_MULLS,
    ASM_ARM_MULLSS,
    ASM_ARM_MULGE,
    ASM_ARM_MULGES,
    ASM_ARM_MULLT,
    ASM_ARM_MULLTS,
    ASM_ARM_MULGT,
    ASM_ARM_MULGTS,
    ASM_ARM_MULLE,
    ASM_ARM_MULLES,
    ASM_ARM_MUL,
    ASM_ARM_MULS,

    /* MVN */
    ASM_ARM_MVNEQ,
    ASM_ARM_MVNEQS,
    ASM_ARM_MVNNE,
    ASM_ARM_MVNNES,
    ASM_ARM_MVNCS,
    ASM_ARM_MVNCSS,
    ASM_ARM_MVNCC,
    ASM_ARM_MVNCCS,
    ASM_ARM_MVNMI,
    ASM_ARM_MVNMIS,
    ASM_ARM_MVNPL,
    ASM_ARM_MVNPLS,
    ASM_ARM_MVNVS,
    ASM_ARM_MVNVSS,
    ASM_ARM_MVNVC,
    ASM_ARM_MVNVCS,
    ASM_ARM_MVNHI,
    ASM_ARM_MVNHIS,
    ASM_ARM_MVNLS,
    ASM_ARM_MVNLSS,
    ASM_ARM_MVNGE,
    ASM_ARM_MVNGES,
    ASM_ARM_MVNLT,
    ASM_ARM_MVNLTS,
    ASM_ARM_MVNGT,
    ASM_ARM_MVNGTS,
    ASM_ARM_MVNLE,
    ASM_ARM_MVNLES,
    ASM_ARM_MVN,
    ASM_ARM_MVNS,

    /* ORR */
    ASM_ARM_ORREQ,
    ASM_ARM_ORREQS,
    ASM_ARM_ORRNE,
    ASM_ARM_ORRNES,
    ASM_ARM_ORRCS,
    ASM_ARM_ORRCSS,
    ASM_ARM_ORRCC,
    ASM_ARM_ORRCCS,
    ASM_ARM_ORRMI,
    ASM_ARM_ORRMIS,
    ASM_ARM_ORRPL,
    ASM_ARM_ORRPLS,
    ASM_ARM_ORRVS,
    ASM_ARM_ORRVSS,
    ASM_ARM_ORRVC,
    ASM_ARM_ORRVCS,
    ASM_ARM_ORRHI,
    ASM_ARM_ORRHIS,
    ASM_ARM_ORRLS,
    ASM_ARM_ORRLSS,
    ASM_ARM_ORRGE,
    ASM_ARM_ORRGES,
    ASM_ARM_ORRLT,
    ASM_ARM_ORRLTS,
    ASM_ARM_ORRGT,
    ASM_ARM_ORRGTS,
    ASM_ARM_ORRLE,
    ASM_ARM_ORRLES,
    ASM_ARM_ORR,
    ASM_ARM_ORRS,

    ASM_ARM_PLD, /* uncond LDRB */

    /* QADD */
    ASM_ARM_QADDEQ,
    ASM_ARM_QADDNE,
    ASM_ARM_QADDCS,
    ASM_ARM_QADDCC,
    ASM_ARM_QADDMI,
    ASM_ARM_QADDPL,
    ASM_ARM_QADDVS,
    ASM_ARM_QADDVC,
    ASM_ARM_QADDHI,
    ASM_ARM_QADDLS,
    ASM_ARM_QADDGE,
    ASM_ARM_QADDLT,
    ASM_ARM_QADDGT,
    ASM_ARM_QADDLE,
    ASM_ARM_QADD,

    /* QDADD */
    ASM_ARM_QDADDEQ,
    ASM_ARM_QDADDNE,
    ASM_ARM_QDADDCS,
    ASM_ARM_QDADDCC,
    ASM_ARM_QDADDMI,
    ASM_ARM_QDADDPL,
    ASM_ARM_QDADDVS,
    ASM_ARM_QDADDVC,
    ASM_ARM_QDADDHI,
    ASM_ARM_QDADDLS,
    ASM_ARM_QDADDGE,
    ASM_ARM_QDADDLT,
    ASM_ARM_QDADDGT,
    ASM_ARM_QDADDLE,
    ASM_ARM_QDADD,

    /* QDSUB */
    ASM_ARM_QDSUBEQ,
    ASM_ARM_QDSUBNE,
    ASM_ARM_QDSUBCS,
    ASM_ARM_QDSUBCC,
    ASM_ARM_QDSUBMI,
    ASM_ARM_QDSUBPL,
    ASM_ARM_QDSUBVS,
    ASM_ARM_QDSUBVC,
    ASM_ARM_QDSUBHI,
    ASM_ARM_QDSUBLS,
    ASM_ARM_QDSUBGE,
    ASM_ARM_QDSUBLT,
    ASM_ARM_QDSUBGT,
    ASM_ARM_QDSUBLE,
    ASM_ARM_QDSUB,

    /* QSUB */
    ASM_ARM_QSUBEQ,
    ASM_ARM_QSUBNE,
    ASM_ARM_QSUBCS,
    ASM_ARM_QSUBCC,
    ASM_ARM_QSUBMI,
    ASM_ARM_QSUBPL,
    ASM_ARM_QSUBVS,
    ASM_ARM_QSUBVC,
    ASM_ARM_QSUBHI,
    ASM_ARM_QSUBLS,
    ASM_ARM_QSUBGE,
    ASM_ARM_QSUBLT,
    ASM_ARM_QSUBGT,
    ASM_ARM_QSUBLE,
    ASM_ARM_QSUB,

    /* RSB */
    ASM_ARM_RSBEQ,
    ASM_ARM_RSBEQS,
    ASM_ARM_RSBNE,
    ASM_ARM_RSBNES,
    ASM_ARM_RSBCS,
    ASM_ARM_RSBCSS,
    ASM_ARM_RSBCC,
    ASM_ARM_RSBCCS,
    ASM_ARM_RSBMI,
    ASM_ARM_RSBMIS,
    ASM_ARM_RSBPL,
    ASM_ARM_RSBPLS,
    ASM_ARM_RSBVS,
    ASM_ARM_RSBVSS,
    ASM_ARM_RSBVC,
    ASM_ARM_RSBVCS,
    ASM_ARM_RSBHI,
    ASM_ARM_RSBHIS,
    ASM_ARM_RSBLS,
    ASM_ARM_RSBLSS,
    ASM_ARM_RSBGE,
    ASM_ARM_RSBGES,
    ASM_ARM_RSBLT,
    ASM_ARM_RSBLTS,
    ASM_ARM_RSBGT,
    ASM_ARM_RSBGTS,
    ASM_ARM_RSBLE,
    ASM_ARM_RSBLES,
    ASM_ARM_RSB,
    ASM_ARM_RSBS,

    /* RSC */
    ASM_ARM_RSCEQ,
    ASM_ARM_RSCEQS,
    ASM_ARM_RSCNE,
    ASM_ARM_RSCNES,
    ASM_ARM_RSCCS,
    ASM_ARM_RSCCSS,
    ASM_ARM_RSCCC,
    ASM_ARM_RSCCCS,
    ASM_ARM_RSCMI,
    ASM_ARM_RSCMIS,
    ASM_ARM_RSCPL,
    ASM_ARM_RSCPLS,
    ASM_ARM_RSCVS,
    ASM_ARM_RSCVSS,
    ASM_ARM_RSCVC,
    ASM_ARM_RSCVCS,
    ASM_ARM_RSCHI,
    ASM_ARM_RSCHIS,
    ASM_ARM_RSCLS,
    ASM_ARM_RSCLSS,
    ASM_ARM_RSCGE,
    ASM_ARM_RSCGES,
    ASM_ARM_RSCLT,
    ASM_ARM_RSCLTS,
    ASM_ARM_RSCGT,
    ASM_ARM_RSCGTS,
    ASM_ARM_RSCLE,
    ASM_ARM_RSCLES,
    ASM_ARM_RSC,
    ASM_ARM_RSCS,

    /* SBC */
    ASM_ARM_SBCEQ,
    ASM_ARM_SBCEQS,
    ASM_ARM_SBCNE,
    ASM_ARM_SBCNES,
    ASM_ARM_SBCCS,
    ASM_ARM_SBCCSS,
    ASM_ARM_SBCCC,
    ASM_ARM_SBCCCS,
    ASM_ARM_SBCMI,
    ASM_ARM_SBCMIS,
    ASM_ARM_SBCPL,
    ASM_ARM_SBCPLS,
    ASM_ARM_SBCVS,
    ASM_ARM_SBCVSS,
    ASM_ARM_SBCVC,
    ASM_ARM_SBCVCS,
    ASM_ARM_SBCHI,
    ASM_ARM_SBCHIS,
    ASM_ARM_SBCLS,
    ASM_ARM_SBCLSS,
    ASM_ARM_SBCGE,
    ASM_ARM_SBCGES,
    ASM_ARM_SBCLT,
    ASM_ARM_SBCLTS,
    ASM_ARM_SBCGT,
    ASM_ARM_SBCGTS,
    ASM_ARM_SBCLE,
    ASM_ARM_SBCLES,
    ASM_ARM_SBC,
    ASM_ARM_SBCS,

    /* SMLAL */
    ASM_ARM_SMLALEQ,
    ASM_ARM_SMLALEQS,
    ASM_ARM_SMLALNE,
    ASM_ARM_SMLALNES,
    ASM_ARM_SMLALCS,
    ASM_ARM_SMLALCSS,
    ASM_ARM_SMLALCC,
    ASM_ARM_SMLALCCS,
    ASM_ARM_SMLALMI,
    ASM_ARM_SMLALMIS,
    ASM_ARM_SMLALPL,
    ASM_ARM_SMLALPLS,
    ASM_ARM_SMLALVS,
    ASM_ARM_SMLALVSS,
    ASM_ARM_SMLALVC,
    ASM_ARM_SMLALVCS,
    ASM_ARM_SMLALHI,
    ASM_ARM_SMLALHIS,
    ASM_ARM_SMLALLS,
    ASM_ARM_SMLALLSS,
    ASM_ARM_SMLALGE,
    ASM_ARM_SMLALGES,
    ASM_ARM_SMLALLT,
    ASM_ARM_SMLALLTS,
    ASM_ARM_SMLALGT,
    ASM_ARM_SMLALGTS,
    ASM_ARM_SMLALLE,
    ASM_ARM_SMLALLES,
    ASM_ARM_SMLAL,
    ASM_ARM_SMLALS,

    /* SMLALXY */
    ASM_ARM_SMLALBBEQ,
    ASM_ARM_SMLALTBEQ,
    ASM_ARM_SMLALBTEQ,
    ASM_ARM_SMLALTTEQ,
    ASM_ARM_SMLALBBNE,
    ASM_ARM_SMLALTBNE,
    ASM_ARM_SMLALBTNE,
    ASM_ARM_SMLALTTNE,
    ASM_ARM_SMLALBBCS,
    ASM_ARM_SMLALTBCS,
    ASM_ARM_SMLALBTCS,
    ASM_ARM_SMLALTTCS,
    ASM_ARM_SMLALBBCC,
    ASM_ARM_SMLALTBCC,
    ASM_ARM_SMLALBTCC,
    ASM_ARM_SMLALTTCC,
    ASM_ARM_SMLALBBMI,
    ASM_ARM_SMLALTBMI,
    ASM_ARM_SMLALBTMI,
    ASM_ARM_SMLALTTMI,
    ASM_ARM_SMLALBBPL,
    ASM_ARM_SMLALTBPL,
    ASM_ARM_SMLALBTPL,
    ASM_ARM_SMLALTTPL,
    ASM_ARM_SMLALBBVS,
    ASM_ARM_SMLALTBVS,
    ASM_ARM_SMLALBTVS,
    ASM_ARM_SMLALTTVS,
    ASM_ARM_SMLALBBVC,
    ASM_ARM_SMLALTBVC,
    ASM_ARM_SMLALBTVC,
    ASM_ARM_SMLALTTVC,
    ASM_ARM_SMLALBBHI,
    ASM_ARM_SMLALTBHI,
    ASM_ARM_SMLALBTHI,
    ASM_ARM_SMLALTTHI,
    ASM_ARM_SMLALBBLS,
    ASM_ARM_SMLALTBLS,
    ASM_ARM_SMLALBTLS,
    ASM_ARM_SMLALTTLS,
    ASM_ARM_SMLALBBGE,
    ASM_ARM_SMLALTBGE,
    ASM_ARM_SMLALBTGE,
    ASM_ARM_SMLALTTGE,
    ASM_ARM_SMLALBBLT,
    ASM_ARM_SMLALTBLT,
    ASM_ARM_SMLALBTLT,
    ASM_ARM_SMLALTTLT,
    ASM_ARM_SMLALBBGT,
    ASM_ARM_SMLALTBGT,
    ASM_ARM_SMLALBTGT,
    ASM_ARM_SMLALTTGT,
    ASM_ARM_SMLALBBLE,
    ASM_ARM_SMLALTBLE,
    ASM_ARM_SMLALBTLE,
    ASM_ARM_SMLALTTLE,
    ASM_ARM_SMLALBB,
    ASM_ARM_SMLALTB,
    ASM_ARM_SMLALBT,
    ASM_ARM_SMLALTT,

    /* SMLAWY */
    ASM_ARM_SMLAWBEQ,
    ASM_ARM_SMLAWTEQ,
    ASM_ARM_SMLAWBNE,
    ASM_ARM_SMLAWTNE,
    ASM_ARM_SMLAWBCS,
    ASM_ARM_SMLAWTCS,
    ASM_ARM_SMLAWBCC,
    ASM_ARM_SMLAWTCC,
    ASM_ARM_SMLAWBMI,
    ASM_ARM_SMLAWTMI,
    ASM_ARM_SMLAWBPL,
    ASM_ARM_SMLAWTPL,
    ASM_ARM_SMLAWBVS,
    ASM_ARM_SMLAWTVS,
    ASM_ARM_SMLAWBVC,
    ASM_ARM_SMLAWTVC,
    ASM_ARM_SMLAWBHI,
    ASM_ARM_SMLAWTHI,
    ASM_ARM_SMLAWBLS,
    ASM_ARM_SMLAWTLS,
    ASM_ARM_SMLAWBGE,
    ASM_ARM_SMLAWTGE,
    ASM_ARM_SMLAWBLT,
    ASM_ARM_SMLAWTLT,
    ASM_ARM_SMLAWBGT,
    ASM_ARM_SMLAWTGT,
    ASM_ARM_SMLAWBLE,
    ASM_ARM_SMLAWTLE,
    ASM_ARM_SMLAWB,
    ASM_ARM_SMLAWT,

    /* SMLAXY */
    ASM_ARM_SMLABBEQ,
    ASM_ARM_SMLATBEQ,
    ASM_ARM_SMLABTEQ,
    ASM_ARM_SMLATTEQ,
    ASM_ARM_SMLABBNE,
    ASM_ARM_SMLATBNE,
    ASM_ARM_SMLABTNE,
    ASM_ARM_SMLATTNE,
    ASM_ARM_SMLABBCS,
    ASM_ARM_SMLATBCS,
    ASM_ARM_SMLABTCS,
    ASM_ARM_SMLATTCS,
    ASM_ARM_SMLABBCC,
    ASM_ARM_SMLATBCC,
    ASM_ARM_SMLABTCC,
    ASM_ARM_SMLATTCC,
    ASM_ARM_SMLABBMI,
    ASM_ARM_SMLATBMI,
    ASM_ARM_SMLABTMI,
    ASM_ARM_SMLATTMI,
    ASM_ARM_SMLABBPL,
    ASM_ARM_SMLATBPL,
    ASM_ARM_SMLABTPL,
    ASM_ARM_SMLATTPL,
    ASM_ARM_SMLABBVS,
    ASM_ARM_SMLATBVS,
    ASM_ARM_SMLABTVS,
    ASM_ARM_SMLATTVS,
    ASM_ARM_SMLABBVC,
    ASM_ARM_SMLATBVC,
    ASM_ARM_SMLABTVC,
    ASM_ARM_SMLATTVC,
    ASM_ARM_SMLABBHI,
    ASM_ARM_SMLATBHI,
    ASM_ARM_SMLABTHI,
    ASM_ARM_SMLATTHI,
    ASM_ARM_SMLABBLS,
    ASM_ARM_SMLATBLS,
    ASM_ARM_SMLABTLS,
    ASM_ARM_SMLATTLS,
    ASM_ARM_SMLABBGE,
    ASM_ARM_SMLATBGE,
    ASM_ARM_SMLABTGE,
    ASM_ARM_SMLATTGE,
    ASM_ARM_SMLABBLT,
    ASM_ARM_SMLATBLT,
    ASM_ARM_SMLABTLT,
    ASM_ARM_SMLATTLT,
    ASM_ARM_SMLABBGT,
    ASM_ARM_SMLATBGT,
    ASM_ARM_SMLABTGT,
    ASM_ARM_SMLATTGT,
    ASM_ARM_SMLABBLE,
    ASM_ARM_SMLATBLE,
    ASM_ARM_SMLABTLE,
    ASM_ARM_SMLATTLE,
    ASM_ARM_SMLABB,
    ASM_ARM_SMLATB,
    ASM_ARM_SMLABT,
    ASM_ARM_SMLATT,

    /* SMULL */
    ASM_ARM_SMULLEQ,
    ASM_ARM_SMULLEQS,
    ASM_ARM_SMULLNE,
    ASM_ARM_SMULLNES,
    ASM_ARM_SMULLCS,
    ASM_ARM_SMULLCSS,
    ASM_ARM_SMULLCC,
    ASM_ARM_SMULLCCS,
    ASM_ARM_SMULLMI,
    ASM_ARM_SMULLMIS,
    ASM_ARM_SMULLPL,
    ASM_ARM_SMULLPLS,
    ASM_ARM_SMULLVS,
    ASM_ARM_SMULLVSS,
    ASM_ARM_SMULLVC,
    ASM_ARM_SMULLVCS,
    ASM_ARM_SMULLHI,
    ASM_ARM_SMULLHIS,
    ASM_ARM_SMULLLS,
    ASM_ARM_SMULLLSS,
    ASM_ARM_SMULLGE,
    ASM_ARM_SMULLGES,
    ASM_ARM_SMULLLT,
    ASM_ARM_SMULLLTS,
    ASM_ARM_SMULLGT,
    ASM_ARM_SMULLGTS,
    ASM_ARM_SMULLLE,
    ASM_ARM_SMULLLES,
    ASM_ARM_SMULL,
    ASM_ARM_SMULLS,

    /* SMULWY */
    ASM_ARM_SMULWBEQ,
    ASM_ARM_SMULWTEQ,
    ASM_ARM_SMULWBNE,
    ASM_ARM_SMULWTNE,
    ASM_ARM_SMULWBCS,
    ASM_ARM_SMULWTCS,
    ASM_ARM_SMULWBCC,
    ASM_ARM_SMULWTCC,
    ASM_ARM_SMULWBMI,
    ASM_ARM_SMULWTMI,
    ASM_ARM_SMULWBPL,
    ASM_ARM_SMULWTPL,
    ASM_ARM_SMULWBVS,
    ASM_ARM_SMULWTVS,
    ASM_ARM_SMULWBVC,
    ASM_ARM_SMULWTVC,
    ASM_ARM_SMULWBHI,
    ASM_ARM_SMULWTHI,
    ASM_ARM_SMULWBLS,
    ASM_ARM_SMULWTLS,
    ASM_ARM_SMULWBGE,
    ASM_ARM_SMULWTGE,
    ASM_ARM_SMULWBLT,
    ASM_ARM_SMULWTLT,
    ASM_ARM_SMULWBGT,
    ASM_ARM_SMULWTGT,
    ASM_ARM_SMULWBLE,
    ASM_ARM_SMULWTLE,
    ASM_ARM_SMULWB,
    ASM_ARM_SMULWT,

    /* SMULXY */
    ASM_ARM_SMULBBEQ,
    ASM_ARM_SMULTBEQ,
    ASM_ARM_SMULBTEQ,
    ASM_ARM_SMULTTEQ,
    ASM_ARM_SMULBBNE,
    ASM_ARM_SMULTBNE,
    ASM_ARM_SMULBTNE,
    ASM_ARM_SMULTTNE,
    ASM_ARM_SMULBBCS,
    ASM_ARM_SMULTBCS,
    ASM_ARM_SMULBTCS,
    ASM_ARM_SMULTTCS,
    ASM_ARM_SMULBBCC,
    ASM_ARM_SMULTBCC,
    ASM_ARM_SMULBTCC,
    ASM_ARM_SMULTTCC,
    ASM_ARM_SMULBBMI,
    ASM_ARM_SMULTBMI,
    ASM_ARM_SMULBTMI,
    ASM_ARM_SMULTTMI,
    ASM_ARM_SMULBBPL,
    ASM_ARM_SMULTBPL,
    ASM_ARM_SMULBTPL,
    ASM_ARM_SMULTTPL,
    ASM_ARM_SMULBBVS,
    ASM_ARM_SMULTBVS,
    ASM_ARM_SMULBTVS,
    ASM_ARM_SMULTTVS,
    ASM_ARM_SMULBBVC,
    ASM_ARM_SMULTBVC,
    ASM_ARM_SMULBTVC,
    ASM_ARM_SMULTTVC,
    ASM_ARM_SMULBBHI,
    ASM_ARM_SMULTBHI,
    ASM_ARM_SMULBTHI,
    ASM_ARM_SMULTTHI,
    ASM_ARM_SMULBBLS,
    ASM_ARM_SMULTBLS,
    ASM_ARM_SMULBTLS,
    ASM_ARM_SMULTTLS,
    ASM_ARM_SMULBBGE,
    ASM_ARM_SMULTBGE,
    ASM_ARM_SMULBTGE,
    ASM_ARM_SMULTTGE,
    ASM_ARM_SMULBBLT,
    ASM_ARM_SMULTBLT,
    ASM_ARM_SMULBTLT,
    ASM_ARM_SMULTTLT,
    ASM_ARM_SMULBBGT,
    ASM_ARM_SMULTBGT,
    ASM_ARM_SMULBTGT,
    ASM_ARM_SMULTTGT,
    ASM_ARM_SMULBBLE,
    ASM_ARM_SMULTBLE,
    ASM_ARM_SMULBTLE,
    ASM_ARM_SMULTTLE,
    ASM_ARM_SMULBB,
    ASM_ARM_SMULTB,
    ASM_ARM_SMULBT,
    ASM_ARM_SMULTT,

    /* STC/STC2 */
    ASM_ARM_STCEQ,
    ASM_ARM_STCEQL,
    ASM_ARM_STCNE,
    ASM_ARM_STCNEL,
    ASM_ARM_STCCS,
    ASM_ARM_STCCSL,
    ASM_ARM_STCCC,
    ASM_ARM_STCCCL,
    ASM_ARM_STCMI,
    ASM_ARM_STCMIL,
    ASM_ARM_STCPL,
    ASM_ARM_STCPLL,
    ASM_ARM_STCVS,
    ASM_ARM_STCVSL,
    ASM_ARM_STCVC,
    ASM_ARM_STCVCL,
    ASM_ARM_STCHI,
    ASM_ARM_STCHIL,
    ASM_ARM_STCLS,
    ASM_ARM_STCLSL,
    ASM_ARM_STCGE,
    ASM_ARM_STCGEL,
    ASM_ARM_STCLT,
    ASM_ARM_STCLTL,
    ASM_ARM_STCGT,
    ASM_ARM_STCGTL,
    ASM_ARM_STCLE,
    ASM_ARM_STCLEL,
    ASM_ARM_STC,
    ASM_ARM_STCL,
    ASM_ARM_STC2,
    ASM_ARM_STC2L,

    /* STM */
    ASM_ARM_STMEQIA,
    ASM_ARM_STMEQIB,
    ASM_ARM_STMEQDA,
    ASM_ARM_STMEQDB,
    ASM_ARM_STMNEIA,
    ASM_ARM_STMNEIB,
    ASM_ARM_STMNEDA,
    ASM_ARM_STMNEDB,
    ASM_ARM_STMCSIA,
    ASM_ARM_STMCSIB,
    ASM_ARM_STMCSDA,
    ASM_ARM_STMCSDB,
    ASM_ARM_STMCCIA,
    ASM_ARM_STMCCIB,
    ASM_ARM_STMCCDA,
    ASM_ARM_STMCCDB,
    ASM_ARM_STMMIIA,
    ASM_ARM_STMMIIB,
    ASM_ARM_STMMIDA,
    ASM_ARM_STMMIDB,
    ASM_ARM_STMPLIA,
    ASM_ARM_STMPLIB,
    ASM_ARM_STMPLDA,
    ASM_ARM_STMPLDB,
    ASM_ARM_STMVSIA,
    ASM_ARM_STMVSIB,
    ASM_ARM_STMVSDA,
    ASM_ARM_STMVSDB,
    ASM_ARM_STMVCIA,
    ASM_ARM_STMVCIB,
    ASM_ARM_STMVCDA,
    ASM_ARM_STMVCDB,
    ASM_ARM_STMHIIA,
    ASM_ARM_STMHIIB,
    ASM_ARM_STMHIDA,
    ASM_ARM_STMHIDB,
    ASM_ARM_STMLSIA,
    ASM_ARM_STMLSIB,
    ASM_ARM_STMLSDA,
    ASM_ARM_STMLSDB,
    ASM_ARM_STMGEIA,
    ASM_ARM_STMGEIB,
    ASM_ARM_STMGEDA,
    ASM_ARM_STMGEDB,
    ASM_ARM_STMLTIA,
    ASM_ARM_STMLTIB,
    ASM_ARM_STMLTDA,
    ASM_ARM_STMLTDB,
    ASM_ARM_STMGTIA,
    ASM_ARM_STMGTIB,
    ASM_ARM_STMGTDA,
    ASM_ARM_STMGTDB,
    ASM_ARM_STMLEIA,
    ASM_ARM_STMLEIB,
    ASM_ARM_STMLEDA,
    ASM_ARM_STMLEDB,
    ASM_ARM_STMIA,
    ASM_ARM_STMIB,
    ASM_ARM_STMDA,
    ASM_ARM_STMDB,

    /* STR */
    ASM_ARM_STREQ,
    ASM_ARM_STRNE,
    ASM_ARM_STRCS,
    ASM_ARM_STRCC,
    ASM_ARM_STRMI,
    ASM_ARM_STRPL,
    ASM_ARM_STRVS,
    ASM_ARM_STRVC,
    ASM_ARM_STRHI,
    ASM_ARM_STRLS,
    ASM_ARM_STRGE,
    ASM_ARM_STRLT,
    ASM_ARM_STRGT,
    ASM_ARM_STRLE,
    ASM_ARM_STR,

    /* STRB */
    ASM_ARM_STREQB,
    ASM_ARM_STRNEB,
    ASM_ARM_STRCSB,
    ASM_ARM_STRCCB,
    ASM_ARM_STRMIB,
    ASM_ARM_STRPLB,
    ASM_ARM_STRVSB,
    ASM_ARM_STRVCB,
    ASM_ARM_STRHIB,
    ASM_ARM_STRLSB,
    ASM_ARM_STRGEB,
    ASM_ARM_STRLTB,
    ASM_ARM_STRGTB,
    ASM_ARM_STRLEB,
    ASM_ARM_STRB,

    /* STRBT */
    ASM_ARM_STREQBT,
    ASM_ARM_STRNEBT,
    ASM_ARM_STRCSBT,
    ASM_ARM_STRCCBT,
    ASM_ARM_STRMIBT,
    ASM_ARM_STRPLBT,
    ASM_ARM_STRVSBT,
    ASM_ARM_STRVCBT,
    ASM_ARM_STRHIBT,
    ASM_ARM_STRLSBT,
    ASM_ARM_STRGEBT,
    ASM_ARM_STRLTBT,
    ASM_ARM_STRGTBT,
    ASM_ARM_STRLEBT,
    ASM_ARM_STRBT,

    /* STRD */
    ASM_ARM_STREQD,
    ASM_ARM_STRNED,
    ASM_ARM_STRCSD,
    ASM_ARM_STRCCD,
    ASM_ARM_STRMID,
    ASM_ARM_STRPLD,
    ASM_ARM_STRVSD,
    ASM_ARM_STRVCD,
    ASM_ARM_STRHID,
    ASM_ARM_STRLSD,
    ASM_ARM_STRGED,
    ASM_ARM_STRLTD,
    ASM_ARM_STRGTD,
    ASM_ARM_STRLED,
    ASM_ARM_STRD,

    /* STRH */
    ASM_ARM_STREQH,
    ASM_ARM_STRNEH,
    ASM_ARM_STRCSH,
    ASM_ARM_STRCCH,
    ASM_ARM_STRMIH,
    ASM_ARM_STRPLH,
    ASM_ARM_STRVSH,
    ASM_ARM_STRVCH,
    ASM_ARM_STRHIH,
    ASM_ARM_STRLSH,
    ASM_ARM_STRGEH,
    ASM_ARM_STRLTH,
    ASM_ARM_STRGTH,
    ASM_ARM_STRLEH,
    ASM_ARM_STRH,

    /* STRT */
    ASM_ARM_STREQT,
    ASM_ARM_STRNET,
    ASM_ARM_STRCST,
    ASM_ARM_STRCCT,
    ASM_ARM_STRMIT,
    ASM_ARM_STRPLT,
    ASM_ARM_STRVST,
    ASM_ARM_STRVCT,
    ASM_ARM_STRHIT,
    ASM_ARM_STRLST,
    ASM_ARM_STRGET,
    ASM_ARM_STRLTT,
    ASM_ARM_STRGTT,
    ASM_ARM_STRLET,
    ASM_ARM_STRT,

    /* SUB */
    ASM_ARM_SUBEQ,
    ASM_ARM_SUBEQS,
    ASM_ARM_SUBNE,
    ASM_ARM_SUBNES,
    ASM_ARM_SUBCS,
    ASM_ARM_SUBCSS,
    ASM_ARM_SUBCC,
    ASM_ARM_SUBCCS,
    ASM_ARM_SUBMI,
    ASM_ARM_SUBMIS,
    ASM_ARM_SUBPL,
    ASM_ARM_SUBPLS,
    ASM_ARM_SUBVS,
    ASM_ARM_SUBVSS,
    ASM_ARM_SUBVC,
    ASM_ARM_SUBVCS,
    ASM_ARM_SUBHI,
    ASM_ARM_SUBHIS,
    ASM_ARM_SUBLS,
    ASM_ARM_SUBLSS,
    ASM_ARM_SUBGE,
    ASM_ARM_SUBGES,
    ASM_ARM_SUBLT,
    ASM_ARM_SUBLTS,
    ASM_ARM_SUBGT,
    ASM_ARM_SUBGTS,
    ASM_ARM_SUBLE,
    ASM_ARM_SUBLES,
    ASM_ARM_SUB,
    ASM_ARM_SUBS,

    /* SWI */
    ASM_ARM_SWIEQ,
    ASM_ARM_SWINE,
    ASM_ARM_SWICS,
    ASM_ARM_SWICC,
    ASM_ARM_SWIMI,
    ASM_ARM_SWIPL,
    ASM_ARM_SWIVS,
    ASM_ARM_SWIVC,
    ASM_ARM_SWIHI,
    ASM_ARM_SWILS,
    ASM_ARM_SWIGE,
    ASM_ARM_SWILT,
    ASM_ARM_SWIGT,
    ASM_ARM_SWILE,
    ASM_ARM_SWI,

    /* SWP */
    ASM_ARM_SWPEQ,
    ASM_ARM_SWPNE,
    ASM_ARM_SWPCS,
    ASM_ARM_SWPCC,
    ASM_ARM_SWPMI,
    ASM_ARM_SWPPL,
    ASM_ARM_SWPVS,
    ASM_ARM_SWPVC,
    ASM_ARM_SWPHI,
    ASM_ARM_SWPLS,
    ASM_ARM_SWPGE,
    ASM_ARM_SWPLT,
    ASM_ARM_SWPGT,
    ASM_ARM_SWPLE,
    ASM_ARM_SWP,

    /* SWPB */
    ASM_ARM_SWPEQB,
    ASM_ARM_SWPNEB,
    ASM_ARM_SWPCSB,
    ASM_ARM_SWPCCB,
    ASM_ARM_SWPMIB,
    ASM_ARM_SWPPLB,
    ASM_ARM_SWPVSB,
    ASM_ARM_SWPVCB,
    ASM_ARM_SWPHIB,
    ASM_ARM_SWPLSB,
    ASM_ARM_SWPGEB,
    ASM_ARM_SWPLTB,
    ASM_ARM_SWPGTB,
    ASM_ARM_SWPLEB,
    ASM_ARM_SWPB,

    /* TEQ */
    ASM_ARM_TEQEQ,
    ASM_ARM_TEQNE,
    ASM_ARM_TEQCS,
    ASM_ARM_TEQCC,
    ASM_ARM_TEQMI,
    ASM_ARM_TEQPL,
    ASM_ARM_TEQVS,
    ASM_ARM_TEQVC,
    ASM_ARM_TEQHI,
    ASM_ARM_TEQLS,
    ASM_ARM_TEQGE,
    ASM_ARM_TEQLT,
    ASM_ARM_TEQGT,
    ASM_ARM_TEQLE,
    ASM_ARM_TEQ,

    /* TST */
    ASM_ARM_TSTEQ,
    ASM_ARM_TSTNE,
    ASM_ARM_TSTCS,
    ASM_ARM_TSTCC,
    ASM_ARM_TSTMI,
    ASM_ARM_TSTPL,
    ASM_ARM_TSTVS,
    ASM_ARM_TSTVC,
    ASM_ARM_TSTHI,
    ASM_ARM_TSTLS,
    ASM_ARM_TSTGE,
    ASM_ARM_TSTLT,
    ASM_ARM_TSTGT,
    ASM_ARM_TSTLE,
    ASM_ARM_TST,

    /* UMLAL */
    ASM_ARM_UMLALEQ,
    ASM_ARM_UMLALEQS,
    ASM_ARM_UMLALNE,
    ASM_ARM_UMLALNES,
    ASM_ARM_UMLALCS,
    ASM_ARM_UMLALCSS,
    ASM_ARM_UMLALCC,
    ASM_ARM_UMLALCCS,
    ASM_ARM_UMLALMI,
    ASM_ARM_UMLALMIS,
    ASM_ARM_UMLALPL,
    ASM_ARM_UMLALPLS,
    ASM_ARM_UMLALVS,
    ASM_ARM_UMLALVSS,
    ASM_ARM_UMLALVC,
    ASM_ARM_UMLALVCS,
    ASM_ARM_UMLALHI,
    ASM_ARM_UMLALHIS,
    ASM_ARM_UMLALLS,
    ASM_ARM_UMLALLSS,
    ASM_ARM_UMLALGE,
    ASM_ARM_UMLALGES,
    ASM_ARM_UMLALLT,
    ASM_ARM_UMLALLTS,
    ASM_ARM_UMLALGT,
    ASM_ARM_UMLALGTS,
    ASM_ARM_UMLALLE,
    ASM_ARM_UMLALLES,
    ASM_ARM_UMLAL,
    ASM_ARM_UMLALS,

    /* UMULL */
    ASM_ARM_UMULLEQ,
    ASM_ARM_UMULLEQS,
    ASM_ARM_UMULLNE,
    ASM_ARM_UMULLNES,
    ASM_ARM_UMULLCS,
    ASM_ARM_UMULLCSS,
    ASM_ARM_UMULLCC,
    ASM_ARM_UMULLCCS,
    ASM_ARM_UMULLMI,
    ASM_ARM_UMULLMIS,
    ASM_ARM_UMULLPL,
    ASM_ARM_UMULLPLS,
    ASM_ARM_UMULLVS,
    ASM_ARM_UMULLVSS,
    ASM_ARM_UMULLVC,
    ASM_ARM_UMULLVCS,
    ASM_ARM_UMULLHI,
    ASM_ARM_UMULLHIS,
    ASM_ARM_UMULLLS,
    ASM_ARM_UMULLLSS,
    ASM_ARM_UMULLGE,
    ASM_ARM_UMULLGES,
    ASM_ARM_UMULLLT,
    ASM_ARM_UMULLLTS,
    ASM_ARM_UMULLGT,
    ASM_ARM_UMULLGTS,
    ASM_ARM_UMULLLE,
    ASM_ARM_UMULLLES,
    ASM_ARM_UMULL,
    ASM_ARM_UMULLS,

    ASM_ARM_BAD,
  };

extern char	*arm_instr_list[ASM_ARM_BAD + 1];
extern int	arm_dataproc_table[512];
extern int	arm_movpsr_table[32];
extern int	arm_clz_table[16];
extern int	arm_swi_table[16];
extern int	arm_ldst_table[128];
extern int	arm_ldst_misc_table[128];
extern int	arm_ldst_mult_table[128];
extern int	arm_swap_table[32];
extern int	arm_multiply_table[256];
extern int	arm_dsp_multiply_table[256];
extern int	arm_dsp_arith_table[64];
extern int	arm_branch1_table[32];
extern int	arm_branch2_table[32];
extern int	arm_coproc_dataproc_table[16];
extern int	arm_coproc_mov_table[32];
extern int	arm_coproc_mov2_table[32];
extern int	arm_coproc_ldst_table[64];


extern int	arm_cond_flagsread_table[17];
extern int	arm_dataproc_flagswritten_table[16];

#endif /* _LIBASM_ARM_H_ */
