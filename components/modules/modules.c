#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "bmp180_lua.h"
#include "buzzer.h"
#include "rgb.h"
#include "lcd.h"
#include "i2c.h"

#include "modules.h"

void registerlib(lua_State *L, const char *name, lua_CFunction f) {
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload"); /* get 'package.preload' */
	lua_pushcfunction(L, f);
	lua_setfield(L, -2, name); /* package.preload[name] = f */
	lua_pop(L, 2); /* pop 'package' and 'preload' tables */
}

void openlibs(lua_State *L) {
	luaL_requiref(L, "_G", luaopen_base, 1);
	luaL_requiref(L, "package", luaopen_package, 1);
	lua_pop(L, 2); /* remove results from previous calls */
	registerlib(L, "bmp180", luaopen_bmp180);
	registerlib(L, "buzzer", luaopen_buzzer);
	registerlib(L, "rgb", luaopen_rgb);
	registerlib(L, "i2c", luaopen_i2c);
	registerlib(L, "lcd", luaopen_lcd1602);
}
