/*
** libasm-structs.h for  in /hate/home/hate/code/libasm_current
** 
** Made by #!HATE#@!
** Login   <hate@epita.fr>
** 
** Started on  Tue Jun 14 05:00:05 2005 #!HATE#@!
** Last update Thu Jun 16 04:56:41 2005 #!HATE#@!
*/

/**
 * Operand structure
 */

struct s_asm_op 
{  
  /* pointer to processor structure. - to remove -	*/
  asm_processor *proc;
  /** operand length. (usefull on ia32 only).
   * operands expressed in R byte from ModRM byte have a null size.	*/
  u_int	len;			
  /* pointer to operand in buffer */
  u_char *ptr;
  /* operand type: contain operand type flags */
  u_int	type;
  /* contain operand size flags	*/
  u_int	size;
  /* Operand content flags (ia32 only) */
  u_int	content;
  /* register set: 8/16/32 bits general registers, segment registers ..
   * (ia32 only) */
  int regset;
  /* operand prefix (ia32 only)	*/
  int prefix;  
  /* immediate value extracted from operand	*/
  int imm; 
  /* base register: primary register extracted	*/
  int base_reg;
  /* index register: auxiliary register */
  int index_reg;
  /* address space (sparc only) 
   * 
   * has to be different than ASM_SP_ASI_P for ASM_SP_OTYPE_IMM_ADDRESS
   * operands on "alternate space" instructions so the output can be correct
   */
  int address_space;  
  /* scale factor (ia32 only) */
  unsigned int scale;
};


/**
 * instruction structure. 
 */

struct s_asm_instr 
{
  /* internal processor structure */
  asm_processor	*proc;
  /* instruction id */
  int instr;
  /* instruction type */
  int type;
  /* instruction prefix (ia32 only) */
  int prefix;
  /* Pointer to instruction prefix (ia32 only)	*/
  void *ptr_prefix;  
  /* pointer to instruction buffer */
  u_char *ptr_instr;
  /* annul bit (sparc only) */
  int annul;
  /* prediction bit (sparc only) */
  int prediction;    
  /* number of operand */
  int nb_op;
  /* first operand : destination */
  asm_operand op1;
  /* second operand : source */
  asm_operand op2;
  /* third operand : optional */
  asm_operand op3;
  /* instruction/operands full lengh */
  u_int len;   
};


/**
 * Processor structure.
 * Contains architecture dependant handler.
 */

struct s_asm_processor 
{
  /* handler to resolving function */
  void (*resolve_immediate)(void *, u_int, char *, u_int); 
  /* handler data pointer */
  void *resolve_data;
  /* processor type */
  int type;
  /* array to instruction memonic by instr field of asm_instr */
  char **instr_table;
  /* fetching instruction. points to processor fetching function. */
  int (*fetch)(asm_instr *, u_char *, u_int, asm_processor *proc);
  /* output handler. print instruction in a readable string	*/
  char *(*display_handle)(asm_instr *instr, int addr);
  /* pointer to an internal structure. */
  void *internals;
};
