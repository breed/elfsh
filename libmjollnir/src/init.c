/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: init.c,v 1.8 2006-12-29 22:38:47 may Exp $
 *
 * Initialization functions
 *
 */
#include "libmjollnir.h"

/* Initialize the world for libmjollnir */
int		mjr_init_session(mjrsession_t *sess) 
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_init(&goto_hash, 100);
  hash_init(&sess->ctx, mjrHashSmall);

  /* register configurable parameters */
  elfsh_config_add_item(
    MJR_COFING_CALL_PREFIX,
    ELFSH_CONFIG_TYPE_STR,
    ELFSH_CONFIG_MODE_RW,
    MJR_CALL_PREFIX
  );

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Change the current analysis context */
int		mjr_set_current_context(mjrsession_t *sess, char *name) 
{
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 if ((sess->cur = hash_get(&sess->ctx,name)) == NULL)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));

}

/* Create a new analysis context */
int		mjr_add_context(mjrsession_t *sess, mjrcontext_t *ctx) 
{
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 hash_add(&sess->ctx,ctx->obj->name,ctx);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Create a context and set it as the current one */
int		mjr_create_context_as_current(mjrsession_t *sess, elfshobj_t *obj) 
{
 mjrcontext_t	*ctx;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 ctx = mjr_create_context(obj);
 mjr_add_context(sess, ctx);
 mjr_set_current_context(sess, obj->name);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


/* Create a new context */
mjrcontext_t	*mjr_create_context(elfshobj_t *obj) 
{
 mjrcontext_t	*ctx;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 XALLOC(ctx, sizeof(mjrcontext_t), NULL);
 bzero(ctx, sizeof(mjrcontext_t));
 ctx->obj = obj;
 hash_init(&ctx->blkhash, mjrHashLarge);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ctx));
}



/* Virtualisation of the libasm initialization depending on the architecture */
int		mjr_setup_processor(mjrsession_t *sess) 
{
 u_int         arch;

 arch =  elfsh_get_arch(sess->cur->obj->hdr);
 switch(arch)
   {
   case EM_SPARC:
     //   asm_init_sparc(&sess->proc);
     break;
   case EM_386:
     asm_init_i386(&sess->cur->proc);
     break;
   default:
     printf("unsupported architecture\n");
     return (-1);
   }
 return 1;
}
