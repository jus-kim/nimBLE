/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       soc_lib.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for nRF52 Soc and misc. related stuff.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/shell/shell.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/posix/unistd.h>

#include <ctype.h>
#include <version.h>
#include <stdlib.h>

#include <lib/misc/soc_lib.h>
#include <lib/ble/ble_lib.h>
#include <lib/uart/uart_lib.h>

LOG_MODULE_REGISTER(LOG_SOC_LIB);


// Enable internal DC/DC converter
#define NRFX_POWER_ENABLED                   1
#define NRFX_POWER_CONFIG_DEFAULT_DCDCEN     1
#define NRFX_POWER_CONFIG_DEFAULT_DCDCENHV   1


void error(char *function_name, int32_t err)
{
   LOG_ERR("%s failed with error code: %d", function_name, err);
   while (true) {
      k_sleep(K_MSEC(1000));  // Indefinitely sleep
   }
}

static void soc_lib_set_vdd_3v0(void)
{
   // First check to see if REGOUT0 is not 3V0
   if (NRF_UICR->REGOUT0 != UICR_REGOUT0_VOUT_3V0)
   {
      NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
         __NOP();
      }
      NRF_UICR->REGOUT0 = UICR_REGOUT0_VOUT_3V0;
      NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
         __NOP();
      }
      // System reset is needed to update UICR registers
      NVIC_SystemReset();
   }
}

static void soc_lib_init_peripherals(void)
{
   int err;
   const struct device *dev_usb_shell;

   // Initialize all core nRF peripherals
   /*err = uart_lib_init();
   if (err != 0) {
      error("uart_lib_init()", err);
   }*/
   err = ble_lib_init();
   if (err != 0) {
      error("ble_lib_init()", err);
   }
	dev_usb_shell = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
   if (device_is_ready(dev_usb_shell) == 0) {
      error("device_is_ready()", 0);
   }
   err = usb_enable(NULL);
   if (err != 0) {
      error("usb_enable()", err);
   }
}


void soc_lib_init(void)
{
   soc_lib_set_vdd_3v0();

   soc_lib_init_peripherals();

   LOG_INF("nRF52 SoC initialized");
}
