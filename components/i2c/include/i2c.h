/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */

#ifndef COMPONENTS_I2C_INCLUDE_I2C_H_
#define COMPONENTS_I2C_INCLUDE_I2C_H_

#include "lua.h"

#define I2C_DATA_PIN (18)
#define I2C_CLOCK_PIN (19)

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     // disabled
#define I2C_MASTER_RX_BUF_LEN    0                     // disabled
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        I2C_DATA_PIN
#define I2C_MASTER_SCL_IO        I2C_CLOCK_PIN

int init_i2c(lua_State *L);
int luaopen_i2c (lua_State *L);


#endif /* COMPONENTS_I2C_INCLUDE_I2C_H_ */
