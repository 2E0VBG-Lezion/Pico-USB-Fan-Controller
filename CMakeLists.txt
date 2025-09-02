cmake_minimum_required(VERSION 3.13)

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")

include(pico-sdk/pico_sdk_init.cmake)
include(pico-sdk/lib/tinyusb/hw/bsp/family_support.cmake)

project(USBFanController)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_executable(USBFanController main.cpp Fan.cpp DutyCycle.cpp Project_Definitions.cpp Project_Functions.cpp usb_functions.cpp usb_descriptors.cpp usb_comms.cpp)

target_sources(USBFanController PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp 
        ${CMAKE_CURRENT_LIST_DIR}/Fan.cpp    
        ${CMAKE_CURRENT_LIST_DIR}/DutyCycle.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Project_Definitions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Project_Functions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PwmIn.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_functions.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_comms.cpp
        )

target_include_directories(USBFanController PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

    pico_generate_pio_header(USBFanController ${CMAKE_CURRENT_LIST_DIR}/PwmIn.pio)


target_link_libraries(USBFanController
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_pwm
    hardware_gpio
    hardware_adc
    hardware_clocks
    hardware_irq
    hardware_flash
    hardware_watchdog
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(USBFanController)

pico_enable_stdio_usb(USBFanController 0)
pico_enable_stdio_uart(USBFanController 1)