#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <app_priv.h>
#include <app_reset.h>


static const char *TAG = "app_main";
uint16_t temp_temp_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::temp_endpoint;
using namespace esp_matter::cluster;

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
    ESP_LOGI(TAG, "Identification callback: type: %d, effect: %d", type, effect_id);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t temp_endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    return ESP_OK;
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;
    bmp280_t bmp280_dev_descriptor;

    bmp280_sensor_init(&bmp280_dev_descriptor);

    /* Initialize the ESP NVS layer */
    nvs_flash_init();

    /* Create a Matter node and add the mandatory Root Node device type on endpoint 0 */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);

    if(!node) {
        ESP_LOGE(TAG, "Matter node creation failed");
    }

    temperature_sensor::config_t temp_config;
    temp_endpoint_t *temp_endpoint = temperature_sensor::create(node, &temp_config, CLUSTER_FLAG_SERVER, NULL);
    
    /* These node and temp_endpoint handles can be used to create/add other endpoints and clusters. */
    if (!temp_endpoint) {
        ESP_LOGE(TAG, "Matter temp_endpoint creation failed");
    }

    temp_temp_endpoint_id = temp_endpoint::get_id(temp_endpoint);
    ESP_LOGI(TAG, "Temp measure created with temp_endpoint_id %d", temp_endpoint_id);

    relative_humidity_sensor::config_t
    endpoint_t *humidity_endpoint = endpoint::create(node, CLUSTER_FLAG_SERVER);
    if(!humidity_endpoint) {
        ESP_LOGE(TAG, "Matter humidity_endpoint creation failed");
    }

    temperature_measurement::config_t temperature_measurement_config;
    cluster_t *cluster = temperature_measurement::create(temp_endpoint, &temperature_measurement_config, CLUSTER_FLAG_SERVER);

    uint16_t cluster_id = cluster::get_id(cluster);
    ESP_LOGI(TAG, "Temp measurement cluster created with temp_endpoint_id %d", temp_endpoint_id);

    /* Matter start */
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter start failed: %d", err);
    }


    esp_matter_attr_val_t temperature_reading;

    while(1) {
        temperature_reading = bmp280_sensor_update(&bmp280_dev_descriptor);

        update(temp_temp_endpoint_id, cluster_id, chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Id, &temperature_reading);
        printf("%d degrees: ", temperature_reading.val); 
        val_print(temp_temp_endpoint_id, cluster_id, chip::app::Clusters::TemperatureMeasurement::Attributes::MeasuredValue::Id, &temperature_reading);

        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
