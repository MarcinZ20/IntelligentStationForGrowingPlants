# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/marcin/esp/esp-idf/components/bootloader/subproject"
  "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader"
  "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix"
  "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix/tmp"
  "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix/src"
  "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/marcin/esp/IntelligentStationForGrowingPlants/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
