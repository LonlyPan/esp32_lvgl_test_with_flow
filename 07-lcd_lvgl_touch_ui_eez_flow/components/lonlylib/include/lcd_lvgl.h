/*
 * lcd.h
 *
 *  Created on: 2024年7月9日
 *      Author: 23714
 */

#ifndef COMPONENTS_LONLYLIB_INCLUDE_LCD_LVGL_H_
#define COMPONENTS_LONLYLIB_INCLUDE_LCD_LVGL_H_

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_st7796s.h"

extern esp_lcd_panel_io_handle_t io_handle;
extern esp_lcd_panel_handle_t panel_handle;
/* LCD settings */

#define LCD_DRAW_BUFF_DOUBLE (1)
#define LCD_DRAW_BUFF_HEIGHT (50)
#define LCD_BL_ON_LEVEL     (1)

/* LCD pins */

// ESP32 S3 LCD的SPI接口选择
#define LCD_SPI_HOST   SPI2_HOST

// 使用默认引脚可以使用IOMUX将GPIO翻转速率提高到80MHz，GPIO矩阵不建议超过26MHz
// ESP32 S3 LCD的SPI接口选择
/* 芯片类型  |  CLK    | MOSI   | MISO |   CS   |  DC   |  RST   | BLK   |
| -------- | ------ | ------ |
| ESP32 C3 |   6    |   7    |  2   |   10   |  9    |   8    ｜  5   ｜*/
#define LCD_SPI_MISO_PIN_NUM    GPIO_NUM_NC  // 部分屏幕没有该引脚
#define LCD_SPI_MOSI_PIN_NUM    8  // SPI2默认的MOSI引脚
#define LCD_SPI_CLK_PIN_NUM     6  // SPI2默认的CLK引脚
#define LCD_SPI_CS_PIN_NUM      7  // SPI2默认的CS引脚

#define LCD_DC_PIN_NUM   9    // D/C线，0-命令，1-数据
#define LCD_RST_PIN_NUM  GPIO_NUM_NC    // RST复位引脚。-1为空置不使用。但LCD的RST引脚应上拉接高电平。
#define LCD_BLK_PIN_NUM 3    // 背光BLK引脚。0-关闭，1-打开

#define LCD_LEDC_CH            LEDC_CHANNEL_0
/**
 * @brief LCD屏幕像素分辨率定义。（需要用户手动配置）
 *        只能保留一个，其余的都注释，做备选
 */
#define LCD_H_RES             (480)  // 横向，正对屏幕，左右像素
#define LCD_V_RES             (320)  // 竖向，正对屏幕，上下像素


#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)  // MAX 80Mhz   //设置像素时钟的频率，单位为Hz。该值不应超过LCD规格中建议的范围。

/* LCD display color bits */
#define LCD_BITS_PER_PIXEL      (16)
/* LCD display color space */
#define LCD_COLOR_SPACE         (ESP_LCD_COLOR_SPACE_BGR)

esp_err_t lcd_lvgl_lcd_init(esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io);
esp_err_t lcd_lvgl_touch_init(void);
void lcd_lvgl_init();


esp_err_t bsp_display_brightness_init(void);
/**
 * @brief Set display's brightness
 *
 * Brightness is controlled with PWM signal to a pin controling backlight.
 *
 * @param[in] brightness_percent Brightness in [%]
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Parameter error
 */
esp_err_t bsp_display_brightness_set(int brightness_percent);

/**
 * @brief Turn on display backlight
 *
 * Display must be already initialized by calling bsp_display_start()
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Parameter error
 */
esp_err_t bsp_display_backlight_on(void);

/**
 * @brief Turn off display backlight
 *
 * Display must be already initialized by calling bsp_display_start()
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Parameter error
 */
esp_err_t bsp_display_backlight_off(void);
#endif /* COMPONENTS_LONLYLIB_INCLUDE_LCD_LVGL_H_ */
