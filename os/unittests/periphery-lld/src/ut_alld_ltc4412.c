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

#include <ut_alld_ltc4412.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_LTC4412)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_ltc4412.h>

aos_utresult_t utAlldLtc4412Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL);

  // local variables
  aos_utresult_t result = {0,0};
  uint32_t status;

  chprintf(stream, "read ctrl pin... \n");
  ltc4412_lld_ctrl_t ctrl;
  status = ltc4412_lld_get_ctrl((LTC4412Driver*)ut->data, &ctrl);
  if(status == APAL_STATUS_SUCCESS){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write ctrl pin... \n");
  status = ltc4412_lld_set_ctrl((LTC4412Driver*)ut->data, LTC4412_LLD_CTRL_ACTIVE);
  status |= ltc4412_lld_get_ctrl((LTC4412Driver*)ut->data, &ctrl);
  if (status == APAL_STATUS_SUCCESS && ctrl == LTC4412_LLD_CTRL_ACTIVE) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read stat pin... \n");
  ltc4412_lld_stat_t stat;
  status = ltc4412_lld_get_stat((LTC4412Driver*)ut->data, &stat);
  if(status == APAL_STATUS_SUCCESS){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(LTC4412Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_LTC4412) */
