/**
 * @file app_main.cpp
 * @author Samuel Barrett (samarbarrett)
 * @brief Main application file for the ESP32 Matter Sensor Test
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_core.h>

#include <string>

#include <i2cdev.h>
#include <bh1750.h>
#include <scd30.h>

#include <freertos/task.h>

#include <esp_log.h>
#include <esp_err.h>

#include <app_priv.h>


using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace esp_matter::cluster;

static const char *TAG = "app_main";

/**
 * @brief On off light endpoint identifier
 */
uint16_t light_endpoint_id = 0;

/**
 * @brief App Event Callback Function. Currently logs any events that occur
 * 
 * @param[in] event  Chip Device event
 * @param[in] arg  Any arguments 
 * 
 * @return 
*/
static void app_event_cb(const ChipDeviceEvent * event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address Changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    default:
        break;
    }
}

/**
 * @brief Callback for identification
 * 
 * @param type Type of callback, either START, STOP, or EFFECT
 * @param endpoint_id Unique identifier for the endpoint in the system
 * @param effect_id Unique idenitfier for the effect
 * @param priv_data Pointer to the private data passed while creating the endpoint.
 * 
 * @return ESP_OK on success.
 * @return Error in case of failure.
 */
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       void *priv_data)
{
    return ESP_OK;
}

/**
 * @brief This function is called when an attribute update request is received, allowing the application to handle the update.
 * 
 * @param type Type of callback, either PRE_UPDATE, POST_UPDATE, READ, or WRITE
 * @param endpoint_id Unique identifier for the endpoint in the system
 * @param cluster_id Identifier for the cluster to which the attribute belongs
 * @param attribute_id Identifier for the attribute being updated
 * @param val Pointer to the new value for the attribute
 * @param priv_data Pointer to private data passed during registration
 * 
 * @return esp_err_t indicating success or failure of the update
*/
static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Driver update */
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        err = app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}


/**
 * @brief This function creates a temperature sensor endpoint and associated cluster. The created endpoint and cluster are 
 * stored in the returned matter_config_t structure. The function also sets the name and attribute_id fields of 
 * the matter_config_t structure. 
 * 
 * @param node  A pointer to the node_t structure
 * 
 * @return  A structure that contains information about the created temperature sensor endpoint and cluster
 */
matter_config_t create_temp_sensor(node_t * node) {
    matter_config_t matter_sensor;
    temperature_sensor::config_t endpoint_config;
    temperature_measurement::config_t cluster_config;

    matter_sensor.name = "Temperature Sensor";
    matter_sensor.attribute_id = chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Id;

    //Create a temperature sensor endpoint
    matter_sensor.endpoint_p = temperature_sensor::create(node, &endpoint_config, CLUSTER_FLAG_SERVER, NULL);
    if (!matter_sensor.endpoint_p) {
        ESP_LOGE(TAG, "Matter temperature endpoint creation failed");
    }
    matter_sensor.endpoint_id = endpoint::get_id(matter_sensor.endpoint_p);
    ESP_LOGI(TAG, "Temp measure endpoint created with id %d", matter_sensor.endpoint_id);

    //Create a cluster for the temperature endpoint
    matter_sensor.cluster_p = temperature_measurement::create(matter_sensor.endpoint_p, &cluster_config, CLUSTER_FLAG_SERVER);
    matter_sensor.cluster_id = cluster::get_id(matter_sensor.cluster_p);
    ESP_LOGI(TAG, "Temp measurement cluster created with cluster_id %d", matter_sensor.cluster_id);

    return matter_sensor;
}

/**
 * @brief Creates an illumination sensor endpoint and cluster.
 * 
 * This function creates an endpoint and a cluster for the illumination sensor. 
 * The created endpoint is of type `illuminance_sensor` and the created cluster is of type `illuminance_measurement`. 
 * The created endpoint and cluster are stored in a `matter_config_t` structure and returned as the result of the function.
 * 
 * @param node  Pointer to the node that the endpoint and cluster belong to.
 * 
 * @return  Structure containing the created endpoint and cluster information.
 */
