

#include <libasm.h>
#include <stdio.h>

void	test_sparc();
void	test_ia32();
void	test_mips();
void	test_arm();

int main()
{
  test_sparc();
  test_mips();
  test_ia32();
  test_arm();
}

void	test_sparc()
{
  asm_processor	proc;

  asm_init_arch(&proc, ASM_PROC_SPARC);
}

void	test_mips()
{
  asm_processor	proc;

  asm_init_arch(&proc, ASM_PROC_MIPS);
}

void	test_ia32()
{
  asm_processor proc;
  
  asm_init_arch(&proc, ASM_PROC_IA32);
}

void	test_arm()
{
  asm_processor proc;
  
  asm_init_arch(&proc, ASM_PROC_ARM);
}
