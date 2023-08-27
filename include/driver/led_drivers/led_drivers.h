/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       led_drivers.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Zephyr driver for LED drivers.
 */

#ifndef ZEPHYR_DRIVER_LED_DRIVERS_H_
#define ZEPHYR_DRIVER_LED_DRIVERS_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>
#include <zephyr/kernel.h>


typedef int (*led_drivers_rst_t)(const struct device *dev);
typedef int (*led_drivers_set_led_t)(const struct device *dev, const uint8_t led_num, 
    const uint8_t on_percent);

__subsystem struct led_drivers_api {
   led_drivers_rst_t        rst;
   led_drivers_set_led_t    set_led;
};


/**
 * @brief Resets the LED driver device.
 *
 * @param[in] dev LED drivers driver device instance.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
__syscall int led_drivers_rst(const struct device *dev);

static inline int z_impl_led_drivers_rst(const struct device *dev)
{
    const struct led_drivers_api *api = (const struct led_drivers_api *)dev->api;
    return api->rst(dev);
}

/**
 * @brief Sets the LED brightness.
 *
 * @param[in] dev LED drivers driver device instance.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
__syscall int led_drivers_set_led(const struct device *dev, const uint8_t led_num, 
    const uint8_t on_percent);

static inline int z_impl_led_drivers_set_led(const struct device *dev, 
    const uint8_t led_num, const uint8_t on_percent)
{
    const struct led_drivers_api *api = (const struct led_drivers_api *)dev->api;
    return api->set_led(dev, led_num, on_percent);
}


#include <syscalls/led_drivers.h>

#endif /* ZEPHYR_DRIVER_LED_DRIVERS_H_ */
