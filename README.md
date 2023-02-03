# ESP32 Matter Temperature and Illumination Sensor

## Introduction

This code demonstrates a proof of concept for an IoT sensor setup to measure temperature, humidity, illumination (lux), and even co2, using the Matter standard.

## Required Hardware

* Adafruit ESP32 Feather microcontroller
* BMP280 Temperature and Pressure Sensor (connected using the I2C protocol)
* BH1750 Light Sensor (connected using the I2C protocol)
* Matter hub (Apple TV 4k 3rd Gen with Ethernet)
* iPhone with the Apple Home App installed

## Resources

* [ESP-IDF: Espressif's official IoT Development Framework](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

* [ESP-MATTER: Espressif's official SDK for Matter applications](https://github.com/samuel-barrett/esp-matter)
  * Link to a forked repository which includes functions for light and pressure readings

* [ESP-IDF-LIB: Library containing drivers for sensor readings](https://esp-idf-lib.readthedocs.io/en/latest)

* [Documentation for setting up Matter](https://github.com/espressif/connectedhomeip/blob/4088a77f557e8571a39338fad51a1d8eb0131d79/docs/guides/BUILDING.md)


## Purpose

The purpose of this code is to provide a sample implementation of a temperature and illumination IoT sensor setup using the Matter standard. The code can be used as a starting point for building similar IoT sensor applications.

## Setup Procedure (Apple Silicon Mac)

This setup procedure was test with an Apple Silicon Mac, but could likely be modified to work for Intel Macs, Linux machines, and potentially Windows.

### Step 1: Clone repository

* Install git lf

      brew install git-lfs 

* Clone the repository, using the following command:

      git clone --recursive https://github.com/samuel-barrett/esp32-matter-sensor-test.git
* If you have cloned the repository, but need to add the submodules, use:

      git submodule update --init --recursive
* If you alerady have ESP-MATTER installed, you can do a shallow clone, and then fork that repository to use the samuel-barrett/esp-matter.git remote fork

    * To preserve your current esp-matter repository, you can copy the contents first

          cp -rH [PATH TO ESP-MATTER]/esp-matter [SOME PATH]/esp-matter-fork
    * To do a shallow clone:

          git clone https://github.com/samuel-barrett/esp32-matter-sensor-test.git
    * Then change directories to the esp-matter repository you want to modify, and change the remote to point to this fork: [samuel-barrett/esp-matter](https://github.com/samuel-barrett/esp-matter.git)

          cd [ESP-MATTER Repository to modify]
          git remote set-url origin https://github.com/samuel-barrett/esp-matter.git
          git pull


### Step 3: Install and Configure the ESP-IDF and VSCode Extension

* Install Visual Studio Code if not installed

* Install and configure the extension named "Espressif IDF" by following the instructions provided [here](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md)

  * Note: If you have done a recursive clone then, the esp-idf repository is already downloaded, and you can select it during setup


### Step 4: Configure VS Extension to for project repository

* Open the esp32-matter-sensor-test folder in visual studio code

* Type [CMD]+[SHIFT]+[P], then type 
    
      >ESP-IDF: Install ESP-Matter

* Select "Yes, download all submodules" -> "Use existing repository"

  * Select the forked repository from the previous step

* Type [CMD]+[SHIFT]+[P], then type 
    
      >ESP-IDF: Build your project

### Step 5: Configure hardware

* Connect the BMP280, and BH1750 sensors to the microcontroller using the i2C protocol

* To ensure, that the devices are connected properly, you can use the [Arduino I2C scanner sketch](https://playground.arduino.cc/Main/I2cScanner/)

  * First install the [Arduino ID ESP32 addon](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

### Step 6: Build flash and monitor

* Build and flash the code to the ESP32 microcontroller using the ESP-IDF tools. Type [CMD]+[SHIFT]+[P], then

      >ESP-IDF: Build, Flash and start a monitor on your device

### Step 7: Connect device via QR code

    TODO: Find out how to get QR code

## Limitations

The code has been tested with a very specific set of devices, and is not designed to be portable. It can however, be modified to fit 

## Future Work

The code is currently a basic implementation of the temperature and illumination sensor setup, and there is potential for future updates and improvements, such as:

- Adding more sensors or endpoints
- Adding support for different kinds of ESP32 development boards
- Improving the logging and data storage functionality
- Adding support for remote management and control

## Conclusion