/*
 *
 * fix and fill
 *          - Adam 'pi3' Zabrocki
 *
 *
 *  Manuel Martin - 2007 
 */

#include <libasm.h>

struct e_mips_instr e_mips_instrs [] = {


/* CPU: Arithmetic Instructions => ALL from MIPS Vol. 2 + Extra 2 */

  {"add"      ,ASM_MIPS_ADD       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_ADD       ,0x0,		asm_mips_add}, 
  {"addi"     ,ASM_MIPS_ADDI      ,MIPS_OPCODE_ADDI          ,0x0                   ,0x0,		asm_mips_addi},
  {"addiu"    ,ASM_MIPS_ADDIU     ,MIPS_OPCODE_ADDIU         ,0x0                   ,0x0,		asm_mips_addiu},
  {"addu"     ,ASM_MIPS_ADDU      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_ADDU      ,0x0,		asm_mips_addu},
  {"clo"      ,ASM_MIPS_CLO       ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_CLO       ,0x0,		asm_mips_clo},
  {"clz"      ,ASM_MIPS_CLZ       ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_CLZ       ,0x0,		asm_mips_clz},
//
  {"dadd"     ,ASM_MIPS_DADD      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DADD      ,0x0,		asm_mips_dadd},
  {"daddi"    ,ASM_MIPS_DADDI     ,MIPS_OPCODE_DADDI         ,0x0                   ,0x0,		asm_mips_daddi},
  {"daddiu"   ,ASM_MIPS_DADDIU    ,MIPS_OPCODE_DADDIU        ,0x0                   ,0x0,		asm_mips_daddiu},
  {"daddu"    ,ASM_MIPS_DADDU     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DADDU     ,0x0,		asm_mips_daddu},
  {"dclo"     ,ASM_MIPS_DCLO      ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_DCLO      ,0x0,		asm_mips_dclo},
  {"dclz"     ,ASM_MIPS_DCLZ      ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_DCLZ      ,0x0,		asm_mips_dclz},
  {"ddiv"     ,ASM_MIPS_DDIV      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DDIV      ,0x0,		asm_mips_ddiv},
  {"ddivu"    ,ASM_MIPS_DDIVU     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DDIVU     ,0x0,		asm_mips_ddivu},
//
  {"div"      ,ASM_MIPS_DIV       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DIV       ,0x0,		asm_mips_div},
  {"divu"     ,ASM_MIPS_DIVU      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DIVU      ,0x0,		asm_mips_divu},
//
  {"dmult"    ,ASM_MIPS_DMULT     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DMULT     ,0x0,		asm_mips_dmult},
  {"dmultu"   ,ASM_MIPS_DMULTU    ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DMULTU    ,0x0,		asm_mips_dmultu},
  {"dsub"     ,ASM_MIPS_DSUB      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSUB      ,0x0,		asm_mips_dsub},
  {"dsubu"    ,ASM_MIPS_DSUBU     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSUBU     ,0x0,		asm_mips_dsubu},
//
  {"madd"     ,ASM_MIPS_MADD      ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_MADD      ,0x0,		asm_mips_madd},
  {"maddu"    ,ASM_MIPS_MADDU     ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_MADDU     ,0x0,		asm_mips_maddu},
  {"msub"     ,ASM_MIPS_MSUB      ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_MSUB      ,0x0,		asm_mips_msub},
  {"msubu"    ,ASM_MIPS_MSUBU     ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_MSUBU     ,0x0,		asm_mips_msubu},
  {"mul"      ,ASM_MIPS_MUL       ,MIPS_OPCODE_SPECIAL2      ,MIPS_OPCODE_MUL       ,0x0,		asm_mips_mul},
  {"mult"     ,ASM_MIPS_MULT      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MULT      ,0x0,		asm_mips_mult},
  {"multu"    ,ASM_MIPS_MULTU     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MULTU     ,0x0,		asm_mips_multu},
  {"slt"      ,ASM_MIPS_SLT       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLT       ,0x0,		asm_mips_slt},
  {"slti"     ,ASM_MIPS_SLTI      ,MIPS_OPCODE_SLTI          ,0x0                   ,0x0,		asm_mips_slti},
  {"sltiu"    ,ASM_MIPS_SLTIU     ,MIPS_OPCODE_SLTIU         ,0x0                   ,0x0,		asm_mips_sltiu},
  {"sltu"     ,ASM_MIPS_SLTU      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLTU      ,0x0,		asm_mips_sltu},
  {"sub"      ,ASM_MIPS_SUB       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SUB       ,0x0,		asm_mips_sub},
  {"subu"     ,ASM_MIPS_SUBU      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SUBU      ,0x0,		asm_mips_subu},
  /* Extra 2: */
  {"seb"      ,ASM_MIPS_SEB       ,MIPS_OPCODE_SPECIAL3      ,MIPS_OPCODE_BSHFL     ,MIPS_OPCODE_SEB,	asm_mips_seb},
  {"seh"      ,ASM_MIPS_SEH       ,MIPS_OPCODE_SPECIAL3      ,MIPS_OPCODE_BSHFL     ,MIPS_OPCODE_SEH,	asm_mips_seh},


/* CPU: Branch and Jump Instructions => ALL from MIPS Vol. 2 + Extra 2 */

