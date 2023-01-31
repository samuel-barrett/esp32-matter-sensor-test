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
#include <bh1750.h>
#include <string.h>

#define CONFIG_EXAMPLE_I2C_ADDRESS_LO 0
//#define CONFIG_EXAMPLE_I2C_ADDRESS_HI 0 

#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_LO)
#define ADDR BH1750_ADDR_LO
#endif
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_HI)
#define ADDR BH1750_ADDR_HI
#endif

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif


typedef struct {
   esp_matter_attr_val_t temperature_reading;
   esp_matter_attr_val_t pressure_reading;
} matter_attr_val_bmp280_reading_t; 


void bh1750_sensor_init(i2c_dev_t * bh1750_dev_descriptor);

esp_matter_attr_val_t bh1750_sensor_update(i2c_dev_t * bh1750_dev_descriptor);

matter_attr_val_bmp280_reading_t bmp280_sensor_update(bmp280_t * bmp280_dev_descriptor);

void bmp280_sensor_init(bmp280_t * bmp280_dev_descriptor);



