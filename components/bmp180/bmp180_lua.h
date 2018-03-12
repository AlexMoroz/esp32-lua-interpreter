#ifndef COMPONENTS_BMP180_BMP180_LUA_H_
#define COMPONENTS_BMP180_BMP180_LUA_H_

#include "lua.h"

#define I2C_DATA_PIN (32)
#define I2C_CLOCK_PIN (33)

int init_i2c(lua_State *L);
int bmp180_get_values(lua_State *L);
int luaopen_bmp180 (lua_State *L);

#endif /* COMPONENTS_BMP180_BMP180_LUA_H_ */
