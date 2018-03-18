/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */
#include "lcd.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "rom/uart.h"

#include "i2c.h"
#include "smbus.h"
#include "i2c-lcd1602.h"

static i2c_lcd1602_info_t * lcd_info;
static smbus_info_t * smbus_info;

int init(lua_State *L) {
	i2c_port_t i2c_num = I2C_MASTER_NUM;
	uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;

	// Set up the SMBus
	smbus_info = smbus_malloc();
	smbus_init(smbus_info, i2c_num, address);
	smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

	// Set up the LCD1602 device with backlight off
	lcd_info = i2c_lcd1602_malloc();
	i2c_lcd1602_init(lcd_info, smbus_info, true);
	return 0;
}

int lcd_set_pointer(lua_State *L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);

	i2c_lcd1602_set_cursor(lcd_info, true);
	i2c_lcd1602_move_cursor(lcd_info, x, y);

	return 0;
}

int lcd_write(lua_State *L) {
	char *s = luaL_checkstring(L, 1);

	i2c_lcd1602_write_string(lcd_info, s);
	return 0;
}

int lcd_clear(lua_State *L) {
	i2c_lcd1602_clear(lcd_info);
	return 0;
}

static const struct luaL_Reg lcd[] = {
		{ "init", init },
		{ "set", lcd_set_pointer},
		{ "write", lcd_write},
		{ "clear", lcd_clear},
		{ NULL, NULL } /* sentinel */
};

int luaopen_lcd1602 (lua_State *L) {
        luaL_newlib(L, lcd);
        return 1;
}
