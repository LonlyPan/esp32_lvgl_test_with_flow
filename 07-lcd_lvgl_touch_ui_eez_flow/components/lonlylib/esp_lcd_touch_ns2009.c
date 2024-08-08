/*
 * esp_lcd_touch_ns2009.c
 *
 *  Created on: 2024年7月17日
 *      Author: 23714
 */
 #include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_ns2009.h"
#include "driver/i2c_master.h"

static const char *TAG = "NS2009";


/*******************************************************************************
* Public API functions
*******************************************************************************/
static esp_err_t touch_ns2009_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);
static int touch_ns2009_read_reg(esp_lcd_touch_handle_t tp, uint16_t reg);

static esp_err_t esp_lcd_touch_ns2009_read_data(esp_lcd_touch_handle_t tp)
{
    esp_err_t err;
    uint8_t buf[2];
    uint8_t touch_cnt = 1;
    size_t i = 0;

    assert(tp != NULL);

    //ESP_RETURN_ON_ERROR(touch_ns2009_i2c_read(tp, ESP_LCD_TOUCH_GT911_PRODUCT_ID_REG, (uint8_t *)&buf[0], 3), TAG, "GT911 read error!");


   // err = touch_ns2009_i2c_read(tp, NS2009_LOW_POWER_READ_Z1, (uint8_t *)&buf[0],2);
    //ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

	int press = touch_ns2009_read_reg(tp, NS2009_LOW_POWER_READ_Z1);
	if (press > 30)
    {
        int x, y = 0;

        x = touch_ns2009_read_reg(tp,NS2009_LOW_POWER_READ_X);
        y = touch_ns2009_read_reg(tp,NS2009_LOW_POWER_READ_Y);

        /* Fill all coordinates */
        for (i = 0; i < touch_cnt; i++) {
            tp->data.coords[i].x = x * tp->config.x_max / 4096;
            tp->data.coords[i].y = y * tp->config.y_max / 4096;
            tp->data.coords[i].strength = press;
        }
        tp->data.points = 1;
    }else{
		return ESP_OK;
    }

    return ESP_OK;
}
static bool esp_lcd_touch_ns2009_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    assert(tp != NULL);
    assert(x != NULL);
    assert(y != NULL);
    assert(point_num != NULL);
    assert(max_point_num > 0);

    portENTER_CRITICAL(&tp->data.lock);

    /* Count of points */
    *point_num = (tp->data.points > max_point_num ? max_point_num : tp->data.points);

    for (size_t i = 0; i < *point_num; i++) {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;

        if (strength) {
            strength[i] = tp->data.coords[i].strength;
        }
    }

    /* Invalidate */
    tp->data.points = 0;

    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

static esp_err_t esp_lcd_touch_ns2009_del(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    /* Reset GPIO pin settings */
    if (tp->config.int_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.int_gpio_num);
        if (tp->config.interrupt_callback) {
            gpio_isr_handler_remove(tp->config.int_gpio_num);
        }
    }
    free(tp);

    return ESP_OK;
}

static esp_err_t esp_lcd_touch_ns2009_enter_sleep(esp_lcd_touch_handle_t tp)
{
    return ESP_OK;
}

static esp_err_t esp_lcd_touch_ns2009_exit_sleep(esp_lcd_touch_handle_t tp)
{
    return ESP_OK;
}

esp_err_t esp_lcd_touch_new_i2c_ns2009(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *out_touch)
{
    esp_err_t ret = ESP_OK;

    assert(io != NULL);
    assert(config != NULL);
    assert(out_touch != NULL);

    /* Prepare main structure */
    esp_lcd_touch_handle_t esp_lcd_touch_ns2009 = heap_caps_calloc(1, sizeof(esp_lcd_touch_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(esp_lcd_touch_ns2009, ESP_ERR_NO_MEM, err, TAG, "no mem for NS2009 controller");

    /* Communication interface */
    esp_lcd_touch_ns2009->io = io;

    /* Only supported callbacks are set */
    esp_lcd_touch_ns2009->read_data = esp_lcd_touch_ns2009_read_data;
    esp_lcd_touch_ns2009->get_xy = esp_lcd_touch_ns2009_get_xy;
    esp_lcd_touch_ns2009->del = esp_lcd_touch_ns2009_del;
    esp_lcd_touch_ns2009->enter_sleep = esp_lcd_touch_ns2009_enter_sleep;
    esp_lcd_touch_ns2009->exit_sleep = esp_lcd_touch_ns2009_exit_sleep;

    /* Mutex */
    esp_lcd_touch_ns2009->data.lock.owner = portMUX_FREE_VAL;

    /* Save config */
    memcpy(&esp_lcd_touch_ns2009->config, config, sizeof(esp_lcd_touch_config_t));

    /* Prepare pin for touch interrupt */
    if (esp_lcd_touch_ns2009->config.int_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_INPUT,
            .intr_type = (esp_lcd_touch_ns2009->config.levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE),
            .pin_bit_mask = BIT64(esp_lcd_touch_ns2009->config.int_gpio_num)
        };
        ret = gpio_config(&int_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");

        /* Register interrupt callback */
        if (esp_lcd_touch_ns2009->config.interrupt_callback) {
            esp_lcd_touch_register_interrupt_callback(esp_lcd_touch_ns2009, esp_lcd_touch_ns2009->config.interrupt_callback);
        }
    }
err:
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (0x%x)! Touch controller GT911 initialization failed!", ret);
        if (esp_lcd_touch_ns2009) {
            esp_lcd_touch_ns2009_del(esp_lcd_touch_ns2009);
        }
    }

    *out_touch = esp_lcd_touch_ns2009;

    return ret;
}

/*******************************************************************************
* Private API function
*******************************************************************************/
//read 12bit data
static int touch_ns2009_read_reg(esp_lcd_touch_handle_t tp, uint16_t reg)
{

    
	esp_err_t err;
	uint8_t buf[2];
	/* Write data */
	
	//err =touch_ns2009_i2c_write(tp, 2);
    //ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");
    //err = i2c_master_receive(tp->io, buf, 2, -1);
	//static esp_err_t touch_ns2009_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data);
	err =touch_ns2009_i2c_read(tp, reg, (uint8_t *)&buf[0],2);
    ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");
   
    return (buf[0] << 4) | (buf[1] >> 4);
}

static esp_err_t touch_ns2009_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len)
{
    assert(tp != NULL);
    assert(data != NULL);

    /* Read data */
    return esp_lcd_panel_io_rx_param(tp->io, reg, data, len);
}

static esp_err_t touch_ns2009_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data)
{
    assert(tp != NULL);

    // *INDENT-OFF*
    return esp_lcd_panel_io_tx_param(tp->io, reg, (uint8_t[]){data}, 1);
    // *INDENT-ON*
}