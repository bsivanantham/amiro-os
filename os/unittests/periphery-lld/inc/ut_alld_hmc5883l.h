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

#ifndef _AMIROOS_UT_HMC5883L_LLD_HPP_
#define _AMIROOS_UT_HMC5883L_LLD_HPP_

#include <aos_unittest.h>
#include <amiro-lld.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_HMC5883L)) || defined(__DOXYGEN__)

#include <alld_hmc5883l.h>

/**
 * @brief   Constum data structure for this unit test.
 */
typedef struct {
  /**
   * @brief   HMC5338L driver to use.
   */
  HMC5883LDriver* driver;

  /**
   * @brief   Event source for certain tests.
   */
  event_source_t *drdyEvtSrc;

  /**
   * @brief   Event flags to watch.
   */
  eventflags_t evtflags;

  /**
   * @brief   Timeout value in microseconds.
   */
  apalTime_t timeout;
} ut_hmc5883ldata_t;

#ifdef __cplusplus
extern "C" {
#endif
  aos_utresult_t utAlldHmc5883lFunc(BaseSequentialStream* stream, aos_unittest_t* ut);
#ifdef __cplusplus
}
#endif

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_HMC5883L) */


#endif /* _AMIROOS_UT_HMC5883L_LLD_HPP_ */
