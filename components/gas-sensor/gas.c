/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */
#include "driver/adc.h"
#include "driver/gpio.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "gas.h"

int read_gas_value(lua_State *L) {
	gpio_set_direction(GAS_SENSOR_GPIO, GPIO_MODE_INPUT);

	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db);

	int level = gpio_get_level(GAS_SENSOR_GPIO);
	int val = adc1_get_voltage(ADC1_CHANNEL_0);

	lua_pushnumber(L, level);
	lua_pushnumber(L, val);
	return 2;
}

static const struct luaL_Reg gas [] = {
        {"get", read_gas_value},
        {NULL, NULL}  /* sentinel */
};

int luaopen_gas (lua_State *L) {
        luaL_newlib(L, gas);
        return 1;
}