  {"b"        ,ASM_MIPS_B         ,MIPS_OPCODE_BEQ           ,0x0                   ,0x0,		asm_mips_b},
  {"bal"      ,ASM_MIPS_BAL       ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BGEZAL    ,0x0,		asm_mips_bal},
  {"beq"      ,ASM_MIPS_BEQ       ,MIPS_OPCODE_BEQ           ,0x0                   ,0x0,		asm_mips_beq},
  {"bgez"     ,ASM_MIPS_BGEZ      ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BGEZ      ,0x0,		asm_mips_bgez},
  {"bgezal"   ,ASM_MIPS_BGEZAL    ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BGEZAL    ,0x0,		asm_mips_bgezal},
  {"bgtz"     ,ASM_MIPS_BGTZ      ,MIPS_OPCODE_BGTZ          ,0x0                   ,0x0,		asm_mips_bgtz},
  {"blez"     ,ASM_MIPS_BLEZ      ,MIPS_OPCODE_BLEZ          ,0x0                   ,0x0,		asm_mips_blez},
  {"bltz"     ,ASM_MIPS_BLTZ      ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BLTZ      ,0x0,		asm_mips_bltz},
  {"bltzal"   ,ASM_MIPS_BLTZAL    ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BLTZAL    ,0x0,		asm_mips_bltzal},
  {"bne"      ,ASM_MIPS_BNE       ,MIPS_OPCODE_BNE           ,0x0                   ,0x0,		asm_mips_bne},
  {"j"        ,ASM_MIPS_J         ,MIPS_OPCODE_J             ,0x0                   ,0x0,		asm_mips_j},
  {"jal"      ,ASM_MIPS_JAL       ,MIPS_OPCODE_JAL           ,0x0                   ,0x0,		asm_mips_jal},
  {"jalr"     ,ASM_MIPS_JALR      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_JALR      ,0x0,		asm_mips_jalr},
  {"jr"       ,ASM_MIPS_JR        ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_JR        ,0x0,		asm_mips_jr},
  /* Extra 2: */
  {"jalr.hb"  ,ASM_MIPS_JALR_HB   ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_JALR      ,0x0,		asm_mips_jalr_hb},
  {"jr.hb"    ,ASM_MIPS_JR_HB     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_JR        ,0x0,		asm_mips_jr_hb},


/* CPU: Instruction Control Instructions => ALL from MIPS Vol. 2 + Extra 1 */

  {"nop"      ,ASM_MIPS_NOP       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLL       ,MIPS_OPCODE_NOP,	asm_mips_nop},
  {"ssnop"    ,ASM_MIPS_SSNOP     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLL       ,MIPS_OPCODE_SSNOP,	asm_mips_ssnop},
  /* Extra 1: */
//  {"ehb"      ,ASM_MIPS_EHB       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLL       ,MIPS_OPCODE_EHB,	asm_mips_ehb},


/* CPU: Load, Store, and Memory Control Instructions => ALL from MIPS Vol. 2 + Extra 1 */

