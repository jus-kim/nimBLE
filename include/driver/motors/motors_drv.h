/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       motors_drv.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Zephyr driver for DRV8220 DC and A4988 stepper motors.
 */

#ifndef ZEPHYR_DRIVER_MOTORS_DRV_H_
#define ZEPHYR_DRIVER_MOTORS_DRV_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>
#include <zephyr/kernel.h>


enum MOTOR_DIRECTION {
   MOTOR_DIR_FORWARD = 0,
   MOTOR_DIR_BACKWARD,
   MOTOR_DIR_NULL,
};


typedef int (*motors_drv_set_dc_pwm_t)(const struct device *dev, float duty_cycle);
typedef int (*motors_drv_move_dc_t)(const struct device *dev, const uint8_t dir, const uint8_t duty_cycle_per);
typedef int (*motors_drv_move_step_t)(const struct device *dev, const uint8_t dir, const uint32_t n_steps);

__subsystem struct motors_drv_api {
   motors_drv_set_dc_pwm_t  set_dc_pwm;
   motors_drv_move_dc_t     move_dc;
   motors_drv_move_step_t   move_step;
};


/**
 * @brief Sets the PWM duty cycle for the DC motor.
 *
 * @param[in] dev Motors driver device instance.
 * @param[in] duty_cycle PWM duty cycle from 0.0 to 1.0.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
__syscall int motors_drv_set_dc_pwm(const struct device *dev, float duty_cycle);

static inline int z_impl_motors_drv_set_dc_pwm(const struct device *dev, float duty_cycle)
{
    const struct motors_drv_api *api = (const struct motors_drv_api *)dev->api;
    return api->set_dc_pwm(dev, duty_cycle);
}

/**
 * @brief Moves the DC motor with specified direction and PWM percentage. Note that the 
 *        period of the DC motor PWM is set in 'motors0' in the device tree.
 *
 * @param[in] dev Motors driver device instance.
 * @param[in] dir Direction of the DC motor (see enum MOTOR_DIRECTION).
 * @param[in] duty_cycle_per PWM duty cycle percentage from 0 to 100.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
__syscall int motors_drv_move_dc(const struct device *dev, const uint8_t dir, const uint8_t duty_cycle_per);

static inline int z_impl_motors_drv_move_dc(const struct device *dev, const uint8_t dir, const uint8_t duty_cycle_per)
{
    const struct motors_drv_api *api = (const struct motors_drv_api *)dev->api;
    return api->move_dc(dev, dir, duty_cycle_per);
}

/**
 * @brief Moves the stepper motor with specified direction and total steps. Note that the 
 *        period of the stepper motor delay is set in 'motors0' in the device tree.
 *
 * @param[in] dev Motors driver device instance.
 * @param[in] dir Direction of the stepper motor (see enum MOTOR_DIRECTION).
 * @param[in] n_steps Total number of steps to complete.
 *
 * @retval 0 on success.
 * @retval Error code on failure.
 */
__syscall int motors_drv_move_step(const struct device *dev, const uint8_t dir, const uint32_t n_steps);

static inline int z_impl_motors_drv_move_step(const struct device *dev, const uint8_t dir, const uint32_t n_steps)
{
    const struct motors_drv_api *api = (const struct motors_drv_api *)dev->api;
    return api->move_step(dev, dir, n_steps);
}


#include <syscalls/motors_drv.h>

#endif /* ZEPHYR_DRIVER_MOTORS_DRV_H_ */
