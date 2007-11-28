/**
 * $Id: register.c,v 1.10 2007-11-28 00:21:37 strauss Exp $
 *
 * @file register.c
 * @brief Registration of instruction and operand handlers.
 */

#include <libasm.h>
#include <libasm-int.h>


/**
 * Initialize instruction level vector
 * @param proc Pointer to processor structure
 * @param machine Currently not used.
 * @return 1 on success, 0 on error
 */
int	asm_arch_register(asm_processor *proc, int machine)
{  
  int	to_ret;
  
  LIBASM_PROFILE_FIN();
  if (proc->type == ASM_PROC_IA32) 
    {
      asm_register_ia32(proc);
    }  
  else if (proc->type == ASM_PROC_SPARC) 
    {
      asm_register_sparc();
    }
  else if (proc->type == ASM_PROC_MIPS)
    {
      asm_register_mips();
    }
  //
  // Add your architecture handler here.
  // 
  else
    {
      to_ret = 0;
      goto out;
    }
  to_ret = 1;
 out:
  LIBASM_PROFILE_FOUT(to_ret);
}

/**
 * @brief Create a new operand vector of dimension 1 and size <size>
 * @param vector_name Name of the vector
 * @param size Size of the vector
 * @return 1 on success, or 0 on error.
 */
int	asm_register_operand_create(const char *vector_name, int size)
{
  u_int		*dims;
  char		**dimstr;
  
  dims = malloc(1 * sizeof (u_int));
  if (!dims)
    {
      return 0;
    }
  dimstr = malloc(1 * sizeof (char *));
  if (!dimstr)
    {
      return 0;
    }
  dims[0] = size;
  dimstr[0] = "OPERAND";
  aspect_register_vector((char *)vector_name, asm_operand_fetch_default,
			 dims, dimstr, 1, ASPECT_TYPE_CADDR);
  return (1);
}


/**
 * Generic function to register operand handlers.
 * @param vector_name name of the vector to store the handlers (arch specific)
 * @param operand_type Type of the operand to register
 * @param fcn Function pointer.
 * @return 1 on success, 0 on error.
 */

int asm_register_operand(const char *vector_name, int operand_type, 
                          unsigned long fcn)
{
  vector_t	*vec;
  u_int		dim[1];

  LIBASM_PROFILE_FIN();
  vec = aspect_vector_get((char *)vector_name);
  dim[0] = operand_type;
  aspect_vectors_insert(vec, dim, fcn);
  LIBASM_PROFILE_FOUT(1);
}


/**
 *
 *
 *
 */

int	asm_register_opcode_create(const char *vector_name, int size)
{
  u_int		*dims;
  char		**dimstr;
  
  dims = malloc(1 * sizeof (u_int));
  if (!dims)
    {
      return 0;
    }
  dimstr = malloc(1 * sizeof (char *));
  if (!dimstr)
    {
      return 0;
    }
  dims[0] = size;
  dimstr[0] = "opcode";
  aspect_register_vector((char *) vector_name, asm_operand_fetch_default,
			 dims, dimstr, 1, ASPECT_TYPE_CADDR);  
  return (1);
}

/**
 *
 *
 *
 *
 *
 */

int	asm_register_opcode(const char *vector_name, int opcode,
			    unsigned long fcn)
{
  vector_t	*vec;
  u_int		dim[1];
  int		to_ret;
  
  LIBASM_PROFILE_FIN();
  to_ret = 0;  
  if ((vec = aspect_vector_get((char *)vector_name)) != NULL)
    {
      dim[0] = opcode;
      aspect_vectors_insert(vec, dim, fcn);
      to_ret = 1;
    }
  LIBASM_PROFILE_FOUT(to_ret);  
}
