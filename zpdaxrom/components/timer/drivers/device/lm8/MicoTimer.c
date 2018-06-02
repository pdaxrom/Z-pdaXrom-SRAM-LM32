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
**  3.0   Mar-25-2008  Added Header
**  3.1   Sep-13-2012  LM8 Timer Device Driver Added
**
**---------------------------------------------------------------------------
*****************************************************************************/
#include "MicoTimer.h"

/* *****************************************************************************
 * Timer initialization.
 *
 * Arguments:
 *    MicoTimerCtx_t *ctx			: Timer context
 *
 *****************************************************************************
 */
void MicoTimerInit( MicoTimerCtx_t *ctx )
{
	size_t timer_address = (size_t) ctx->base;

	// Disable/clear interrupts and Reset Timer
    MICO_TIMER_WRITE_STATUS (timer_address, 0x0);
    MICO_TIMER_WRITE_CONTROL (timer_address, 0x0);

    return;
}

/*
 *****************************************************************************
 * Interrupt handler. Each Timer instance has it's own default interrupt handler
 * implementation. If the developer wishes to use his own interrupt handler,
 * he must define the __MICO_TIMER_USER_IRQ_HANDLER__ preprocessor define.
 *
 * Arguments:
 *    MicoTimerCtx_t *ctx			: Timer context
 *
 *****************************************************************************
 */
#ifndef __MICO_NO_INTERRUPTS__
#ifndef __MICO_TIMER_USER_IRQ_HANDLER__
void MicoTimerISR (MicoTimerCtx_t *ctx)
{
	size_t timer_address = (size_t) ctx->base;

	//Clear Pending Interrupt
	MICO_TIMER_WRITE_STATUS(timer_address, 0x0);
}
#endif
#endif

/*****************************************************************************
 * Starts the MICO count-down timer with timerCount as the initial value
 * Note: User MUST make sure that timerCount is within the counter range
 *
 * Arguments:
 *    MicoTimerCtx_t *ctx		: Timer context
 *    unsigned char periodic	: If 0, the timer will not reload timerCount
 *     							  else the timer is programmed to auto reload
 *     							  timerCount when reaching 0 (CONT bit set to 1)
 *    unsigned long timerCount	: Number of ticks (Initial Value) to load into counter
 *
 * Return Value:
 *    char:
 *         0 => successful starts
 *		  -1 => Invalid timerCount
 *****************************************************************************
 */
char MicoTimerStart(MicoTimerCtx_t *ctx, unsigned char periodic, unsigned long timerCount)
{
	size_t timer_address = (size_t) ctx->base;
	unsigned char period_width = ctx->period_width;

	// Check the timerCount is valid value
	if (timerCount > (unsigned long) ((0x1 << period_width) - 1) || timerCount == 0){
		return (-1);
	}

	// Load the initial value into Period Register
	unsigned char * p_timerCount = (unsigned char *) &timerCount;
	MICO_TIMER_WRITE_PERIOD_BYTE0(timer_address, *p_timerCount);
	if (period_width > 8){
		MICO_TIMER_WRITE_PERIOD_BYTE1(timer_address, *(p_timerCount+1));
		if (period_width > 16) {
			MICO_TIMER_WRITE_PERIOD_BYTE2(timer_address, *(p_timerCount+2));
			if (period_width > 24) {
				MICO_TIMER_WRITE_PERIOD_BYTE3(timer_address, *(p_timerCount+3));
			}
		}
	}

	// Configure the Control Register
	unsigned char control = MICO_TIMER_CONTROL_START_BIT_MASK;
	if (periodic != 0) control = control | MICO_TIMER_CONTROL_CONT_BIT_MASK;
#ifndef __MICO_NO_INTERRUPTS__
	control = control | MICO_TIMER_CONTROL_INT_BIT_MASK;
#endif
	MICO_TIMER_WRITE_CONTROL(timer_address, control);


	return (0);
}

/*****************************************************************************
 * Stop the MICO count-down timer and reset interrupt
 *
 * Arguments:
 *    MicoTimerCtx_t *ctx		: Timer context
 *
 * Return Value:
 *    char:
 *         0 => successful stops
 *****************************************************************************
 */
char MicoTimerStop(MicoTimerCtx_t *ctx)
{
	size_t timer_address = (size_t) ctx->base;

    //Stop the timer first and Reset any pending interrupts
    MICO_TIMER_WRITE_CONTROL(timer_address, MICO_TIMER_CONTROL_STOP_BIT_MASK);
    MICO_TIMER_WRITE_STATUS(timer_address, 0x0);

    return (0);
}

/*****************************************************************************
 * Read the count-down timer snapshot register
 *
 * Arguments:
 *    MicoTimerCtx_t *ctx		: Timer context
 *
 * Return Value:
 *   unsigned long snapShot		: Snapshot value of the internal counter
 *****************************************************************************
 */
unsigned long MicoTimerSnapshot(MicoTimerCtx_t *ctx)
{
	size_t timer_address = (size_t) ctx->base;
	unsigned char period_width = ctx->period_width;

	unsigned long snapShot;
	unsigned char * p_snapShot = (unsigned char *) &snapShot;
	MICO_TIMER_READ_SNAPSHOT_BYTE0(timer_address, *p_snapShot);
	if (period_width > 8){
		MICO_TIMER_READ_SNAPSHOT_BYTE1(timer_address, *(p_snapShot+1));
		if (period_width > 16) {
			MICO_TIMER_READ_SNAPSHOT_BYTE2(timer_address, *(p_snapShot+2));
			if (period_width > 24) {
				MICO_TIMER_READ_SNAPSHOT_BYTE3(timer_address, *(p_snapShot+3));
			}
		}
	}

    return snapShot;
}