#ifndef MODULES_H
#define MODULES_H

#include "lua.h"

void registerlib(lua_State *L, const char *name, lua_CFunction f);
void openlibs(lua_State *L);


#endif /* MODULES_H_ */
