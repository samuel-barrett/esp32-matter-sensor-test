/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <string>
#include <i2cdev.h>
#include <bmp280.h>
#include <bh1750.h>
#include <esp_matter_core.h>
#include <app_reset.h>

#include <driver/gpio.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;


#define CONFIG_EXAMPLE_I2C_ADDRESS_LO 0
//#define CONFIG_EXAMPLE_I2C_ADDRESS_HI 0 

#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_LO)
#define ADDR BH1750_ADDR_LO
#endif
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_HI)
#define ADDR BH1750_ADDR_HI
#endif

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define TEST_LED_GPIO GPIO_NUM_13

typedef void *app_driver_handle_t;

typedef struct {
   std::string name;
   endpoint_t * endpoint_p;
   uint16_t endpoint_id;
   cluster_t * cluster_p;
   uint16_t cluster_id;
   uint32_t measured_attribute_id;
} matter_sensor_config_t;

typedef struct {
   esp_matter_attr_val_t temperature_reading;
   esp_matter_attr_val_t pressure_reading;
} matter_attr_val_bmp280_reading_t; 


typedef struct {
   bool data_ready;
   esp_matter_attr_val_t co2_reading;
   esp_matter_attr_val_t temperature_reading;
   esp_matter_attr_val_t humidity_reading;
} matter_attr_val_scd30_reading_t; 


/* BH1850 Driver Functions */
void bh1750_sensor_init(i2c_dev_t * bh1750_dev_descriptor);
esp_matter_attr_val_t bh1750_sensor_update(i2c_dev_t * bh1750_dev_descriptor);

/* BMP280 Sensor Functions*/
void bmp280_sensor_init(bmp280_t * bmp280_dev_descriptor);
matter_attr_val_bmp280_reading_t bmp280_sensor_update(bmp280_t * bmp280_dev_descriptor);

/* SCD30 Sensor Function */
void scd30_sensor_init(i2c_dev_t * scd30_dev_descriptor);
matter_attr_val_scd30_reading_t scd30_sensor_update(i2c_dev_t * scd30_dev_descriptor);


/* Test Light */
esp_err_t configure_led();
esp_err_t app_driver_light_set_power(uint8_t s_led_state);

/** Driver Update
 *
 * This API should be called to update the driver for the attribute being updated.
 * This is usually called from the common `app_attribute_update_cb()`.
 *
 * @param[in] endpoint_id Endpoint ID of the attribute.
 * @param[in] cluster_id Cluster ID of the attribute.
 * @param[in] attribute_id Attribute ID of the attribute.
 * @param[in] val Pointer to `esp_matter_attr_val_t`. Use appropriate elements as per the value type.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);


esp_err_t app_driver_light_set_power(bool s_led_state);