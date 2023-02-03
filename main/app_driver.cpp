/**
 * @file app_driver.cpp
 * @author Samuel Barrett (samarbarrett)
 * @brief Containers for the various sensors used by this application
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright This example code is in the Public Domain (or CC0 licensed, at your option.)
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */


#include <i2cdev.h>
#include <bh1750.h>
#include <scd30.h>

#include "driver/gpio.h"

#include <app_priv.h>

static const char *TAG = "app_driver";

/**
 * @brief On off light endpoint identifier
 */
extern uint16_t light_endpoint_id;


/**
 * @brief Initializes the bh1850 CO2, pressure, and temperature sensor in continuous mode
 * 
 * @param[out] bh1750_dev_descriptor  The i2c device descriptor
*/
void bh1750_sensor_init(i2c_dev_t * bh1750_dev_descriptor) {
    memset(bh1750_dev_descriptor, 0, sizeof(i2c_dev_t)); // Zero descriptor

    ESP_ERROR_CHECK(bh1750_init_desc(bh1750_dev_descriptor, BH1750_ADDR, 0, (gpio_num_t) CONFIG_EXAMPLE_I2C_MASTER_SDA, (gpio_num_t) CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bh1750_setup(bh1750_dev_descriptor, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH));
}

/**
 * @brief Reads a value from the BH1750 sensor
 * 
 * @param[in] bh1750_dev_descriptor  The i2c device descriptor
 * 
 * @return  A nullable<int16_t> containing the lux
*/
esp_matter_attr_val_t bh1750_sensor_update(i2c_dev_t * bh1750_dev_descriptor) {
    esp_err_t err = ESP_OK;
    
    uint16_t lux;
    
    err = bh1750_read(bh1750_dev_descriptor, &lux);

    if(err != ESP_OK) {
        printf("BH1750 Read error, returning invalid\n");
        return esp_matter_invalid(NULL);
    }
    return esp_matter_nullable_int16(nullable<int16_t>(lux));
}

/**
 * @brief  Initializes the SCD30 CO2, humidity, and temperature sensor in continuous mode
 * 
 * @param[out] scd30_dev_descriptor  I2C device descriptor
*/
void scd30_sensor_init(i2c_dev_t * scd30_dev_descriptor) {

    ESP_ERROR_CHECK(scd30_init_desc(scd30_dev_descriptor, 0, (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SDA, (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SCL));
    
    uint16_t version, major_ver, minor_ver;
    ESP_ERROR_CHECK(scd30_read_firmware_version(scd30_dev_descriptor, &version));

    major_ver = (version >> 8) & 0xf;
    minor_ver = version & 0xf;

    ESP_LOGI(TAG, "SCD30 Firmware Version: %d.%d", major_ver, minor_ver);

    ESP_LOGI(TAG, "Starting continuous measurement");
    ESP_ERROR_CHECK(scd30_trigger_continuous_measurement(scd30_dev_descriptor, 0));
}

/** Updates the SCD30 CO2, humidity: 
 * 
*/
matter_attr_val_scd30_reading_t scd30_sensor_update(i2c_dev_t * scd30_dev_descriptor) {
    float co2 = 0.0, temperature = 0.0, humidity = 0.0;
    bool data_ready;

    scd30_get_data_ready_status(scd30_dev_descriptor, &data_ready);
    if(data_ready) {
        if(scd30_read_measurement(scd30_dev_descriptor, &co2, &temperature, &humidity) != ESP_OK) {
            ESP_LOGE(TAG, "SCD30 Read error");
            data_ready = false;
        }
    }

    matter_attr_val_scd30_reading_t scd30_reading = {
        .data_ready = data_ready,
        .co2_reading = esp_matter_nullable_int16(nullable<int16_t>(co2)),
        .temperature_reading = esp_matter_nullable_int16(nullable<int16_t>(temperature*100)),
        .humidity_reading = esp_matter_nullable_int16(nullable<int16_t>(humidity*100))
    };

    return scd30_reading;
}

/** Test Light
 * 
 * @brief Initializes TEST_LED_GPIO as INPUT + OUTPUT GPIO so that the LED can be turned onn or off
 */
void test_led_init() {
    ESP_ERROR_CHECK(gpio_reset_pin(TEST_LED_GPIO));
    ESP_ERROR_CHECK(gpio_set_direction(TEST_LED_GPIO, GPIO_MODE_INPUT_OUTPUT));
}

/** Get LED state
 * 
 * @return returns true if the indicator LED is on, otherwise false
 */
bool led_on() {
    return gpio_get_level(TEST_LED_GPIO);
}

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
esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, 
    uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val) {
    if(
        endpoint_id == light_endpoint_id 
        && cluster_id == chip::app::Clusters::OnOff::Id
        && attribute_id == chip::app::Clusters::OnOff::Attributes::OnOff::Id
    ) {
        return gpio_set_level(TEST_LED_GPIO, (uint8_t)val->val.b);
    }
    return ESP_OK;
}
