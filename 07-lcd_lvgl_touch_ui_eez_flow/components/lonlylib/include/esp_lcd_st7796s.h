/*
 * esp_lcd_st7796s.h
 *
 *  Created on: 2024年7月8日
 *      Author: 23714
 */

#ifndef COMPONENTS_LONLYLIB_INCLUDE_ESP_LCD_ST7796S_H_
#define COMPONENTS_LONLYLIB_INCLUDE_ESP_LCD_ST7796S_H_



/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file
 * @brief ESP LCD: ST7796
 */

#pragma once

#include "hal/lcd_types.h"
#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_LCD_st7796s_VER_MAJOR 1
#define ESP_LCD_st7796s_VER_MINOR 0
#define ESP_LCD_st7796s_VER_PATCH 0
/**
 * @brief LCD panel initialization commands.
 *
 */
typedef struct {
    int cmd;                /*<! The specific LCD command */
    const void *data;       /*<! Buffer that holds the command specific data */
    size_t data_bytes;      /*<! Size of `data` in memory, in bytes */
    unsigned int delay_ms;  /*<! Delay in milliseconds after this command */
} st7796s_lcd_init_cmd_t;

/**
 * @brief LCD panel vendor configuration.
 *
 * @note  This structure needs to be passed to the `vendor_config` field in `esp_lcd_panel_dev_config_t`.
 *
 */
typedef struct {
    const st7796s_lcd_init_cmd_t *init_cmds;     /*!< Pointer to initialization commands array. Set to NULL if using default commands.
                                                 *   The array should be declared as `static const` and positioned outside the function.
                                                 *   Please refer to `vendor_specific_init_default` in source file.
                                                 */
    uint16_t init_cmds_size;                    /*<! Number of commands in above array */
} st7796s_vendor_config_t;

/**
 * @brief Create LCD panel for model st7796s
 *
 * @note  Vendor specific initialization can be different between manufacturers, should consult the LCD supplier for initialization sequence code.
 *
 * @param[in] io LCD panel IO handle
 * @param[in] panel_dev_config general panel device configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_st7796s(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);
/**
 * @brief LCD panel SPI bus configuration structure
 *
 * @param[in] sclk CLK引脚定义
 * @param[in] mosi MOSI引脚定义
 * @param[in] max_trans_sz // 最大传输字节数，非DMA最大64bytes,DMA最大4096bytes 
 *            传入该函数的图像缓存的字节数可以大于 max_transfer_sz，此时 esp_lcd 驱动内部会根据 SPI 单次 DMA 传输允许的最大字节数进行分包处理。
 * @note 没有MISO引脚
 */
#define ST7796s_PANEL_BUS_SPI_CONFIG(sclk, mosi, max_trans_sz)  \
    {                                                           \
        .sclk_io_num = sclk,                                    \
        .mosi_io_num = mosi,                                    \
        .miso_io_num = -1,                                      \
        .quadhd_io_num = -1,                                    \
        .quadwp_io_num = -1,                                    \
        .max_transfer_sz = max_trans_sz,                        \
    }
/**
 * @brief LCD panel IO configuration structure
 *
 * @param[in] cs 片选引脚Maximum bytes of data to be transferred in one transaction. Normally set to the size of one frame bytes.
 * @param[in] dc data/command pin number
 * @param[in] cb 颜色数据传输完成后调用的回调，没有就填NULL
 * @param[in] cb_ctx 用户私有数据，作为参数直接传递给 cb 回调函数，没有就填NULL
 *（注意：普通GPIO最大只能30MHz，而IOMUX默认的SPI-IO，CLK最大可以设置到80MHz）
 */
#define ST7796s_PANEL_IO_SPI_CONFIG(cs, dc, cb, cb_ctx)         \
    {                                                           \
        .cs_gpio_num = cs,                                      \
        .dc_gpio_num = dc,                                      \
        .spi_mode = 0,                                          \
        .pclk_hz = 40 * 1000 * 1000,                            \
        .trans_queue_depth = 10,                                \
        .on_color_trans_done = cb,                              \
        .user_ctx = cb_ctx,                                     \
        .lcd_cmd_bits = 8,                                      \
        .lcd_param_bits = 8,                                    \
    }
/**
 * @brief LCD panel bus configuration structure
 *
 * @param[in] max_trans_bytes Maximum bytes of data to be transferred in one transaction. Normally set to the size of one frame bytes.
 * @param[in] data_width Data bus width
 * @param[in] dc I80 data/command pin number
 * @param[in] wr I80 write clock pin number
 * @param[in] d[0:15] I80 data pin number 0 ~ 15. Set to -1 if not used.
 *
 */
#define st7796s_PANEL_BUS_I80_CONFIG(max_trans_bytes, data_width, dc, wr, d0, d1, d2, d3, d4,    \
                                    d5, d6, d7, d8 , d9, d10, d11, d12, d13, d14, d15)          \
    {                                                                                           \
        .clk_src = LCD_CLK_SRC_PLL160M,                                                         \
        .dc_gpio_num = dc,                                                                      \
        .wr_gpio_num = wr,                                                                      \
        .data_gpio_nums = {                                                                     \
            d0, d1, d2, d3, d4, d5, d6, d7, d8 , d9, d10, d11, d12, d13, d14, d15               \
        },                                                                                      \
        .bus_width = data_width,                                                                \
        .max_transfer_bytes = max_trans_bytes,                                                  \
        .psram_trans_align = 64,                                                                \
        .sram_trans_align = 4,                                                                  \
    }
/**
 * @brief LCD panel IO configuration structure
 *
 * @param[in] cs I80 chip select pin number
 * @param[in] cb Callback function when data transfer is done
 * @param[in] cb_ctx Callback function context
 *
 */
#define st7796s_PANEL_IO_I80_CONFIG(cs, cb, cb_ctx)  \
    {                                               \
        .cs_gpio_num = cs,                          \
        .pclk_hz = 10 * 1000 * 1000,                \
        .trans_queue_depth = 10,                    \
        .dc_levels = {                              \
            .dc_idle_level = 0,                     \
            .dc_cmd_level = 0,                      \
            .dc_dummy_level = 0,                    \
            .dc_data_level = 1,                     \
        },                                          \
        .flags = {                                  \
            .swap_color_bytes = 1,                  \
        },                                          \
        .on_color_trans_done = cb,                  \
        .user_ctx = cb_ctx,                         \
        .lcd_cmd_bits = 8,                          \
        .lcd_param_bits = 8,                        \
    }

#ifdef __cplusplus
}
#endif


#endif /* COMPONENTS_LONLYLIB_INCLUDE_ESP_LCD_st7796S_H_ */
