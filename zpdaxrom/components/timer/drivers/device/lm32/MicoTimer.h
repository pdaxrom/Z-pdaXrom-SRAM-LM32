/****************************************************************************
**
**  Name: MicoTimer.h
**
**  Description:
**        Implements functions for manipulating LatticeMico32 Timer and
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
**
**---------------------------------------------------------------------------
*****************************************************************************/

#ifndef MICO32_MICOTIMER_HEADER_FILE
#define MICO32_MICOTIMER_HEADER_FILE

#include "MicoTypes.h"
#include "DDStructs.h"

/****************************************************************************
 * Mico-timer driver provides the ability to use a Mico-32 timer in either  *
 * a single-shot mode or a periodic mode.                                   *
 *--------------------------------------------------------------------------*
 * Mico Timers must be located in a non-cached region to use this driver    *
 ****************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/******************************************************************************
 * Data Structures:                                                           *
 ******************************************************************************/
    /* timer callback function type */
    typedef void(*TimerCallback_t)(void *);


    /* mico-timer register structure */
    typedef struct st_MicoTimer{
        volatile unsigned int Status;
        volatile unsigned int Control;
        volatile unsigned int Period;
        volatile unsigned int Snapshot;
    }MicoTimer_t;


    /**************************************************************************
     * MICO32 TIMER REGISTER DEFINITIONS                                      *
     * (NOTE: OFFSETS REPRESENT BYTES)                                        *
     **************************************************************************/
    #define MICO32_TIMER_STATUS_REG_OFFSET                  (0x00)
    #define MICO32_TIMER_CONTROL_REG_OFFSET                 (0x04)
    #define MICO32_TIMER_PERIOD_REG_OFFSET                  (0x08)
    #define MICO32_TIMER_SNAPSHOT_REG_OFFSET                (0x0c)


    /* status-register bits: */
    #define MICO32_TIMER_STATUS_TO_BIT_MASK                 (0x1)


    /* control-register bits */
    #define MICO32_TIMER_CONTROL_INT_BIT_MASK               (0x1)
    #define MICO32_TIMER_CONTROL_CONT_BIT_MASK              (0x2)
    #define MICO32_TIMER_CONTROL_START_BIT_MASK             (0x4)
    #define MICO32_TIMER_CONTROL_STOP_BIT_MASK              (0x8)


/******************************************************************************
 * functions                                                                  *
 ******************************************************************************/

/* initializes Mico32 timer */
void MicoTimerInit( MicoTimerCtx_t *ctx );


/*
 ******************************************************************************
 * Starts a Mico32 timer                                                      *
 *----------------------------------------------------------------------------*
 * Inputs:                                                                    *
 *     MicoTimerCtx_t *ctx: pointer to valid ctx                              *
 *                                                                            *
 *     TimerCallback_t callback: User-provided callback function, called      *
 *             in interrupt-context.                                          *
 *             typedef void(*TimerCallback_t)(void *)                         *
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
 ******************************************************************************
 */
mico_status MicoTimerStart( MicoTimerCtx_t *ctx, TimerCallback_t callback,
            void *priv, unsigned int timerCount, int periodic );


/* stops a mico-32 timer */
mico_status MicoTimerStop(MicoTimerCtx_t *ctx);


/*
 * Reads timer-count snapshot; snapshot value is returned
 * as function return value
 */
unsigned int MicoTimerSnapshot(MicoTimerCtx_t *ctx);


#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif /*MICO32_MICOTIMER_HEADER_FILE */

