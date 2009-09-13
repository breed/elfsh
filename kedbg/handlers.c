/**
* @file kedbg/handlers.c
** @ingroup kedbg
** @brief Implement kedbg-specific vector handlers.
**
** $Id$
*/
#include "kedbg.h"
#include "interface.h"
#include "kedbg.h"
#include "interface.h"


/**
 * This function emulates the "monitor r cr0" we can send in VMWare to
 * check the cr0 register. However, when the server replies, the first
 * 0 has to be discarded.
 * @ingroup kedbg
 */
static Bool     kedbg_isrealmodewmon(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  char          reply[100];
  char          *ret;
  uint8_t       i;
  static Bool   passed;

  PROFILER_INQ();
  ret = gdbwrap_remotecmd(loc, MONITOREMUL);

  if (strlen(ret) > sizeof(reply) || !strstr(ret, CR0STR))
    {
      if (passed == FALSE)
        printf("If you are in VMWare: Impossible to detect the mode. Upgrade your"
               "VMWare version and use the proc <protected> to switch manually to"
               "protected mode.\n");
      passed = TRUE;
      PROFILER_ERRQ("Impossible to determine CPU mode", FALSE);
    }
  if (gdbwrap_cmdnotsup(loc))
    PROFILER_ROUTQ(FALSE);

  /* We add +1 to discard the first char.*/
  for (i = 0; ret[i] != '\0'; i++)
    reply[i] = (char)gdbwrap_atoh(ret + BYTE_IN_CHAR * i + 1, 2);


  /* Last bit is 0. */
  if (!(gdbwrap_atoh(reply + strlen(reply) - 2, 1) & 0x1))
    {
      kedbgworld.pmode = FALSE;
      PROFILER_ROUTQ(TRUE);
    }
  else
    {
      kedbgworld.pmode = TRUE;
      PROFILER_ROUTQ(FALSE);
    }
}


/**
 * When the monitor doesn't work, we inject directly a "mov %cr0,
 * %eax" and we check the last bit of %eax. We save the state of all
 * the general purpose registers that we restore afterwards.  We
 * inject the code at address 0x500 to be less intrusive. If we cannot
 * access this memory area, it means that we already are un protected
 * mode.
 * @ingroup kedbg
 */
static Bool     kedbg_isrealmodeinject(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  char          code[]="\x0f\x20\xc0";
  gdbwrap_gdbreg32 regs;

  PROFILER_INQ();

  kedbg_writemem(NULL, MEMINJECT, code, strlen(code));

  if (gdbwrap_erroroccured(loc))
    {
      kedbgworld.pmode = TRUE;
      PROFILER_ROUTQ(kedbgworld.pmode);
    }

  memcpy(&regs, &loc->reg32, sizeof(regs));
  kedbg_writereg(offsetof(gdbwrap_gdbreg32, eip) / sizeof(ureg32), MEMINJECT);
  gdbwrap_stepi(loc);
  gdbwrap_readgenreg(loc);
  if (loc->reg32.eax & 0x1)
    kedbgworld.pmode = TRUE;
  else
    kedbgworld.pmode = FALSE;

  memcpy(&loc->reg32, &regs, sizeof(regs));
  gdbwrap_shipallreg(loc);

  PROFILER_ROUTQ(kedbgworld.pmode);
}

/**
 * @ingroup kedbg
 */
Bool           kedbg_isrealmode(void)
{
  static u_char choice = 0;
  static Bool   first_time = TRUE;
  gdbwrap_t     *loc = gdbwrap_current_get();
  Bool          ret;

  PROFILER_INQ();

  /* If we are not running in a VM, we've nothing to do here. */
  if (kedbgworld.state != KEDBG_VM)
    {
      asm_ia32_switch_mode(&world.proc_ia32, INTEL_PROT);
      PROFILER_ROUTQ(FALSE);
    }

  do
    {
      switch (choice)
        {
          case 0:
            ret = kedbg_isrealmodewmon();
            if (gdbwrap_cmdnotsup(loc))
              {
                if (first_time == TRUE)
                  choice++;
                else
                  goto exit;
              }
            break;

          case 1:
            ret = kedbg_isrealmodeinject();
            if (gdbwrap_cmdnotsup(loc))
              {
                if (first_time == TRUE)
                  choice++;
                else
                  goto exit;
              }
            break;

          default:
            fprintf(stderr, "Impossible to determine the mode.\n");
            break;
        }
    } while (gdbwrap_cmdnotsup(loc));

 exit:
  if (kedbgworld.pmode == TRUE)
    asm_ia32_switch_mode(&world.proc_ia32, INTEL_PROT);

  first_time = FALSE;

  PROFILER_ROUTQ(ret);
}

/**
 * @ingroup kedbg
 */
void            kedbg_resetstep(void)
{
  PROFILER_INQ();
  e2dbgworld.curthread->step = FALSE;

  PROFILER_OUTQ();
}


/**
 * @ingroup kedbg
 */
void            kedbg_setstep(void)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  e2dbgworld.curthread->step = TRUE;

  PROFILER_OUTQ();
}

