/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       motors_drv.c
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Driver for DRV8220 DC and A4988 stepper motors.
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include <errno.h>

#include <driver/motors/motors_drv.h>

LOG_MODULE_REGISTER(LOG_MOTORS_DRV);

#define DT_DRV_COMPAT juskim_motors


struct motors_drv_config
{
   struct gpio_dt_spec nsleep_gpio;
   struct gpio_dt_spec en_gpio;
   struct gpio_dt_spec dc_ph_gpio;
   struct gpio_dt_spec step_step_gpio;
   struct gpio_dt_spec step_dir_gpio;
   struct gpio_dt_spec step_nrst_gpio;
   int32_t step_period_us;
};

struct motors_drv_data {
   const struct device *dev;
   const struct pwm_dt_spec dc_en_pwm;
};

struct motors_states_s {
   bool dc_en;
   bool step_en;
};


static struct motors_states_s motors_states = { 
   .dc_en = false, 
   .step_en = false,
};
static struct k_timer timer_step_motor;
static struct gpio_dt_spec step_gpio, nsleep_gpio, en_gpio;
static volatile bool step_toggle;
static volatile uint32_t step_toggle_cnt, step_toggle_cnt_max;


static void timer_step_motor_cb(struct k_timer *timer)
{
   // TODO: use GPIOTE + PPI to generate pulse signal
   if (step_toggle) {
      gpio_pin_set_dt(&step_gpio, 1);
   }
   else
   {
      gpio_pin_set_dt(&step_gpio, 0);
      step_toggle_cnt++;
      if (step_toggle_cnt >= step_toggle_cnt_max)
      {
         // We're done! Stop timer and power down motors if we can
         k_timer_stop(&timer_step_motor);
         motors_states.step_en = false;
         if (motors_states.dc_en == 0)
         {
            gpio_pin_configure_dt(&nsleep_gpio, GPIO_OUTPUT_LOW);
            gpio_pin_configure_dt(&en_gpio, GPIO_OUTPUT_LOW);
         }
      }
   }
   step_toggle = !step_toggle;
}

static int32_t move_step(const struct device *dev, const uint8_t dir, const uint32_t n_steps)
{
   const struct motors_drv_config *cfg = dev->config;

   // TODO: put some limit on n_steps

   // Set direction pin and lift !RST pin
   switch (dir)
   {
   case MOTOR_DIR_FORWARD:
      gpio_pin_set_dt(&cfg->step_dir_gpio, 0);
      break;
   case MOTOR_DIR_BACKWARD:
      gpio_pin_set_dt(&cfg->step_dir_gpio, 1);
      break;
   case MOTOR_DIR_NULL:
      break;
   default:
      LOG_ERR("Invalid direction param, dir: %d", (int32_t)dir);
      return -EINVAL;
   }

   // Set stepper motor enable flag and turn on motors
   motors_states.step_en = true;
   gpio_pin_configure_dt(&nsleep_gpio, GPIO_OUTPUT_HIGH);
   gpio_pin_configure_dt(&en_gpio, GPIO_OUTPUT_HIGH);

   // Set step pulse period and start timer
   step_toggle_cnt = 0;
   step_toggle_cnt_max = n_steps * 2;
   k_timer_start(&timer_step_motor, K_USEC(cfg->step_period_us), K_USEC(cfg->step_period_us));

   return 0;
}

static int32_t set_dc_pwm(const struct device *dev, const float duty_cycle)
{
   int32_t ret = 0;
   struct motors_drv_data *data = dev->data;

   // Ensure correct duty cycle value
   if (duty_cycle < 0.0f || duty_cycle > 1.0f)
   {
      LOG_ERR("Invalid duty cycle value: %f", duty_cycle);
      return -EINVAL;
   }

   // Set the PWM duty cycle
   ret = pwm_set_pulse_dt(&data->dc_en_pwm, data->dc_en_pwm.period * duty_cycle);
   if (ret != 0)
   {
      LOG_ERR("pwm_set_pulse_dt() failed, err %d", ret);
      return ret;
   }

   return 0;
}

static int32_t move_dc(const struct device *dev, const uint8_t dir, const uint8_t duty_cycle_per)
{
   int32_t ret = 0;
   const struct motors_drv_config *cfg = dev->config;

   // Ensure correct duty cycle percent value
   if (duty_cycle_per < 0 || duty_cycle_per > 100)
   {
      LOG_ERR("Invalid duty cycle percent value: %d", (int32_t)duty_cycle_per);
      return -EINVAL;
   }

   // Set direction pin
   switch (dir)
   {
   case MOTOR_DIR_FORWARD:
      gpio_pin_configure_dt(&cfg->dc_ph_gpio, GPIO_OUTPUT_HIGH);
      break;
   case MOTOR_DIR_BACKWARD:
      gpio_pin_configure_dt(&cfg->dc_ph_gpio, GPIO_OUTPUT_LOW);
      break;
   case MOTOR_DIR_NULL:
      break;
   default:
      LOG_ERR("Invalid direction param, dir: %d", (int32_t)dir);
      return -EINVAL;
   }

   // Set PWM duty cycle
   float duty_cycle = ((float)duty_cycle_per) / 100.0;
   ret = set_dc_pwm(dev, duty_cycle);
   if (ret != 0)
   {
      LOG_ERR("set_dc_pwm() failed, err %d", ret);
      return ret;
   }

   // Turn on DC motor or turn off all motors if we can
   if (duty_cycle_per > 0)
   {
      motors_states.dc_en = true;
      gpio_pin_configure_dt(&nsleep_gpio, GPIO_OUTPUT_HIGH);
      gpio_pin_configure_dt(&en_gpio, GPIO_OUTPUT_HIGH);
   }
   else
   {
      motors_states.dc_en = false;
      if (motors_states.step_en == 0) {
         gpio_pin_configure_dt(&nsleep_gpio, GPIO_OUTPUT_LOW);
         gpio_pin_configure_dt(&en_gpio, GPIO_OUTPUT_LOW);
      }
   }


   return 0;
}

