/*
* @file libui/include/libui.h
** 
** Started on Sun Jan 22 19:04:04 2006 mxatone
** Updated on Tue Mar  6 08:42:17 2007 jfv
**
** $Id$
**
*/

#ifndef __LIBUI_H__
#define __LIBUI_H__

#include <stdio.h>

/* Thanks to GNU we have readline */
#if defined(USE_READLN)
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "revm.h"
#include "libaspect.h"

//extern int	(*rl_getc)(FILE*);

/** 
 * @brief Completion structure 
 */
typedef struct        s_comp
{
#define		      LIBUI_COMPMAX   16
  char		      **cmds[LIBUI_COMPMAX];         /* !< @brief Matchs list for readline */
}		      rlcomp_t;

/* Prototypes for readline functions */
int		readln_init(int mode, char *history);
int		readln_quit(int mode, char *history);
void		readln_ctrl_set(int i, char c);
char            *readln_match(const char *text, int state);
void            readln_completion_install(char mode, char side);
void		readln_completion_commands(hash_t *cmd_hash);
char		**readln_completion(const char* text, int start, int end);
int		readln_column_update();
int             readln_prompt_update(char *ptr, int size);
int             readln_prompt_restore();
char            *readln_input_check();
void            readln_input_log(char *str);
void            readln_install_clearscreen();
void            readln_screen_change(u_short isnew, char promptdisplay);
void            readln_history_dump(char mode, char *history);
void            readln_terminal_prepare(char mode);
void            readln_terminal_unprepare(char mode);
void		readln_ln_handler(char *c);

#endif