  {"lb"       ,ASM_MIPS_LB        ,MIPS_OPCODE_LB            ,0x0                   ,0x0,		asm_mips_lb},
  {"lbu"      ,ASM_MIPS_LBU       ,MIPS_OPCODE_LBU           ,0x0                   ,0x0,		asm_mips_lbu},
//
  {"ld"       ,ASM_MIPS_LD        ,MIPS_OPCODE_LD            ,0x0                   ,0x0,		asm_mips_ld},
  {"ldl"      ,ASM_MIPS_LDL       ,MIPS_OPCODE_LDL           ,0x0                   ,0x0,		asm_mips_ldl},
  {"ldr"      ,ASM_MIPS_LDR       ,MIPS_OPCODE_LDR           ,0x0                   ,0x0,		asm_mips_ldr},
//
  {"lh"       ,ASM_MIPS_LH        ,MIPS_OPCODE_LH            ,0x0                   ,0x0,		asm_mips_lh},
  {"lhu"      ,ASM_MIPS_LHU       ,MIPS_OPCODE_LHU           ,0x0                   ,0x0,		asm_mips_lhu},
  {"ll"       ,ASM_MIPS_LL        ,MIPS_OPCODE_LL            ,0x0                   ,0x0,		asm_mips_ll},
//
  {"lld"      ,ASM_MIPS_LLD       ,MIPS_OPCODE_LLD           ,0x0                   ,0x0,		asm_mips_lld},
//
  {"lw"       ,ASM_MIPS_LW        ,MIPS_OPCODE_LW            ,0x0                   ,0x0,		asm_mips_lw},
  {"lwl"      ,ASM_MIPS_LWL       ,MIPS_OPCODE_LWL           ,0x0                   ,0x0,		asm_mips_lwl},
  {"lwr"      ,ASM_MIPS_LWR       ,MIPS_OPCODE_LWR           ,0x0                   ,0x0,		asm_mips_lwr},
//
  {"lwu"      ,ASM_MIPS_LWU       ,MIPS_OPCODE_LWU           ,0x0                   ,0x0,		asm_mips_lwu},
//
  {"pref"     ,ASM_MIPS_PREF      ,MIPS_OPCODE_PREF          ,0x0                   ,0x0,		asm_mips_pref},
  {"sb"       ,ASM_MIPS_SB        ,MIPS_OPCODE_SB            ,0x0                   ,0x0,		asm_mips_sb},
  {"sc"       ,ASM_MIPS_SC        ,MIPS_OPCODE_SC            ,0x0                   ,0x0,		asm_mips_sc},
//
  {"scd"      ,ASM_MIPS_SCD       ,MIPS_OPCODE_SCD           ,0x0                   ,0x0,		asm_mips_scd},
  {"sd"       ,ASM_MIPS_SD        ,MIPS_OPCODE_SD            ,0x0                   ,0x0,		asm_mips_sd},
  {"sdl"      ,ASM_MIPS_SDL       ,MIPS_OPCODE_SDL           ,0x0                   ,0x0,		asm_mips_sdl},
  {"sdr"      ,ASM_MIPS_SDR       ,MIPS_OPCODE_SDR           ,0x0                   ,0x0,		asm_mips_sdr},
//
  {"sh"       ,ASM_MIPS_SH        ,MIPS_OPCODE_SH            ,0x0                   ,0x0,		asm_mips_sh},
  {"sw"       ,ASM_MIPS_SW        ,MIPS_OPCODE_SW            ,0x0                   ,0x0,		asm_mips_sw},
  {"swl"      ,ASM_MIPS_SWL       ,MIPS_OPCODE_SWL           ,0x0                   ,0x0,		asm_mips_swl},
  {"swr"      ,ASM_MIPS_SWR       ,MIPS_OPCODE_SWR           ,0x0                   ,0x0,		asm_mips_swr},
  {"sync"     ,ASM_MIPS_SYNC      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SYNC      ,0x0,		asm_mips_sync},
  /* Extra: 1 */
  {"synci"    ,ASM_MIPS_SYNCI     ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_SYNCI     ,0x0,		asm_mips_synci},


/* CPU: Logical Instructions => ALL from MIPS Vol. 2 + Extra 3 */

