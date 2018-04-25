/*
 *  Created on: 29 Mar 2018
 *      Author: Alex Moroz
 */

#include "esp_log.h"
#include "esp_err.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "ultrasonic.h"

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "delay.h"


int get_distance(lua_State *L) {

	//trigger a signal on the trigger pin
	gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(TRIG_PIN, 0);
	udelay(2);
	gpio_set_level(TRIG_PIN, 1);
	udelay(10);
	gpio_set_level(TRIG_PIN, 0);


	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_MAX);
	int val = adc1_get_raw(ECHO_PIN);

	printf("%d", val);

	return 0;
}

static const struct luaL_Reg ultrasonic [] = {
        {"get", get_distance},
        {NULL, NULL}  /* sentinel */
};

int luaopen_ultrasonic (lua_State *L) {
        luaL_newlib(L, ultrasonic);
        return 1;
}
