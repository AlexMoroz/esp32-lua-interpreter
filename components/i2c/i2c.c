/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */
#include "i2c.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define TAG "i2c"

int init_i2c(lua_State *L) {
	ESP_LOGI( TAG, "Initializing I2C" );
	int i2c_master_port = I2C_MASTER_NUM;
	    i2c_config_t conf;
	    conf.mode = I2C_MODE_MASTER;
	    conf.sda_io_num = I2C_MASTER_SDA_IO;
	    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
	    conf.scl_io_num = I2C_MASTER_SCL_IO;
	    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
	    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
	    i2c_param_config(i2c_master_port, &conf);
	    i2c_driver_install(i2c_master_port, conf.mode,
	                       I2C_MASTER_RX_BUF_LEN,
	                       I2C_MASTER_TX_BUF_LEN, 0);
    return 0;
}

static const struct luaL_Reg i2c [] = {
        {"init", init_i2c},
        {NULL, NULL}  /* sentinel */
};

int luaopen_i2c (lua_State *L) {
        luaL_newlib(L, i2c);
        return 1;
}


