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

#ifndef _AMIROOS_UT_ALLD_BQ27500_BQ24193A_H_
#define _AMIROOS_UT_ALLD_BQ27500_BQ24193A_H_

#include <aos_unittest.h>
#include <amiro-lld.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ27500) && defined(AMIROLLD_CFG_USE_BQ24103A)) || defined(__DOXYGEN__)

#include <alld_bq27500.h>
#include <alld_bq24103a.h>

/**
 * @brief   Custom data structure for the unit test.
 */
typedef struct {
  /**
   * @brief   BQ27500 driver to use.
   */
  BQ27500Driver* bq27500;

  /**
   * @brief   BQ24103A driver to use.
   */
  BQ24103ADriver* bq24103a;

  /**
   * @brief   Timeout value (in us).
   */
  apalTime_t timeout;
} ut_bq27500bq24103adata_t;

#ifdef __cplusplus
extern "C" {
#endif
  aos_utresult_t utAlldBq27500Bq24103aFunc(BaseSequentialStream* stream, aos_unittest_t* ut);
#ifdef __cplusplus
}
#endif

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ27500) && defined(AMIROLLD_CFG_USE_BQ24103A) */

#endif /* _AMIROOS_UT_ALLD_BQ27500_BQ24193A_H_ */
