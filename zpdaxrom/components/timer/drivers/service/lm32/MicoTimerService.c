/****************************************************************************
**
**  Name: MicoTimerService.c
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

#include "MicoTimerService.h"
#include "MicoUtils.h"
#include "MicoMacros.h"
#include "MicoInterrupts.h"
#include "LookupServices.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/******************************************************************************
 * Type-definitions and manifest constants and structures                     *
 ******************************************************************************/
typedef struct st_ActivityListEntry{
    void                    *ctx;
    MicoSysTimerActivity_t  activity;
}ActivityListEntry_t;


/******************************************************************************
 * data memebers                                                              *
 ******************************************************************************/
static ActivityListEntry_t      s_SysTimerActivity   = {0,0};
static MicoTimerCtx_t           *s_MicoSystemTimer   = 0;
static unsigned long long int   s_MicoCPUTicks;
static unsigned int             s_MicoSysTicks;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/******************************************************************************
 * debug routines                                                             *
 ******************************************************************************/
#ifdef _MICO_TIMER_SERVICES_DEBUG_ENABLED_

void DumpMicoTimerServicesActivityList()
{
    /* ToDo */
}


void DumpMicoTimerServicesRegisteredTimers()
{
    /* ToDo */
}

#endif


/******************************************************************************
 * system-timer ISR                                                           *
 ******************************************************************************/
static void MicoTimerServicesSysTimerISR(void *pContext)
{
    MicoSysTimerActivity_t activity;

    /* increment CPU ticks */
    s_MicoCPUTicks += s_MicoSysTicks;

    /* if there's an activity registered for system-tick, invoke it */
    if(s_SysTimerActivity.activity != 0){
        activity = s_SysTimerActivity.activity;
        activity(s_SysTimerActivity.ctx);
    };

    return;
}



/******************************************************************************
 * Initializes TimerServices                                                  *
 ******************************************************************************/
static void MicoTimerServicesInit()
{
    static int s_Initialized = 0;
    if(s_Initialized == 0) {
        s_Initialized = 1;
        s_SysTimerActivity.activity = 0;
        s_MicoSystemTimer           = 0;
        s_MicoCPUTicks              = 0;
    }
}


/******************************************************************************
 * registers a system-timer if one isn't already registered..                 *
 ******************************************************************************/
MicoTimerCtx_t* RegisterSystemTimer(MicoTimerCtx_t *pTimer, unsigned int TicksInMS)
{
	MicoTimerServicesInit();

    /* 
     * If there's a system-timer registered, don't modify it but simply
     * return its context information
     */
    if(s_MicoSystemTimer != 0)
        return(s_MicoSystemTimer);

    /*
     * there's none, so register this one as the system-timer
     */
    if(pTimer != 0){
        s_MicoSystemTimer = pTimer;
        if(TicksInMS == 0)
            TicksInMS = 1;
        s_MicoSysTicks = (TicksInMS * (MICO32_CPU_CLOCK_MHZ/1000));
        MicoTimerStart( s_MicoSystemTimer,
                        MicoTimerServicesSysTimerISR,
                        0,
                        s_MicoSysTicks-1,
                        1);
    }

    return(s_MicoSystemTimer);
}



/******************************************************************************
 * get cpu-ticks                                                              *
 ******************************************************************************/
void MicoGetCPUTicks(unsigned long long int *ticks)
{
    /* 
     * We need to get a finer resolution than the system-tick and also
     * account for a possible roll-over just after we read the snaphot.
     * We're definitely not going to be exact on the dot but we'll be
     * pretty darn close; higher the clock-speed, less the error (in
     * seconds).
     */
    unsigned long long int cpuTicks = 0;
    unsigned int intrMask;
    unsigned int snapshot;

    if(s_MicoSystemTimer != 0) {

        /* disable interrupts and read the gross resolution tick-count */
        intrMask = MicoDisableInterrupts();
        cpuTicks = s_MicoCPUTicks;


        /* read the snapshot to get a finer tuning */
        snapshot = MicoTimerSnapshot(s_MicoSystemTimer);


        /* 
         * Since the timer is an external peripheral over a bus, there may be
         * contention and we may not get the right reading.  So allow another
         * interrupt if it happened, to make our tick-reading as accurate
         * as possible.
         */
        MicoEnableInterrupt(s_MicoSystemTimer->intrLevel);


        /* we're done; decide what our reading should be */
        MicoDisableInterrupts();
        if(cpuTicks == s_MicoCPUTicks)
            cpuTicks += (s_MicoSysTicks - snapshot -1);
        else
            cpuTicks = s_MicoCPUTicks;


        /* restore interrupts */
        MicoEnableInterrupts(intrMask);
    }

    if(ticks != 0)
        *ticks = cpuTicks;

    return;
}


/******************************************************************************
 * Registers a periodic-activity (system-timer ticks)                         *
 *----------------------------------------------------------------------------*
 * System-timer must exist for this to function correctly.                    *
 ******************************************************************************/
void MicoRegisterActivity(MicoSysTimerActivity_t activity, void *ctx)
{
	MicoTimerServicesInit();

    if(s_MicoSystemTimer == 0)
        return;

    MicoDisableInterrupt(s_MicoSystemTimer->intrLevel);

    s_SysTimerActivity.ctx      = ctx;
    s_SysTimerActivity.activity = activity;

    MicoEnableInterrupt(s_MicoSystemTimer->intrLevel);

    return;
}

