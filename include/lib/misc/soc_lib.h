/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       soc_lib.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for SoC and misc. related stuff.
 */

#ifndef SOC_LIB_H_
#define SOC_LIB_H_

#include <zephyr/types.h>


/**
 * @brief Initializes all core SoC peripherals. This includes setting VDD to desired value 
 *        as well as peripherals related to power and communication (serial/USB and BLE).
 */
void soc_lib_init(void);


#endif /* SOC_LIB_H_ */
