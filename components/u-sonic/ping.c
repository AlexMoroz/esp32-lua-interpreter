/*
 *  Created on: 18 Mar 2018
 *      Author: Alex Moroz
 */


#include "freertos/FreeRTOS.h"

#include <math.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/driver.h>

#include <drivers/sensor.h>
#include <drivers/gpio.h>

driver_error_t *ping28015_setup(sensor_instance_t *unit);
driver_error_t *ping28015_acquire(sensor_instance_t *unit, sensor_value_t *values);
driver_error_t *ping28015_set(sensor_instance_t *unit, const char *id, sensor_value_t *setting);

// Sensor specification and registration
static const sensor_t __attribute__((used,unused,section(".sensors"))) ping28015_sensor = {
	.id = "PING28015",
	.interface = {
		{.type = GPIO_INTERFACE},
	},
	.data = {
		{.id = "distance", .type = SENSOR_DATA_DOUBLE},
	},
	.interface_name = {"SIG"},
	.properties = {
		{.id = "calibration", .type = SENSOR_DATA_DOUBLE},
		{.id = "temperature", .type = SENSOR_DATA_DOUBLE},
	},
	.setup = ping28015_setup,
	.acquire = ping28015_acquire,
	.set = ping28015_set
};

/*
 * Operation functions
 */
driver_error_t *ping28015_setup(sensor_instance_t *unit) {
	// Set default calibration value
	unit->properties[0].doubled.value = 0;

	// Set temperature to 20 ºC if no current temperature is provided
	unit->properties[1].doubled.value = 20;

	// Ignore some measures
	sensor_value_t tmp[2];
	int i;

	for(i = 0;i < 2;i++) {
		ping28015_acquire(unit, tmp);
		udelay(200);
	}

	return NULL;
}

driver_error_t *ping28015_set(sensor_instance_t *unit, const char *id, sensor_value_t *setting) {
	if (strcmp(id,"calibration") == 0) {
		memcpy(&unit->properties[0], setting, sizeof(sensor_value_t));
	} else if (strcmp(id,"temperature") == 0) {
		memcpy(&unit->properties[1], setting, sizeof(sensor_value_t));
	}

	return NULL;
}

driver_error_t *ping28015_acquire(sensor_instance_t *unit, sensor_value_t *values) {
	// Configure pin as output
	gpio_pin_output(unit->setup[0].gpio.gpio);

	// Trigger pulse
	gpio_pin_set(unit->setup[0].gpio.gpio);
	udelay(5);
	gpio_pin_clr(unit->setup[0].gpio.gpio);

	// Configure pin as input
	gpio_pin_input(unit->setup[0].gpio.gpio);

	// Get echo pulse width in usecs
	double time = gpio_get_pulse_time(unit->setup[0].gpio.gpio, 1, 18500);
	if (time < 0.0) {
		return driver_error(SENSOR_DRIVER, SENSOR_ERR_TIMEOUT, NULL);
	}

	/*
	 * Calculate distance
	 *
	 * Sound speed depends on ambient temperature
	 * sound speed = 331.5 + (0.6 * temperature) m/sec
	 * sound speed = 331500 + (60 * temperature) mm/sec
	 *
	 */

	// First calculate mm per usec
	double mm_per_usecs = (331500.0 + (60.0 * (unit->properties[1].doubled.value))) / 1000000.0;

	// Calculate distance in centimeters.
	// Please, take note that the width of the echo is the time that the ultrasonic pulse takes to travel
	// from the sensor to the object, plus the time to back to the sensor, so we have to consider time / 2.
	// 1 decimal precision.
	if (time == 0) {
		values[0].doubled.value = 2;
	} else {
		values[0].doubled.value =
			  round((((((time / 2.0)) * mm_per_usecs) / 10.0 +
			  unit->properties[0].doubled.value)) * 10.00) / 10.00;
	}

	// Next value can get in 200 useconds
	gettimeofday(&unit->next, NULL);
	unit->next.tv_usec += 200;

	return NULL;
}
