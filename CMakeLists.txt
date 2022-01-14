cmake_minimum_required(VERSION 3.13)

# ---- Raspberry Pi Pico SDK ----
include(pico_sdk_import.cmake)

# ---- OpenOCD binary ----
set(OPENOCD_BIN_PATH $ENV{OPENOCD_BIN_PATH})
if (NOT OPENOCD_BIN_PATH)
    message("Defaulting OPENOCD_BIN_PATH to /usr/bin/openocd")
    set(OPENOCD_BIN_PATH /usr/bin/openocd)
endif ()
if (NOT EXISTS ${OPENOCD_BIN_PATH})
    message(FATAL_ERROR "OPENOCD_BIN_PATH: bad path ${OPENOCD_BIN_PATH}")
endif ()

# ---- OpenOCD scripts ----
set(OPENOCD_SCRIPTS_PATH $ENV{OPENOCD_SCRIPTS_PATH})
if (NOT OPENOCD_SCRIPTS_PATH)
    message("Defaulting OPENOCD_SCRIPTS_PATH to /usr/share/openocd/scripts")
    set(OPENOCD_SCRIPTS_PATH /usr/share/openocd/scripts)
endif ()
if (NOT EXISTS ${OPENOCD_SCRIPTS_PATH}/.)
    message(FATAL_ERROR "OPENOCD_SCRIPTS_PATH: bad path ${OPENOCD_SCRIPTS_PATH}")
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
    COMMAND ${OPENOCD_BIN_PATH}
    -f ${OPENOCD_SCRIPTS_PATH}/interface/picoprobe.cfg
    -f ${OPENOCD_SCRIPTS_PATH}/target/rp2040.cfg
    -s ${OPENOCD_SCRIPTS_PATH}
    -c "program ${CMAKE_CURRENT_BINARY_DIR}/delichon.elf verify reset exit"
    DEPENDS delichon
)

# ---- serial console target ----
add_custom_target(console COMMAND screen /dev/ttyACM0 115200)

# ---- gdb server target ----
add_custom_target(gdb-server
    COMMAND ${OPENOCD_BIN_PATH}
    -f ${OPENOCD_SCRIPTS_PATH}/interface/picoprobe.cfg
    -f ${OPENOCD_SCRIPTS_PATH}/target/rp2040.cfg
    -s ${OPENOCD_SCRIPTS_PATH}
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
