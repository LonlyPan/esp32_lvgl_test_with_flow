#include "ui.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/unistd.h>
#include "lonlylib.h"
#include <esp_log.h>



#include "esp_lvgl_port.h"


#define KEY_GPIO GPIO_NUM_9
#define LED_GPIO GPIO_NUM_3
#define BUZZER_GPIO GPIO_NUM_4

extern "C" void app_main(void)
{	
	esp_log_level_set("*", ESP_LOG_DEBUG);

	int i = 0;
	// 配置LED为推挽输出，下拉，初始电平为0
	//key_init(KEY_GPIO);
	//led_init();
	//heater_init();
	//xTaskCreate(printf_task, "printf_task", 1024, NULL, 1, NULL);
	printf("lcd_panel start!\n");
	lcd_lvgl_init();
	lvgl_port_lock(0);
	ui_init();
    lvgl_port_unlock();
	printf("lcd_panel end!\n");
	while(1) {

		i++;
		if(i>8192)
			i=0;
		//heater_set_duty(i);
		//led_set_brightness(i);
		printf("lcd_panel start! %d \n",i);
		    /* Task lock */
	    //lvgl_port_lock(0);
	    
	    //ui_();      /* A widgets example */
	        /* Task unlock */
	    //lvgl_port_unlock();
		vTaskDelay(1000/portTICK_PERIOD_MS);
		//usleep(1000);
	}

}
