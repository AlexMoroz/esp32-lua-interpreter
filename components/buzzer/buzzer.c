/*
 Copyright (c) 2017 Teemu Kärkkäinen

 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freq.h"

#include "buzzer.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static const char* TAG = "Buzzer";

int play_song(lua_State *L) {

	ledc_timer_config_t buzzer_timer = {
	        //set timer counter bit number
	        .duty_resolution = LEDC_TIMER_12_BIT,
	        //set frequency of pwm
	        .freq_hz = 1000,
	        //timer mode,
	        .speed_mode = LEDC_HIGH_SPEED_MODE,
	        //timer index
	        .timer_num = LEDC_TIMER_1
	    };
	ledc_timer_config( &buzzer_timer );

	ledc_channel_config_t buzzer_channel = {
			.channel = LEDC_CHANNEL_3,
			.gpio_num = GPIO_BUZZER,
			.duty = DUTY_MAX / 2,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.timer_sel = LEDC_TIMER_1 };
	ledc_channel_config(&buzzer_channel);

	for (int song_i=0; song_i < BWV147_LENGTH; song_i++) {
		esp_err_t song_err = ledc_set_freq(buzzer_channel.speed_mode,
				buzzer_channel.timer_sel, BWV147[song_i]);
		if (song_err == ESP_ERR_INVALID_ARG) {
			ESP_LOGI(TAG, "ledc_set_freg -> ESP_ERR_INVALID_ARG");
		} else if (song_err == ESP_FAIL) {
			ESP_LOGI(TAG, "ledc_set_freg -> ESP_FAIL");
		}

		vTaskDelay(300 / portTICK_PERIOD_MS);
		ledc_timer_pause(buzzer_channel.speed_mode, buzzer_channel.timer_sel);
		vTaskDelay(30 / portTICK_PERIOD_MS);
		ledc_timer_resume(buzzer_channel.speed_mode, buzzer_channel.timer_sel);
	}
	ledc_timer_pause(buzzer_channel.speed_mode, buzzer_channel.timer_sel);
	return 0;
}

static const struct luaL_Reg buzzer [] = {
        {"play", play_song},
        {NULL, NULL}  /* sentinel */
};

int luaopen_buzzer (lua_State *L) {
        luaL_newlib(L, buzzer);
        return 1;
}
