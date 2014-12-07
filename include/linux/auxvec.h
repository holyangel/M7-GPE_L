#ifndef _LINUX_AUXVEC_H
#define _LINUX_AUXVEC_H

#include <asm/auxvec.h>

#define AT_NULL   0	
#define AT_IGNORE 1	
#define AT_EXECFD 2	
#define AT_PHDR   3	
#define AT_PHENT  4	
#define AT_PHNUM  5	
#define AT_PAGESZ 6	
#define AT_BASE   7	
#define AT_FLAGS  8	
#define AT_ENTRY  9	
#define AT_NOTELF 10	
#define AT_UID    11	
#define AT_EUID   12	
#define AT_GID    13	
#define AT_EGID   14	
#define AT_PLATFORM 15  
#define AT_HWCAP  16    
#define AT_CLKTCK 17	
#define AT_SECURE 23   
#define AT_BASE_PLATFORM 24	
#define AT_RANDOM 25	

#define AT_EXECFN  31	

#ifdef __KERNEL__
#define AT_VECTOR_SIZE_BASE 19 
  
#endif

#endif 
