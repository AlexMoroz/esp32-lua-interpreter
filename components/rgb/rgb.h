#ifndef COMPONENTS_RGB_RGB_H_
#define COMPONENTS_RGB_RGB_H_

#define COLOR_RED (0)
#define COLOR_GREEN (1)
#define COLOR_BLUE (2)

#define GPIO_RED (25)
#define GPIO_GREEN (26)
#define GPIO_BLUE (27)

#include "lua.h"

int light(lua_State *L);
int luaopen_rgb (lua_State *L);

#endif /* COMPONENTS_RGB_RGB_H_ */
