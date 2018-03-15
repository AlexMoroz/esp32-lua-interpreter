/*
 Copyright (c) 2017 Teemu Kärkkäinen

 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "rgb.h"
#include "buzzer.h"

typedef struct {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} color_t;

uint32_t color_to_duty(uint16_t color) {
	return DUTY_MAX - color * 16;
}

void setup(int red, int green, int blue, int off) {
	/* Configure timer for PWM */
	    ledc_timer_config_t ledc_timer = {
	        //set timer counter bit number
	        .duty_resolution = LEDC_TIMER_12_BIT,
	        //set frequency of pwm
	        .freq_hz = 1000,
	        //timer mode,
	        .speed_mode = LEDC_HIGH_SPEED_MODE,
	        //timer index
	        .timer_num = LEDC_TIMER_0
	    };
		ledc_timer_config(&ledc_timer);

		ledc_channel_config_t ledc_channel[ 3 ] = {
		        {
		            .channel    = LEDC_CHANNEL_0,
		            .duty       = DUTY_MAX,
		            .gpio_num   = GPIO_RED,
		            .speed_mode = LEDC_HIGH_SPEED_MODE,
		            .timer_sel  = LEDC_TIMER_0
		        },
		        {
		            .channel    = LEDC_CHANNEL_1,
		            .duty       = DUTY_MAX,
		            .gpio_num   = GPIO_GREEN,
		            .speed_mode = LEDC_HIGH_SPEED_MODE,
		            .timer_sel  = LEDC_TIMER_0
		        },
		        {
		            .channel    = LEDC_CHANNEL_2,
		            .duty       = DUTY_MAX,
		            .gpio_num   = GPIO_BLUE,
		            .speed_mode = LEDC_HIGH_SPEED_MODE,
		            .timer_sel  = LEDC_TIMER_0
		        },
		    };

		//set the configuration
		ledc_channel_config(&ledc_channel[ COLOR_RED]);
		ledc_channel_config(&ledc_channel[ COLOR_GREEN]);
		ledc_channel_config(&ledc_channel[ COLOR_BLUE]);

		// RGB
		ledc_set_duty( ledc_channel[ COLOR_RED ].speed_mode, ledc_channel[ COLOR_RED ].channel,
		color_to_duty( red ) );
		ledc_update_duty( ledc_channel[ COLOR_RED ].speed_mode, ledc_channel[ COLOR_RED ].channel );

		ledc_set_duty( ledc_channel[ COLOR_GREEN ].speed_mode, ledc_channel[ COLOR_GREEN ].channel,
		color_to_duty( green ) );
		ledc_update_duty( ledc_channel[ COLOR_GREEN ].speed_mode, ledc_channel[ COLOR_GREEN ].channel );

		ledc_set_duty( ledc_channel[ COLOR_BLUE ].speed_mode, ledc_channel[ COLOR_BLUE ].channel,
	    color_to_duty( blue ) );
	    ledc_update_duty( ledc_channel[ COLOR_BLUE ].speed_mode, ledc_channel[ COLOR_BLUE ].channel );

	    if(off) {
	    		ledc_stop(ledc_channel[ COLOR_RED ].speed_mode, ledc_channel[ COLOR_RED ].channel, 1);
	    		ledc_stop(ledc_channel[ COLOR_GREEN ].speed_mode, ledc_channel[ COLOR_GREEN ].channel, 1);
	    		ledc_stop(ledc_channel[ COLOR_BLUE ].speed_mode, ledc_channel[ COLOR_BLUE ].channel, 1);
	    }
}

int set_color(lua_State *L) {
	int red = luaL_checkinteger(L, 1);
	if (red > 255)
		red %= 255;
	int green = luaL_checkinteger(L, 2);
	if (green > 255)
		green %= 255;
	int blue = luaL_checkinteger(L, 3);
	if (blue > 255)
		blue %= 255;

	lua_pushinteger(L, red);
	lua_rawsetp(L, LUA_REGISTRYINDEX, "static_rgb_red");
	lua_pushinteger(L, green);
	lua_rawsetp(L, LUA_REGISTRYINDEX, "static_rgb_green");
	lua_pushinteger(L, blue);
	lua_rawsetp(L, LUA_REGISTRYINDEX, "static_rgb_blue");

	setup(red, green, blue, 0);
	return 0;
}

int turn_on(lua_State *L) {
	int red, green, blue;

	lua_rawgetp(L, LUA_REGISTRYINDEX, "static_rgb_red");
	red = lua_tointeger(L, -1);
	lua_rawgetp(L, LUA_REGISTRYINDEX, "static_rgb_green");
	green = lua_tointeger(L, -1);
	lua_rawgetp(L, LUA_REGISTRYINDEX, "static_rgb_blue");
	blue = lua_tointeger(L, -1);

	setup(red, green, blue, 0);
	return 0;
}

int turn_off(lua_State *L) {
	setup(0,0,0,1);
	return 0;
}


static const struct luaL_Reg rgb [] = {
        {"set", set_color},
		{"on", turn_on},
		{"off", turn_off},
        {NULL, NULL}  /* sentinel */
};

int luaopen_rgb (lua_State *L) {
        luaL_newlib(L, rgb);
        return 1;
}
