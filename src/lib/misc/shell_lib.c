/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       shell_lib.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for custom shell commands.
 */

#include <zephyr/logging/log.h>
#include <bluetooth/services/nus.h>

#include <stdio.h>
#include <stdarg.h>

#include <lib/misc/shell_lib.h>
#include <lib/ble/ble_lib.h>

LOG_MODULE_REGISTER(LOG_SHELL_LIB);


static int32_t shell_lib_ble_tx(const struct device *dev, const uint8_t *buf, uint16_t len)
{
   int32_t ret = 0;

   if (ble_lib_get_connection_status()) {
      ret = bt_nus_send(NULL, buf, len);
      if (ret != 0)
      {
         LOG_ERR("bt_nus_send() failed, err %d", ret);
         return ret;
      }
   }

   return 0;
}


int32_t shell_lib_print(const struct shell *sh, const uint8_t *_buf, ...)
{
   int32_t ret = 0;
   char buf[64];
   va_list args;

   va_start(args, _buf);
   vsnprintf(buf, sizeof(buf), _buf, args);
   va_end(args);

	shell_print(sh, "%s", buf);
   ret = shell_lib_ble_tx(NULL, buf, strlen(buf));
   if (ret != 0) {
      return ret;
   }

	return 0;
}

int32_t shell_lib_error(const struct shell *sh, const uint8_t *_buf, ...)
{
   int32_t ret = 0;
   char buf[64];
   va_list args;
   
   va_start(args, _buf);
   vsnprintf(buf, sizeof(buf), _buf, args);
   va_end(args);

	shell_error(sh, "%s", buf);
   ret = shell_lib_ble_tx(NULL, buf, strlen(buf));
   if (ret != 0) {
      return ret;
   }

	return 0;
}