matter_config_t create_illumination_sensor(node_t * node) {
    matter_config_t matter_sensor;
    illuminance_sensor::config_t endpoint_config;
    illuminance_measurement::config_t cluster_config;

    matter_sensor.name = "Illumination Sensor";
    matter_sensor.attribute_id = chip::app::Clusters::IlluminanceMeasurement::Attributes::MeasuredValue::Id;

    //Create an illumination sensor endpoint
    matter_sensor.endpoint_p = illuminance_sensor::create(node, &endpoint_config, CLUSTER_FLAG_SERVER, NULL);
    if (!matter_sensor.endpoint_p) {
        ESP_LOGE(TAG, "Matter illuminance endpoint creation failed");
    }
    matter_sensor.endpoint_id = endpoint::get_id(matter_sensor.endpoint_p);
    ESP_LOGI(TAG, "Illuminance measure endpoint created with id %d", matter_sensor.endpoint_id);

    //Create a cluster for the illumination endpoint
    matter_sensor.cluster_p = illuminance_measurement::create(matter_sensor.endpoint_p, &cluster_config, CLUSTER_FLAG_SERVER);
    matter_sensor.cluster_id = cluster::get_id(matter_sensor.cluster_p);
    ESP_LOGI(TAG, "Illuminance measurement cluster created with cluster_id %d", matter_sensor.cluster_id);

    return matter_sensor;
}


/**
 * @brief This function creates an endpoint and a cluster for the humidity sensor. The created 
 * endpoint is of type `relative_humidity_sensor` and the created cluster is of type 
 * `relative_humidity_measurement`. 
 * 
 * @param node Pointer to the node that the endpoint and cluster belong to.
 * 
 * @return matter_config_t structure containing the created endpoint and cluster information.
 */
matter_config_t create_humidity_sensor(node_t * node) {
    matter_config_t matter_sensor;
    relative_humidity_sensor::config_t endpoint_config;
    relative_humidity_measurement::config_t cluster_config;

    matter_sensor.name = "Humidity Sensor";
    matter_sensor.attribute_id = chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::Id;

    //Create an humidity sensor endpoint
    matter_sensor.endpoint_p = relative_humidity_sensor::create(node, &endpoint_config, CLUSTER_FLAG_SERVER, NULL);
    if (!matter_sensor.endpoint_p) {
        ESP_LOGE(TAG, "Matter illuminance endpoint creation failed");
    }
    matter_sensor.endpoint_id = endpoint::get_id(matter_sensor.endpoint_p);
    ESP_LOGI(TAG, "Humidity measure endpoint created with id %d", matter_sensor.endpoint_id);

    //Create a cluster for the humidity endpoint
    matter_sensor.cluster_p = relative_humidity_measurement::create(matter_sensor.endpoint_p, &cluster_config, CLUSTER_FLAG_SERVER);
    matter_sensor.cluster_id = cluster::get_id(matter_sensor.cluster_p);
    ESP_LOGI(TAG, "Humidity measurement cluster created with cluster_id %d", matter_sensor.cluster_id);

    return matter_sensor;
}

/**
 * @brief This function creates an endpoint and a cluster for an on off light. The created 
 * endpoint is of type `on_off_light` and the created cluster is of type 
 * `on_off`. 
 * 
 * @param node Pointer to the node that the endpoint and cluster belong to.
 * 
 * @return matter_config_t structure containing the created endpoint and cluster information.
 */
