/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <device.h>
#include <esp_matter.h>
#include <esp_matter_console.h>
#include <led_driver.h>

#include <app_priv.h>
#include <app_reset.h>

static const char *TAG = "app_driver";

namespace relative_humidity_measurement {
    namespace attribute {

        attribute_t *create_relative_humidity_measured_value(cluster_t *cluster, nullable<int16_t> value)
        {
            return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::MeasuredValue::Id,
                                                ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
        }

        attribute_t *create_relative_humidity_min_measured_value(cluster_t *cluster, nullable<int16_t> value)
        {
            return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::MinMeasuredValue::Id,
                                                ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
        }

        attribute_t *create_relative_humidity_max_measured_value(cluster_t *cluster, nullable<int16_t> value)
        {
            return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::MaxMeasuredValue::Id,
                                                ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
        }

    } /* attribute */


    const function_generic_t *function_list = NULL;
    const int function_flags = CLUSTER_FLAG_NONE;

    cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
    {
        cluster_t *cluster = cluster::create(endpoint, RelativeHumidityMeasurement::Id, flags);
        if (!cluster) {
            ESP_LOGE(TAG, "Could not create cluster");
            return NULL;
        }

        if (flags & CLUSTER_FLAG_SERVER) {
            set_plugin_server_init_callback(cluster, MatterTemperatureMeasurementPluginServerInitCallback);
            add_function_list(cluster, function_list, function_flags);
        }
        if (flags & CLUSTER_FLAG_CLIENT) {
            set_plugin_client_init_callback(cluster, MatterTemperatureMeasurementPluginClientInitCallback);
            create_default_binding_cluster(endpoint);
        }

        if (flags & CLUSTER_FLAG_SERVER) {
            /* Attributes managed internally */
            global::attribute::create_feature_map(cluster, 0);

            /* Attributes not managed internally */
            if (config) {
                global::attribute::create_cluster_revision(cluster, config->cluster_revision);
                attribute::create_relative_humidity_measured_value(cluster, config->measured_value);
                attribute::create_relative_humidity_min_measured_value(cluster, config->min_measured_value);
                attribute::create_relative_humidity_max_measured_value(cluster, config->max_measured_value);
            } else {
                ESP_LOGE(TAG, "Config is NULL. Cannot add some attributes.");
            }
        }

        return cluster;
}

namespace relative_humidity_sensor {
    uint32_t get_device_type_id()
    {
        return ESP_MATTER_RELATIVE_HUMIDITY_SENSOR_DEVICE_TYPE_ID;
    }

    uint8_t get_device_type_version()
    {
        return ESP_MATTER_RELATIVE_HUMIDITY_SENSOR_DEVICE_TYPE_VERSION;
    }

    endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data)
    {
        endpoint_t *endpoint = endpoint::create(node, flags, priv_data);
        return add(endpoint, config);
    }

    endpoint_t *add(endpoint_t *endpoint, config_t *config)
    {
        if (!endpoint) {
            ESP_LOGE(TAG, "Endpoint cannot be NULL");
            return NULL;
        }
        add_device_type(endpoint, get_device_type_id(), get_device_type_version());

        descriptor::create(endpoint, CLUSTER_FLAG_SERVER);
        identify::create(endpoint, &(config->identify), CLUSTER_FLAG_SERVER);
        relative_humidity_measurement::create(endpoint, &(config->relative_humidity_measurement), CLUSTER_FLAG_SERVER);

        return endpoint;
    }
} /* relative_humidity_sensor */



void bmp280_sensor_init(bmp280_t * bmp280_dev_descriptor) {
    ESP_ERROR_CHECK(i2cdev_init());

    //Initialize temp sensor params
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(bmp280_dev_descriptor, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(bmp280_dev_descriptor, BMP280_I2C_ADDRESS_1, 0, (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SDA, (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(bmp280_dev_descriptor, &params));

    ESP_LOGE(TAG, "Found temperature, and relative_humidity sensor with id: %d\n", bmp280_dev_descriptor->id);
}


esp_matter_attr_val_t bmp280_sensor_update(bmp280_t * bmp280_dev_descriptor) {
    esp_err_t err = ESP_OK;

    float pressure, temp, relative_humidity;

    err = bmp280_read_float(bmp280_dev_descriptor, &temp, &pressure, &relative_humidity);
    
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "No temperature found, returning null: %d\n", bmp280_dev_descriptor->id);
        return esp_matter_nullable_int16(nullable<int16_t>());
    }

    return esp_matter_nullable_int16((nullable<int16_t>) (temp*100));
}

