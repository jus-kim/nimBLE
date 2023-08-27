/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       bat_charger.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Zephyr driver for battery charger.
 */

#ifndef ZEPHYR_DRIVER_BAT_CHARGER_H_
#define ZEPHYR_DRIVER_BAT_CHARGER_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>
#include <zephyr/kernel.h>


typedef int (*bat_charger_get_status_t)(const struct device *dev);

__subsystem struct bat_charger_api {
   bat_charger_get_status_t     get_status;
};


/**
 * @brief Gets the status register values of the battery charger. This may include 
 *        charging status, etc.
 *
 * @param[in] dev Battery charger driver device instance.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
__syscall int bat_charger_get_status(const struct device *dev);

static inline int z_impl_bat_charger_get_status(const struct device *dev)
{
    const struct bat_charger_api *api = (const struct bat_charger_api *)dev->api;
    return api->get_status(dev);
}


#include <syscalls/bat_charger.h>

#endif /* ZEPHYR_DRIVER_BAT_CHARGER_H_ */
