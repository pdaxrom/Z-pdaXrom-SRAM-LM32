/****************************************************************************
**
**  Name: MicoTimer.h
**
**  Description:
**        Implements functions for manipulating LatticeMICO Timer and
**        defines timer register map
**
**  $Revision: $
**
** Disclaimer:
**
**   This source code is intended as a design reference which
**   illustrates how these types of functions can be implemented.  It
**   is the user's responsibility to verify their design for
**   consistency and functionality through the use of formal
**   verification methods.  Lattice Semiconductor provides no warranty
**   regarding the use or functionality of this code.
**
** --------------------------------------------------------------------
**
**                     Lattice Semiconductor Corporation
**                     5555 NE Moore Court
**                     Hillsboro, OR 97214
**                     U.S.A
**
**                     TEL: 1-800-Lattice (USA and Canada)
**                          (503)268-8001 (other locations)
**
**                     web:   http://www.latticesemi.com
**                     email: techsupport@latticesemi.com
**
** --------------------------------------------------------------------------
**
**  Change History (Latest changes on top)
**
**  Ver    Date        Description
** --------------------------------------------------------------------------
**
**	3.0   Mar-25-2008  Added Header
**  3.1   Sep-13-2012  LM8 Timer Device Driver Added
**
**---------------------------------------------------------------------------
*****************************************************************************/
#include "DDStructs.h"

/***********************************************************************
 *                                                                     *
 * TIMER PHYSICAL DEVICE SPECIFIC INFORMATION           			   *
 *                                                                     *
 ***********************************************************************/
#define MICO_TIMER_STATUS_REG_OFFSET					(0x00)
#define MICO_TIMER_CONTROL_REG_OFFSET					(0x04)
#define MICO_TIMER_PERIOD_REG_OFFSET					(0x08)
#define MICO_TIMER_SNAPSHOT_REG_OFFSET					(0x0c)

//Status Register Bit Mask
#define MICO_TIMER_STATUS_TO_BIT_MASK					(0x01)

//Control Register Bit Mask
#define MICO_TIMER_CONTROL_INT_BIT_MASK              	(0x01)
#define MICO_TIMER_CONTROL_CONT_BIT_MASK              	(0x02)
#define MICO_TIMER_CONTROL_START_BIT_MASK             	(0x04)
#define MICO_TIMER_CONTROL_STOP_BIT_MASK              	(0x08)
/***********************************************************************
 *                                                                     *
 * TIMER USER DEFINE                                                   *
 *                                                                     *
 ***********************************************************************/


/***********************************************************************
 *                                                                     *
 * TIMER USER MACROS                                                   *
 *                                                                     *
 ***********************************************************************/
/* Macros for Writing/Reading each byte of the Status Register */
#define MICO_TIMER_READ_STATUS(X, Y) \
	(Y) = (__builtin_import((size_t)(X)+MICO_TIMER_STATUS_REG_OFFSET))

#define MICO_TIMER_WRITE_STATUS(X, Y) \
	(__builtin_export((char)(Y & 0x01), (size_t)(X)+MICO_TIMER_STATUS_REG_OFFSET))

/* Macros for Writing/Reading the Control Register */
#define MICO_TIMER_READ_CONTROL(X, Y) \
	(Y) = (__builtin_import((size_t)(X)+MICO_TIMER_CONTROL_REG_OFFSET))

#define MICO_TIMER_WRITE_CONTROL(X, Y) \
	(__builtin_export((char)(Y & 0x0F), (size_t)(X)+MICO_TIMER_CONTROL_REG_OFFSET))

/* Macros for Reading each byte of the Period Register */
#define MICO_TIMER_READ_PERIOD_BYTE0(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+0)))

#define MICO_TIMER_READ_PERIOD_BYTE1(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+1)))

#define MICO_TIMER_READ_PERIOD_BYTE2(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+2)))

#define MICO_TIMER_READ_PERIOD_BYTE3(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+3)))

/* Macros for writing each byte of the Period Register */
#define MICO_TIMER_WRITE_PERIOD_BYTE0(X, Y) \
	(__builtin_export((char)(Y), (size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+0)))

#define MICO_TIMER_WRITE_PERIOD_BYTE1(X, Y) \
	(__builtin_export((char)(Y), (size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+1)))

#define MICO_TIMER_WRITE_PERIOD_BYTE2(X, Y) \
	(__builtin_export((char)(Y), (size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+2)))

#define MICO_TIMER_WRITE_PERIOD_BYTE3(X, Y) \
	(__builtin_export((char)(Y), (size_t)(X+MICO_TIMER_PERIOD_REG_OFFSET+3)))

/* Macros for Reading each byte of the Snapshot Register */
#define MICO_TIMER_READ_SNAPSHOT_BYTE0(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_SNAPSHOT_REG_OFFSET+0)))

#define MICO_TIMER_READ_SNAPSHOT_BYTE1(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_SNAPSHOT_REG_OFFSET+1)))

#define MICO_TIMER_READ_SNAPSHOT_BYTE2(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_SNAPSHOT_REG_OFFSET+2)))

#define MICO_TIMER_READ_SNAPSHOT_BYTE3(X, Y) \
	(Y) = (__builtin_import((size_t)(X+MICO_TIMER_SNAPSHOT_REG_OFFSET+3)))
/***********************************************************************
 *                                                                     *
 * TIMER USER FUNCTIONS                                                *
 *                                                                     *
 ***********************************************************************/
#ifndef __MICO_NO_INTERRUPTS__
#ifndef __MICO_TIMER_USER_IRQ_HANDLER__
void MicoTimerISR(MicoTimerCtx_t *ctx);	// Timer context
#endif
#endif

char MicoTimerStart( MicoTimerCtx_t *ctx,		// Timer context
					unsigned char periodic,		// Enable the CONT bit
					unsigned long timerCount);	// Number of ticks

char MicoTimerStop(MicoTimerCtx_t *ctx);	// Timer context

unsigned long MicoTimerSnapshot(MicoTimerCtx_t *ctx);	// Timer context


