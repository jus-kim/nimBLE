/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       led_drivers_shell.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Driver shell for LED drivers.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <lib/misc/shell_lib.h>
#include <driver/led_drivers/ltc3220.h>
#include <driver/led_drivers/led_drivers.h>


static const struct device *dev_led_drivers = DEVICE_DT_GET_ONE(adi_ltc3220);


static int32_t cmd_on(const struct shell *sh, size_t argc, char **argv)
{
   ARG_UNUSED(argc);
   int32_t ret = 0;
   char *end;
   
   float arg_led_num = strtof(argv[1], &end);
   if (*end != '\0')
   {
      shell_lib_error(sh, "Invalid arg[1]: %s", argv[1]);
      return -EINVAL;
   }
   float arg_led_per = strtof(argv[2], &end);
   if (*end != '\0')
   {
      shell_lib_error(sh, "Invalid arg[2]: %s", argv[2]);
      return -EINVAL;
   }

   ret = led_drivers_set_led(dev_led_drivers, arg_led_num, arg_led_per);
   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}

static int32_t cmd_allon(const struct shell *sh, size_t argc, char **argv)
{
   ARG_UNUSED(argc);
   int32_t ret = 0;
   const struct ltc3220_config *cfg = dev_led_drivers->config;

   for (uint8_t i = 0; i < cfg->led_count; i++)
   {
      ret = led_drivers_set_led(dev_led_drivers, i, 100);
      if (ret != 0)
      {
         shell_lib_error(sh, "ret err %d", ret);
         return -EIO;
      }
   }

	return 0;
}

static int32_t cmd_rst(const struct shell *sh, size_t argc, char **argv)
{
   ARG_UNUSED(argc);
   ARG_UNUSED(argv);
   int32_t ret = 0;

   ret = led_drivers_rst(dev_led_drivers);
   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

   return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(led_drivers_cmd,
	SHELL_CMD_ARG(on, NULL, "led_drivers on [led#] [percent]", cmd_on, 3, 0),
	SHELL_CMD_ARG(allon, NULL, "led_drivers rst", cmd_allon, 1, 0),
	SHELL_CMD_ARG(rst, NULL, "led_drivers rst", cmd_rst, 1, 0),
	SHELL_SUBCMD_SET_END // Array terminated
);
SHELL_CMD_REGISTER(led_drivers, &led_drivers_cmd, "led drivers driver cmds", NULL);
