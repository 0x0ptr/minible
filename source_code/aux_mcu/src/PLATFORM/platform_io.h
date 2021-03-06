/*!  \file     platform_io.h
*    \brief    Platform IO related functions
*    Created:  22/08/2018
*    Author:   Mathieu Stephan
*/

#ifndef PLATFORM_IO_H_
#define PLATFORM_IO_H_

#include "defines.h"

/* Prototypes */
uint32_t platform_io_get_cursense_conversion_result(BOOL trigger_conversion);
BOOL platform_io_is_current_sense_conversion_result_ready(void);
void platform_io_update_step_down_voltage(uint16_t voltage);
void platform_io_uart_debug_printf(const char *fmt, ...);
void platform_io_set_high_cur_sense_as_pull_down(void);
void platform_io_generate_no_comms_wakeup_pulse(void);
uint16_t platform_io_get_dac_data_register_set(void);
void platform_io_enable_battery_charging_ports(void);
void platform_io_prepare_ports_for_sleep_exit(void);
void platform_io_enable_step_down(uint16_t voltage);
void platform_io_init_ble_ports_for_disabled(void);
void platform_io_set_high_cur_sense_as_sense(void);
BOOL platform_io_is_ble_wakeup_output_high(void);
void platform_io_init_no_comms_pullup_port(void);
RET_TYPE platform_io_is_no_comms_asserted(void);
void platform_io_prepare_ports_for_sleep(void);
void platform_io_disable_no_comms_signal(void);
void platform_io_disable_charge_mosfets(void);
void platform_io_enable_charge_mosfets(void);
void platform_io_ble_disabled_actions(void);
void platform_io_disable_no_comms_int(void);
BOOL platform_io_is_wakeup_in_pin_low(void);
void platform_io_enable_no_comms_int(void);
void platform_io_init_no_comms_input(void);
void platform_io_deassert_ble_enable(void);
void platform_io_deassert_ble_wakeup(void);
void platform_io_disable_main_comms(void);
void platform_io_assert_ble_enable(void);
void platform_io_ble_enabled_inits(void);
void platform_io_enable_main_comms(void);
void platform_io_disable_step_down(void);
void platform_io_assert_ble_wakeup(void);
void platform_io_enable_debug_uart(void);
void platform_io_disable_usb_ports(void);
void platform_io_reset_ble_uarts(void);
void platform_io_disable_ble_int(void);
void platform_io_enable_ble_int(void);
void platform_io_init_aux_comms(void);
void platform_io_init_usb_ports(void);
void platform_io_enable_eic(void);
void platform_io_init_ports(void);


#endif /* PLATFORM_IO_H_ */