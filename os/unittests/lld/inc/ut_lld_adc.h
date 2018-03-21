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

#ifndef _AMIROOS_UT_LLD_ADC_H_
#define _AMIROOS_UT_LLD_ADC_H_

#include <aos_unittest.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (HAL_USE_ADC == TRUE))|| defined(__DOXYGEN__)

/**
 * @brief   Custom data structure for the unit test.
 */
typedef struct {
  /**
   * @brief   Pointer to the driver to use.
   */
  ADCDriver* driver;

  /**
   * @brief   Pointer to the conversion group to use.
   */
  ADCConversionGroup* convgroup;
} ut_adcdata_t;

#ifdef __cplusplus
extern "C" {
#endif
  aos_utresult_t utAdcFunc(BaseSequentialStream* stream, aos_unittest_t* ut);
#ifdef __cplusplus
}
#endif

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (HAL_USE_ADC == TRUE) */

#endif /* _AMIROOS_UT_LLD_ADC_H_ */
