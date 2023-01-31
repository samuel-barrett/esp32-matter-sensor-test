/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>


#include <app_priv.h>
#include <app_reset.h>

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

    printf("Lux: %d\n", lux);
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
    
    printf("Temperature: %f, Pressure: %f\n", temp, pressure);

    bmp280_reading = { 
        esp_matter_nullable_int16((nullable<int16_t>) (temp*100)), 
        esp_matter_nullable_int16((nullable<int16_t>) (pressure ))
    };

    return bmp280_reading;
}

