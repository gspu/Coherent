/*
 * /usr/include/sys/__paddr.h
 *
 * The definition of __paddr_t.
 *
 * Revised Mon Mar 22 13:12:44 1993 CST
 */

#ifndef __SYS___PADDR_H__  
#define __SYS___PADDR_H__  
  
/*  
 * This internal header file is intended to be the sole point of definition  
 * for the internal data type "__paddr_t", which is used as the basis of the  
 * definition of the internal kernel data type "paddr_t".  
 *  
 * The semantics of this type are not well-defined. Any access to a data item 
 * of this type or a type derived from it is non-portable, and is not  
 * guaranteed to remain the same across releases of the operating system.  
 * Not even the size of such an item can be assumed to remain constant, so  
 * use of such an item in a structure visible to user or driver code is  
 * prohibited. [ Exception; certain header files from earlier releases of the 
 * Coherent operating system defined such structures, which remain in the  
 * current release but which are being phased out. Therefore, access to those 
 * structures is similarly non-portable by transitivity, and developers are  
 * cautioned to find portable alternatives. ]  
 */  
  
typedef long    __paddr_t;  
  
#endif  /* ! defined (__SYS___PADDR_H__) */  
