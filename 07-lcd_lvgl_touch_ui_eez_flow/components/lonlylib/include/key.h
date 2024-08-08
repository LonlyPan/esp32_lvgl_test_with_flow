/*
 * key.h
 *
 *  Created on: 2024年6月28日
 *      Author: 23714
 */

#ifndef COMPONENTS_LONLYLIB_INCLUDE_KEY_H_
#define COMPONENTS_LONLYLIB_INCLUDE_KEY_H_


#include "driver/gpio.h"
#include <unistd.h>  // sleep库

void key_init(uint32_t button_num);  //按键IO初始化函数


#endif /* COMPONENTS_LONLYLIB_INCLUDE_KEY_H_ */