/**
 * @ingroup kedbg
 */
void		*kedbg_bt_ia32(void *frame)
{
  la32          ptr = 0;

  PROFILER_INQ();

  if (kedbg_isrealmode())
    kedbg_readmema(NULL,(eresi_Addr)(uintptr_t)frame, &ptr, WORD_IN_BYTE);
  else
    kedbg_readmema(NULL,(eresi_Addr)(uintptr_t)frame, &ptr, DWORD_IN_BYTE);

  PROFILER_ROUTQ((void *)ptr);
}

/**
 * @ingroup kedbg
 */
eresi_Addr	*kedbg_getfp(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  PROFILER_ROUTQ((eresi_Addr *)loc->reg32.ebp);
}

/**
 * @ingroup kedbg
 */
void            *kedbg_getret_ia32(void *frame)
{
  la32          ptr = 0;

  PROFILER_INQ();
  if (kedbg_isrealmode())
    kedbg_readmema(NULL, (la32)((la32 *)frame + 1), &ptr, WORD_IN_BYTE);
  else
    kedbg_readmema(NULL, (la32)((la32 *)frame + 1), &ptr, DWORD_IN_BYTE);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (void *)ptr);
}

/**
 * @ingroup kedbg
 */
static int      kedbg_simplesetbp(elfshobj_t *f, elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  NOT_USED(f);
  gdbwrap_simplesetbp(loc, bp->addr);

  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
static int      kedbg_setbpwint3(elfshobj_t *f, elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  NOT_USED(f);
  gdbwrap_setbp(loc, bp->addr, bp->savedinstr);

  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
int             kedbg_setbp(elfshobj_t *f, elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  static u_char choice ;
  static Bool   first_time = TRUE;

  PROFILER_INQ();
  do
    {
      switch (choice)
        {
          case 0:
            kedbg_simplesetbp(f, bp);
            kedbgworld.offset = 0;
            if (gdbwrap_cmdnotsup(loc))
              {
                if (first_time == TRUE)
                  choice++;
                else
                  goto exit;
              }
            break;

          case 1:
            kedbg_setbpwint3(f, bp);
            kedbgworld.offset = 1;
            if (gdbwrap_cmdnotsup(loc))
              {
                if (first_time == TRUE)
                  choice++;
                else
                  goto exit;
              }
            break;

          default:
            /* Should not occur */
            PROFILER_ERRQ("Bp not supported", -1);
        }
    } while (gdbwrap_cmdnotsup(loc));

 exit:
  first_time = FALSE;

  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
static int      kedbg_delbpwint3(elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  gdbwrap_delbp(loc, bp->addr, bp->savedinstr);

  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
static int      kedbg_simpledelbp(elfshbp_t *bp)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  gdbwrap_simpledelbp(loc, bp->addr);

  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
int             kedbg_delbp(elfshbp_t *bp)
{
  PROFILER_INQ();

  if (!kedbgworld.offset)
    kedbg_simpledelbp(bp);
  else if (kedbgworld.offset == 1)
    kedbg_delbpwint3(bp);
  else
    ASSERT(FALSE);

  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
void            kedbg_print_reg(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_gdbreg32 *reg;

  PROFILER_INQ();
  reg = &loc->reg32;
  e2dbg_register_dump("EAX", reg->eax);
  e2dbg_register_dump("EBX", reg->ebx);
  e2dbg_register_dump("ECX", reg->ecx);
  e2dbg_register_dump("EDX", reg->edx);
  e2dbg_register_dump("ESI", reg->esi);
  e2dbg_register_dump("EDI", reg->edi);
  e2dbg_register_dump("ESP", reg->esp);
  e2dbg_register_dump("EBP", reg->ebp);
  e2dbg_register_dump("EIP", reg->eip);
  e2dbg_register_dump("EFLAGS", reg->eflags);
  e2dbg_register_dump("CS", reg->cs);
  e2dbg_register_dump("DS", reg->ds);
  e2dbg_register_dump("SS", reg->ss);
  e2dbg_register_dump("ES", reg->es);
  e2dbg_register_dump("FS", reg->fs);
  e2dbg_register_dump("GS", reg->gs);

  PROFILER_OUTQ();
}

/**
 * @ingroup kedbg
 */
void            kedbg_sigint(int sig)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  NOT_USED(sig);
  gdbwrap_ctrl_c(loc);
  fflush(stdout);
  kedbgworld.interrupted = TRUE;
  PROFILER_OUTQ();
}


/**
 * Reads the content of the memory and returns it as binary.
 * @ingroup kedbg
 */
void            *kedbg_readmema(elfshobj_t *file, eresi_Addr addr,
                                void *buf, u_int size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  char          *ret;
  char          *charbuf = buf;
  u_int         i;

  PROFILER_INQ();
  NOT_USED(file);

  if (size)
    {
      if (buf == NULL)
        PROFILER_ERRQ("buf is NULL !", buf);
      ASSERT(buf != NULL);
      ret = gdbwrap_readmem(loc, addr, size);

      /* gdbserver sends a string, we need to convert it. Note that 2
         characters = 1 real Byte.*/
      for (i = 0; i < size; i++)
        charbuf[i] = (u_char) gdbwrap_atoh(ret + 2 * i, 2);
    }

  PROFILER_ROUTQ(charbuf);
}

/**
 * @ingroup kedbg
 */
void            *kedbg_readmem(elfshsect_t *sect)
{
  void *ptr;

  PROFILER_INQ();
  if(!elfsh_get_section_writableflag(sect->shdr) ||
     !elfsh_get_section_allocflag(sect->shdr))
    ptr = sect->data;
  else
    {
      ptr = malloc(sect->shdr->sh_size);
      kedbg_readmema(sect->parent, sect->shdr->sh_addr, ptr,
                     sect->shdr->sh_size);
    }

  PROFILER_ROUTQ(ptr);
}

/**
 * @ingroup kedbg
 */
int             kedbg_writemem(elfshobj_t *file, eresi_Addr addr, void *data,
                               u_int size)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  NOT_USED(file);
  PROFILER_INQ();
  gdbwrap_writemem(loc, addr, data, size);
  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
eresi_Addr      *kedbg_getpc_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  /* First update all the reg. */
  gdbwrap_readgenreg(loc);
  PROFILER_ROUTQ((eresi_Addr *)&loc->reg32.eip);
}


/**
 * We first sync the registers with the server, then we write the new
 * set ones. They'll be sent to the server when we'll do a "cont".
 * @ingroup kedbg
 */
void            kedbg_set_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_gdbreg32   *reg;

  PROFILER_INQ();
  reg =  gdbwrap_readgenreg(loc);
  if (reg != NULL)
    {

      E2DBG_SETREG(E2DBG_EAX_VAR, reg->eax);
      E2DBG_SETREG(E2DBG_EBX_VAR, reg->ebx);
      E2DBG_SETREG(E2DBG_ECX_VAR, reg->ecx);
      E2DBG_SETREG(E2DBG_EDX_VAR, reg->edx);
      E2DBG_SETREG(E2DBG_ESI_VAR, reg->esi);
      E2DBG_SETREG(E2DBG_EDI_VAR, reg->edi);
      E2DBG_SETREG(E2DBG_SP_VAR,  reg->esp);
      E2DBG_SETREG(E2DBG_FP_VAR,  reg->ebp);
      E2DBG_SETREG(E2DBG_PC_VAR,  reg->eip);
    }
  PROFILER_OUTQ();
}

/**
 * @ingroup kedbg
 */
void            kedbg_get_regvars_ia32(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  gdbwrap_readgenreg(loc);


  E2DBG_GETREG(E2DBG_EAX_VAR, loc->reg32.eax);
  E2DBG_GETREG(E2DBG_EBX_VAR, loc->reg32.ebx);
  E2DBG_GETREG(E2DBG_ECX_VAR, loc->reg32.ecx);
  E2DBG_GETREG(E2DBG_EDX_VAR, loc->reg32.edx);
  E2DBG_GETREG(E2DBG_ESI_VAR, loc->reg32.esi);
  E2DBG_GETREG(E2DBG_EDI_VAR, loc->reg32.edi);
  E2DBG_GETREG(E2DBG_SP_VAR,  loc->reg32.esp);
  //E2DBG_GETREG(E2DBG_SSP_VAR, loc->reg32.eax);
  E2DBG_GETREG(E2DBG_FP_VAR,  loc->reg32.ebp);
  E2DBG_GETREG(E2DBG_PC_VAR,  loc->reg32.eip);
  PROFILER_OUTQ();
}

/**
 * @ingroup kedbg
 */
void            kedbg_shipallreg(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  static Bool   activated  = TRUE;

  if (activated)
    {
      gdbwrap_shipallreg(loc);
      if (gdbwrap_erroroccured(loc))
        {
          fprintf(stderr, "Desactivating the set registers"
                  "(not supported by the server).\n");
          activated = FALSE;
        }
    }
}

/**
 * @ingroup kedbg
 */
int             kedbg_writereg(ureg32 regNum, la32 val)
{
  gdbwrap_t     *loc = gdbwrap_current_get();

  PROFILER_INQ();
  gdbwrap_writereg(loc, regNum, val);
  PROFILER_ROUTQ(0);
}

/**
 * @ingroup kedbg
 */
eresi_Addr      kedbg_realmode_memalloc(u_int size, u_int prot)
{
  char          *ret = alloca(size + 1);
  eresi_Addr    addr = 0x504;
  u_int         i;
  u_int         result = 0;

  NOT_USED(prot);
  PROFILER_INQ();
  do
    {
      kedbg_readmema(NULL, addr, ret, size);
      for (i = 0; i < size; i++)
        result += ret[i];
      if (result)
        addr += size;
    } while (result && addr < 0xffff0);

  if (!result)
    PROFILER_ROUTQ(addr);
  else
    PROFILER_ROUTQ(ELFSH_INVALID_ADDR);
}
