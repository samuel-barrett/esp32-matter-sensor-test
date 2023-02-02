
/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_core.h>

#include <string>

#include <i2cdev.h>
//#include <bmp280.h>
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
uint16_t light_endpoint_id = 0;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
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

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t temp_endpoint_id, uint8_t effect_id,
                                       void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: type: %d, effect: %d\n", type, effect_id);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "Attribute update callback: type: %d, cluster: %d, attribute %d, value %d\n", type, cluster_id, attribute_id, &val);

    if (type == PRE_UPDATE) {
        /* Driver update */
        app_driver_handle_t driver_handle = (app_driver_handle_t)priv_data;
        err = app_driver_attribute_update(driver_handle, endpoint_id, cluster_id, attribute_id, val);
    }

    return err;
}

matter_sensor_config_t create_temp_sensor(node_t * node) {
    matter_sensor_config_t matter_sensor;
    temperature_sensor::config_t endpoint_config;
    temperature_measurement::config_t cluster_config;

    matter_sensor.name = "Temperature Sensor";
    matter_sensor.measured_attribute_id = chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Id;

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


matter_sensor_config_t create_illumination_sensor(node_t * node) {
    matter_sensor_config_t matter_sensor;
    illuminance_sensor::config_t endpoint_config;
    illuminance_measurement::config_t cluster_config;

    matter_sensor.name = "Illumination Sensor";
    matter_sensor.measured_attribute_id = chip::app::Clusters::IlluminanceMeasurement::Attributes::MeasuredValue::Id;

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

matter_sensor_config_t create_humidity_sensor(node_t * node) {
    matter_sensor_config_t matter_sensor;
    relative_humidity_sensor::config_t endpoint_config;
    relative_humidity_measurement::config_t cluster_config;

    matter_sensor.name = "Humidity Sensor";
    matter_sensor.measured_attribute_id = chip::app::Clusters::RelativeHumidityMeasurement::Attributes::MeasuredValue::Id;

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

matter_sensor_config_t create_on_off_light(node_t * node) {
    matter_sensor_config_t matter_on_off_light;
    on_off_light::config_t endpoint_config;
    on_off::config_t cluster_config;

    matter_on_off_light.name = "On OFF Light";
    matter_on_off_light.measured_attribute_id = 0; //NA

    //Create an humidity sensor endpoint
    matter_on_off_light.endpoint_p = on_off_light::create(node, &endpoint_config, CLUSTER_FLAG_SERVER, NULL);
    if (!matter_on_off_light.endpoint_p) {
        ESP_LOGE(TAG, "Matter on/off light endpoint creation failed");
    }
    matter_on_off_light.endpoint_id = endpoint::get_id(matter_on_off_light.endpoint_p);
    ESP_LOGI(TAG, "Matter on/off light endpoint created with id %d", matter_on_off_light.endpoint_id);

    //Create a cluster for the humidity endpoint
    matter_on_off_light.cluster_p = on_off::create(matter_on_off_light.endpoint_p, &cluster_config, ENDPOINT_FLAG_NONE, NULL);
    matter_on_off_light.cluster_id = cluster::get_id(matter_on_off_light.cluster_p);
    ESP_LOGI(TAG, "Matter on/off light cluster created with cluster_id %d", matter_on_off_light.cluster_id);

    return matter_on_off_light;
}

void update_matter_sensor(matter_sensor_config_t sensor_config, esp_matter_attr_val_t * val) {
    update(sensor_config.endpoint_id, sensor_config.cluster_id, sensor_config.measured_attribute_id, val);
    //ESP_LOGI(TAG, "Sensor %s value: ", sensor_config.name.c_str());
    //val_print(sensor_config.endpoint_id, sensor_config.cluster_id, sensor_config.measured_attribute_id, val);
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    matter_sensor_config_t temp_sensor_config;
    matter_sensor_config_t illumination_sensor_config;
    matter_sensor_config_t humidity_sensor_config;
    matter_sensor_config_t on_off_light_config;

    configure_led();

    //bmp280_t bmp280_dev_descriptor;
    //matter_attr_val_bmp280_reading_t bmp280_reading;

    matter_attr_val_scd30_reading_t scd30_reading;
    i2c_dev_t scd30_dev_descriptor = {0};

    i2c_dev_t bh1750_dev_descriptor;
    esp_matter_attr_val_t bh1750_reading;

    ESP_ERROR_CHECK(i2cdev_init()); // Init i2c library

    //Initialize Sensors
    bh1750_sensor_init(&bh1750_dev_descriptor);
    //bmp280_sensor_init(&bmp280_dev_descriptor);
    scd30_sensor_init(&scd30_dev_descriptor);

    // Initialize the ESP NVS layer
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

    while(1) {
        //bmp280_reading = bmp280_sensor_update(&bmp280_dev_descriptor);
        bh1750_reading = bh1750_sensor_update(&bh1750_dev_descriptor);
        scd30_reading = scd30_sensor_update(&scd30_dev_descriptor);


        //If RED Test LED is low, the lux value will actually show a CO2 reading
        //(Matter does not natively support CO2 readings)
        //If it is high, the lux value will be read in.
        if(gpio_get_level(TEST_LED_GPIO)) {
            printf("Reading lux value: %d\n", bh1750_reading);
            update_matter_sensor(illumination_sensor_config, &bh1750_reading);
        } else if (scd30_reading.data_ready) {
            printf("Reading co2 ppm value: %d\n", scd30_reading.co2_reading.val);
            update_matter_sensor(illumination_sensor_config, &scd30_reading.co2_reading);
        }

        if(scd30_reading.data_ready) {
            update_matter_sensor(temp_sensor_config, &(scd30_reading.temperature_reading));
            update_matter_sensor(humidity_sensor_config, &scd30_reading.humidity_reading);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
