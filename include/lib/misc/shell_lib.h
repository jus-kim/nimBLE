/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       shell_lib.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Library for custom shell commands.
 */

#ifndef SHELL_LIB_H_
#define SHELL_LIB_H_

#include <zephyr/types.h>
#include <zephyr/shell/shell.h>


int32_t shell_lib_print(const struct shell *sh, const uint8_t *_buf, ...);

int32_t shell_lib_error(const struct shell *sh, const uint8_t *_buf, ...);


#endif /* SHELL_LIB_H_ */
