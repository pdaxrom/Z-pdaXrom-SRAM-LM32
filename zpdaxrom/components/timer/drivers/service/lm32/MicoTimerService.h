/****************************************************************************
**
**  Name: MicoTimerService.h
**
**  Description:
**        Implements timer service functionality:
**               -- registering a timer as a system timer (64-bit)
**                  and retrieving the system tick
**
**               -- registering a timer as a "lookup-able" device
**                  (user does not have to work with these functions)                  
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


#ifndef MICO32_TIMERSERVICE_HEADER_FILE
#define MICO32_TIMERSERVICE_HEADER_FILE

#include "MicoTypes.h"
#include "MicoTimer.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/******************************************************************************
 * Data Structures/types                                                      *
 ******************************************************************************/
typedef void (* MicoSysTimerActivity_t) ( void *);



/******************************************************************************
 * functions                                                                  *
 ******************************************************************************/

/* 
 * Registers system-tick periodic activity
 *  Arguments;
 *      MicoSysTimerActivity_t: activity function pointer as described
 *      by the prototype.
 *      void *ctx: pointer to user-data that will be passed back on
 *      system-tick.
 */
void MicoRegisterActivity(MicoSysTimerActivity_t activity, void *ctx);


/* 
 * Retrieves 64-bit tick count, if a system-timer is registered.
 * Else, it returns 0.
 */
void MicoGetCPUTicks(unsigned long long int *ticks);


/* 
 * Registers system-timer if one isn't already registered.  Once registered,
 * you cannot register some other timer as a the system timer.  You can
 * always stop the timer using LatticeMico32 device-driver routine
 * and bring the system-timer to a halt.
 *  Arguments:
 *      MicoTimerCtx_t *ctx: LatticeMico32 Timer instance information
 *      structure (context) that should be used as the system timer.
 *      unsigned int TickInMS: system-tick value in milli-seconds.
 *  Return Value:
 *      MicoTimerCtx_t *: LatticeMico32 timer instance information structure
 *      of the timer being used as the system-timer.  If there is no system
 *      timer, it returns 0.
 */
MicoTimerCtx_t* RegisterSystemTimer(MicoTimerCtx_t *ctx, unsigned int TickInMS);



#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif

