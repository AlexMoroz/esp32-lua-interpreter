#ifndef MODULES_H_
#define MODULES_H_

#include <stdint.h>

#define COLOR_RED (0)
#define COLOR_GREEN (1)
#define COLOR_BLUE (2)

#define GPIO_RED (25)
#define GPIO_GREEN (26)
#define GPIO_BLUE (27)

#define GPIO_BUZZER (33)

#define I2C_DATA_PIN (32)
#define I2C_CLOCK_PIN (33)

#define DUTY_MAX (4095)


typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} color_t;

#endif /* MODULES_H_ */