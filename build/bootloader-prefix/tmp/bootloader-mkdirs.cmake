# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v4.4.6/components/bootloader/subproject"
  "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader"
  "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader-prefix"
  "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader-prefix/tmp"
  "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader-prefix/src"
  "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/OS/Desktop/ESP-IDF_PROJECT/aaa_Wifi_Module_My_Project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
