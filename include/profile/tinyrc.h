/**
 * \copyright  Copyright 2023 juskim. All rights reserved.
 *             The code for this project follow the Apache 2.0 license and details 
 *             are provided in the LICENSE file located in the root folder of this 
 *             project. Details of SOUP used in this project can also be found in 
 *             the SOUP file located in the root folder.
 * 
 * @file       tinyrc.h
 * @author     juskim (GitHub: jus-kim, YouTube: @juskim)
 * @brief      Profile for tinyRC.
 */

#ifndef PROFILE_TINYRC_H_
#define PROFILE_TINYRC_H_

#include <zephyr/types.h>


// LED colour mapping
//
// There are x3 RGB LEDs in the front (i.e., 9 LED signals in front) and x3 RGB LEDs in 
// the back (i.e., 9 LED signals in back).
//
//        FRONT
//        CENTRE
//       ________
// LEFT |        | RIGHT
//      |   /\   |
//      |   ||   |
// LEFT |________| RIGHT
//        CENTRE
//         BACK
//
// Letter notations:
//    RED/GRE/BLU -> red/green/blue
//    F/B         -> front/back
//    L/R         -> left/right
//
// BACK LEDs
#define TINYRC_LED_RED_B_L             0
#define TINYRC_LED_BLU_B_L             1
#define TINYRC_LED_GRE_B_L             2
#define TINYRC_LED_RED_B_C             3
#define TINYRC_LED_BLU_B_C             4
#define TINYRC_LED_GRE_B_C             5
#define TINYRC_LED_RED_B_R             6
#define TINYRC_LED_BLU_B_R             7
#define TINYRC_LED_GRE_B_R             8
// FRONT LEDs
#define TINYRC_LED_RED_F_R             9
#define TINYRC_LED_BLU_F_R             10
#define TINYRC_LED_GRE_F_R             11
#define TINYRC_LED_RED_F_C             12
#define TINYRC_LED_BLU_F_C             13
#define TINYRC_LED_GRE_F_C             14
#define TINYRC_LED_RED_F_L             15
#define TINYRC_LED_BLU_F_L             16
#define TINYRC_LED_GRE_F_L             17

// LED default and other brightness percentages
#define TINYRC_LED_BRI_DEFAULT_FRONT   10
#define TINYRC_LED_BRI_DEFAULT_BACK    10
#define TINYRC_LED_BRI_BLINKER_FRONT   100
#define TINYRC_LED_BRI_BLINKER_BACK    100

#define TINYRC_LED_BLINKER_PERIOD_MS   250

typedef enum {
   TINYRC_BLINKER_LEFT = 0,
   TINYRC_BLINKER_RIGHT,
} tinyrc_blinker_side_t;


int32_t tinyrc_led_set_default(void);

int32_t tinyrc_led_set_allonoff(bool on);

int32_t tinyrc_led_set_blinker(tinyrc_blinker_side_t side, bool enable);

int32_t tinyrc_init(void);


#endif /* PROFILE_TINYRC_H_ */
