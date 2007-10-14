/*
** $Id: output_ia32.c,v 1.12 2007-10-14 00:01:41 heroine Exp $
** 
** Author  : <sk at devhell dot org>
** Started : Xxx Xxx xx xx:xx:xx 2002
** Updated : Thu Mar 11 00:40:31 2004
*/

/*
  Sat Jul 13 00:15:51 2002
  removed a bug in scalar output.

*/

#include <libasm.h>
#include <libasm-int.h>

/**
 * Default resolving handler for virtual address
 * 
 */

void	asm_resolve_ia32(void *d, u_int val, char *buf, u_int len)
{
  snprintf(buf, len, "0x%x",  val);
}

/**
 * Instruction label handler.
 * Return ia32 instruction label
 *
 *
 */

char	*asm_get_instr_name(asm_instr *i) {
  return (i->proc->instr_table[i->instr]);
}

void	output_instr(asm_instr *instr) {
  
  printf("%10s  ", instr->proc->instr_table[instr->instr]);
  if (instr->op[0].type) {
    switch(instr->op[0].content) {
      
    }
  } /* !instr->op1 */ else {
    
  }
  /*
  printf("\t;; len: %5u   ", instr->len);
  if (instr->type & IS_MEM_WRITE)
    printf("MW : Y ");
  else
    printf("MW : N ");

  if (instr->type & IS_MEM_READ)
    printf("MR : Y   ");
  else
    printf("MR : N   ");
  if (instr->type & IS_CALL)
    printf("CALL : Y    ");
  else 
    printf("CALL : N    ");
  if (instr->type & IS_JMP)
    printf("JMP : Y  ");
  else
    printf("JMP : N  ");
  if (instr->type & IS_COND_BRANCH)
    printf("CONDBR : Y  "); 
  else
    printf("CONDBR : N   ");
  */
  puts("");
    
}

/**
 * Return register ascii string
 * @param r Register
 * @param regset Register Set (32 bits registers, 16 bits, control ...)
 */  