matter_config_t create_on_off_light(node_t * node) {
    matter_config_t matter_on_off_light;
    on_off_light::config_t endpoint_config;
    on_off::config_t cluster_config;

    matter_on_off_light.name = "On OFF Light";
    matter_on_off_light.attribute_id = 0; //NA

    //Create an humidity sensor endpoint
    matter_on_off_light.endpoint_p = on_off_light::create(node, &endpoint_config, CLUSTER_FLAG_SERVER, NULL);
    if (!matter_on_off_light.endpoint_p) {
        ESP_LOGE(TAG, "Matter on/off light endpoint creation failed");
    }
    matter_on_off_light.endpoint_id = endpoint::get_id(matter_on_off_light.endpoint_p);
    ESP_LOGI(TAG, "Matter on/off light endpoint created with id %d", matter_on_off_light.endpoint_id);

    //Create a cluster for the humidity endpoint
    matter_on_off_light.cluster_p = on_off::create(matter_on_off_light.endpoint_p, &cluster_config, ENDPOINT_FLAG_NONE, 0);
    matter_on_off_light.cluster_id = cluster::get_id(matter_on_off_light.cluster_p);
    ESP_LOGI(TAG, "Matter on/off light cluster created with cluster_id %d", matter_on_off_light.cluster_id);

    return matter_on_off_light;
}


/**
 * @brief Updates the matter sensor with new values
 * 
 * @param matter_config Configuration structure for matter sensor
 * @param val Pointer to the new attribute value to be set
 */
void update_matter_sensor(matter_config_t matter_config, esp_matter_attr_val_t * val) {
    update(matter_config.endpoint_id, matter_config.cluster_id, matter_config.attribute_id, val);
    
    if (val->type == ESP_MATTER_VAL_TYPE_BOOLEAN) {
        ESP_LOGI(TAG, "%s updated with value: %d | attribute id: %u", matter_config.name.c_str(), val->val.b, matter_config.attribute_id);
    } else if (val->type == ESP_MATTER_VAL_TYPE_NULLABLE_INT16) {
        ESP_LOGI(TAG, "%s updated with value: %d", matter_config.name.c_str(), val->val.i16);
    }
}

/**
 * @brief Application entrypoint
 */
extern "C" void app_main() {
    esp_err_t err = ESP_OK;

    matter_config_t temp_sensor_config;
    matter_config_t illumination_sensor_config;
    matter_config_t humidity_sensor_config;
    matter_config_t on_off_light_config;

    matter_attr_val_scd30_reading_t scd30_reading;
    i2c_dev_t scd30_dev_descriptor = {0};

    i2c_dev_t bh1750_dev_descriptor;
    esp_matter_attr_val_t bh1750_reading;

    //Initialize I2C library
    ESP_ERROR_CHECK(i2cdev_init());

    //Initialize Peripherals
    bh1750_sensor_init(&bh1750_dev_descriptor);
    scd30_sensor_init(&scd30_dev_descriptor);
    test_led_init();

    //Initialize the ESP NVS layer
    nvs_flash_init();

    // Create a Matter node and add the mandatory Root Node device type on endpoint 0
    node::config_t node_config;
    node_t * node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    if(!node) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    //Create endpoints and cluster for sensors. 
    temp_sensor_config = create_temp_sensor(node);
    humidity_sensor_config = create_humidity_sensor(node);
    illumination_sensor_config = create_illumination_sensor(node);
    on_off_light_config = create_on_off_light(node);

    light_endpoint_id = on_off_light_config.endpoint_id;

    /* Start Matter */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }

    while(true) {
        bh1750_reading = bh1750_sensor_update(&bh1750_dev_descriptor);
        scd30_reading = scd30_sensor_update(&scd30_dev_descriptor);


        //If RED Test LED is on, the lux value will actually show a CO2 reading
        //(Matter does not support CO2 readings).
        //If it is low, the lux value will be shown.
        if(led_on()) {
            if(scd30_reading.data_ready) {
                update_matter_sensor(illumination_sensor_config, &scd30_reading.co2_reading);
            }
        } else if (bh1750_reading.type != ESP_MATTER_VAL_TYPE_INVALID) {
            update_matter_sensor(illumination_sensor_config, &bh1750_reading);
        }

        if(scd30_reading.data_ready) {
            update_matter_sensor(temp_sensor_config, &(scd30_reading.temperature_reading));
            update_matter_sensor(humidity_sensor_config, &scd30_reading.humidity_reading);
        }

        printf("\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
