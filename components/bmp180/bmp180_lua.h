#ifndef COMPONENTS_BMP180_BMP180_LUA_H_
#define COMPONENTS_BMP180_BMP180_LUA_H_

#include "lua.h"

int bmp180_get_values(lua_State *L);
int luaopen_bmp180 (lua_State *L);

#endif /* COMPONENTS_BMP180_BMP180_LUA_H_ */