char *get_reg_intel(int r, int regset) 
{
  char	*rsub[8] ={ "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
  char	*r16[8] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
  char	*r32[8] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
  char	*rseg[8] = { "es", "cs", "ss", "ds", "fs", "gs", "#@!", "#@!" };
  char	*rmm[8] = { "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"};
  char	*rxmm[8] = {"xmm0", "xmm1", "xmm2","xmm3", "xmm4", "xmm5","xmm6", "xmm7"};
  char	*rcr[8] = { "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7"};
  char	*rdr[8] = { "db0", "db1", "db2", "db3", "db4", "db5", "db6", "db7"};
  char	*rdefault[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
  
  if (r >= 0 && r < 8)
    switch(regset) {
    case ASM_REGSET_R8:
      return (rsub[r]);
    case ASM_REGSET_R16:
      return (r16[r]);
    case ASM_REGSET_R32:
      return (r32[r]);
    case ASM_REGSET_XMM:
      return (rxmm[r]);
    case ASM_REGSET_CREG:
      return (rcr[r]);
    case ASM_REGSET_DREG:
      return (rdr[r]);
    case ASM_REGSET_MM:
      return (rmm[r]);
    case ASM_REGSET_SREG:
      return (rseg[r]);
    default:
      return (rdefault[r]);
    }
  return ("\?\?\?");
}


/**
 * Dump an operand output in att syntax to a buffer.
 * @param instr Pointer to instruction structure
 * @param num Number of operand to dump
 * @param addr Virtual address of instruction
 * @param bufptr Buffer to dump operand to
 * @bug 2007-06-16 : Raise a segfault when used lonely
 */

void	att_dump_operand(asm_instr *ins, int num, unsigned int addr,
			 void *bufptr) 
{
  char	resolved[256];
  int	baser;
  int	indexr;
  int	scale;
  int	imm;
  asm_operand *op;
  char	*buffer;

  op = 0;
  buffer = bufptr;

  baser = indexr = scale = imm = 0;

  switch(num)
    {
    case 1:
      op = &ins->op[0];
      break;
    case 2:
      op = &ins->op[1];
      break;
    case 3:
      op = &ins->op[2];
      break;
    }


  // if (op->content & ASM_OP_VALUE)
    asm_operand_get_immediate(ins, num, addr, &imm);
    // if (op->content & ASM_OP_BASE)
    asm_operand_get_basereg(ins, num, addr, &baser);
    // if (op->content & ASM_OP_INDEX)
    asm_operand_get_indexreg(ins, num, addr, &indexr);
    // if (op->content & ASM_OP_SCALE)
    asm_operand_get_scale(ins, num, addr, &scale);
  
  if (ASM_OP_ADDRESS & op->content) {
    if (ASM_OP_REFERENCE & op->content) 
      ins->proc->resolve_immediate(ins->proc->resolve_data, 
					     imm, resolved, 256);
    else if (ASM_OP_VALUE & op->content)
      ins->proc->resolve_immediate(ins->proc->resolve_data, 
					     imm + addr + ins->len, resolved, 256);
    else
      ins->proc->resolve_immediate(ins->proc->resolve_data, 
					     imm + addr + ins->len, resolved, 256);
  } else
    ins->proc->resolve_immediate(ins->proc->resolve_data,
					   imm, resolved, 256);
  
  switch(op->content & ~ASM_OP_FIXED) {
  case ASM_OP_BASE|ASM_OP_ADDRESS:
    sprintf(buffer, "*%%%s", 
	    get_reg_intel(baser, op->regset));
    break;
  case ASM_OP_BASE:
    sprintf(buffer, "%%%s", 
	    get_reg_intel(baser, op->regset));
    break;
    /*
      case OP_SUBREG: 
      sprintf(buffer, "%%%s",  
      get_subreg_intel(baser)); 
      break;
      case OP_SEGREG:
      break;
    */
  case ASM_OP_VALUE:
    sprintf(buffer, "$%s", 
	    resolved);
    break;
  case ASM_OP_VALUE | ASM_OP_ADDRESS:
    sprintf(buffer, "%s", 
	    resolved);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE:
    sprintf(buffer, "%s", 
	    resolved);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_ADDRESS:
    sprintf(buffer, "*%s", 
	    resolved);
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE:
    sprintf(buffer, "(%%%s)", 
	    get_reg_intel(baser, op->regset));
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_ADDRESS:
    sprintf(buffer, "*(%%%s)", 
	    get_reg_intel(baser, op->regset));
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE:
    sprintf(buffer, "%s(%%%s)", 
	    resolved
	    , get_reg_intel(baser, op->regset));
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_ADDRESS:
    sprintf(buffer, "*%s(%%%s)", 
	    resolved
	    , get_reg_intel(baser, op->regset));
    break;
  case 
    ASM_OP_REFERENCE | ASM_OP_ADDRESS | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE:
      sprintf(buffer, "*(%%%s,%%%s,%d)",
	      get_reg_intel(baser, op->regset),
	      get_reg_intel(indexr, op->regset), 
	      scale);
  break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_SCALE:
    sprintf(buffer, "(%%%s,%d)", 
	    get_reg_intel(baser, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE:
    sprintf(buffer, "(%%%s,%%%s,%d)",
	    get_reg_intel(baser, op->regset),
	    get_reg_intel(indexr, op->regset), 
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE | ASM_OP_BASE | ASM_OP_SCALE:
    sprintf(buffer, "%s(%%%s,%d)",
	    resolved,
	    get_reg_intel(baser, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE | ASM_OP_VALUE:
    sprintf(buffer, "%s(%%%s,%%%s,%d)", 
	    resolved, 
	    get_reg_intel(baser, op->regset), 
	    get_reg_intel(indexr, op->regset), 
	    scale);
    break;
    case ASM_OP_REFERENCE | ASM_OP_BASE | ASM_OP_INDEX | ASM_OP_SCALE | 
      ASM_OP_VALUE | ASM_OP_ADDRESS:
	sprintf(buffer, "*%s(%%%s,%%%s,%d)", 
		resolved, 
		get_reg_intel(baser, op->regset), 
		get_reg_intel(indexr, op->regset), 
		scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_INDEX | ASM_OP_VALUE | ASM_OP_SCALE |ASM_OP_ADDRESS:
    sprintf(buffer, "*%s(,%%%s,%d)",
	    resolved,
	    get_reg_intel(indexr, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_VALUE  | ASM_OP_INDEX | ASM_OP_SCALE:
    sprintf(buffer, "%s(,%%%s,%d)",
	    resolved,
	    get_reg_intel(indexr, op->regset),
	    scale);
    break;
  case ASM_OP_REFERENCE | ASM_OP_INDEX | ASM_OP_SCALE:
    sprintf(buffer, "(,%%%s,%d)",
	    get_reg_intel(indexr, op->regset),
	    scale);
    break;
  case ASM_OP_FPU | ASM_OP_BASE:
    strcat(buffer, "%st");
    break;
  case ASM_OP_FPU | ASM_OP_BASE | ASM_OP_SCALE:
    sprintf(buffer, "%%st(%d)", scale);
    break;
    
  case 0:
    break;
  default:
    sprintf(buffer, "(...)");
  }
}

/**
 *
 *
 *
 */

int	asm_operand_get_att(asm_instr *ins, int num, int opt, void *valptr)
{
  att_dump_operand(ins, num, opt, valptr);
  return (1);
}




/**
 * @brief Return at&t ascii representation of an instruction
 * @param instr Pointer to an instruction structure.
 * @param addr Address of the instruction. May be used to compute offset of branch.
 * @return A Pointer to a static buffer containing ascii instruction
 */

char	*asm_ia32_display_instr_att(asm_instr *instr, 
			       int addr) {
  static char	buffer[1024];
  int		len;

  if (!instr)
    return (0);

  memset(buffer, 0, 1024);
  
  if (instr->prefix & ASM_PREFIX_LOCK)
    strcat(buffer, "lock ");
  if (instr->prefix & ASM_PREFIX_REP)
    strcat(buffer, "repz ");
  if (instr->prefix & ASM_PREFIX_REPNE)
    strcat(buffer, "repnz ");
  
  if (instr->instr >= 0 && instr->instr <= ASM_BAD)
    { 
      if (instr->proc->instr_table[instr->instr] != NULL)
	sprintf(buffer + strlen(buffer), "%s", instr->proc->instr_table[instr->instr]);
      else
	sprintf(buffer + strlen(buffer), "missing");
    }  
else 
    {
      sprintf(buffer + strlen(buffer), "out_of_range");
      return (buffer);
    }
  
  
  if (instr->op[0].type) {
    //instr->op[0].proc = instr->proc;

    /* Add spaces */
    len = strlen(buffer);
    while(len++ < (int)config_get_data(ASM_CONFIG_ATT_MARGIN_FLAG))
      strcat(buffer, " ");

    if (instr->op[2].type) {
      //instr->op[2].proc = instr->proc;
      asm_operand_get_att(instr, 3, addr, buffer + strlen(buffer));
      /*
      att_dump_operand(buffer + strlen(buffer), &instr->op[2],
		       addr + instr->len);
      */    
      strcat(buffer, ",");
    }
 
    if (instr->op[1].type) {
      //instr->op[1].proc = instr->proc;
      switch(instr->op[1].prefix & ASM_PREFIX_SEG) {
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      }
      asm_operand_get_att(instr, 2, addr, buffer + strlen(buffer));
      /*
      att_dump_operand(buffer + strlen(buffer), &instr->op[1], 
		       addr + instr->len);
      */
      strcat(buffer, ",");
    }
      switch(instr->op[0].prefix & ASM_PREFIX_SEG) {
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      }
      
      switch (instr->prefix & ASM_PREFIX_MASK) {
      case ASM_PREFIX_CS:
	strcat(buffer, "%cs:");
	break;
      case ASM_PREFIX_ES:
	strcat(buffer, "%es:");
	break;
      case ASM_PREFIX_DS:
	strcat(buffer, "%ds:");
	break;
      case ASM_PREFIX_GS:
	strcat(buffer, "%gs:");
	break;
      case ASM_PREFIX_FS:
	strcat(buffer, "%fs:");
	break;
      case ASM_PREFIX_SS:
	strcat(buffer, "%ss:");
	break;
      }
      asm_operand_get_att(instr, 1, addr, buffer + strlen(buffer));
      /*
      att_dump_operand(buffer + strlen(buffer), &instr->op[0], 
		       addr + instr->len);
      */
  }
  
  return(buffer);
}

