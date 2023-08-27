/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ltc3220.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Driver for LTC3220 LED drivers.
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#include <errno.h>

#include <driver/led_drivers/ltc3220.h>
#include <driver/led_drivers/led_drivers.h>

LOG_MODULE_REGISTER(LOG_LTC3220);

#define DT_DRV_COMPAT adi_ltc3220


static int32_t ltc3220_set_led(const struct device *dev, const uint8_t led_num, 
   const uint8_t on_percent)
{
   int32_t ret = 0;
   struct ltc3220_data *data = dev->data;
   const struct ltc3220_config *cfg = dev->config;
   uint8_t data_tx[2] = { led_num + 1, 0x00 };

   // Ensure params are good
   if ((led_num < 0) || (led_num >= cfg->led_count) ||
      (on_percent < 0) || (on_percent > 100))
   {
      LOG_ERR("Invalid parameter, led_num: %d, on_percent: %d", (int32_t)led_num, 
         (int32_t)on_percent);
      return -EINVAL;
   }

   data_tx[1] |= (uint8_t)((((float)on_percent) / 100.0) * 
      ((float)LTC3220_MAX_LED_LVL - 1.0));
   ret = i2c_burst_write_dt(&data->i2c, data_tx[0], &data_tx[1], 1);
   if (ret != 0)
   {
      LOG_ERR("Failed to write LTC3220 reg 0x%02X, err: %d", data_tx[0], ret);
      return ret;
   }

   LOG_DBG("Set LED num %d to %d%%", (int32_t)led_num, (int32_t)on_percent);
   
   return 0;
}

static int32_t ltc3220_rst(const struct device *dev)
{
   const struct ltc3220_config *cfg = dev->config;

   gpio_pin_set_dt(&cfg->nrst_gpio, 0);
   k_sleep(K_USEC(1));
   gpio_pin_set_dt(&cfg->nrst_gpio, 1);

   LOG_DBG("LED drivers reset");

   return 0;
}

static int32_t ltc3220_init(const struct device *dev)
{
   int32_t ret = 0;
   struct ltc3220_data *data = dev->data;
   const struct ltc3220_config *cfg = dev->config;
   data->dev = dev;

   if (!device_is_ready(data->i2c.bus))
   {
      LOG_ERR("Failed to get I2C device");
      return -EINVAL;
   }

   // Enable GPIO
   if (!device_is_ready(cfg->nrst_gpio.port))
   {
      LOG_ERR("GPIO device %s is not ready", cfg->nrst_gpio.port->name);
      return -EINVAL;
   }
   ret = gpio_pin_configure_dt(&cfg->nrst_gpio, GPIO_OUTPUT_HIGH);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure nrst_gpio pin");
      return ret;
   }

   LOG_INF("LTC3220 driver successfully initialized");

   return 0;
}


static const struct led_drivers_api drv_api = {
   .rst     = ltc3220_rst,
   .set_led = ltc3220_set_led,
};

// Driver instantiation macro
#define LTC3220_DEFINE(inst)                                               \
                                                                           \
   static struct ltc3220_data ltc3220_data##inst = {                       \
      .i2c = I2C_DT_SPEC_GET(DT_ALIAS(ltc3220)),                           \
   };                                                                      \
                                                                           \
   static const struct ltc3220_config ltc3220_config##inst = {             \
      .nrst_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(ltc3220), nrst_gpios),        \
      .led_count = DT_PROP(DT_ALIAS(ltc3220), led_count),                  \
   };                                                                      \
                                                                           \
   DEVICE_DT_INST_DEFINE(inst,                                             \
                         ltc3220_init, NULL,                               \
                         &ltc3220_data##inst, &ltc3220_config##inst,       \
                         POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,    \
                         &drv_api);

DT_INST_FOREACH_STATUS_OKAY(LTC3220_DEFINE)  // Ignore VS Code error here
