/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ble_uart.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 BLE NUS.
 */

#ifndef BLE_UART_H_
#define BLE_UART_H_

#include <zephyr/types.h>


/**
 * @brief Initializes the Nordic Semiconductor's BLE UART service (NUS). 
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
int32_t ble_uart_init(void);


#endif /* BLE_UART_H_ */
