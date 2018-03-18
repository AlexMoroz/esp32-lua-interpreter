/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */

#ifndef COMPONENTS_GAS_SENSOR_INCLUDE_GAS_H_
#define COMPONENTS_GAS_SENSOR_INCLUDE_GAS_H_

#include "lua.h"
#include "driver/adc.h"
#include "driver/gpio.h"

#define GAS_SENSOR_GPIO GPIO_NUM_5
#define GAS_SENSOR_ADC ADC1_CHANNEL_0

int read_gas_value(lua_State *L);
int luaopen_gas (lua_State *L);

#endif /* COMPONENTS_GAS_SENSOR_INCLUDE_GAS_H_ */
