/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2018  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _AMIROOS_UT_ALLD_A3906_H_
#define _AMIROOS_UT_ALLD_A3906_H_

#include <aos_unittest.h>
#include <amiro-lld.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_A3906)) || defined(__DOXYGEN__)

#include <alld_a3906.h>

/**
 * @brief   Custom data structure for the unit test.
 */
typedef struct {
  /**
   * @brief   Pointer to the driver to use.
   */
  A3906Driver* driver;

  /**
   * @brief   PWM driver information.
   */
  struct {
    /**
     * @brief   The PWM driver to use.
     */
    apalPWMDriver_t* driver;

    /**
     * @brief   PWM channel information.
     */
    struct {
      /**
     * @brief   PWM channel for the left wheel forward direction.
     */
      apalPWMchannel_t left_forward;

      /**
     * @brief   PWM channel for the left wheel backward direction.
     */
      apalPWMchannel_t left_backward;

      /**
     * @brief   PWM channel for the right wheel forward direction.
     */
      apalPWMchannel_t right_forward;

      /**
     * @brief   PWM channel for the right wheel backward direction.
     */
      apalPWMchannel_t right_backward;
    } channel;
  } pwm;

  /**
   * @brief   QEI driver information
   */
  struct {
    /**
     * @brief   QEI driver for the left wheel.
     */
    apalQEIDriver_t* left;

    /**
     * @brief   QEI driver for the right wheel.
     */
    apalQEIDriver_t* right;

    /**
     * @brief   QEI increments per wheel revolution.
     */
    apalQEICount_t increments_per_revolution;
  } qei;

  /**
   * @brief   Wheel diameter in m.
   */
  float wheel_diameter;

  /**
   * @brief   Timeout value (in us).
   */
  apalTime_t timeout;
} ut_a3906data_t;

#ifdef __cplusplus
extern "C" {
#endif
  aos_utresult_t utAlldA3906Func(BaseSequentialStream* stream, aos_unittest_t* ut);
#ifdef __cplusplus
}
#endif

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_A3906) */

#endif /* _AMIROOS_UT_ALLD_A3906_H_ */
