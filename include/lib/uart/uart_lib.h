/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       uart_lib.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 UART.
 */

#ifndef UART_LIB_H
#define UART_LIB_H

#include <zephyr/drivers/uart.h>


#define UART_BUF_SIZE CONFIG_BT_NUS_UART_BUFFER_SIZE


struct uart_data_t {
   void *fifo_reserved;
   uint8_t data[UART_BUF_SIZE];
   uint16_t len;
};


/**
 * @brief Puts the string of specified data to be outputted via UART.
 *
 * @param[in] tx String to output via UART.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
int32_t uart_lib_put(struct uart_data_t *tx);

/**
 * @brief Gets the UART datasets which includes the received data and total length of the 
 *        received data.
 *
 * @retval Returns the UART data structure.
 */
struct uart_data_t *uart_lib_get(void);

/**
 * @brief Initializes the SoC UART (HW) module. 
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
int32_t uart_lib_init(void);


#endif /* UART_LIB_H */
