#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "bmp180.h"


void lua_preload_libs(lua_State *L) {
    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    lua_pushcfunction(L, luaopen_bmp180);
    lua_setfield(L, -2, bmp180);
    lua_pop(L, 1);  // remove PRELOAD table
}