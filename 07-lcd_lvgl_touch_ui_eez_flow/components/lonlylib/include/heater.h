/*
 * heater.h
 *
 *  Created on: 2024年7月5日
 *      Author: 23714
 */

#ifndef COMPONENTS_LONLYLIB_INCLUDE_HEATER_H_
#define COMPONENTS_LONLYLIB_INCLUDE_HEATER_H_

#include <stdint.h>

void heater_init(void);
void heater_set_duty(uint32_t duty);
#endif /* COMPONENTS_LONLYLIB_INCLUDE_HEATER_H_ */
