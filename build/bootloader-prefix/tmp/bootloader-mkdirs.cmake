# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/esp-test/esp-idf/components/bootloader/subproject"
  "/Users/esp-test/matter_temp_measure_esp32/build/bootloader"
  "/Users/esp-test/matter_temp_measure_esp32/build/bootloader-prefix"
  "/Users/esp-test/matter_temp_measure_esp32/build/bootloader-prefix/tmp"
  "/Users/esp-test/matter_temp_measure_esp32/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/esp-test/matter_temp_measure_esp32/build/bootloader-prefix/src"
  "/Users/esp-test/matter_temp_measure_esp32/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/esp-test/matter_temp_measure_esp32/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
