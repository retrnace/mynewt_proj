#include <nrf_temp.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <os/mynewt.h>
#include <nimble/ble.h>
#include <host/ble_hs.h>
#include <services/gap/ble_svc_gap.h>

#include "ble_temp_sensor.h"
#include "gatt_svr.h"
#include "temp.h"


#define TEMP_TIMER_PERIOD     (OS_TICKS_PER_SEC/10) // 10Hz

/* The timer callout */
static struct os_callout temp_callout;

extern struct log logger;

int16_t temp_buff[NUM_TEMP_READINGS] = {0};
int8_t temp_readings_tail_index  = -1;
bool temp_buff_full = false;


/* Returns the internal temperature of the nRF52 in degC (2 decimal places, scaled) */
int16_t
get_temp_measurement(void)
{
    int16_t temp;
    /* Start the temperature measurement. */
    NRF_TEMP->TASKS_START = 1;
    while(NRF_TEMP->EVENTS_DATARDY != TEMP_INTENSET_DATARDY_Set) {};
    /* Temp reading is in units of 0.25degC, so divide by 4 to get in units of degC
     * (scale by 100 to avoid representing as decimal). */
    temp = (nrf_temp_read() * 100) / 4.0;

    return temp;
}

static void
add_new_temp(int16_t temp){
	int8_t index = (temp_readings_tail_index + 1) % NUM_TEMP_READINGS;
	temp_buff[index] = temp;
  temp_readings_tail_index = index;
	if (!temp_buff_full && (index == (NUM_TEMP_READINGS-1))){
		temp_buff_full = true;
	}

#if 0
	int i = 0;
	int tmp_index;
	while( i < NUM_TEMP_READINGS){
					tmp_index = (0 <= (temp_readings_tail_index - i)) ? temp_readings_tail_index - i : NUM_TEMP_READINGS + (temp_readings_tail_index - i); 
					LOG(INFO, "%d ", temp_buff[tmp_index]);
					i++;
	}
	LOG(INFO, "\n");
#endif
}

/*
 * Event callback function for timer events. It toggles the led pin.
 */
static void
timer_ev_cb(struct os_event *ev)
{   
    assert(ev != NULL);
    
    uint16_t temp = get_temp_measurement();
		add_new_temp(temp); 
		os_callout_reset(&temp_callout, TEMP_TIMER_PERIOD);
}

void
init_temp_timer(void)
{
    /*
     * Initialize the callout for a timer event.
     */
    os_callout_init(&temp_callout, os_eventq_dflt_get(),
                    timer_ev_cb, NULL);

    os_callout_reset(&temp_callout, TEMP_TIMER_PERIOD);
}

uint8_t 
get_temp_readings_buff_size(void){
	return NUM_TEMP_READINGS;
} 

/* returns the get_temp_readings_buff_size() most recent temperature readings, 
 * sampled over the previous 1-second period at a rate of 10Hz (one sample every 100ms) 
 * 
 * param buff[get_temp_readings_buff_size()]
 * return:  number of samples in buffer
 */
uint8_t 
get_temp_readings(int16_t * buff)
{
	if(!buff)
		return -1;

	int i = 0;
	int tmp_index;
	while( i < NUM_TEMP_READINGS){
					tmp_index = (0 <= (temp_readings_tail_index - i)) ? temp_readings_tail_index - i : 
														NUM_TEMP_READINGS + (temp_readings_tail_index - i); 
					
					buff[i] = temp_buff[tmp_index];
					i++;
	}

	return NUM_TEMP_READINGS;
} 

