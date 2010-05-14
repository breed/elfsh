/**
 * @file libmjollnir/src/init.c
 * @brief Initialization functions
 * @ingroup libmjollnir
 *
 * (C) 2006-2010 Eresi Project
 */

#include "libmjollnir.h"

/**
 * @brief Initialize the world for libmjollnir 
 * @param sess The session structure
 * @ingroup libmjollnir
 */
int		mjr_init_session(mjrsession_t *sess) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_init(&sess->ctx, "mjrcontexts", mjrHashSmall, ASPECT_TYPE_UNKNOW);

  /* register configurable parameters */
  config_add_item(MJR_CONFIG_BLOC_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_BLOC_PREFIX);
  config_add_item(MJR_CONFIG_BLOC_POSTFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_BLOC_POSTFIX);
  config_add_item(MJR_CONFIG_FUNC_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_FUNC_PREFIX);
  config_add_item(MJR_CONFIG_LINK_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_LINK_PREFIX);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Change the current analysis context
 * @param sess The session structure
 * @param name The name of the session
 * @ingroup libmjollnir
 */
int		mjr_set_current_context(mjrsession_t *sess, char *name) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if ((sess->cur = hash_get(&sess->ctx, name)) == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Create a new analysis context 
 * @param sess The session structure
 * @param ctx The context structure
 * @ingroup libmjollnir
 */
int		mjr_add_context(mjrsession_t *sess, mjrcontext_t *ctx) 
{
 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 hash_add(&sess->ctx, ctx->obj->name, ctx);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Delete an analysis context 
 * @param sess The session structure
 * @param name Object Name
 * @ingroup libmjollnir
 */
int		mjr_del_context(mjrsession_t *sess, char *name) 
{
  mjrcontext_t *ctx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  ctx = hash_get(&sess->ctx, name);
  elist_destroy(ctx->func_stack);

  /* Check if current context is the same we want to remove
     if yes set cur to NULL
  */
  if (sess && sess->cur && (!strcmp(sess->cur->obj->name,
                                    name)))
    {
      sess->cur = NULL;
    }

  hash_del(&sess->ctx, name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Create a context and set it as the current one 
 * @param sess The session strucutre
 * @param obj elfshell obj
 * @ingroup libmjollnir
 */
int		mjr_create_context_as_current(mjrsession_t *sess, elfshobj_t *obj) 
{
 mjrcontext_t	*ctx;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 ctx = mjr_create_context(obj);
 mjr_add_context(sess, ctx);
 mjr_set_current_context(sess, obj->name);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


/**
 * @brief Create a new context 
 * @param obj elf shell object
 * @ingroup libmjollnir
 */
mjrcontext_t	*mjr_create_context(elfshobj_t *obj) 
{
 mjrcontext_t	*ctx;
 char           *lname;
 char		buff[BUFSIZ];

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 XALLOC(__FILE__, __FUNCTION__, __LINE__, lname, BSIZE_SMALL, NULL);
 XALLOC(__FILE__, __FUNCTION__, __LINE__, ctx, sizeof(mjrcontext_t), NULL);
 bzero(ctx, sizeof(mjrcontext_t));
 ctx->obj = obj;

 snprintf(buff, sizeof(buff), "%s_functions", obj->name);
 hash_init(&ctx->funchash , strdup(buff), mjrHashVerySmall, ASPECT_TYPE_FUNC);
 snprintf(buff, sizeof(buff), "%s_blocks", obj->name);
 hash_init(&ctx->blkhash  , strdup(buff) , mjrHashMedium, ASPECT_TYPE_BLOC);
 snprintf(buff, sizeof(buff), "%s_links", obj->name);
 hash_init(&ctx->linkhash  , strdup(buff), mjrHashMedium, ASPECT_TYPE_LINK);

 snprintf(buff, sizeof(buff), "%s_gotos", obj->name);
 hash_init(&ctx->goto_hash, strdup(buff), mjrHashVerySmall, ASPECT_TYPE_STR);

 XALLOC(__FILE__, __FUNCTION__, __LINE__, ctx->func_stack, sizeof(list_t), NULL);

 snprintf(lname, BSIZE_SMALL, "%s"AFMT, (char *) "funcpath_", obj->id);
 elist_init(ctx->func_stack, lname, ASPECT_TYPE_FUNC);
 ctx->cntnrs_size = MJR_CNTNRS_INCREMENT;
 ctx->next_id = 1;
 ctx->block_btree = NULL;
 ctx->curfunc = NULL;
 mjr_init_containers(ctx);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ctx));
}

/**
 * @brief Virtualisation of the libasm initialization depending on the architecture 
 * @param sess Mjollnir session strucutre
 * @param existing current processor
 * @ingroup libmjollnir
 */
int		mjr_setup_processor(mjrsession_t *sess, asm_processor *existing) 
{
 u_int         arch;

 if (existing)
   {
     memcpy(&sess->cur->proc, existing, sizeof(asm_processor));
     return (1);
   }
 arch =  elfsh_get_arch(sess->cur->obj->hdr);
 switch(arch)
   {
   case EM_SPARC:
   case EM_SPARCV9:
   case EM_SPARC32PLUS:
     asm_init_arch(&sess->cur->proc, ASM_PROC_SPARC);
     break;
   case EM_386:
     asm_init_arch(&sess->cur->proc, ASM_PROC_IA32);
     break;
   case EM_MIPS:
     asm_init_arch(&sess->cur->proc, ASM_PROC_MIPS);
     break;
   case EM_ARM:
     asm_init_arch(&sess->cur->proc, ASM_PROC_ARM);
     break;
   default:
     fprintf(D_DESC," [E] Libmjollnir unsupported architecture\n");
     return (-1);
   }
 return 1;
}

