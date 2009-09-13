/**
* @file kernsh/include/kernsh-help.h
 * @ingroup kernsh
 * $Id$
 *
 */
#ifndef __KERNSH_HELP_H_
 #define __KERNSH_HELP_H_

/* Commands */
#define	HLP_OPENMEM		"open kernel's memory"
#define HLP_CLOSEMEM            "close kernel's memory"
#define HLP_KMODE		"switch mode : static, dynamic"

#define HLP_SCT		        "display the syscalltable"
#define HLP_IDT			"display the idt"
#define HLP_GDT			"display the gdt"

#define HLP_KALLOC		"alloc contiguous kernel memory"
#define HLP_KFREE		"free contiguous kernel memory"
#define HLP_KALLOCNC		"alloc non contiguous kernel memory"
#define HLP_KFREENC		"free non contiguous kernel memory"

#define HLP_KSYM		"search kernel symbol"

#define HLP_KMODULE_RELINK	"Relink a loadable kernel module\n\n"\
				"Parameter format : original_module evil_module new_module\n\n"

#define HLP_KMODULE_INFECT	"Infect a loadable kernel module\n\n"\
				"Parameter format : original_fname evil_fname module\n\n"

#define HLP_KMODULE_LOAD	"Load a loadable kernel module\n\n"\
				"Parameter format : module\n\n"

#define HLP_KMODULE_UNLOAD	"Unload a loadable kernel module\n\n"\
				"Parameter format : module\n\n"

#define HLP_AUTOTYPES		"autotypes"

#define HLP_KMEM_HASH		"Make hash at a specific addr\n\n"\
				"Parameter format : sa output, sa:rva output, sa%rva%size output, sa%size output\n"\
				"- sa : Symbol or addr\n"\
				"- rva : Byte offset form the beginning (optional)\n"\
				"- size : Bytes number limit (optional)"\
				"- If size is 0 or not specified, we search the end of the function\n"\
				"- output : file output (optional)\n\n"
				
#define HLP_KMEM_CHASH		"Check hash\n\n"\
				"Parameter format : format, file\n"\
				"- format : addr:mode:type:size:off:hash\n"\
				"- file : filename\n\n"

#define HLP_KVIRTM_INFO		"Kernel virtual memory informations"
#define HLP_KVIRTM_LOADME      	"Load the kernel virtual memory automatically\n"\
				"Parameter format : module\n\n"
			
				
#define HLP_KVIRTM_READ_PID	"handle kernel virtual memory"
#define HLP_KVIRTM_WRITE_PID	"handle kernel virtual memory"
#define HLP_KVIRTM_DISASM_PID	"handle kernel virtual memory"
#define HLP_KVIRTM_TASK_PID	"handle kernel virtual memory"
#define HLP_KVIRTM_DUMP		"handle kernel virtual memory"


#define HLP_KMEM_READ   	"Read kernel memory\n\n"\
				"Parameter format : sa, sa:rva, sa%rva%size, sa%size\n"\
				"- sa : Symbol or addr\n"\
				"- rva : Byte offset form the beginning (optional)\n"\
				"- size : Bytes number limit\n\n"

#define HLP_KMEM_WRITE	        "handle kernel memory"
#define HLP_KMEM_DISASM	        "Disasm kernel memory\n\n"\
				"Parameter format : sa, sa:rva, sa%rva%size, sa%size\n"   \
				"- sa : Symbol or addr\n"\
				"- rva : Byte offset form the beginning (optional)\n"\
				"- size : Bytes number limit\n\n"

#define HLP_KMEM_INFO	        "Kernel memory informations"

#define HLP_KDUMP_GET_VMA	"vma"
#define HLP_KDUMP_VMA		"vma"

#endif
