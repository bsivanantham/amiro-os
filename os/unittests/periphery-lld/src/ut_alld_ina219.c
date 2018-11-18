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

#include <ut_alld_ina219.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_INA219)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_ina219.h>
#include <math.h>

aos_utresult_t utAlldIna219Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_ina219data_t*)(ut->data))->inad != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  uint16_t data[6];
  uint16_t write_data = 0x1011;
  uint16_t new_data[6];
  uint16_t reset_data;
  uint16_t test_calib = 0;
  int16_t usensor_data = 0;
  uint16_t busready = 0;
  uint32_t power = 0;
  int32_t shunt;
  uint32_t bus;

  chprintf(stream, "read registers...\n");
  status = ina219_lld_read_register(((ut_ina219data_t*)ut->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, data, 6, ((ut_ina219data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "write registers...\n");
  status = ina219_lld_write_register(((ut_ina219data_t*)ut->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, &write_data, 1, ((ut_ina219data_t*)ut->data)->timeout);
  status |= ina219_lld_read_register(((ut_ina219data_t*)ut->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, new_data, 6, ((ut_ina219data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && new_data[0] == write_data) {
    uint8_t errors = 0;
    for (uint8_t dataIdx = 1; dataIdx < 6; dataIdx++) {
      if (new_data[dataIdx] != data[dataIdx]) {
        ++errors;
      }
    }
    if (errors == 0) {
      aosUtPassed(stream, &result);
    } else {
      aosUtFailed(stream, &result);
    }
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "reset...\n");
  status = ina219_lld_reset(((ut_ina219data_t*)ut->data)->inad, ((ut_ina219data_t*)ut->data)->timeout);
  status |= ina219_lld_read_register(((ut_ina219data_t*)ut->data)->inad, INA219_LLD_REGISTER_CONFIGURATION, &reset_data, 1, ((ut_ina219data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && reset_data == 0x399F) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed\n");
    ++result.failed;
  }

  chprintf(stream, "read config...\n");
  ina219_lld_cfg_t ina_config;
  status = ina219_lld_read_config(((ut_ina219data_t*)ut->data)->inad, &ina_config, ((ut_ina219data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "write config...\n");
  ina_config.data = 0x7FFu;
  status = ina219_lld_write_config(((ut_ina219data_t*)ut->data)->inad, ina_config, ((ut_ina219data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }


  chprintf(stream, "calibrate...\n");
  ina219_lld_calib_input_t calib_in;
  calib_in.shunt_resistance_0 = 0.1;
  calib_in.max_expected_current_A = 0.075;
  calib_in.current_lsb_uA = 10;
  calib_in.cfg_reg = ina_config;
  ina219_lld_calib_output_t calib_out;
  status = ina219_lld_calibration(((ut_ina219data_t*)ut->data)->inad, &calib_in, &calib_out);
  status |= ina219_lld_write_calibration(((ut_ina219data_t*)ut->data)->inad, calib_out.calibration & 0xFFFEu, ((ut_ina219data_t*)ut->data)->timeout);
  status |= ina219_lld_write_calibration(((ut_ina219data_t*)ut->data)->inad, 0xA000, ((ut_ina219data_t*)ut->data)->timeout);
  ina219_lld_cfg_t test_config;
  ((ut_ina219data_t*)ut->data)->inad->current_lsb_uA = 0xA;
  status |= ina219_lld_read_config(((ut_ina219data_t*)ut->data)->inad, &test_config, ((ut_ina219data_t*)ut->data)->timeout);
  status |= ina219_lld_read_calibration(((ut_ina219data_t*)ut->data)->inad, &test_calib, ((ut_ina219data_t*)ut->data)->timeout);
  while (!busready || power == 0) {
    aosThdMSleep(20);
    status |= ina219_lld_bus_conversion_ready(((ut_ina219data_t*)ut->data)->inad, &busready, ((ut_ina219data_t*)ut->data)->timeout);
    status |= ina219_lld_read_power(((ut_ina219data_t*)ut->data)->inad, &power, ((ut_ina219data_t*)ut->data)->timeout);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read shunt voltage...\n");
  status = ina219_lld_read_shunt_voltage(((ut_ina219data_t*)ut->data)->inad, &shunt, ((ut_ina219data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tshunt voltage: %fV\n", (float)shunt/1000000.f);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read bus voltage (%u%% tolerance)...\n", (uint8_t)(((ut_ina219data_t*)ut->data)->tolerance * 100.f + 0.5f));
  status = ina219_lld_read_bus_voltage(((ut_ina219data_t*)ut->data)->inad, &bus, ((ut_ina219data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tbus voltage: %fV\n", (float)bus/1000000.f);
  if ((status == APAL_STATUS_SUCCESS) && (fabs(((float)bus/1000000.f) - ((ut_ina219data_t*)ut->data)->v_expected) < ((ut_ina219data_t*)ut->data)->v_expected * ((ut_ina219data_t*)ut->data)->tolerance)) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read power...\n");
  status = ina219_lld_read_power(((ut_ina219data_t*)ut->data)->inad, &power, ((ut_ina219data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tpower: %fW\n", (float)(power)/1000000.f);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read current...\n");
  status = ina219_lld_read_current(((ut_ina219data_t*)ut->data)->inad, &usensor_data, ((ut_ina219data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tcurrent: %fA\n", (float)(usensor_data)/1000000.f);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  aosUtInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(INA219Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_INA219) */
