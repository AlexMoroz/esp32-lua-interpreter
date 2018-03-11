#ifndef BMP180_
#define BMP180_

#include "bmp180.h"

#include "driver/i2c.h"
#include "esp_log.h"

/***********************************************************************/
/* Private */
/***********************************************************************/
uint8_t bmp180_eeprom_registers[ 11 ] = {
		BMP180_EEPROM_AC1,
		BMP180_EEPROM_AC2,
		BMP180_EEPROM_AC3,
		BMP180_EEPROM_AC4,
		BMP180_EEPROM_AC5,
		BMP180_EEPROM_AC6,
		BMP180_EEPROM_B1,
		BMP180_EEPROM_B2,
		BMP180_EEPROM_MB,
		BMP180_EEPROM_MC,
		BMP180_EEPROM_MD
};

/** Delay needed for pressure measurement in milliseconds. */
int8_t bmp180_delays[ 4 ] = { 5, 8, 14, 26 };

/** Control register values to write for the different oversampling settings. */
uint8_t bmp180_pressure_commands[ 4 ] = { 0x34, 0x74, 0xB4, 0xF4 };

esp_err_t bmp180_read_register( i2c_port_t i2c_num, uint8_t reg, uint16_t *value ) {
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start( cmd );

    /* Device address + write bit */
    i2c_master_write_byte( cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN );
    /* Register to read. */
    i2c_master_write_byte( cmd, reg, ACK_CHECK_EN );

    /* Restart condition */
    i2c_master_start( cmd );

    /* Device address + read bit */
    i2c_master_write_byte( cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, ACK_CHECK_EN );

    /* Read the register value */
    uint8_t msb, lsb;
    i2c_master_read_byte( cmd, &msb, ACK_VAL );
    i2c_master_read_byte( cmd, &lsb, NACK_VAL ); /* Last byte is NACKed instead of ACKed */

    i2c_master_stop( cmd );
    ret = i2c_master_cmd_begin( i2c_num, cmd, 1000 / portTICK_RATE_MS ); /* Blocks until done */
    i2c_cmd_link_delete( cmd );
    if ( ret != ESP_OK ) {
        return ret;
    }

    /* Set the value */
    *value = ( msb << 8 | lsb );

    return ESP_OK;
}

esp_err_t bmp180_read_register_pressure( i2c_port_t i2c_num, uint8_t reg, uint32_t *value,
        bmp180_oversampling_t sampling ) {
    esp_err_t ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start( cmd );

    /* Device address + write bit */
    i2c_master_write_byte( cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN );
    /* Register to read. */
    i2c_master_write_byte( cmd, reg, ACK_CHECK_EN );

    /* Restart condition */
    i2c_master_start( cmd );

    /* Device address + read bit */
    i2c_master_write_byte( cmd, DEVICE_ADDR << 1 | I2C_MASTER_READ, ACK_CHECK_EN );

    /* Read the register value */
    uint8_t msb, lsb, xlsb;
    i2c_master_read_byte( cmd, &msb, ACK_VAL );
    i2c_master_read_byte( cmd, &lsb, ACK_VAL );
    i2c_master_read_byte( cmd, &xlsb, NACK_VAL ); /* Last byte is NACKed instead of ACKed */

    i2c_master_stop( cmd );
    ret = i2c_master_cmd_begin( i2c_num, cmd, 1000 / portTICK_RATE_MS ); /* Blocks until done */
    i2c_cmd_link_delete( cmd );
    if ( ret != ESP_OK ) {
        return ret;
    }

    /* Set the value */
    *value = ( ( msb << 16 ) + ( lsb << 8 ) + xlsb ) >> ( 8 - sampling );

    return ESP_OK;
}

/**
 * Writes the control register of the BMP180.
 * +----+----+----+----+----+----+----+----+
 * |   oss   | sco|   measurement control  |
 * +----+----+----+----+----+----+----+----+
 */
esp_err_t bmp180_write_control_reg( i2c_port_t i2c_num, uint8_t value ) {
    esp_err_t ret;

    /* Create command list to send */
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start( cmd );
    /* Device address + write bit */
    i2c_master_write_byte( cmd, DEVICE_ADDR << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN );
    /* Register to write. */
    i2c_master_write_byte( cmd, BMP180_CONTROL, ACK_CHECK_EN );
    /* Register value. */
    i2c_master_write_byte( cmd, value, ACK_CHECK_EN );
    i2c_master_stop( cmd );

    /* Send the commands */
    ret = i2c_master_cmd_begin( i2c_num, cmd, 1000 / portTICK_RATE_MS ); /* Blocks until done */
    i2c_cmd_link_delete( cmd );

    return ret;
}
/***********************************************************************/


