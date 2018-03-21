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

#include <ut_alld_tps62113.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TPS62113)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_tps62113.h>

aos_utresult_t utAlldTps62113Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL);

  // local variables
  aos_utresult_t result = {0,0};
  uint32_t status;
  tps62113_lld_power_en_t power;

  chprintf(stream, "read pin... \n");
  status = tps62113_lld_get_power_en((TPS62113Driver*)ut->data, &power);
  if(status == APAL_STATUS_SUCCESS){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write pin... \n");
  status = tps62113_lld_set_power_en((TPS62113Driver*)ut->data, TPS62113_LLD_POWER_ENABLED);
  status |= tps62113_lld_get_power_en((TPS62113Driver*)ut->data, &power);
  if(status == APAL_STATUS_SUCCESS && power == TPS62113_LLD_POWER_ENABLED){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(TPS62113Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TPS62113) */


