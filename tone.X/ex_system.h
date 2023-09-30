/* 
 * File:   ex_system.h
 * Comments: Example system.h
 * Created on September 30, 2023
 */
#ifndef EX_SYSTEM_H
#define	EX_SYSTEM_H

// Fcycle 40 MIPS (Fosc/2 = 80 MHz / 2)
#define FCY 40000000UL
// system headers
#include <xc.h>
#include <libpic30.h>  // __delay_us()) and __delay_ms())

// local headers
#include "sask/h/sask.h"
#include "sask/h/WM8510CodecDrv.h"

#endif	/* EX_SYSTEM_H */

