#include "libe2dbg.h"
#include "libasm-int.h"
#include "gdbwrapper.h"
#include "interface.h"

#define  ERROR_CONNECTION  "problem when connecting... exiting\n"
#define  USAGE             "Usage: ./netgdb <[server]:port> <file>\n"
#define  KEDBGNAME         "kedbg"
/* The commands are for internal debuging purpose. */
#define  COM1              "youpla"
#define  HELPCOM1          "Not much to say..."

#define  CMD_IVT           "ivt"
#define  CMD_HOOKIVT       "ivthook"

#define  HLP_IVT           "Displays the IVT of the remote VM"
#define  HLP_HOOKIVT       "Adds breakpoints to all addresses defined in the IVT"

#define  BPCODE            0xcc
#define  MEMINJECT         0x500


/* Debug flags */
#define __DEBUG_KEDBG__	   FALSE

#if __DEBUG_KEDBG__
#define DEBUGMSG(_command)				\
   do							\
      {							\
         _command;					\
      } while(0)
#else
#define DEBUGMSG(_command)				
#endif

typedef struct 
{
  /* Offset for the bp (see cmd_kedbgcont). */
  u_char offset;
  Bool   interrupted;
  Bool   run_in_vm;
  Bool   pmode;
} kedbgworld_t;

extern          kedbgworld_t kedbgworld;
void            kedbg_register_command(void);
void            kedbg_register_vector(void);
int             cmd_linkmap(void);
int             e2dbg_linkmap_load(char *name);
int             cmd_com1(void);
int             cmd_kedbgcont(void);
int             cmd_kedbgstep(void);
int             cmd_kedbgquit(void);
int             cmd_kedbgdisasm(void);
int             cmd_kedbghookivt(void);
int             cmd_kedbgitrace(void);
int             cmd_kedbgproc(void);
int             cmd_kedbggraph(void);
