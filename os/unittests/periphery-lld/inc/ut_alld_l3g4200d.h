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

#ifndef _AMIROOS_UT_L3G4200D_LLD_HPP_
#define _AMIROOS_UT_L3G4200D_LLD_HPP_

#include <aos_unittest.h>
#include <amiro-lld.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_L3G4200D)) || defined(__DOXYGEN__)

#include <alld_l3g4200d.h>

/**
 * @brief   Custom data structure for the unit test.
 */
typedef struct {
  /**
   * @brief   L3G4200D driver to use.
   */
  L3G4200DDriver *l3gd;

  /**
   * @brief   SPI configuratoin.
   */
  SPIConfig *spiconf;

  /**
   * @brief   Event source for certain tests.
   */
  event_source_t *src;

  /**
   * @brief   Event flags to watch.
   */
  eventflags_t evtflags;
} ut_l3g4200ddata_t;

#ifdef __cplusplus
extern "C" {
#endif
  aos_utresult_t utAlldL3g4200dFunc(BaseSequentialStream* stream, aos_unittest_t* ut);
#ifdef __cplusplus
}
#endif

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_L3G4200D) */

#endif /* _AMIROOS_UT_L3G4200D_LLD_HPP_ */

