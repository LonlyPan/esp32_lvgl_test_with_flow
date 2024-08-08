/*
 * key.c
 *
 *  Created on: 2024年6月28日
 *      Author: 23714
 */
#include "iot_button.h" 
#include "key.h"
#include "esp_pm.h"
#include "esp_log.h"
#include "esp_sleep.h"
/* Most development boards have "boot" button attached to GPIO0.
 * You can also change this to another pin.
 */
#define BUTTON_ACTIVE_LEVEL     0

static const char *TAG = "button_power_save";

const char *button_event_table[] = {
    "BUTTON_PRESS_DOWN",
    "BUTTON_PRESS_UP",
    "BUTTON_PRESS_REPEAT",
    "BUTTON_PRESS_REPEAT_DONE",
    "BUTTON_SINGLE_CLICK",
    "BUTTON_DOUBLE_CLICK",
    "BUTTON_MULTIPLE_CLICK",
    "BUTTON_LONG_PRESS_START",
    "BUTTON_LONG_PRESS_HOLD",
    "BUTTON_LONG_PRESS_UP",
};

static void button_event_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Button event %s", button_event_table[(button_event_t)data]);
    if((button_event_t)data == BUTTON_PRESS_DOWN)
    {
		ESP_LOGI(TAG, "BUTTON_PRESS_DOWN2");
	}
}
void key_init(uint32_t button_num)
{
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = button_num,
            .active_level = BUTTON_ACTIVE_LEVEL,
        },
    };
    button_handle_t btn = iot_button_create(&btn_cfg);
    assert(btn);
    esp_err_t err = iot_button_register_cb(btn, BUTTON_PRESS_DOWN, button_event_cb, (void *)BUTTON_PRESS_DOWN);
    ESP_ERROR_CHECK(err);
}



