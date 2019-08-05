/*
 * logic_sleep.c
 *
 * Created: 30/07/2019 20:09:38
 *  Author: limpkin
 */ 
#include "platform_defines.h"
#include "conf_serialdrv.h"
#include "comms_raw_hid.h"
#include "driver_timer.h"
#include "platform_io.h"
#include "logic_sleep.h"
#include "serial_drv.h"
#include "platform.h"
#include "defines.h"
#include "logic.h"
#include "main.h"
/* Boolean indicating if the ble module is set to sleep between events */
BOOL logic_sleep_ble_module_sleep_between_events = FALSE;
/* Booleans indicating what woke us up */
volatile BOOL logic_sleep_awoken_by_no_comms = FALSE;
volatile BOOL logic_sleep_awoken_by_ble = FALSE;
/* Full platform sleep requested */
BOOL logic_sleep_full_platform_sleep_requested = FALSE;

/*! \fn     logic_sleep_set_awoken_by_ble(void)
*   \brief  Set awoken by ble bool
*/
void logic_sleep_set_awoken_by_ble(void)
{
    logic_sleep_awoken_by_ble = TRUE;
}

/*! \fn     logic_sleep_set_awoken_by_no_comms(void)
*   \brief  Set awoken by no comms bool
*/
void logic_sleep_set_awoken_by_no_comms(void)
{
    logic_sleep_awoken_by_no_comms = TRUE;
}

/*! \fn     logic_sleep_wakeup_main_mcu_if_needed(void)
*   \brief  Wake-up main MCU if needed
*/
void logic_sleep_wakeup_main_mcu_if_needed(void)
{
    /* First check if main MCU is asleep */
    if (logic_sleep_full_platform_sleep_requested != FALSE)
    {
        // TODO if needed: use a timestamp to make sure we're not generating that pulse just after we've been set to sleep (very unlikely)
        
        /* Generate wakeup pulse */
        platform_io_generate_no_comms_wakeup_pulse();
        
        /* Re-enable main comms */
        platform_io_enable_main_comms();
        
        /* Leave some time for correct no comms readout */
        timer_delay_ms(1);
        
        /* Reset bool now that the main MCU is awake */
        logic_sleep_full_platform_sleep_requested = FALSE;
    }
}

/*! \fn     logic_sleep_set_ble_to_sleep_between_events(void)
*   \brief  Allow BLE module to sleep between events
*/
void logic_sleep_set_ble_to_sleep_between_events(void)
{
    /* Check that no data needs to be processed */
    if(host_event_data_ready_pin_level())
    {
        ble_wakeup_pin_set_low();
        DBG_SLP_LOG("ATBTLC to sleep btw events");
        logic_sleep_ble_module_sleep_between_events = TRUE;
    }
    else
    {
        DBG_SLP_LOG("Couldn't set ATBTLC to sleep btw events: events to be processed");
    }
}

/*! \fn     logic_sleep_ble_not_sleeping_between_events(void)
*   \brief  Indicate that the BLE module is not sleeping between events
*/
void logic_sleep_ble_not_sleeping_between_events(void)
{
    logic_sleep_ble_module_sleep_between_events = FALSE;
    DBG_SLP_LOG("ATBTLC to NOT sleep btw events");    
    timer_start_timer(TIMER_BT_WAKEUP_ENABLED, BT_NB_MS_BEFORE_DEASSERTING_WAKEUP);
}

/*! \fn     logic_sleep_ble_signal_to_sleep(void)
*   \brief  Called by BLE to signal the platform it can go to sleep
*/
void logic_sleep_ble_signal_to_sleep(void)
{
    if (logic_sleep_full_platform_sleep_requested != FALSE)
    {
        if (!host_event_data_ready_pin_level())
        {
            asm volatile("NOP");
            DBG_SLP_LOG("can't go to sleep: data ready pin low");
        }
        if (ble_wakeup_pin_level())
        {
            asm volatile("NOP");
            DBG_SLP_LOG("can't go to sleep: wakeup pin high");
        }
    }
    
    /* If main MCU asserted no comms in the mean time... no point in going to sleep anymore, re-enable comms with main MCU */
    if (logic_is_no_comms_unavailable() == FALSE)
    {
        if ((logic_sleep_full_platform_sleep_requested != FALSE) && (platform_io_is_no_comms_asserted() != RETURN_OK))
        {
            logic_sleep_full_platform_sleep_requested = FALSE;
            platform_io_enable_main_comms();
            comms_main_init_rx();
        }
    }
    
    /* If full platform sleep was requested, if no processing needs to be done, if we enable sleep between events */
    if ((logic_sleep_full_platform_sleep_requested != FALSE) && (host_event_data_ready_pin_level()) && (!ble_wakeup_pin_level()))
    {
        /* Clear bools */
        logic_sleep_awoken_by_no_comms = FALSE;
        logic_sleep_awoken_by_ble = FALSE;
        
        /* Set Host RTS to High */
        platform_set_ble_rts_high();
        
        /* Some processing was requested? Do not go to sleep */    
        if(!host_event_data_ready_pin_level())
        {
            platform_set_ble_rts_low();
            return;
        }
        
        DBG_SLP_LOG("Going to sleep");
        
        /* Enable BLE interrupt for wakeup */
        platform_io_enable_ble_int();
        
        /* Go to sleep, re-enable comms only if we were awoken by the main MCU */
        main_standby_sleep(FALSE, &logic_sleep_awoken_by_no_comms);
        
        /* We just woke up */
        platform_io_disable_ble_int();
        DBG_SLP_LOG("Waking up");
        
        /* Set Host RTS Low to receive the data */
        platform_set_ble_rts_low();
        
        /* Check if we were awoken by the main MCU */
        if(logic_sleep_awoken_by_no_comms != FALSE)
        {
            logic_sleep_full_platform_sleep_requested = FALSE;
        }
    }
}

/*! \fn     logic_sleep_set_full_platform_sleep_requested(void)
*   \brief  Called when a full platform sleep is requested
*/
void logic_sleep_set_full_platform_sleep_requested(void)
{
    logic_sleep_full_platform_sleep_requested = TRUE;
    DBG_SLP_LOG("full platform sleep requested");
}

/*! \fn     logic_sleep_is_full_platform_sleep_requested(void)
*   \brief  Know if full platform sleep was requested
*/
BOOL logic_sleep_is_full_platform_sleep_requested(void)
{
    return logic_sleep_full_platform_sleep_requested;
}

/*! \fn     logic_sleep_routine_ble_call(void)
*   \brief  logic sleep routine, called by ble routine
*/
void logic_sleep_routine_ble_call(void)
{
    /* BLE module was set to not sleep between events */
    if ((logic_sleep_ble_module_sleep_between_events == FALSE) && (timer_has_timer_expired(TIMER_BT_WAKEUP_ENABLED, FALSE) == TIMER_EXPIRED))
    {
        logic_sleep_set_ble_to_sleep_between_events();
    }
}