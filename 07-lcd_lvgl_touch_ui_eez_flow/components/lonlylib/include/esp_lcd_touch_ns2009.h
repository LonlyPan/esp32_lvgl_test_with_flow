/*
 * esp_lcd_touch_ns2009.h
 *
 *  Created on: 2024年7月17日
 *      Author: 23714
 */
#include "esp_lcd_touch.h"

#ifndef COMPONENTS_LONLYLIB_INCLUDE_ESP_LCD_TOUCH_NS2009_H_
#define COMPONENTS_LONLYLIB_INCLUDE_ESP_LCD_TOUCH_NS2009_H_

#define ESP_LCD_TOUCH_I2C_NS2009_ADDRESS 0x48

#define NS2009_LOW_POWER_READ_X 0xc0
#define NS2009_LOW_POWER_READ_Y 0xd0
#define NS2009_LOW_POWER_READ_Z1 0xe0

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t esp_lcd_touch_new_i2c_ns2009(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *out_touch);

/**
 * @brief Touch IO configuration structure
 *
 */
#define ESP_LCD_TOUCH_IO_I2C_NS2009_CONFIG()           \
    {                                       \
        .dev_addr = ESP_LCD_TOUCH_I2C_NS2009_ADDRESS, \
        .scl_speed_hz = 100000,   \
        .control_phase_bytes = 1,           \
        .dc_bit_offset = 0,                 \
        .lcd_cmd_bits = 8,                  \
        .flags =                            \
        {                                   \
            .disable_control_phase = 1,     \
        }                                   \
    }
#endif /* COMPONENTS_LONLYLIB_INCLUDE_ESP_LCD_TOUCH_NS2009_H_ */

#ifdef __cplusplus
}
#endif