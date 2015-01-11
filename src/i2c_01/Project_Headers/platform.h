/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         Global definitions for the whole project.
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          12.09.2014
 *
 * \b Language:   Ansi-C \n\n
 * \b Target:     MC Car \n
 *
 * $Id: platform.h 904 2014-09-12 10:58:43Z chj-hslu $
 *--------------------------------------------------------------------
 */
#ifndef PLATFORM_H
#define PLATFORM_H

/* Include the derivative-specific header file */
#include <mc9s08jm60.h>
#include <hidef.h> /* for EnableInterrupts macro */

#define BUSCLOCK              24000000  // Hz

#define _Stop asm ( stop; )
  /*!< Macro to enter stop modes, STOPE bit in SOPT1 register must be set prior to executing this macro */

#define _Wait asm ( wait; )
  /*!< Macro to enter wait mode */

typedef struct {
    unsigned int  stackFree;                    /* Number of free bytes on the stack                            */
    unsigned int  stackUsed;                    /* Number of bytes used on the stack                            */
} tStackData;

void ClearStack(void);
tStackData CheckStackSize(void);


// Use Project Properties => HCS08 Compiler => Type Sizes to adapt to target
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned int uint16;
typedef signed int int16;
typedef unsigned long uint32;
typedef signed long int32;
typedef char bool;

#define TRUE                1
#define FALSE               0

#endif /* PLATFORM_H_ */
