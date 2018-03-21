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

#include <ut_alld_pklcs1212e4001.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_PKLCS1212E4001)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>

aos_utresult_t utAlldPklcs1212e4001Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_pklcs1212e4001_t*)ut->data)->driver != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;

  chprintf(stream, "checking PWM configuration...\n");
  status = pklcs1212e4001_lld_checkPWMconfiguration(((ut_pklcs1212e4001_t*)ut->data)->driver);
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
    // abort at this point
    return result;
  }

  chprintf(stream, "buzzing for one second...\n");
  status = pklcs1212e4001_lld_enable(((ut_pklcs1212e4001_t*)ut->data)->driver, ((ut_pklcs1212e4001_t*)ut->data)->channel, true);
  aosThdSSleep(1);
  status |= pklcs1212e4001_lld_enable(((ut_pklcs1212e4001_t*)ut->data)->driver, ((ut_pklcs1212e4001_t*)ut->data)->channel, false);
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_PKLCS1212E4001) */
