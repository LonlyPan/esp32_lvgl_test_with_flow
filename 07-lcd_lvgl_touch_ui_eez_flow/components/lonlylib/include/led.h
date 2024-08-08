/*
 * led.h
 *
 *  Created on: 2024年6月27日
 *      Author: 23714
 */

#ifndef COMPONENTS_LONLYLIB_INCLUDE_LED_H_
#define COMPONENTS_LONLYLIB_INCLUDE_LED_H_

#include "driver/gpio.h"
#include "led_indicator.h"

void led_init(void);
void led_set_brightness(uint32_t brightness);


#endif /* COMPONENTS_LONLYLIB_INCLUDE_LED_H_ */
