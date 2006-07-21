
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: core.c,v 1.5 2006-07-21 18:26:46 thor Exp $
 *
 */

#include "libmjollnir.h"

int mjr_analyse(mjrSession *sess,int flags) {
 char *shtName;
 elfsh_Shdr *shtlist,*shdr;
 elfsh_Sym  *sym;
 int num_sht,idx_sht;

 if (NULL == sess)
  return -1;

 if (!(shtlist = elfsh_get_sht(sess->obj, &num_sht))) {
  printf("faild to get sht!\n");
  return -1;
 }

#if __DEBUG__
 fprintf(D_DESC,"[__DEBUG__] mjr_analize: Found %d sections.\n",num_sht);
#endif

 for (idx_sht = 0; idx_sht < num_sht; idx_sht++) {
  shdr = (shtlist + idx_sht);
  sym = elfsh_get_sym_from_shtentry(sess->obj, shdr);
  shtName = elfsh_get_symbol_name(sess->obj, sym);

  if (elfsh_get_section_execflag(shdr)) {
#if __DEBUG__
   fprintf(D_DESC,"[__DEBUG__] mjr_analize: Executable section name=(%14s) index=(%02i)\n", shtName, idx_sht);
#endif
   mjr_find_calls(sess,shtName);
  }
 }
 
 /* set the flag */
 sess->analysed_calls = 1;

 return 1;
}

/*
 This function will find calls including
 calls trought a pointer
 */

int mjr_find_calls(mjrSession *sess,char *section_name) {

 elfshsect_t    *sct;
 asm_instr       instr;
 unsigned char  *ptr;
 unsigned long   curr,len;
 u_int           vaddr, ilen, dest;
 char 			*tmp;
 mjr_block		*newBlock;

#if __DEBUG__
 char *_d_type;
#endif

 ELFSH_NOPROFILE_IN();

 /* load section */
#if __DEBUG__
 fprintf(D_DESC, "[__DEBUG__] mjrFindCalls: loading %s\n",section_name);
#endif

 sct = elfsh_get_section_by_name(sess->obj, section_name, NULL, NULL, NULL);

 if (!sct) {
  ELFSH_NOPROFILE_ROUT(NULL);
 }

#if __DEBUG_CALLS__
 fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: %x %d %d\n",
  sct->shdr->sh_addr,
  sct->shdr->sh_size,
  sct->shdr->sh_offset);
#endif

 /* allocate memory for in-memory copy */
 XALLOC(ptr,sct->shdr->sh_size,-1);
 memset(ptr, 0, sct->shdr->sh_size);

 /* load it */
 elfsh_raw_read(sess->obj, sct->shdr->sh_offset, ptr, sct->shdr->sh_size);

 len = sct->shdr->sh_size;
 curr = 0;
 vaddr = sct->shdr->sh_addr;

 newBlock = mjr_create_block(vaddr,section_name,MJR_TYPE_SECT_START);
 tmp = _vaddr2string(vaddr);
 hash_add(&sess->blocks,tmp,newBlock);

 while (curr < len) {

  if ((ilen = asm_read_instr(&instr, ptr + curr, len - curr, &sess->proc)) > 0) {
   mjr_history_update(sess, instr);
   sess->curVaddr = vaddr + curr;
   dest = 0;

#if __DEBUG_READ__
   fprintf(D_DESC, "[__DEBUG_READ__] mjrFindCalls: v:%x c:%x l:%x il:%x %x/%x\n",
    vaddr + curr, curr, len,
    ilen,
    sct->shdr->sh_offset + sct->shdr->sh_size,
    sct->shdr->sh_offset + curr);
#endif
   switch (instr.type) {

    case ASM_TYPE_CALLPROC:
#if __DEBUG_CALLS__
     fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: CALL v:0x%lx\n", vaddr + curr);
#endif
	sess->st_calls_seen++;
     if (mjr_get_call_dst(sess,&dest)>0) {
	  dest += curr + ilen;
	  if (vaddr + dest != 0x00) {

	   sess->st_calls_found++;

	   tmp = _vaddr2string(vaddr+dest);
	   if (hash_get(&sess->blocks,tmp) == NULL) {
	    char *md5;
	    mjrFunction *fun;
#if __DEBUG__
		_d_type = "NEW";
#endif
		newBlock = mjr_create_block(vaddr+dest,section_name,MJR_TYPE_FUNCT);
		fun = mjr_function_create(vaddr+dest);

		hash_add(&sess->blocks,tmp,newBlock);
		md5 = mjr_fingerprint_function(sess,ptr+dest,MJR_FNG_TYPE_MD5);
		if (md5)
 		 fun->md5 = elfsh_strdup(md5);

	   } else {
#if __DEBUG__
		_d_type = "OLD";
#endif
	   }

#if __DEBUG__
	 fprintf(D_DESC, "[__DEBUG__] Found %s CALL at v:0x%x -> [0x%08x] \n",
	  _d_type,sess->curVaddr,vaddr+dest);
#endif
	  }
     }
	 break;

	default:
	 break;

   /* end of switch */
   }

  /* end of if asm_read_instr */
  }

  /* check if disassembler is missing something */
  if (ilen <= 0) {
   fprintf(D_DESC, "asm_read_instr error: 0x%lx: %d\n", vaddr + curr, ilen);
   fprintf(D_DESC, ";; dumping opcodes: %02x %02x %02x %02x\n",
              *(ptr + curr), *(ptr + curr + 1),
              *(ptr + curr + 2), *(ptr + curr + 3));
              ilen = 1;
  }
 curr += ilen;
 /* end of while */
 }

 elfsh_free(ptr);
 ELFSH_NOPROFILE_ROUT(NULL);
}
