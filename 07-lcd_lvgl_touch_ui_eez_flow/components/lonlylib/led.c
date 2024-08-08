/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <led.h>
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_idf_version.h"
#include "esp_log.h"


#define GPIO_LED_PIN       GPIO_NUM_3
#define GPIO_ACTIVE_LEVEL  1
#define LEDC_LED_CHANNEL   LEDC_CHANNEL_0

static const char *TAG = "led_ledc";
static led_indicator_handle_t led_handle = NULL;

/**
 * @brief Brightness set to 25% with a priority level of 3.
 *
 */
static const blink_step_t bright_25_percent[] = {
    {LED_BLINK_BRIGHTNESS, LED_STATE_OFF, 0},
    {LED_BLINK_STOP, 0, 0},
};

blink_step_t const *led_mode[] = {
    [0] = bright_25_percent,
};

void led_init(void)
{
    led_indicator_ledc_config_t ledc_config = {
        .is_active_level_high = GPIO_ACTIVE_LEVEL,
        .timer_inited = false,
        .timer_num = LEDC_TIMER_0,
        .gpio_num = GPIO_LED_PIN,
        .channel = LEDC_LED_CHANNEL,
    };

    const led_indicator_config_t config = {
        .mode = LED_LEDC_MODE,
        .led_indicator_ledc_config = &ledc_config,
        .blink_lists = led_mode,
        .blink_list_num = 1,
    };

    led_handle = led_indicator_create(&config);
    assert(led_handle != NULL);

    led_indicator_start(led_handle, 0);
    ESP_LOGI(TAG, "start blink");
    //vTaskDelay(4000 / portTICK_PERIOD_MS);
    //led_indicator_stop(led_handle, i);
}

void led_set_brightness(uint32_t brightness)
{
	led_indicator_set_brightness(led_handle, brightness);
}
