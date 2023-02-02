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

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define BH1750_ADDR BH1750_ADDR_LO
#define TEST_LED_GPIO GPIO_NUM_13

typedef void * app_driver_handle_t;

/**
 * @brief Holds information realted to a particular functionality (i.e. a sensor or a switch)
 * 
 * @param name (std::string) The name of the functionality
 * @param endpoint_p (endpoint_t *) Pointer to the endpoint
 * @param endpoint_id (uint16_t) Id of the enpoint
 * @param cluster_p (cluster_t *) Pointer to the cluster
 * @param cluster_id (uint16_t) Id of the cluster
 * @param measured_attribute_id (uint32_t) Id of the attribute
*/
typedef struct {
   std::string name;
   endpoint_t * endpoint_p;
   uint16_t endpoint_id;
   cluster_t * cluster_p;
   uint16_t cluster_id;
   uint32_t measured_attribute_id;
} matter_config_t;

/**
 * @brief Holds information related to the scd30 
 * 
 * @param data_ready (bool) Indicates if the data is ready to be read
 * @param co2_reading (esp_matter_attr_val_t) CO2 data, which should be in ppm
 * @param temperature_reading (esp_matter_attr_val_t) Temperature reading, which should be in Celsius
 * @param humidity_reading (esp_matter_attr_val_t) Humidity reading, which should be a percent
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