  {"and"      ,ASM_MIPS_AND       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_AND       ,0x0,		asm_mips_and},
  {"andi"     ,ASM_MIPS_ANDI      ,MIPS_OPCODE_ANDI          ,0x0                   ,0x0,		asm_mips_andi},
  {"lui"      ,ASM_MIPS_LUI       ,MIPS_OPCODE_LUI           ,0x0                   ,0x0,		asm_mips_lui},
  {"nor"      ,ASM_MIPS_NOR       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_NOR       ,0x0,		asm_mips_nor},
  {"or"       ,ASM_MIPS_OR        ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_OR        ,0x0,		asm_mips_or},
  {"ori"      ,ASM_MIPS_ORI       ,MIPS_OPCODE_ORI           ,0x0                   ,0x0,		asm_mips_ori},
  {"xor"      ,ASM_MIPS_XOR       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_XOR       ,0x0,		asm_mips_xor},
  {"xori"     ,ASM_MIPS_XORI      ,MIPS_OPCODE_XORI          ,0x0                   ,0x0,		asm_mips_xori},
  /* Extra: 3 */
  {"ext"      ,ASM_MIPS_EXT       ,MIPS_OPCODE_SPECIAL3      ,MIPS_OPCODE_EXT       ,0x0,		asm_mips_ext},
  {"ins"      ,ASM_MIPS_INS       ,MIPS_OPCODE_SPECIAL3      ,MIPS_OPCODE_INS       ,0x0,		asm_mips_ins},
  {"wsbh"     ,ASM_MIPS_WSBH      ,MIPS_OPCODE_SPECIAL3      ,MIPS_OPCODE_BSHFL     ,MIPS_OPCODE_WSBH,	asm_mips_wsbh},


/* CPU: Move Instructions => ALL from MIPS Vol. 2 */

  {"mfhi"     ,ASM_MIPS_MFHI      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MFHI      ,0x0,		asm_mips_mfhi},
  {"mflo"     ,ASM_MIPS_MFLO      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MFLO      ,0x0,		asm_mips_mflo},
  {"movf"     ,ASM_MIPS_MOVF      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MOVCI     ,MIPS_OPCODE_MOVF,	asm_mips_movf},
  {"movn"     ,ASM_MIPS_MOVN      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MOVN      ,0x0,		asm_mips_movn},
  {"movt"     ,ASM_MIPS_MOVT      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MOVCI     ,MIPS_OPCODE_MOVT,	asm_mips_movt},
  {"movz"     ,ASM_MIPS_MOVZ      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MOVZ      ,0x0,		asm_mips_movz},
  {"mthi"     ,ASM_MIPS_MTHI      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MTHI      ,0x0,		asm_mips_mthi},
  {"mtlo"     ,ASM_MIPS_MTLO      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_MTLO      ,0x0,		asm_mips_mtlo},


/* CPU: Shift Instructions => ALL from MIPS Vol. 2 + Extra 3 */

//

  {"dsll"     ,ASM_MIPS_DSLL      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSLL      ,0x0,		asm_mips_dsll},
  {"dsll32"   ,ASM_MIPS_DSLL32    ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSLL32    ,0x0,		asm_mips_dsll32},
  {"dsllv"    ,ASM_MIPS_DSLLV     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSLLV     ,0x0,		asm_mips_dsllv},
  {"dsra"     ,ASM_MIPS_DSRA      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSRA      ,0x0,		asm_mips_dsra},
  {"dsra32"   ,ASM_MIPS_DSRA32    ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSRA32    ,0x0,		asm_mips_dsra32},
  {"dsrav"    ,ASM_MIPS_DSRAV     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSRAV     ,0x0,		asm_mips_dsrav},
  {"dsrl"     ,ASM_MIPS_DSRL      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSRL      ,0x0,		asm_mips_dsrl},
  {"dsrl32"   ,ASM_MIPS_DSRL32    ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSRL32    ,0x0,		asm_mips_dsrl32},
  {"dsrlv"    ,ASM_MIPS_DSRLV     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_DSRLV     ,0x0,		asm_mips_dsrlv},
//
  {"sll"      ,ASM_MIPS_SLL       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLL       ,0x0,		asm_mips_sll},
  {"sllv"     ,ASM_MIPS_SLLV      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SLLV      ,0x0,		asm_mips_sllv},
  {"sra"      ,ASM_MIPS_SRA       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SRA       ,0x0,		asm_mips_sra},
  {"srav"     ,ASM_MIPS_SRAV      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SRAV      ,0x0,		asm_mips_srav},
  {"srl"      ,ASM_MIPS_SRL       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SRL       ,MIPS_OPCODE_SRL,	asm_mips_srl},
  {"srlv"     ,ASM_MIPS_SRLV      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SRLV      ,MIPS_OPCODE_SRLV,	asm_mips_srlv},
  /* Extra: 3 */
  {"rdhwr"    ,ASM_MIPS_RDHWR     ,MIPS_OPCODE_SPECIAL3      ,MIPS_OPCODE_RDHWR     ,0x0,		asm_mips_rdhwr},
  {"rotr"     ,ASM_MIPS_ROTR      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SRL       ,MIPS_OPCODE_ROTR,	asm_mips_rotr},
  {"rotrv"    ,ASM_MIPS_ROTRV     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SRLV      ,MIPS_OPCODE_ROTRV,	asm_mips_rotrv},


/* CPU: Trap Instructions => ALL from MIPS Vol. 2 */

