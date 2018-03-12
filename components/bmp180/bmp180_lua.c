/*
 Copyright (c) 2017 Teemu Kärkkäinen

 */
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "bmp180.h"

#include "bmp180_lua.h"

static const char* TAG = "BMP180";

int init_i2c(lua_State *L) {
	ESP_LOGI( TAG, "Initializing I2C" );
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_DATA_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_CLOCK_PIN;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config( I2C_NUM_0, &conf );
    i2c_driver_install( I2C_NUM_0, conf.mode, 0, 0, 0);
    return 0;
}

int bmp180_get_values(lua_State *L) {
    ESP_LOGI( TAG, "Reading BMP180 EEPROM" );
    bmp180_eeprom_t bmp180_eeprom = { 0 };
    ESP_LOGI( TAG, "Initial values: A1=%d, A2=%d, A3=%d, A4=%u, A5=%u, A6=%u, b1=%d, b2=%d, mb=%d, mc=%d, md=%d",
        bmp180_eeprom.ac1, bmp180_eeprom.ac2, bmp180_eeprom.ac3,
        bmp180_eeprom.ac4, bmp180_eeprom.ac5, bmp180_eeprom.ac6, bmp180_eeprom.b1, bmp180_eeprom.b2,
        bmp180_eeprom.mb, bmp180_eeprom.mc, bmp180_eeprom.md );
    esp_err_t result;
    result = bmp180_read_eeprom( I2C_NUM_0, &bmp180_eeprom );
    if ( result == ESP_OK ) {
        ESP_LOGI( TAG, "Reading successful: A1=%d, A2=%d, A3=%d, A4=%u, A5=%u, A6=%u, b1=%d, b2=%d, mb=%d, mc=%d, md=%d",
        bmp180_eeprom.ac1, bmp180_eeprom.ac2, bmp180_eeprom.ac3,
        bmp180_eeprom.ac4, bmp180_eeprom.ac5, bmp180_eeprom.ac6, bmp180_eeprom.b1, bmp180_eeprom.b2,
        bmp180_eeprom.mb, bmp180_eeprom.mc, bmp180_eeprom.md );
    } else {
        ESP_LOGI( TAG, "Reading failed." );
    }

    ESP_LOGI( TAG, "Reading raw temperature" );
    uint16_t raw_temperature = 0;
    result = bmp180_read_raw_temperature( I2C_NUM_0, &raw_temperature );
    if ( result == ESP_OK ) {
        ESP_LOGI( TAG, "Success: raw temperature = %u", raw_temperature );
        int32_t temperature = bmp180_true_temperature( raw_temperature, bmp180_eeprom );
        float tmp = temperature / 10.0;
        lua_pushnumber(L, tmp);
        ESP_LOGI( TAG, "Calibrated temperature: %d (%f C)", temperature, tmp );
    } else {
        ESP_LOGI( TAG, "Failed to read the raw temperature." );
    }

    ESP_LOGI( TAG, "Reading raw pressure" );
    bmp180_oversampling_t sampling = BMP180_SAMPLING_HIGH;
    uint32_t raw_pressure = 0;
    result = bmp180_read_raw_pressure( I2C_NUM_0, sampling, &raw_pressure );
    if ( result == ESP_OK ) {
        ESP_LOGI( TAG, "Success: raw pressure = %u", raw_pressure );
        int64_t pressure = bmp180_true_pressure( raw_pressure, raw_temperature, bmp180_eeprom,
            sampling );
        lua_pushnumber(L, pressure);
        ESP_LOGI( TAG, "Calibrated pressure: %lld", pressure );
    } else {
        ESP_LOGI( TAG, "Failed to read the raw pressure." );
    }

    return 2;
}

static const struct luaL_Reg bmp180 [] = {
        {"get", bmp180_get_values},
        {NULL, NULL}  /* sentinel */
};

int luaopen_bmp180 (lua_State *L) {
        luaL_newlib(L, bmp180);
        return 1;
}
