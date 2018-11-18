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

#include <ut_alld_tps62113_ina219.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TPS62113) && defined(AMIROLLD_CFG_USE_INA219)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>

aos_utresult_t utAlldTps62113Ina219Func(BaseSequentialStream *stream, aos_unittest_t *ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_tps62113ina219data_t*)ut->data)->tps62113 != NULL && ((ut_tps62113ina219data_t*)ut->data)->ina219 != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  tps62113_lld_power_en_t power;
  uint32_t v_buson, v_busoff;

  chprintf(stream, "read pin... \n");
  status = tps62113_lld_get_power_en(((ut_tps62113ina219data_t*)ut->data)->tps62113, &power);
  aosThdSSleep(1);
  status |= ina219_lld_read_bus_voltage(((ut_tps62113ina219data_t*)ut->data)->ina219, &v_buson, ((ut_tps62113ina219data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && power == TPS62113_LLD_POWER_ENABLED) {
    aosUtPassedMsg(stream, &result, "enabled, %fV\n", (float)v_buson / 1000000.0f);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write pin... \n");
  status = tps62113_lld_set_power_en(((ut_tps62113ina219data_t*)ut->data)->tps62113, TPS62113_LLD_POWER_DISABLED);
  status |= tps62113_lld_get_power_en(((ut_tps62113ina219data_t*)ut->data)->tps62113, &power);
  aosThdSSleep(1);
  status |= ina219_lld_read_bus_voltage(((ut_tps62113ina219data_t*)ut->data)->ina219, &v_busoff, ((ut_tps62113ina219data_t*)ut->data)->timeout);
  status |= tps62113_lld_set_power_en(((ut_tps62113ina219data_t*)ut->data)->tps62113, TPS62113_LLD_POWER_ENABLED);
  if (status == APAL_STATUS_SUCCESS && power == TPS62113_LLD_POWER_DISABLED) {
    chprintf(stream, "\tdisabled, %fV\n", (float)v_busoff / 1000000.0f);
    if (v_buson > v_busoff) {
      aosUtPassed(stream, &result);
    } else {
      aosUtFailedMsg(stream, &result, "on: %fV; off: %fV\n", (float)v_buson / 1000000.0f, (float)v_busoff / 1000000.0f);
    }
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TPS62113) && defined(AMIROLLD_CFG_USE_INA219) */
