/*
 * lcd.c
 *
 *  Created on: 2024年7月9日
 *      Author: 23714
 */
//#include "core/lv_obj_style_gen.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include <esp_log.h>
#include "esp_random.h"
#include "esp_err.h"
#include "esp_check.h"
#include "unity.h"
#include "string.h"
#include <stdbool.h>
#include <unistd.h>
#include "driver/ledc.h"
#include <lcd_lvgl.h>
#include "esp_lvgl_port.h"
#include "esp_lcd_touch_ns2009.h"
#include "driver/i2c_master.h"
#include "driver/i2c.h"
#include "lv_demos.h"
esp_lcd_panel_io_handle_t lcd_io_handle = NULL;
esp_lcd_panel_handle_t lcd_panel_handle = NULL;

#define EXAMPLE_TOUCH_I2C_NUM       (0)
// LVGL image declare
//LV_IMG_DECLARE(esp_logo)
/* Touch settings */
#define EXAMPLE_TOUCH_I2C_NUM       (0)
#define EXAMPLE_TOUCH_I2C_CLK_HZ    (400000)

/* LCD touch pins */
#define EXAMPLE_TOUCH_I2C_SCL       (GPIO_NUM_20)
#define EXAMPLE_TOUCH_I2C_SDA       (GPIO_NUM_21)
#define EXAMPLE_TOUCH_GPIO_INT      (GPIO_NUM_NC)

esp_lcd_touch_handle_t touch_handle = NULL;

/* LVGL display and touch */
lv_display_t *lvgl_disp = NULL;
lv_indev_t *lvgl_touch_indev = NULL;

static const char *TAG = "lcd_lvgl";



/**
 * @brief  屏幕初始化
 *      - 
 * @return
 *     - none
 */
esp_err_t lcd_lvgl_lcd_init(esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
	ESP_LOGI(TAG, "Initialize Brightness");
    esp_err_t ret= ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Brightness init failed");
    
    // 初始化SPI总线
    // 配置 MISO、MOSI、CLK、CS 的引脚，和DMA最大传输字节数
    ESP_LOGI(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = ST7796s_PANEL_BUS_SPI_CONFIG(LCD_SPI_CLK_PIN_NUM, LCD_SPI_MOSI_PIN_NUM, LCD_H_RES * LCD_DRAW_BUFF_HEIGHT * LCD_BITS_PER_PIXEL / 8);
    ESP_RETURN_ON_ERROR(spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO),TAG, "SPI init failed");
    
    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = ST7796s_PANEL_IO_SPI_CONFIG(LCD_SPI_CS_PIN_NUM, LCD_DC_PIN_NUM,NULL, NULL);
    // Attach the LCD to the SPI bus
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &io_config,ret_io), err, TAG, "New panel IO failed");

    ESP_LOGD(TAG, "Install LCD driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_RST_PIN_NUM, // Shared with Touch reset
        .rgb_endian = LCD_COLOR_SPACE,
        .bits_per_pixel = LCD_BITS_PER_PIXEL,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7796s(*ret_io, &panel_config, ret_panel), err, TAG, "New panel failed");
    
    esp_lcd_panel_reset(*ret_panel);
    esp_lcd_panel_init(*ret_panel);
    //esp_lcd_panel_mirror(*ret_panel, false, true);
    //esp_lcd_panel_swap_xy(*ret_panel, true);
    //esp_lcd_panel_invert_color(*ret_panel, true);
    // 横屏
    //esp_lcd_panel_set_gap(*ret_panel,40,53);// the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    // 竖屏
    //esp_lcd_panel_set_gap(*ret_panel, 52,40);
     esp_lcd_panel_disp_on_off(*ret_panel, true);
        // turn on backlight
	bsp_display_backlight_on();
    return ret;

