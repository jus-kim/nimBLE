/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       mcp73831.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Driver for MCP73831 battery charger.
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#include <errno.h>

#include <driver/bat_charger/mcp73831.h>
#include <driver/bat_charger/bat_charger.h>

LOG_MODULE_REGISTER(LOG_MCP73831);

#define DT_DRV_COMPAT mt_mcp73831


struct mcp73831_config
{
   struct gpio_dt_spec stat_gpio;
   int32_t maxcharge_ma;
};

struct mcp73831_data {
   const struct device *dev;
};


static int32_t mcp73831_get_status(const struct device *dev)
{
   // TODO: get the STAT signal and indicate if charging or not

   return 0;
}

static int32_t mcp73831_init(const struct device *dev)
{
   int32_t ret;
   const struct mcp73831_config *cfg = dev->config;
   
   // Enable GPIO
   if (!device_is_ready(cfg->stat_gpio.port))
   {
      LOG_ERR("GPIO device %s is not ready", cfg->stat_gpio.port->name);
      return -EINVAL;
   }
   ret = gpio_pin_configure_dt(&cfg->stat_gpio, GPIO_INPUT);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure stat_gpio pin");
      return ret;
   }

   LOG_INF("MCP73831 driver successfully initialized");

   return 0;
}


static const struct bat_charger_api drv_api = {
   .get_status = mcp73831_get_status,
};

// Driver instantiation macro
#define MCP73831_DEFINE(inst)                                              \
                                                                           \
   static struct mcp73831_data mcp73831_data##inst;                        \
                                                                           \
   static const struct mcp73831_config mcp73831_config##inst = {           \
      .stat_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(mcp73831), stat_gpios),       \
      .maxcharge_ma = DT_PROP(DT_ALIAS(mcp73831), maxcharge_ma),           \
   };                                                                      \
                                                                           \
   DEVICE_DT_INST_DEFINE(inst,                                             \
                         mcp73831_init, NULL,                              \
                         &mcp73831_data##inst, &mcp73831_config##inst,     \
                         POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,    \
                         &drv_api);

DT_INST_FOREACH_STATUS_OKAY(MCP73831_DEFINE)  // Ignore VS Code error here
