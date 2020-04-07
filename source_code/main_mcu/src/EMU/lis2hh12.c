#include "lis2hh12.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "emulator.h"

static int urandom = -1;

BOOL lis2hh12_check_data_received_flag_and_arm_other_transfer(accelerometer_descriptor_t* descriptor_pt, BOOL arm_other_transfer)
{
    int16_t base_y = emu_get_lefthanded() ? -100 : 100;
    (void)arm_other_transfer;
    for (uint16_t i = 0; i < MEMBER_ARRAY_SIZE(acc_single_fifo_read_t, acc_data_array); i++)
    {
        /* Only fill the lower bytes as the rng uses them */
        descriptor_pt->fifo_read.acc_data_array[i].acc_x = 100 + ((uint16_t)rand() & 0x000F);
        descriptor_pt->fifo_read.acc_data_array[i].acc_y = base_y + ((uint16_t)rand() & 0x000F);
        descriptor_pt->fifo_read.acc_data_array[i].acc_z = 100 + ((uint16_t)rand() & 0x000F);
    }
    return TRUE; 
}

RET_TYPE lis2hh12_check_presence_and_configure(accelerometer_descriptor_t* descriptor_pt){srand ((unsigned int) time (NULL));return RETURN_OK; }
/*
void lis2hh12_send_command(accelerometer_descriptor_t* descriptor_pt, uint8_t* data, uint32_t length){}
void lis2hh12_manual_acc_data_read(accelerometer_descriptor_t* descriptor_pt, acc_data_t* data_pt){}
void lis2hh12_deassert_ncs_and_go_to_sleep(accelerometer_descriptor_t* descriptor_pt){}
void lis2hh12_sleep_exit_and_dma_arm(accelerometer_descriptor_t* descriptor_pt){}
int16_t lis2hh12_get_temperature(accelerometer_descriptor_t* descriptor_pt){}
void lis2hh12_dma_arm(accelerometer_descriptor_t* descriptor_pt){}
void lis2hh12_reset(accelerometer_descriptor_t* descriptor_pt){}
*/