static int32_t motors_drv_init(const struct device *dev)
{
   int32_t ret = 0;
   struct motors_drv_data *data = dev->data;
   const struct motors_drv_config *cfg = dev->config;
   data->dev = dev;

   // Enable PWM
   if (!device_is_ready(data->dc_en_pwm.dev))
   {
		LOG_ERR("PWM device is not ready");
		return -EINVAL;
	}

   // Enable all GPIOs
   if (!device_is_ready(cfg->nsleep_gpio.port))
   {
      LOG_ERR("GPIO device %s is not ready", cfg->nsleep_gpio.port->name);
      return -EINVAL;
   }
   nsleep_gpio = cfg->nsleep_gpio;
   ret = gpio_pin_configure_dt(&cfg->nsleep_gpio, GPIO_OUTPUT_LOW);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure nsleep_gpio pin");
      return ret;
   }


   if (!device_is_ready(cfg->en_gpio.port))
   {
      LOG_ERR("GPIO device %s is not ready", cfg->en_gpio.port->name);
      return -EINVAL;
   }
   en_gpio = cfg->en_gpio;
   ret = gpio_pin_configure_dt(&cfg->en_gpio, GPIO_OUTPUT_LOW);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure en_gpio pin");
      return ret;
   }


   if (!device_is_ready(cfg->dc_ph_gpio.port))
   {
      LOG_ERR("GPIO device %s is not ready", cfg->dc_ph_gpio.port->name);
      return -EINVAL;
   }
   ret = gpio_pin_configure_dt(&cfg->dc_ph_gpio, GPIO_OUTPUT_LOW);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure dc_ph_gpio pin");
      return ret;
   }
   ret = gpio_pin_configure_dt(&cfg->step_step_gpio, GPIO_OUTPUT_LOW);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure step_step_gpio pin");
      return ret;
   }
   step_gpio = cfg->step_step_gpio;
   ret = gpio_pin_configure_dt(&cfg->step_dir_gpio, GPIO_OUTPUT_LOW);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure step_dir_gpio pin");
      return ret;
   }
   ret = gpio_pin_configure_dt(&cfg->step_nrst_gpio, GPIO_OUTPUT_HIGH);
   if (ret != 0)
   {
      LOG_ERR("Unable to configure step_nrst_gpio pin");
      return ret;
   }

   // Power off all motors at the beginning
   ret = move_dc(dev, MOTOR_DIR_FORWARD, 0);
   if (ret != 0)
   {
      LOG_ERR("move_dc() failed, err %d", ret);
      return ret;
   }

   // Init timer for stepper motor
   k_timer_init(&timer_step_motor, timer_step_motor_cb, NULL);

   LOG_INF("Motors driver successfully initialized");

   return 0;
}


static const struct motors_drv_api drv_api = {
   .set_dc_pwm = set_dc_pwm,
   .move_dc    = move_dc,
   .move_step  = move_step,
};


// Driver instantiation macro
#define MOTORS_DRV_DEFINE(inst)                                               \
                                                                              \
   static struct motors_drv_data motors_drv_data##inst = {                    \
      .dc_en_pwm = PWM_DT_SPEC_GET(DT_ALIAS(motors0)),                        \
   };                                                                         \
                                                                              \
   static const struct motors_drv_config motors_drv_config##inst = {          \
      .nsleep_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(motors0), nsleep_gpios),       \
      .en_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(motors0), en_gpios),       \
      .dc_ph_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(motors0), dc_ph_gpios),         \
      .step_step_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(motors0), step_step_gpios), \
      .step_dir_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(motors0), step_dir_gpios),   \
      .step_nrst_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(motors0), step_nrst_gpios), \
      .step_period_us = DT_PROP(DT_ALIAS(motors0), step_period_us),           \
   };                                                                         \
                                                                              \
   DEVICE_DT_INST_DEFINE(inst,                                                \
                         motors_drv_init, NULL,                               \
                         &motors_drv_data##inst, &motors_drv_config##inst,    \
                         POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,       \
                         &drv_api);

DT_INST_FOREACH_STATUS_OKAY(MOTORS_DRV_DEFINE)  // Ignore VS Code error here
