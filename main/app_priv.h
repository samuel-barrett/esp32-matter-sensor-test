/**
 * @file app_priv.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 *    This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. 
 */

#pragma once

#include <string>
#include <i2cdev.h>
#include <bh1750.h>
#include <esp_matter_core.h>
#include <app_reset.h>

#include <driver/gpio.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define BH1750_ADDR BH1750_ADDR_LO
#define TEST_LED_GPIO GPIO_NUM_13

#define CONFIG_EXAMPLE_I2C_MASTER_SDA 23
#define CONFIG_EXAMPLE_I2C_MASTER_SCL 22

typedef void * app_driver_handle_t;

/**
 * @brief Holds information related to a particular functionality, such as a sensor or a switch
*/
typedef struct {
   std::string name; /**< The name of the functionality, represented as a std::string*/
   endpoint_t * endpoint_p; /**< A pointer to the endpoint_t type object*/
   uint16_t endpoint_id; /**< The id of the endpoint, represented as an uint16_t*/
   cluster_t * cluster_p; /**< A pointer to the cluster_t type object*/
   uint16_t cluster_id; /**< The id of the cluster, represented as an uint16_t*/
   uint32_t measured_attribute_id; /**< The id of the attribute, represented as an uint32_t*/
} matter_config_t;


/**
 * @struct Matter Attribute Values for SCD30 Readings Type
 * 
 * @brief Holds information related to the scd30 as matter attribute values
 * 
 * @var data_ready (bool) 
 * Indicates if the data is ready to be read
 * @var co2_reading (esp_matter_attr_val_t) 
 * CO2 data, which should be in ppm
 * @var temperature_reading (esp_matter_attr_val_t) 
 * Temperature reading, which should be in Celsius
 * @var humidity_reading (esp_matter_attr_val_t) 
 * Humidity reading, which should be a percent
*/
typedef struct {
   bool data_ready;
   esp_matter_attr_val_t co2_reading;
   esp_matter_attr_val_t temperature_reading;
   esp_matter_attr_val_t humidity_reading;
} matter_attr_val_scd30_reading_t; 


void bh1750_sensor_init(i2c_dev_t * bh1750_dev_descriptor);
esp_matter_attr_val_t bh1750_sensor_update(i2c_dev_t * bh1750_dev_descriptor);

void scd30_sensor_init(i2c_dev_t * scd30_dev_descriptor);
matter_attr_val_scd30_reading_t scd30_sensor_update(i2c_dev_t * scd30_dev_descriptor);

void test_led_init();
bool led_on();

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val);