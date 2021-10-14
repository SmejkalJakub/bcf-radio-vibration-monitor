#include <application.h>

#define TEMPERATURE_TAG_PUB_NO_CHANGE_INTEVAL (15 * 60 * 1000)
#define TEMPERATURE_TAG_PUB_VALUE_CHANGE 0.2f

#define UPDATE_NORMAL_INTERVAL             (5 * 1000)

// LED instance
twr_led_t led;

// Accelerometer
twr_lis2dh12_t acc;
twr_lis2dh12_result_g_t result;

float magnitude;
float last_magnitude;

twr_tick_t next_pub;

void lis2_event_handler(twr_lis2dh12_t *self, twr_lis2dh12_event_t event, void *event_param)
{
    if (event == TWR_LIS2DH12_EVENT_UPDATE)
    {

        twr_lis2dh12_get_result_g(&acc, &result);

        magnitude = pow(result.x_axis, 2) + pow(result.y_axis, 2) + pow(result.z_axis, 2);
        magnitude = sqrt(magnitude);

        if ((fabs(magnitude - last_magnitude) >= TEMPERATURE_TAG_PUB_VALUE_CHANGE) || (next_pub < twr_scheduler_get_spin_tick()))
        {
            // Make longer pulse when transmitting
            twr_led_pulse(&led, 100);

            twr_radio_pub_float("magnitude", &magnitude);
            last_magnitude = magnitude;
            next_pub = twr_scheduler_get_spin_tick() + TEMPERATURE_TAG_PUB_NO_CHANGE_INTEVAL;
        }


    }
}
void application_init(void)
{
    // Initialize logging
    //twr_log_init(TWR_LOG_LEVEL_DUMP, TWR_LOG_TIMESTAMP_ABS);

    // Initialize LED
    twr_led_init(&led, TWR_GPIO_LED, false, false);
    twr_led_set_mode(&led, TWR_LED_MODE_OFF);
    twr_led_pulse(&led, 1000);

    twr_radio_init(TWR_RADIO_MODE_NODE_SLEEPING);
    twr_radio_pairing_request("vibration-monitor", VERSION);


    twr_lis2dh12_init(&acc, TWR_I2C_I2C0, 0x19);
    twr_lis2dh12_set_event_handler(&acc, lis2_event_handler, NULL);
    twr_lis2dh12_set_update_interval(&acc, UPDATE_NORMAL_INTERVAL);
}

