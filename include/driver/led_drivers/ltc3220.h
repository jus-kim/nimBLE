/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ltc3220.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Driver for LTC3220 LED drivers.
 */

#ifndef ZEPHYR_DRIVER_LTC3220_H_
#define ZEPHYR_DRIVER_LTC3220_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>


// LTC3220 device parameters (DO NOT MODIFY!)
#define LTC3220_I2C_ADDR               0x1C
#define LTC3220_TOTAL_LEDS             18
#define LTC3220_MAX_LED_LVL            64

// LTC3220 register addresses
#define LTC3220_COMMAND                0x00
#define LTC3220_ULED1                  0x01
#define LTC3220_ULED2                  0x02
#define LTC3220_ULED3                  0x03
#define LTC3220_ULED4                  0x04
#define LTC3220_ULED5                  0x05
#define LTC3220_ULED6                  0x06
#define LTC3220_ULED7                  0x07
#define LTC3220_ULED8                  0x08
#define LTC3220_ULED9                  0x09
#define LTC3220_ULED10                 0x0A
#define LTC3220_ULED11                 0x0B
#define LTC3220_ULED12                 0x0C
#define LTC3220_ULED13                 0x0D
#define LTC3220_ULED14                 0x0E
#define LTC3220_ULED15                 0x0F
#define LTC3220_ULED16                 0x10
#define LTC3220_ULED17                 0x11
#define LTC3220_ULED18                 0x12
#define LTC3220_GRAD_BLINK             0x13

// LTC3220 register masks
#define LTC3220_COMMAND_QCKWR_MASK     0x01
#define LTC3220_COMMAND_MODE_MASK      0x06
#define LTC3220_COMMAND_SHDWN_MASK     0x08
#define LTC3220_ULED1_TO_18_LED_MASK   0x3F
#define LTC3220_ULED1_TO_18_MODE_MASK  0xC0


struct ltc3220_config
{
   struct gpio_dt_spec nrst_gpio;
   int32_t led_count;
};

struct ltc3220_data {
   const struct device *dev;
   const struct i2c_dt_spec i2c;
};


#endif /* ZEPHYR_DRIVER_LTC3220_H_ */
