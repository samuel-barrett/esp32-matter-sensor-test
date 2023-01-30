/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_matter.h>

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <bmp280.h>
#include <string.h>

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define ESP_MATTER_RELATIVE_HUMIDITY_SENSOR_DEVICE_TYPE_ID 0x0307
#define ESP_MATTER_RELATIVE_HUMIDITY_SENSOR_DEVICE_TYPE_VERSION 2



namespace esp_matter {
   namespace cluster {
      namespace relative_humidity_measurement {
         typedef struct config {
            uint16_t cluster_revision;
            nullable<int16_t> measured_value;
            nullable<int16_t> min_measured_value;
            nullable<int16_t> max_measured_value;
            config() : cluster_revision(4), measured_value(), min_measured_value(), max_measured_value() {}
         } config_t;

         cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

         namespace attribute {
            attribute_t *create_relative_humidity_measured_value(cluster_t *cluster, nullable<int16_t> value);
            attribute_t *create_relative_humidity_min_measured_value(cluster_t *cluster, nullable<int16_t> value);
            attribute_t *create_relative_humidity_max_measured_value(cluster_t *cluster, nullable<int16_t> value);
         } /* attribute */
      } /* relative_humidity_measurement */
   }


   namespace relative_humidity_sensor {
      typedef struct config {
         cluster::identify::config_t identify;
         cluster::relative_humidity_measurement::config_t relative_humidity_measurement;
      } config_t;
      uint32_t get_device_type_id();
      uint8_t get_device_type_version();
      endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
      endpoint_t *add(endpoint_t *endpoint, config_t *config);
   }
}

/** Get updated temperature sensor reading
 *
 * This 
 *
 * @return Handle on success.
 * @return NULL in case of failure.
 */
esp_matter_attr_val_t bmp280_sensor_update(bmp280_t * bmp280_dev_descriptor);

void bmp280_sensor_init(bmp280_t * bmp280_dev_descriptor);