/***********************************************************************/
/* API */
/***********************************************************************/
esp_err_t bmp180_read_eeprom( i2c_port_t i2c_num, bmp180_eeprom_t *eeprom ) {
    esp_err_t ret;

    /* Pointers to the values to set */
    uint16_t *eeprom_regs[ 11 ] = {
        ( uint16_t* )( &( eeprom->ac1 ) ),
        ( uint16_t* )( &( eeprom->ac2 ) ),
        ( uint16_t* )( &( eeprom->ac3 ) ),
        &( eeprom->ac4 ),
        &( eeprom->ac5 ),
        &( eeprom->ac6 ),
        ( uint16_t* )( &( eeprom->b1 ) ),
        ( uint16_t* )( &( eeprom->b2 ) ),
        ( uint16_t* )( &( eeprom->mb ) ),
        ( uint16_t* )( &( eeprom->mc ) ),
        ( uint16_t* )( &( eeprom->md ) ),
    };

    /* Loop through all the eeprom registers */
    for ( int i = 0; i < 11; i++ ) {
        ret = bmp180_read_register( i2c_num, bmp180_eeprom_registers[ i ], eeprom_regs[ i ] );
        if ( ret != ESP_OK ) {
            return ret;
        }
    }

    return ESP_OK;
}

esp_err_t bmp180_read_raw_temperature( i2c_port_t i2c_num, uint16_t *value ) {
    esp_err_t ret;

    /* Start temperature measurement */
    ret = bmp180_write_control_reg( i2c_num, BMP180_CMD_TEMP );
    if ( ret != ESP_OK ) return ret;

    /* Need to wait 4.5 ms */
    vTaskDelay( 5 / portTICK_RATE_MS );

    /* Read raw temperature */
    ret = bmp180_read_register( i2c_num, BMP180_ADC_OUT, value );
    if ( ret != ESP_OK ) return ret;

    return ESP_OK;
}

esp_err_t bmp180_read_raw_pressure( i2c_port_t i2c_num, bmp180_oversampling_t oversampling, uint32_t *value ) {
    esp_err_t ret;

    /* Start pressure measurement */
    ret = bmp180_write_control_reg( i2c_num, bmp180_pressure_commands[ oversampling ] );
    if ( ret != ESP_OK ) return ret;

    /* Need to wait */
    vTaskDelay( bmp180_delays[ oversampling ] / portTICK_RATE_MS );

    /* Read raw pressure */
    ret = bmp180_read_register_pressure( i2c_num, BMP180_ADC_OUT, value, oversampling );
    if ( ret != ESP_OK ) return ret;

    return ESP_OK;
}

int32_t bmp180_true_temperature( uint16_t raw_temperature, bmp180_eeprom_t calibration ) {
    int32_t X1 = ((raw_temperature - calibration.ac6) * calibration.ac5) >> 15;
	int32_t X2 = (calibration.mc << 11) / (X1 + calibration.md);
	int32_t B5 = X1 + X2;
	int32_t T = ((B5 + 8) >> 4);
    return T;
}

int64_t bmp180_true_pressure( uint32_t raw_pressure, int16_t raw_temperature, bmp180_eeprom_t calibration,
        bmp180_oversampling_t oversampling ) {
    int64_t UT, UP, B6, B5, X1, X2, X3, B3, p;
	uint64_t B4, B7;

    UT = raw_temperature;
	UP = raw_pressure;

	X1 = ((UT - calibration.ac6) * calibration.ac5) >> 15;
	X2 = (calibration.mc << 11) / (X1 + calibration.md);

	B5 = X1 + X2;

	B6 = B5 - 4000;

	X1 = (calibration.b2 * (B6 * B6) >> 12) >> 11;
	X2 = (calibration.ac2 * B6) >> 11;
	X3 = X1 + X2;

	B3 = ((((calibration.ac1 * 4) + X3) << oversampling ) + 2) / 4;
	X1 = (calibration.ac3 * B6) >> 13;
	X2 = (calibration.b1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;

	B4 = calibration.ac4 * (unsigned long)(X3 + 32768) >> 15;
	B7 = ((unsigned long) UP - B3) * (50000 >> oversampling );
	
	if (B7 < 0x80000000) {
		p = (B7 * 2) / B4;
	} else {
		p = (B7 / B4) * 2;
	}
	
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;
	p = p + ((X1 + X2 + 3791) >> 4);
	
	return p;
}
/***********************************************************************/
//create bindings

static const char* TAG = "BMP180";

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


#endif