err:
	if (*ret_panel) {
		esp_lcd_panel_del(*ret_panel);
	}
	if (*ret_io) {
		esp_lcd_panel_io_del(*ret_io);
	}
	spi_bus_free(LCD_SPI_HOST);
	return ret;
}
#if 1
esp_err_t lcd_lvgl_touch_init(void)
{
	    /* Initilize I2C */
    const i2c_master_bus_config_t i2c_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = EXAMPLE_TOUCH_I2C_SDA,
        .scl_io_num = EXAMPLE_TOUCH_I2C_SCL,
        .i2c_port = I2C_NUM_0,   // -1 自动选择端口 I2C_NUM_0
        .flags.enable_internal_pullup = true,
        //.glitch_ignore_cnt = 7,
    };
    
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_conf, &bus_handle));
/*
    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = EXAMPLE_TOUCH_I2C_SDA,
        .scl_io_num = EXAMPLE_TOUCH_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0));
*/
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_NS2009_CONFIG();
    /*
    esp_lcd_panel_io_i2c_config_t tp_io_config = {
        .dev_addr = 0x48,
        .scl_speed_hz = TEST_LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1, // According to SSD1306 datasheet
        .dc_bit_offset = 6,       // According to SSD1306 datasheet
        .lcd_cmd_bits = 8,        // According to SSD1306 datasheet
        .lcd_param_bits = 8,      // According to SSD1306 datasheet
    };*/
    //ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_0, &tp_io_config, &tp_io_handle), TAG, "");
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(bus_handle, &tp_io_config, &tp_io_handle), TAG, "");
    
    /* Initialize touch HW */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_V_RES,
        .y_max = LCD_H_RES,
        .rst_gpio_num = GPIO_NUM_NC, // Shared with LCD reset
        .int_gpio_num = GPIO_NUM_NC,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 1,
            .mirror_x = 0,
            .mirror_y = 1,
        },
    };

    return esp_lcd_touch_new_i2c_ns2009(tp_io_handle, &tp_cfg, &touch_handle);
}
#endif
esp_err_t bsp_display_brightness_init(void)
{
    // Setup LEDC peripheral for PWM backlight control
    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num = LCD_BLK_PIN_NUM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = 1,
        .duty = 0,
        .hpoint = 0
    };
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = 1,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&LCD_backlight_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&LCD_backlight_channel));

    return ESP_OK;
}
esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }
    if (brightness_percent < 0) {
        brightness_percent = 0;
    }

    ESP_LOGI(TAG, "Setting LCD backlight: %d%%", brightness_percent);
    uint32_t duty_cycle = (1023 * (brightness_percent)) / 100; // LEDC resolution set to 10bits, thus: 100% = 1023
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH));

    return ESP_OK;
}

esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(50);
}

esp_err_t lcd_lvgl_lvgl_init(esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = *ret_io,
        .panel_handle = *ret_panel,
        .buffer_size = LCD_H_RES * LCD_DRAW_BUFF_HEIGHT,
        .double_buffer = LCD_DRAW_BUFF_DOUBLE,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
#if LVGL_VERSION_MAJOR >= 9
        .color_format = LV_COLOR_FORMAT_RGB565,
#endif
        .rotation = {
            .swap_xy = true,
            .mirror_x = true,
            .mirror_y = true,
        },
        .flags = {
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,
#endif
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    /* Add touch input (for selected screen) */
 
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = touch_handle,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);
    return ESP_OK;
}
static void _app_button_cb(lv_event_t *e)
{
    lv_disp_rotation_t rotation = lv_disp_get_rotation(lvgl_disp);
    rotation++;
    if (rotation > LV_DISPLAY_ROTATION_270) {
        rotation = LV_DISPLAY_ROTATION_0;
    }

    /* LCD HW rotation */
    lv_disp_set_rotation(lvgl_disp, rotation);
}

static void lcd_lvgl_test(void)
{
    lv_obj_t *scr = lv_scr_act();

    /* Task lock */
    lvgl_port_lock(0);

    /* Your LVGL objects code here .... */

    /* Label */
    lv_obj_t *label = lv_label_create(scr);
    lv_obj_set_width(label, LCD_H_RES);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_color_t color = lv_palette_main(LV_PALETTE_RED);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
#if LVGL_VERSION_MAJOR == 8
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, "#FF0000 "LV_SYMBOL_BELL" Hello world Espressif and LVGL "LV_SYMBOL_BELL"#\n#FF9400 "LV_SYMBOL_WARNING" For simplier initialization, use BSP "LV_SYMBOL_WARNING" #");
#else
    lv_label_set_text(label, LV_SYMBOL_BELL" Hello world Espressif and LVGL "LV_SYMBOL_BELL"\n "LV_SYMBOL_WARNING" For simplier initialization, use BSP "LV_SYMBOL_WARNING);
	
#endif
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);

    /* Button */
    lv_obj_t *btn = lv_btn_create(scr);
    label = lv_label_create(btn);
    lv_label_set_text_static(label, "Rotate screen");
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -30);
	lv_obj_add_event_cb(btn, _app_button_cb, LV_EVENT_CLICKED, NULL);
    /* Task unlock */
    lvgl_port_unlock();
}


void lcd_lvgl_init()
{
	ESP_ERROR_CHECK(lcd_lvgl_lcd_init(&lcd_panel_handle, &lcd_io_handle));
	    /* Touch initialization */
    ESP_ERROR_CHECK(lcd_lvgl_touch_init());
	ESP_ERROR_CHECK(lcd_lvgl_lvgl_init(&lcd_panel_handle, &lcd_io_handle));
	//lcd_lvgl_test();
	
    /* Task lock */
    //lvgl_port_lock(0);
    
   // lv_demo_widgets();      /* A widgets example */
        /* Task unlock */
    //lvgl_port_unlock();
}