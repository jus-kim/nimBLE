/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       ble_lib.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Shell for nRF52 BLE library.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <lib/misc/shell_lib.h>
#include <lib/ble/ble_lib.h>


#define BLE_LIB_TOTAL_CMD_ADV   2


static int32_t cmd_adv(const struct shell *sh, size_t argc, char **argv)
{
   const char *cmd_w_param[BLE_LIB_TOTAL_CMD_ADV] = {
      "start", "stop" };
   ARG_UNUSED(argc);
   int32_t ret = 0;
   
   if (strcmp(argv[1], cmd_w_param[0]) == 0) { // start
      ret = ble_lib_adv_start();
   }
   else if (strcmp(argv[1], cmd_w_param[1]) == 0) { // stop
      ret = ble_lib_adv_stop();
   }

   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(ble_lib_cmd,
	SHELL_CMD_ARG(adv, NULL, "ble adv [start/stop]", cmd_adv, 2, 0),
	SHELL_SUBCMD_SET_END // Array terminated
);
SHELL_CMD_REGISTER(ble, &ble_lib_cmd, "ble library cmds", NULL);
