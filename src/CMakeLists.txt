#
# Copyright (c) 2023 juskim. All rights reserved.
# GitHub: jus-kim, YouTube: @juskim
#

cmake_minimum_required(VERSION 3.20.0)
add_compile_options(-Werror)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nimBLE)

# NORDIC SDK APP START
target_sources(app PRIVATE
   main.c
   lib/ble/ble_lib.c
   lib/ble/ble_uart.c
   lib/ble/ble_shell.c
   lib/misc/shell_lib.c
   lib/misc/soc_lib.c
   lib/uart/uart_lib.c
)

# Include profile specific modules
target_sources_ifdef(CONFIG_MOTORS_DRV app PRIVATE
   driver/motors/motors_drv.c
   driver/motors/motors_drv_shell.c
)
target_sources_ifdef(CONFIG_LED_DRIVERS app PRIVATE
   driver/led_drivers/led_drivers_shell.c
)
target_sources_ifdef(CONFIG_LTC3220 app PRIVATE
   driver/led_drivers/ltc3220.c
)
target_sources_ifdef(CONFIG_MCP73831 app PRIVATE
   driver/bat_charger/mcp73831.c
)
target_sources_ifdef(CONFIG_PROFILE_TINYRC app PRIVATE
   profile/tinyrc/tinyrc.c
   profile/tinyrc/tinyrc_shell.c
)

add_subdirectory(lib)
