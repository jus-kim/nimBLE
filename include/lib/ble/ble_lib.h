/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ble_lib.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 BLE.
 */

#ifndef BLE_LIB_H_
#define BLE_LIB_H_

#include <zephyr/types.h>


/**
 * @brief Gets the BLE connection status.
 *
 * @retval True (or false) if BLE is connected.
 */
bool ble_lib_get_connection_status(void);

/**
 * @brief Starts BLE advertisement with the specified packet datasets initialized in 
 *        ble_lib.c (i.e, bt_data ad[] and sd[]).
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
int32_t ble_lib_adv_start(void);

/**
 * @brief Stops BLE advertisement.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
int32_t ble_lib_adv_stop(void);

/**
 * @brief Initializes the BLE module which includes security (internal flash may be used 
 *        for storing keys) and all services. Advertisement 
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
int32_t ble_lib_init(void);


#endif /* BLE_LIB_H_ */
