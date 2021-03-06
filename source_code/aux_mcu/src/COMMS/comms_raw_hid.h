/*
 * comms_usb.h
 *
 * Created: 23/08/2018 21:52:22
 *  Author: limpkin
 */ 


#ifndef COMMS_USB_H_
#define COMMS_USB_H_

#include "defines.h"
#include "comms_main_mcu.h"

/* Type defs */
typedef struct
{
    struct
    {
        uint8_t  payload_len:6;
        uint8_t  ack_flag_or_req:1;
        uint8_t  flip_bit:1;
    } byte0;
    struct
    {
        uint8_t  total_packets:4;
        uint8_t  packet_id:4;
    } byte1;
    uint8_t payload[62];    
} moolticute_comms_hid_packet_t;

typedef struct
{
    union
    {
        moolticute_comms_hid_packet_t mtc_hid_packet;
        uint32_t raw_packet_uint32[64/4];
        uint8_t raw_packet[64];
    };
} hid_packet_t;

/* Prototypes */
void comms_raw_hid_send_packet(hid_interface_te hid_interface, hid_packet_t* packet, BOOL wait_send, uint16_t payload_size);
void comms_raw_hid_send_hid_message(hid_interface_te hid_interface, aux_mcu_message_t* message);
void comms_raw_hid_recv_callback(hid_interface_te hid_interface, uint16_t recv_bytes);
hid_packet_t* comms_raw_hid_get_send_buffer(hid_interface_te hid_interface);
void comms_raw_hid_connection_set_callback(hid_interface_te hid_interface);
uint8_t* comms_raw_hid_get_recv_buffer(hid_interface_te hid_interface);
void comms_raw_hid_arm_packet_receive(hid_interface_te hid_interface);
void comms_raw_hid_set_idle_config(uint8_t interface, uint8_t val);
void comms_raw_hid_set_protocol(uint8_t interface, uint8_t val);
void comms_raw_hid_send_callback(hid_interface_te hid_interface);
void comms_raw_hid_update_device_status_cache(uint8_t* buffer);
uint8_t* comms_raw_hid_get_idle_config(uint8_t interface);
uint8_t* comms_raw_hid_get_protocol(uint8_t interface);
comms_usb_ret_te comms_usb_communication_routine(void);
void comms_usb_debug_printf(const char *fmt, ...);
void comms_usb_clear_enumerated(void);
BOOL comms_usb_is_enumerated(void);


#endif /* COMMS_USB_H_ */
