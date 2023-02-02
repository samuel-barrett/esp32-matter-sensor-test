/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <i2cdev.h>
#include <bmp280.h>
#include <bh1750.h>
#include <scd30.h>

#include <app_priv.h>

static const char *TAG = "app_driver";


void bh1750_sensor_init(i2c_dev_t * bh1750_dev_descriptor) {
    memset(bh1750_dev_descriptor, 0, sizeof(i2c_dev_t)); // Zero descriptor

    ESP_ERROR_CHECK(bh1750_init_desc(bh1750_dev_descriptor, ADDR, 0, (gpio_num_t) CONFIG_EXAMPLE_I2C_MASTER_SDA, (gpio_num_t) CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bh1750_setup(bh1750_dev_descriptor, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH));
}


esp_matter_attr_val_t bh1750_sensor_update(i2c_dev_t * bh1750_dev_descriptor) {
    esp_err_t err = ESP_OK;
    
    uint16_t lux;
    
    err = bh1750_read(bh1750_dev_descriptor, &lux);

    if(err != ESP_OK) {
        printf("BH1750 Read error, returning null\n");
        return esp_matter_nullable_int16(nullable<int16_t>());
    }
    return esp_matter_nullable_int16(nullable<int16_t>(lux));
}


void bmp280_sensor_init(bmp280_t * bmp280_dev_descriptor) {

    //Initialize temp sensor params
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(bmp280_dev_descriptor, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(bmp280_dev_descriptor, BMP280_I2C_ADDRESS_1, 0, (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SDA, (gpio_num_t)CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(bmp280_dev_descriptor, &params));

    ESP_LOGE(TAG, "Found temperature, and pressure sensor with id: %d\n", bmp280_dev_descriptor->id);
}


matter_attr_val_bmp280_reading_t bmp280_sensor_update(bmp280_t * bmp280_dev_descriptor) {
    esp_err_t err = ESP_OK;

    float pressure, temp, humidity;

    matter_attr_val_bmp280_reading_t bmp280_reading = { 
        esp_matter_nullable_int16(nullable<int16_t>()), 
        esp_matter_nullable_int16(nullable<int16_t>())
    };

    err = bmp280_read_float(bmp280_dev_descriptor, &temp, &pressure, &humidity);
    
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "BMP280 read error, returning null: %d\n", bmp280_dev_descriptor->id);
        return bmp280_reading;
    }
    
    ESP_LOGI(TAG, "Temperature: %f, Pressure: %f\n", temp, pressure);

    bmp280_reading = { 
        esp_matter_nullable_int16(nullable<int16_t>(temp*100)), 
        esp_matter_nullable_int16(nullable<int16_t>(pressure))
    };

    return bmp280_reading;
}

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

matter_attr_val_scd30_reading_t scd30_sensor_update(i2c_dev_t * scd30_dev_descriptor) {
    float co2, temperature, humidity;
    bool data_ready;

    matter_attr_val_scd30_reading_t scd30_reading = { 
        false,
        esp_matter_nullable_int16(nullable<int16_t>()), 
        esp_matter_nullable_int16(nullable<int16_t>()),
        esp_matter_nullable_int16(nullable<int16_t>())
    };

    scd30_get_data_ready_status(scd30_dev_descriptor, &data_ready);

    if(scd30_read_measurement(scd30_dev_descriptor, &co2, &temperature, &humidity) == ESP_OK && data_ready) {

        ESP_LOGI(TAG, "SCD30 CO2: %.0f ppm, Temperature: %.2f °C, Humidity: %.2f %%\n", co2, temperature, humidity);

        scd30_reading = {
            true,
            esp_matter_nullable_int16(nullable<int16_t>(co2)), 
            esp_matter_nullable_int16(nullable<int16_t>(temperature*100)),
            esp_matter_nullable_int16(nullable<int16_t>(humidity*100))
        };
    }else{
        ESP_LOGI(TAG, "SCD30 data not ready");
    }

    return scd30_reading;
}