  {"break"    ,ASM_MIPS_BREAK     ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_BREAK     ,0x0,		asm_mips_break},
  {"syscall"  ,ASM_MIPS_SYSCALL   ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_SYSCALL   ,0x0,		asm_mips_syscall},
  {"teq"      ,ASM_MIPS_TEQ       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_TEQ       ,0x0,		asm_mips_teq},
  {"teqi"     ,ASM_MIPS_TEQI      ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_TEQI      ,0x0,		asm_mips_teqi},
  {"tge"      ,ASM_MIPS_TGE       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_TGE       ,0x0,		asm_mips_tge},
  {"tgei"     ,ASM_MIPS_TGEI      ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_TGEI      ,0x0,		asm_mips_tgei},
  {"tgeiu"    ,ASM_MIPS_TGEIU     ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_TGEIU     ,0x0,		asm_mips_tgeiu},
  {"tgeu"     ,ASM_MIPS_TGEU      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_TGEU      ,0x0,		asm_mips_tgeu},
  {"tlt"      ,ASM_MIPS_TLT       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_TLT       ,0x0,		asm_mips_tlt},
  {"tlti"     ,ASM_MIPS_TLTI      ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_TLTI      ,0x0,		asm_mips_tlti},
  {"tltiu"    ,ASM_MIPS_TLTIU     ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_TLTIU     ,0x0,		asm_mips_tltiu},
  {"tltu"     ,ASM_MIPS_TLTU      ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_TLTU      ,0x0,		asm_mips_tltu},
  {"tne"      ,ASM_MIPS_TNE       ,MIPS_OPCODE_SPECIAL       ,MIPS_OPCODE_TNE       ,0x0,		asm_mips_tne},
  {"tnei"     ,ASM_MIPS_TNEI      ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_TNEI      ,0x0,		asm_mips_tnei},


/* CPU: Obsolete CPU Branch Instructions => ALL from MIPS Vol. 2 */

  {"beql"     ,ASM_MIPS_BEQL      ,MIPS_OPCODE_BEQL          ,0x0                   ,0x0,		asm_mips_beql},
  {"bgezall"  ,ASM_MIPS_BGEZALL   ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BGEZALL   ,0x0,		asm_mips_bgezall},
  {"bgezl"    ,ASM_MIPS_BGEZL     ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BGEZL     ,0x0,		asm_mips_bgezl},
  {"bgtzl"    ,ASM_MIPS_BGTZL     ,MIPS_OPCODE_BGTZL         ,0x0                   ,0x0,		asm_mips_bgtzl},
  {"blezl"    ,ASM_MIPS_BLEZL     ,MIPS_OPCODE_BLEZL         ,0x0                   ,0x0,		asm_mips_blezl},
  {"bltzall"  ,ASM_MIPS_BLTZALL   ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BLTZALL   ,0x0,		asm_mips_bltzall},
  {"bltzl"    ,ASM_MIPS_BLTZL     ,MIPS_OPCODE_REGIMM        ,MIPS_OPCODE_BLTZL     ,0x0,		asm_mips_bltzl},
  {"bnel"     ,ASM_MIPS_BNEL      ,MIPS_OPCODE_BNEL          ,0x0                   ,0x0,		asm_mips_bnel},


/*

// FPU Arithmetic:

   abs.dmt
   add.fmt
   div.fmt
   madd.fmt
   msub.fmt
   mul.fmt
   ...
   ...
*/



/* Coprocessor Branch Instructions - COP2 */

  {"bc2f0"    ,ASM_MIPS_BC2F0     ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_BC        ,0x0,		asm_mips_bc2f},
  {"bc2t0"    ,ASM_MIPS_BC2T0     ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_BC        ,0x1,		asm_mips_bc2t},


/* Coprocessor Execute Instruction - COP2 */

  {"cop2"     ,ASM_MIPS_COP2      ,MIPS_OPCODE_COP2          ,0x1                   ,0x0,		asm_mips_cop2},


/* Coprocessor Load and Store Instructions - COP2 */

