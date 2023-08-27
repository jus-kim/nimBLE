/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       tinyrc_shell.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Shell for tinyRC profile.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <lib/misc/shell_lib.h>
#include <profile/tinyrc.h>
#include <driver/motors/motors_drv.h>


static const struct device *dev_motors_drv = DEVICE_DT_GET_ONE(juskim_motors);


static int32_t cmd_m(const struct shell *sh, size_t argc, char **argv)
{
   const char *cmd_w_param[] = {
      "l", "r", "f", "b" };
   ARG_UNUSED(argc);
   int32_t ret = 0;
   char *end;

   int32_t arg_val = strtol(argv[2], &end, 10);

   if (strcmp(argv[1], cmd_w_param[0]) == 0) { // l (left)
      ret = motors_drv_move_step(dev_motors_drv, MOTOR_DIR_FORWARD, arg_val);
   }
   else if (strcmp(argv[1], cmd_w_param[1]) == 0) { // r (right)
      ret =motors_drv_move_step(dev_motors_drv, MOTOR_DIR_BACKWARD, arg_val);
   }
   else if (strcmp(argv[1], cmd_w_param[2]) == 0) { // f (forward)
      ret = motors_drv_move_dc(dev_motors_drv, MOTOR_DIR_FORWARD, arg_val);
   }
   else if (strcmp(argv[1], cmd_w_param[3]) == 0) { // b (backward)
      ret = motors_drv_move_dc(dev_motors_drv, MOTOR_DIR_BACKWARD, arg_val);
   }

   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}

static int32_t cmd_s(const struct shell *sh, size_t argc, char **argv)
{
   ARG_UNUSED(argc);
   ARG_UNUSED(argv);
   int32_t ret = 0;

   ret = motors_drv_move_dc(dev_motors_drv, MOTOR_DIR_FORWARD, 0);
   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}

static int32_t cmd_l(const struct shell *sh, size_t argc, char **argv)
{
   const char *cmd_w_param[] = {
      "def", "on", "off", "bl", "br" };
   ARG_UNUSED(argc);
   int32_t ret = 0;

   if (strcmp(argv[1], cmd_w_param[0]) == 0) { // def (default LEDs)
      ret = tinyrc_led_set_default();
   }
   else if (strcmp(argv[1], cmd_w_param[1]) == 0) { // on (all LEDs on)
      ret = tinyrc_led_set_allonoff(true);
   }
   else if (strcmp(argv[1], cmd_w_param[2]) == 0) { // off (all LEDs off)
      ret = tinyrc_led_set_allonoff(false);
   }
   else if (strcmp(argv[1], cmd_w_param[3]) == 0) // bl (blinker left)
   {
      if (strcmp(argv[2], "1") == 0) {
         ret = tinyrc_led_set_blinker(TINYRC_BLINKER_LEFT, true);
      }
      else if (strcmp(argv[2], "0") == 0) {
         ret = tinyrc_led_set_blinker(TINYRC_BLINKER_LEFT, false);
      }
      else {
         ret = -EINVAL;
      }
   }
   else if (strcmp(argv[1], cmd_w_param[4]) == 0) // br (blinker right)
   {
      if (strcmp(argv[2], "1") == 0) {
         ret = tinyrc_led_set_blinker(TINYRC_BLINKER_RIGHT, true);
      }
      else if (strcmp(argv[2], "0") == 0) {
         ret = tinyrc_led_set_blinker(TINYRC_BLINKER_RIGHT, false);
      }
      else {
         ret = -EINVAL;
      }
   }
   else
   {
      shell_lib_error(sh, "Invalid argument %s", argv[1]);
      ret = -EINVAL;
   }

   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(tinyrc_cmd,
	SHELL_CMD_ARG(m, NULL, "tinyrc m(move) [l/r/f/b] [val]", cmd_m, 3, 0),
	SHELL_CMD_ARG(s, NULL, "tinyrc s(stop)", cmd_s, 1, 0),
	SHELL_CMD_ARG(l, NULL, "tinyrc l(LED)", cmd_l, 2, 1),
	SHELL_SUBCMD_SET_END // Array terminated
);
SHELL_CMD_REGISTER(tinyrc, &tinyrc_cmd, "tinyRC profile cmds", NULL);