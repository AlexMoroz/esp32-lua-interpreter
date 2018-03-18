/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */

#ifndef COMPONENTS_LCD_INCLUDE_LCD_H_
#define COMPONENTS_LCD_INCLUDE_LCD_H_

#define CONFIG_LCD1602_I2C_ADDRESS 0x27

#include "lua.h"

int init(lua_State *L);
int lcd_set_pointer(lua_State *L);
int lcd_write(lua_State *L);
int lcd_clear(lua_State *L);
int luaopen_lcd1602 (lua_State *L);


#endif /* COMPONENTS_LCD_INCLUDE_LCD_H_ */