  {"ldc2"     ,ASM_MIPS_LDC2      ,MIPS_OPCODE_LDC2          ,0x0                   ,0x0,		asm_mips_ldc2},
  {"lwc2"     ,ASM_MIPS_LWC2      ,MIPS_OPCODE_LWC2          ,0x0                   ,0x0,		asm_mips_lwc2},
  {"sdc2"     ,ASM_MIPS_SDC2      ,MIPS_OPCODE_SDC2          ,0x0                   ,0x0,		asm_mips_sdc2},
  {"swc2"     ,ASM_MIPS_SWC2      ,MIPS_OPCODE_SWC2          ,0x0                   ,0x0,		asm_mips_swc2},


/* Coprocessor Move Instructions - COP2 */

  {"cfc2"     ,ASM_MIPS_CFC2      ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_CF        ,0x0,		asm_mips_cfc2},
  {"ctc2"     ,ASM_MIPS_CTC2      ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_CT        ,0x0,		asm_mips_ctc2},
  {"dmfc2"    ,ASM_MIPS_DMFC2     ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_DMF       ,0x0,		asm_mips_dmfc2},
  {"dmtc2"    ,ASM_MIPS_DMTC2     ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_DMT       ,0x0,		asm_mips_dmtc2},
  {"mfc2"     ,ASM_MIPS_MFC2      ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_MF        ,0x0,		asm_mips_mfc2},
  {"mtc2"     ,ASM_MIPS_MTC2      ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_MT        ,0x0,		asm_mips_mtc2},



/* Obsolute Coprocessor Branch Instructions - COP2 */

  {"bc2fl"    ,ASM_MIPS_BC2FL     ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_BC        ,0x2,		asm_mips_bc2fl},
  {"bc2tl"    ,ASM_MIPS_BC2TL     ,MIPS_OPCODE_COP2          ,MIPS_OPCODE_BC        ,0x3,		asm_mips_bc2tl},

  /*TODO
   * - FPU insns
   * - COP2 insns
   * - privileged insns
   */
  
  /**
   * @todo Replace -1 by NULL to avoid compiler warning.
   */
  {-1,-1,-1,-1,-1,-1}
};

struct e_mips_register e_mips_registers [] = {
   {"zero","r0",ASM_MIPS_REG_ZERO},
   {"at","r1",ASM_MIPS_REG_AT},
   {"v0","r2",ASM_MIPS_REG_V0},
   {"v1","r3",ASM_MIPS_REG_V1},
   {"a0","r4",ASM_MIPS_REG_A0},
   {"a1","r5",ASM_MIPS_REG_A1},
   {"a2","r6",ASM_MIPS_REG_A2},
   {"a3","r7",ASM_MIPS_REG_A3},
   {"t0","r8",ASM_MIPS_REG_T0},
   {"t1","r9",ASM_MIPS_REG_T1},
   {"t2","r10",ASM_MIPS_REG_T2},
   {"t3","r11",ASM_MIPS_REG_T3},
   {"t4","r12",ASM_MIPS_REG_T4},
   {"t5","r13",ASM_MIPS_REG_T5},
   {"t6","r14",ASM_MIPS_REG_T6},
   {"t7","r15",ASM_MIPS_REG_T7},
   {"s0","r16",ASM_MIPS_REG_S0},
   {"s1","r17",ASM_MIPS_REG_S1},
   {"s2","r18",ASM_MIPS_REG_S2},
   {"s3","r19",ASM_MIPS_REG_S3},
   {"s4","r20",ASM_MIPS_REG_S4},
   {"s5","r21",ASM_MIPS_REG_S5},
   {"s6","r22",ASM_MIPS_REG_S6},
   {"s7","r23",ASM_MIPS_REG_S7},
   {"t8","r24",ASM_MIPS_REG_T8},
   {"t9","r25",ASM_MIPS_REG_T9},
   {"k0","r26",ASM_MIPS_REG_K0},
   {"k1","r27",ASM_MIPS_REG_K1},
   {"gp","r28",ASM_MIPS_REG_GP},
   {"sp","r29",ASM_MIPS_REG_SP},
   {"fp","r30",ASM_MIPS_REG_FP},
   {"ra","r31",ASM_MIPS_REG_RA},
   {-1,-1,-1}
};

