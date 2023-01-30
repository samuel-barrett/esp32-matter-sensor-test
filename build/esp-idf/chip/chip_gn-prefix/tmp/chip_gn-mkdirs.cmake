# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/esp-test/esp-matter/connectedhomeip/connectedhomeip"
  "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip"
  "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip/chip_gn-prefix"
  "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip/chip_gn-prefix/tmp"
  "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp"
  "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip/chip_gn-prefix/src"
  "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/esp-test/matter_temp_measure_esp32/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp/${subDir}")
endforeach()
