# ESP32 Matter Temperature and Illumination Sensor

## Introduction

This code demonstrates an IoT sensor setup for measuring temperature and illumination (lux) using the Matter standard. It uses the Matter protocol, 

## Tested Hardware

* Adafruit ESP32 Feather microcontroller
* BMP280 Temperature and Pressure Sensor (connected using the I2C protocol)
* BH1750 Light Sensor (connected using the I2C protocol)
* Matter hub (Apple TV 4k 3rd Gen with Ethernet)
* iPhone with the Apple Home App installed

## Software

* ESP-IDF: Espressif's official IoT Development Framework
  * See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html

* ESP-MATTER: Espressif's official SDK for Matter applications
  * Uses a modified repository which includes functions for light and pressure readings

  * See https://github.com/samuel-barrett/esp-matter

* ESP-IDF-LIB:
  * See https://esp-idf-lib.readthedocs.io/en/latest

## Purpose

The purpose of this code is to provide a sample implementation of a temperature and illumination IoT sensor setup using the Matter standard. The code can be used as a starting point for building similar IoT sensor applications.

## Installation

* Clone this repository, using the following command:

* Install git lfs, on mac:

      brew install git-lfs   

* Install the ESP-IDF framework, following the instructions provided at https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html

* Clone the ESP-MATTER repository, using the following command:

        $ git clone --recursive https://github.com/samuel-barrett/esp-matter.git
  * If any submodules are not fully cloned, to update them, use:
        
        git submodule update --init --recursive

* Connect the BMP280, and BH1750 sensors to the microcontroller according to the i2C protocol (See photo below).

* Install ESP-IDF VSCode extension

* Build and flash the code to the ESP32 microcontroller using the ESP-IDF tools.

* Open the serial console to see the temperature and illumination readings.

## Configuration

The code includes default settings for the temperature and illumination readings, but these can be adjusted by modifying the relevant parameters in the code.

## Limitations

The code has been tested with the specified hardware and software, and is known to work correctly. However, it is possible that there may be some limitations or compatibility issues with other hardware or software.

## Future Work

The code is currently a basic implementation of the temperature and illumination sensor setup, and there is potential for future updates and improvements, such as:

Adding more sensors or endpoints
Improving the logging and data storage functionality
Adding support for remote management and control

## Conclusion

The ESP32 Matter Temperature and Illumination Sensor code provides a simple and effective way to set up an IoT sensor for measuring temperature and illumination. It is a great starting point for building similar IoT applications, and has the potential for future updates and improvements.
