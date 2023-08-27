/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       main.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Main file for the nimBLE.
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <lib/misc/soc_lib.h>
#include <profile/tinyrc.h>

LOG_MODULE_REGISTER(LOG_MAIN);


int32_t main(void)
{
   // NOTE: init functions do not return if error occurs!
   soc_lib_init();

   tinyrc_init();

   for (;;) {
      k_sleep(K_MSEC(1000));
   }
}
