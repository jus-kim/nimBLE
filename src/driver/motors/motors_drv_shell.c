/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       motors_drv_shell.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Driver shell for DRV8220 DC and A4988 stepper motors.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <lib/misc/shell_lib.h>
#include <driver/motors/motors_drv.h>


#define MOTORS_DRV_TOTAL_CMD_W      1
#define MOTORS_DRV_TOTAL_CMD_MOVE   2


static const struct device *dev_motors_drv = DEVICE_DT_GET_ONE(juskim_motors);


static int32_t cmd_w(const struct shell *sh, size_t argc, char **argv)
{
   const char *cmd_w_param[MOTORS_DRV_TOTAL_CMD_W] = {
      "pwm", };
   ARG_UNUSED(argc);
   int32_t ret = 0;

   char *end;
   float arg_val = strtof(argv[2], &end);
   if (*end != '\0')
   {
      shell_lib_error(sh, "Invalid arg[2]: %s", argv[2]);
      return -EINVAL;
   }

   if (strcmp(argv[1], cmd_w_param[0]) == 0) { // pwm
      ret = motors_drv_set_dc_pwm(dev_motors_drv, arg_val);
   }

   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}

static int32_t cmd_move(const struct shell *sh, size_t argc, char **argv)
{
   const char *cmd_w_param[MOTORS_DRV_TOTAL_CMD_MOVE] = {
      "dc", "step" };
   ARG_UNUSED(argc);
   int32_t ret = 0;

   char *end;
   int32_t arg_dir = strtol(argv[2], &end, 10);
   if (*end != '\0')
   {
      shell_lib_error(sh, "Invalid arg[2]: %s", argv[2]);
      return -EINVAL;
   }
   int32_t arg_val = strtol(argv[3], &end, 10);
   if (*end != '\0')
   {
      shell_lib_error(sh, "Invalid arg[3]: %s", argv[3]);
      return -EINVAL;
   }

   if (strcmp(argv[1], cmd_w_param[0]) == 0) { // dc
      ret = motors_drv_move_dc(dev_motors_drv, arg_dir, arg_val);
   }
   else if (strcmp(argv[1], cmd_w_param[1]) == 0) { // step
      ret = motors_drv_move_step(dev_motors_drv, arg_dir, arg_val);
   }

   if (ret != 0)
   {
      shell_lib_error(sh, "ret err %d", ret);
      return -EIO;
   }

	return 0;
}


SHELL_STATIC_SUBCMD_SET_CREATE(motors_drv_cmd,
	SHELL_CMD_ARG(w, NULL, "motors_drv w [param] [value]", cmd_w, 3, 0),
	SHELL_CMD_ARG(move, NULL, "motors_drv move [dc/step] [dir] [pwm_duty/steps]", cmd_move, 4, 0),
	SHELL_SUBCMD_SET_END // Array terminated
);
SHELL_CMD_REGISTER(motors_drv, &motors_drv_cmd, "motors driver cmds", NULL);
