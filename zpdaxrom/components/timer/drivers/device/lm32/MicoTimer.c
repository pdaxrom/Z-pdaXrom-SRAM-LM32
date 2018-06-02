/****************************************************************************
**
**  Name: MicoTimer.c
**
**  Description:
**        Implements functions for manipulating LatticeMico32 Timer
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
**
**---------------------------------------------------------------------------
*****************************************************************************/


#include "MicoTimer.h"
#include "MicoTimerService.h"
#include "MicoMacros.h"
#include "MicoInterrupts.h"



/**************************************************************************
 * driver-debug functions                                                 *
 **************************************************************************/
#if _MICO_TIMER_DRIVER_DEBUG_ENABLED_
static void MicoTimerDumpCtx(MicoTimerCtx_t *pCtx)
{
    printf("\n Base     : 0x%x", pCtx->base);
    printf("\n IntrLevel: 0x%x", pCtx->intrLevel);
    printf("\n Context  : 0x%x", pCtx->userCtx);
    printf("\n Callback : 0x%x", pCtx->callback);
    return;
}


void MicoTimerDumpRegs(MicoTimerCtx_t *pCtx)
{
    MicoTimer_t *pTimer = (MicoTimer_t *)pCtx->base;
    printf("\n isr-status  : 0x%x",     pTimer->Status);
    printf("\n isr-control : 0x%x",     pTimer->Control);
    printf("\n isr-period  : 0x%x",     pTimer->Period);
    printf("\n isr-snapshot: 0x%x\n",   pTimer->Snapshot);
}
#endif


/******************************************************************************
 * Timer interrupt-handler                                                    *
 ******************************************************************************/
static void MicoTimerISR(unsigned int intrLevel, void *pContext)
{
    /*
     * flow:
     * - clear the timeout-bit
     * - invoke user-registered callback
     */
    MicoTimerCtx_t *ctx = (MicoTimerCtx_t *)pContext;
    volatile MicoTimer_t *pTimer = (MicoTimer_t *)ctx->base;

    /* acknowledge the interrupt */
    pTimer->Status = 0;

    /* call the isr */
    if(ctx->callback != 0)
        ((TimerCallback_t)ctx->callback)(ctx->userCtx);

    return;
}


/******************************************************************************
 * Initializes a timer                                                        *
 *----------------------------------------------------------------------------*
 * Inputs:                                                                    *
 *     unsigned int IntNum: Interrupt-level                                   *
 * Outputs:                                                                   *
 * Return values:                                                             *
 *            MICO_STATUS_E_INVALID_PARAM                                     *
 *            MICO_STATUS_OK                                                  *
 ******************************************************************************/
void MicoTimerInit( MicoTimerCtx_t *ctx )
{
    /* stop the timer (if it was running) */
    MicoTimerStop(ctx);

    /* 
     * Enable interrupts without registering an isr:
     * this way, any spurious timer interrupt that might have
     * existed prior to this init being called, will cause the
     * Mico Interrupt-framework to acknowlede the CPU's interrupt-pending.
     */
    MicoEnableInterrupt(ctx->intrLevel);

    /* register this timer for lookup service */
    ctx->lookupReg.name = ctx->name;
    ctx->lookupReg.deviceType = "TimerDevice";
    ctx->lookupReg.priv = ctx;
    MicoRegisterDevice( &(ctx->lookupReg) );

    /* all done */
    return;
}


/******************************************************************************
 * Starts a Mico32 timer                                                      *
 *----------------------------------------------------------------------------*
 * Inputs:                                                                    *
 *     MicoTimerCtx_t *ctx: pointer to valid ctx                              *
 *                                                                            *
 *     TimerCallback_t callback: User-provided callback function, called      *
 *             in interrupt-context.                                          *
 *                                                                            *
 *     void *priv: user-provided data that will be called in the callback     *
 *                                                                            *
 *     unsigned int timercount: ticks to load counter with                    *
 *                                                                            *
 *     int periodic: if 1, the timer is programmed to auto-load, else         *
 *           timer is programmed not to reload on reaching terminal value     *
 *                                                                            *
 * Note: user MUST supply a valid ctx.                                        *
 *       user MUST make sure timerCount is non-zero                           *
 ******************************************************************************/
mico_status
MicoTimerStart( MicoTimerCtx_t *ctx, TimerCallback_t callback, void *priv, unsigned int timerCount, int periodic )
{
    volatile MicoTimer_t *pTimer;
    unsigned int regValue;

    if( (ctx == 0) || (timerCount == 0) )
        return(MICO_STATUS_E_INVALID_PARAM);

    /* flow:
     * - stop the timer,
     * - load new timerCount,
     * - configure the timer, taking into account the periodicity
     * - register the isr (user MUST provide an isr)
     * - start the timer.
     */
    pTimer          = (MicoTimer_t *)(ctx->base);
    regValue        = (MICO32_TIMER_CONTROL_START_BIT_MASK|MICO32_TIMER_CONTROL_INT_BIT_MASK);
    ctx->callback   = (void *)callback;
    ctx->userCtx    = priv;

    MicoRegisterISR(ctx->intrLevel, ctx, MicoTimerISR);

    if(periodic != 0)
        regValue |= MICO32_TIMER_CONTROL_CONT_BIT_MASK;


    pTimer->Control = MICO32_TIMER_CONTROL_STOP_BIT_MASK;
    pTimer->Period  = timerCount;
    pTimer->Control = regValue;


    return(MICO_STATUS_OK);
}


/******************************************************************************
 *                                                                            *
 * Stops a Mico32 timer                                                       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *                                                                            *
 * Note: user MUST supply a valid ctx.                                        *
 *                                                                            *
 ******************************************************************************/
mico_status
MicoTimerStop(MicoTimerCtx_t *ctx)
{
    volatile MicoTimer_t *pTimer;
    if(ctx == 0)
        return(MICO_STATUS_E_INVALID_PARAM);


    /* stop the timer first and ack any pending interrupts */
    pTimer = (MicoTimer_t *)(ctx->base);
    pTimer->Control = MICO32_TIMER_CONTROL_STOP_BIT_MASK;
    pTimer->Status = 0;

    /* all done */
    return(MICO_STATUS_OK);
}


/******************************************************************************
 * reads timer-snapshot                                                       *
 *----------------------------------------------------------------------------*
 * Note: user MUST supply a valid ctx.                                        *
 ******************************************************************************/
unsigned int MicoTimerSnapshot( MicoTimerCtx_t *ctx)
{
    volatile MicoTimer_t *pTimer;
    if(ctx == 0){
        return(0);
    }
    pTimer = (MicoTimer_t *)(ctx->base);
    return(pTimer->Snapshot);
}

