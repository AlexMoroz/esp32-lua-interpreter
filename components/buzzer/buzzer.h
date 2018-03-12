#ifndef COMPONENTS_BUZZER_BUZZER_H_
#define COMPONENTS_BUZZER_BUZZER_H_

#include "lua.h"

#define GPIO_BUZZER (33)
#define DUTY_MAX (4095)

int play_song(lua_State *L);
int luaopen_buzzer (lua_State *L);

#endif /* COMPONENTS_BUZZER_BUZZER_H_ */
