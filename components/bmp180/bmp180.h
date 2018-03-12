/*
Copyright (c) 2017 Teemu Kärkkäinen

ESP32 library for interacting with the BMP180 sensor via I2C.
*/

#ifndef BMP180_H_
#define BMP180_H_

#include "driver/i2c.h"



/***********************************************************************/
/* Definitions */
/***********************************************************************/
#define DEVICE_ADDR (0x77)

#define ACK_CHECK_EN    (0x1) /* I2C master will check ack from slave */
#define ACK_CHECK_DIS   (0x0) /* I2C master will not check ack from slave */
#define ACK_VAL         (0x0) /* I2C ack value */
#define NACK_VAL        (0x1) /* I2C nack value */

#define BMP180_EEPROM_AC1 0xAA
#define BMP180_EEPROM_AC2 0xAC
#define BMP180_EEPROM_AC3 0xAE
#define BMP180_EEPROM_AC4 0xB0
#define BMP180_EEPROM_AC5 0xB2
#define BMP180_EEPROM_AC6 0xB4
#define BMP180_EEPROM_B1 0xB6
#define BMP180_EEPROM_B2 0xB8
#define BMP180_EEPROM_MB 0xBA
#define BMP180_EEPROM_MC 0xBC
#define BMP180_EEPROM_MD 0xBE

#define BMP180_CONTROL 0xF4
#define BMP180_ADC_OUT 0xF6

#define BMP180_CMD_TEMP 0x2E

/***********************************************************************/


/***********************************************************************/
/* Types */
/***********************************************************************/
typedef struct {
    int16_t     ac1;
    int16_t     ac2;
    int16_t     ac3;
    uint16_t    ac4;
    uint16_t    ac5;
    uint16_t    ac6;
    int16_t     b1;
    int16_t     b2;
    int16_t     mb;
    int16_t     mc;
    int16_t     md;
} bmp180_eeprom_t;

typedef enum {
    BMP180_SAMPLING_SINGLE = 0,
    BMP180_SAMPLING_STANDARD = 1,
    BMP180_SAMPLING_HIGH = 2,
    BMP180_SAMPLING_ULTRA = 3
} bmp180_oversampling_t;
/***********************************************************************/



/***********************************************************************/
/* Functions */
/***********************************************************************/
esp_err_t bmp180_read_eeprom( i2c_port_t i2c_num, bmp180_eeprom_t *eeprom );
esp_err_t bmp180_read_raw_temperature( i2c_port_t i2c_num, uint16_t *value );
esp_err_t bmp180_read_raw_pressure( i2c_port_t i2c_num, bmp180_oversampling_t oversampling, uint32_t *value );
int32_t   bmp180_true_temperature( uint16_t raw_temperature, bmp180_eeprom_t calibration );
int64_t   bmp180_true_pressure( uint32_t raw_pressure, int16_t raw_temperature, bmp180_eeprom_t calibration,
        bmp180_oversampling_t oversampling );
/***********************************************************************/



#endif /* BMP180_H_ */
