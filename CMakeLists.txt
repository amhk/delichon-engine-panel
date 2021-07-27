cmake_minimum_required(VERSION 3.13)

# ---- Raspberry Pi Pico SDK ----
if (NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH: not set")
endif ()
if (NOT EXISTS $ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)
    message(FATAL_ERROR "PICO_SDK_PATH: bad path $ENV{PICO_SDK_PATH}")
endif ()
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# ---- OpenOCD binary ----
if (NOT DEFINED ENV{OPENOCD_BIN_PATH})
    message(FATAL_ERROR "OPENOCD_BIN_PATH: not set")
endif ()
if (NOT EXISTS $ENV{OPENOCD_BIN_PATH})
    message(FATAL_ERROR "OPENOCD_BIN_PATH: bad path $ENV{OPENOCD_BIN_PATH}")
endif ()

# ---- OpenOCD scripts ----
if (NOT DEFINED ENV{OPENOCD_SCRIPTS_PATH})
    message(FATAL_ERROR "OPENOCD_SCRIPTS_PATH: not set")
endif ()
if (NOT EXISTS $ENV{OPENOCD_SCRIPTS_PATH}/.)
    message(FATAL_ERROR "OPENOCD_SCRIPTS_PATH: bad path $ENV{OPENOCD_SCRIPTS_PATH}")
endif ()

# ---- the main project ----
project(delichon C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

add_executable(delichon
    button.c
    led.c
    main.c
    relay.c
)

target_compile_options(
    delichon
    PUBLIC
    -Wall
    -Wextra
    -Werror
)

target_link_libraries(
    delichon
    hardware_pwm
    pico_stdlib
)

pico_add_extra_outputs(delichon)

# ---- flash target ----
add_custom_target(flash
    COMMAND $ENV{OPENOCD_BIN_PATH}
    -f $ENV{OPENOCD_SCRIPTS_PATH}/interface/picoprobe.cfg
    -f $ENV{OPENOCD_SCRIPTS_PATH}/target/rp2040.cfg
    -s $ENV{OPENOCD_SCRIPTS_PATH}
    -c "program ${CMAKE_CURRENT_BINARY_DIR}/delichon.elf verify reset exit"
    DEPENDS delichon
)

# ---- serial console target ----
add_custom_target(console COMMAND screen /dev/ttyACM0 115200)

# ---- gdb server target ----
add_custom_target(gdb-server
    COMMAND $ENV{OPENOCD_BIN_PATH}
    -f $ENV{OPENOCD_SCRIPTS_PATH}/interface/picoprobe.cfg
    -f $ENV{OPENOCD_SCRIPTS_PATH}/target/rp2040.cfg
    -s $ENV{OPENOCD_SCRIPTS_PATH}
)

# ---- gdb client target ----
add_custom_target(gdb-client
    COMMAND gdb-multiarch
    -ex 'target remote localhost:3333'
    -ex 'load'
    -ex 'monitor reset init'
    -ex 'continue'
    ${CMAKE_CURRENT_BINARY_DIR}/delichon.elf
)
