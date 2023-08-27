#
# Copyright (c) 2023 juskim. All rights reserved.
# GitHub: jus-kim, YouTube: @juskim
#

zephyr_library_named(uart_async_adapter)
zephyr_library_sources_ifdef(CONFIG_BT_NUS_UART_ASYNC_ADAPTER
uart_async_adapter.c
)

target_link_libraries(app PUBLIC uart_async_adapter)
