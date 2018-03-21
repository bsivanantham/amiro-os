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

#include <ut_alld_mpr121.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_MPR121)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <amiroos.h>

#define INTERRUPT_EVENT_ID            1

aos_utresult_t utAlldMpr121Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_mpr121data_t*)ut->data)->mprd != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  mpr121_lld_config_t cfg;
  uint8_t data, wdata, rdata;
  uint16_t edata[4];
  uint8_t tstate = 0;
  event_listener_t event_listener;
  aos_timestamp_t tstart, tcurrent, tend;

  chprintf(stream, "soft reset...\n");
  status = mpr121_lld_soft_reset(((ut_mpr121data_t*)ut->data)->mprd, ((ut_mpr121data_t*)ut->data)->timeout);
  status |= mpr121_lld_read_config(((ut_mpr121data_t*)ut->data)->mprd, &cfg, ((ut_mpr121data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && cfg.registers.ele_config == 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read register...\n");
  status = mpr121_lld_read_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_CONFIG_2, 0, 1, &data, ((ut_mpr121data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tdata: 0x%02X\n", data);
  if (status == APAL_STATUS_SUCCESS && data == (MPR121_LLD_CDT_0_5 | MPR121_LLD_ESI_16)) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write register...\n");
  wdata = MPR121_LLD_CDT_32 | MPR121_LLD_SFI_10 | MPR121_LLD_ESI_16;
  status = mpr121_lld_write_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_CONFIG_2, 0, 1, &wdata, ((ut_mpr121data_t*)ut->data)->timeout);
  aosThdMSleep(500);
  status |= mpr121_lld_read_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_CONFIG_2, 0, 1, &rdata, ((ut_mpr121data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tdata: 0x%02X\n", rdata);
  status |= mpr121_lld_write_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_CONFIG_2, 0, 1, &data, ((ut_mpr121data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && rdata == wdata) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read config...\n");
  status = mpr121_lld_read_config(((ut_mpr121data_t*)ut->data)->mprd, &cfg, ((ut_mpr121data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && cfg.registers.config_1 == 0x10 && cfg.registers.config_2 == 0x24) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\t\tconfig 1: 0x%02X\n", cfg.registers.config_1);
    chprintf(stream, "\t\tconfig 2: 0x%02X\n", cfg.registers.config_2);
    mpr121_lld_read_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_CONFIG_1, 0, 1, &rdata, ((ut_mpr121data_t*)ut->data)->timeout);
    chprintf(stream, "\t\tconfig 1: 0x%02X\n", rdata);
    mpr121_lld_read_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_CONFIG_2, 0, 1, &rdata, ((ut_mpr121data_t*)ut->data)->timeout);
    chprintf(stream, "\t\tconfig 2: 0x%02X\n", rdata);
    for (uint8_t valueIdx = 0; valueIdx < 8; valueIdx++) {
      chprintf(stream, "\t\tdata %d: 0x%02X\n", valueIdx, cfg.values[valueIdx]);
    }
    chprintf(stream, "ele_config: 0x%02X\n", cfg.registers.ele_config);
    chprintf(stream, "config_1: 0x%02X\n", cfg.registers.config_1);
    chprintf(stream, "config_2: 0x%02X\n", cfg.registers.config_2);
    chprintf(stream, "target_level: 0x%02X\n", cfg.registers.target_level);
    chprintf(stream, "low_side_limit: 0x%02X\n", cfg.registers.low_side_limit);
    chprintf(stream, "up_side_limit: 0x%02X\n", cfg.registers.up_side_limit);
    chprintf(stream, "auto_config_2: 0x%02X\n", cfg.registers.auto_config_2);
    chprintf(stream, "auto_config_1: 0x%02X\n", cfg.registers.auto_config_1);
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write config...\n");
  cfg.registers.ele_config = MPR121_LLD_CL_ON_ALL | MPR121_LLD_ELEPROX_0 | 4;
  cfg.registers.config_1 = MPR121_LLD_FFI_18 | 16;
  cfg.registers.config_2 = MPR121_LLD_CDT_1 | MPR121_LLD_SFI_10 | MPR121_LLD_ESI_32;
  cfg.registers.up_side_limit = 0x96u;
  cfg.registers.low_side_limit = 0x58u;
  cfg.registers.target_level = 0x68u;
  cfg.registers.auto_config_1 = MPR121_LLD_FFI_18 | MPR121_LLD_RETRY_2 | MPR121_LLD_BVA_ON_ALL | MPR121_LLD_AC_RECONF_EN;
  status = mpr121_lld_write_config(((ut_mpr121data_t*)ut->data)->mprd, cfg, ((ut_mpr121data_t*)ut->data)->timeout);
  aosThdMSleep(500);
  status |= mpr121_lld_read_config(((ut_mpr121data_t*)ut->data)->mprd, &cfg, ((ut_mpr121data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && cfg.registers.up_side_limit == 0x96u) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read filtered data for ten seconds...\n");
  chprintf(stream, "(values should change if you touch the sensors now)\n");
  for (uint8_t i = 0; i < 10; i++) {
    status = mpr121_lld_read_filtered_data(((ut_mpr121data_t*)ut->data)->mprd, 0, 4, edata, ((ut_mpr121data_t*)ut->data)->timeout);
    chprintf(stream, "\t\t0x%04X | 0x%04X | 0x%04X | 0x%04X\n", edata[0], edata[1], edata[2], edata[3]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "test interrupts...\n");
  status = AOS_OK;
  chEvtRegister(((ut_mpr121data_t*)ut->data)->evtsource, &event_listener, INTERRUPT_EVENT_ID);
  aosSysGetUptime(&tstart);
  tend = tstart + (30 * MICROSECONDS_PER_SECOND);
  do {
    aosSysGetUptime(&tcurrent);
    const aos_timestamp_t ttimeout = MICROSECONDS_PER_SECOND - ((tcurrent - tstart) % MICROSECONDS_PER_SECOND);
    const eventmask_t emask = chEvtWaitOneTimeout(EVENT_MASK(INTERRUPT_EVENT_ID), LL_US2ST(ttimeout));
    const eventflags_t eflags = chEvtGetAndClearFlags(&event_listener);
    if (emask == EVENT_MASK(INTERRUPT_EVENT_ID) && eflags == ((ut_mpr121data_t*)ut->data)->evtflags) {
      // interrupt detected
      status |= mpr121_lld_read_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_TOUCH_STATUS, 0, 1, &tstate, ((ut_mpr121data_t*)ut->data)->timeout);
      chprintf(stream, "\t\tinterrupt detected\n");
      chprintf(stream, "\t\ttstate: %X %X %X %X\n", tstate & 1, (tstate >> 1) & 1, (tstate >> 2) & 1, (tstate >> 3) & 1);
    }
    else if (emask == 0) {
      // timeout
      status |= mpr121_lld_read_register(((ut_mpr121data_t*)ut->data)->mprd, MPR121_LLD_REGISTER_TOUCH_STATUS, 0, 1, &tstate, ((ut_mpr121data_t*)ut->data)->timeout);
      chprintf(stream, "\t\ttstate: %X %X %X %X\n", tstate & 1, (tstate >> 1) & 1, (tstate >> 2) & 1, (tstate >> 3) & 1);
    }
    aosSysGetUptime(&tcurrent);
  } while (tcurrent < tend);
  chEvtUnregister(((ut_mpr121data_t*)ut->data)->evtsource, &event_listener);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(MPR121Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_MPR121) */
