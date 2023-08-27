/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       tinyrc.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Profile for tinyRC.
 */

#include <zephyr/logging/log.h>

#include <profile/tinyrc.h>
#include <driver/led_drivers/ltc3220.h>
#include <driver/led_drivers/led_drivers.h>

LOG_MODULE_REGISTER(LOG_TINYRC);


static const struct device *dev_led_drivers = DEVICE_DT_GET_ONE(adi_ltc3220);

typedef struct work_info {
    struct k_work_delayable work;
    struct k_mutex lock;
} work_info_t;

static work_info_t blinker_work;
static volatile bool blinker_toggle = false;
static bool led_def_enabled = false;
static volatile bool blinker_left_enabled = false, blinker_right_enabled = false;

static void blinker_work_cb(struct k_work *item)
{
   if (!blinker_toggle)
   {
      blinker_toggle = true;

      if (blinker_left_enabled)
      {
         // Front left LEDs
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_L, 
               TINYRC_LED_BRI_BLINKER_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_L, 
               TINYRC_LED_BRI_BLINKER_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_L, 
               TINYRC_LED_BRI_BLINKER_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         // Back left LED
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_L, 
               TINYRC_LED_BRI_BLINKER_BACK) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
      }
      if (blinker_right_enabled)
      {
         // Front right LEDs
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_R, 
               TINYRC_LED_BRI_BLINKER_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_R, 
               TINYRC_LED_BRI_BLINKER_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_R, 
               TINYRC_LED_BRI_BLINKER_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         // Back right LED
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_R, 
               TINYRC_LED_BRI_BLINKER_BACK) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
      }
   }
   else
   {
      blinker_toggle = false;

      if (blinker_left_enabled)
      {
         // Front left LEDs
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_L, 
               TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_L, 
               TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_L, 
               TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         // Back left LED
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_L, 
               TINYRC_LED_BRI_DEFAULT_BACK) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
      }
      if (blinker_right_enabled)
      {
         // Front right LEDs
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_R, 
               TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_R, 
               TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_R, 
               TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
         // Back right LED
         if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_R, 
               TINYRC_LED_BRI_DEFAULT_BACK) != 0) {
            LOG_ERR("Failed led_drivers_set_led()");
         }
      }
   }

   k_work_reschedule(&blinker_work.work, K_MSEC(TINYRC_LED_BLINKER_PERIOD_MS));
}

int32_t tinyrc_led_set_default(void)
{
   led_def_enabled = true;

   // Turn all front LEDs to white with default brightness percentage
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_L, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_L, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_L, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_C, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_C, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_C, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_F_R, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_BLU_F_R, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_GRE_F_R, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }

   // Turn all back LEDs to red with default brightness percentage
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_L, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_C, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }
   if (led_drivers_set_led(dev_led_drivers, TINYRC_LED_RED_B_R, 
         TINYRC_LED_BRI_DEFAULT_FRONT) != 0) {
      return -EIO;
   }

   return 0;
}

int32_t tinyrc_led_set_allonoff(bool on)
{
   int32_t ret = 0;
   const struct ltc3220_config *cfg = dev_led_drivers->config;

   if (on)
   {
      for (uint8_t i = 0; i < cfg->led_count; i++)
      {
         ret = led_drivers_set_led(dev_led_drivers, i, 100);
         if (ret != 0) {
            return -EIO;
         }
      }
   }
   else
   {
      // Reset all LEDs and stop any timer work
      ret = led_drivers_rst(dev_led_drivers);
      k_work_cancel_delayable(&blinker_work.work);
      led_def_enabled = false;
   }
   if (ret != 0) {
      return ret;
   }
   
   return 0;
}

int32_t tinyrc_led_set_blinker(tinyrc_blinker_side_t side, bool enable)
{
   int32_t ret = 0;

   switch (side)
   {
   case TINYRC_BLINKER_LEFT:
      blinker_left_enabled = enable;
      LOG_INF("Left blinker state = %d", (int32_t)blinker_left_enabled);
      break;
   case TINYRC_BLINKER_RIGHT:
      blinker_right_enabled = enable;
      LOG_INF("Right blinker state = %d", (int32_t)blinker_right_enabled);
      break;
   }

   // Stop blinker work if both blinkers are disabled
   if ((blinker_left_enabled == false) && (blinker_right_enabled == false)) {
      k_work_cancel_delayable(&blinker_work.work);
      blinker_toggle = false;
      if (led_def_enabled)
      {
         ret = tinyrc_led_set_default();
         if (ret != 0) {
            return ret;
         }
      }
      else
      {
         ret = tinyrc_led_set_allonoff(false);
         if (ret != 0) {
            return ret;
         }
      }
   }
   else {
      k_work_schedule(&blinker_work.work, K_MSEC(TINYRC_LED_BLINKER_PERIOD_MS));
   }

   return 0;
}

int32_t tinyrc_init(void)
{
   k_work_init_delayable(&blinker_work.work, blinker_work_cb);

   return 0;
}
