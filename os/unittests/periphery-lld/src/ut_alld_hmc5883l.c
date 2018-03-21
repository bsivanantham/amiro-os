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

#include <ut_alld_hmc5883l.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_HMC5883L)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <alld_hmc5883l.h>
#include <aos_thread.h>
#include <aos_system.h>

/**
 * @brief   HMC5338L unit test function.
 *
 * @param[in] stream  Stream for intput/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldHmc5883lFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && (((ut_hmc5883ldata_t*)(ut->data))->drdyEvtSrc != NULL));

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  uint8_t rxbuffer[3];
  uint8_t data;
  uint8_t conf = 0x70;
  hmc5883l_lld_config_t cfg;
  uint16_t mdata[3];
  uint8_t state;
  eventmask_t event_mask = 0;
  event_listener_t el;
  uint16_t data_reads = 0;
  aos_timestamp_t start;
  aos_timestamp_t t;

  chprintf(stream, "check ID...\n");
  status = hmc5883l_lld_check(((ut_hmc5883ldata_t*)ut->data)->driver, rxbuffer, 3, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && rxbuffer[0] == HMC5883L_LLD_IDENTIFICATION_A
                                   && rxbuffer[1] == HMC5883L_LLD_IDENTIFICATION_B
                                   && rxbuffer[2] == HMC5883L_LLD_IDENTIFICATION_C) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed\n");
    ++result.failed;
    chprintf(stream, "\t\tstatus: %d, idA: %d, idB: %d, idC: %d\n", status, rxbuffer[0], rxbuffer[1], rxbuffer[2]);
    chprintf(stream, "\t\texpected idA: %d, idB: %d, idC: %d\n", HMC5883L_LLD_IDENTIFICATION_A, HMC5883L_LLD_IDENTIFICATION_B, HMC5883L_LLD_IDENTIFICATION_C);
  }

  chprintf(stream, "read register...\n");
  status = hmc5883l_lld_read_register(((ut_hmc5883ldata_t*)ut->data)->driver, HMC5883L_LLD_REGISTER_IDENTIFICATION_A, &data, 1, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  if ((status == APAL_STATUS_OK || status == APAL_STATUS_WARNING) && data == HMC5883L_LLD_IDENTIFICATION_A) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write register...\n");
  status = hmc5883l_lld_write_register(((ut_hmc5883ldata_t*)ut->data)->driver, HMC5883L_LLD_REGISTER_CONFIG_A, &conf, 1, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  status |= hmc5883l_lld_read_register(((ut_hmc5883ldata_t*)ut->data)->driver, HMC5883L_LLD_REGISTER_CONFIG_A, &data, 1, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  if ((status == APAL_STATUS_SUCCESS || status == APAL_STATUS_WARNING) && data == 0x70) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n, data: d", status, data);
  }

  chprintf(stream, "read configuration\n");
  status = hmc5883l_lld_read_config(((ut_hmc5883ldata_t*)ut->data)->driver, &cfg, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write configuration\n");
  cfg.avg = HMC5883L_LLD_AVG1;
  cfg.outrate = HMC5883L_LLD_75_HZ;
  cfg.mbias = HMC5883L_LLD_MB_NORMAL;
  cfg.gain = HMC5883L_LLD_GN_0_GA;
  cfg.highspeed = HMC5883L_LLD_HS_DISABLE;
  cfg.mode = HMC5883L_LLD_MM_CONTINUOUS;
  status = hmc5883l_lld_write_config(((ut_hmc5883ldata_t*)ut->data)->driver, cfg, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  status |= hmc5883l_lld_read_config(((ut_hmc5883ldata_t*)ut->data)->driver, &cfg, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  if (status == APAL_STATUS_OK &&
      cfg.avg == HMC5883L_LLD_AVG1 &&
      cfg.outrate == HMC5883L_LLD_75_HZ &&
      cfg.mbias == HMC5883L_LLD_MB_NORMAL &&
      cfg.gain == HMC5883L_LLD_GN_0_GA &&
      cfg.highspeed == HMC5883L_LLD_HS_DISABLE &&
      cfg.mode == HMC5883L_LLD_MM_CONTINUOUS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read all data for five seconds...\n");
  status = hmc5883l_lld_set_register(((ut_hmc5883ldata_t*)ut->data)->driver,HMC5883L_LLD_REGISTER_MODE,HMC5883L_LLD_MM_CONTINUOUS,((ut_hmc5883ldata_t*)ut->data)->timeout);
  for (uint8_t i = 0; i < 5; ++i) {
    status |= hmc5883l_lld_read_data(((ut_hmc5883ldata_t*)ut->data)->driver, mdata, ((ut_hmc5883ldata_t*)ut->data)->timeout);
    chprintf(stream, "\t\tX = 0x%04X\tY = 0x%04X\tZ = 0x%04X\n",mdata[0],mdata[1],mdata[2]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read status...\n");
  status = hmc5883l_lld_read_lock(((ut_hmc5883ldata_t*)ut->data)->driver, &state, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  chprintf(stream, "\t\tsensor lock: %d\n", state);
  status |= hmc5883l_lld_read_rdy(((ut_hmc5883ldata_t*)ut->data)->driver, &state, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  chprintf(stream, "\t\tsensor rdy: %d\n", state);
  status |= hmc5883l_lld_read_status(((ut_hmc5883ldata_t*)ut->data)->driver, &state, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  chprintf(stream, "\t\tsensor status: %d\n", state);
  if (status == APAL_STATUS_OK || status == APAL_STATUS_WARNING) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "interrupt (partial data read)...\n");
  chEvtRegister(((ut_hmc5883ldata_t*)ut->data)->drdyEvtSrc, &el, 0);
  status = hmc5883l_lld_write_config(((ut_hmc5883ldata_t*)ut->data)->driver, cfg, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  aosThdSSleep(1);
  status |= hmc5883l_lld_read_data(((ut_hmc5883ldata_t*)ut->data)->driver, mdata, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  aosThdMSleep(10);
  event_mask = chEvtWaitOneTimeout(EVENT_MASK(0), TIME_IMMEDIATE);
  status |= hmc5883l_lld_read_register(((ut_hmc5883ldata_t*)ut->data)->driver, HMC5883L_LLD_REGISTER_DATA_OUT_X_MSB, &data, 1, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  aosThdMSleep(10);
  status |= hmc5883l_lld_read_register(((ut_hmc5883ldata_t*)ut->data)->driver, HMC5883L_LLD_REGISTER_DATA_OUT_X_LSB, &data, 1, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  aosThdMSleep(10);
  event_mask = chEvtWaitOneTimeout(EVENT_MASK(0), TIME_IMMEDIATE);
  chprintf(stream, "\t\teventmask = 0x%08X (should be 0)\n", event_mask);
  if (event_mask != EVENT_MASK(0)) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "interrupt (full data read)...\n");
  status |= hmc5883l_lld_read_data(((ut_hmc5883ldata_t*)ut->data)->driver, mdata, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  event_mask = chEvtWaitOneTimeout(EVENT_MASK(0), LL_US2ST(100 * MICROSECONDS_PER_MILLISECOND));
  chprintf(stream, "\t\teventmask = 0x%08X (should be 1)\n", event_mask);
  if (event_mask == EVENT_MASK(0)) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read data based on interrupt...\n");
  status = hmc5883l_lld_read_config(((ut_hmc5883ldata_t*)ut->data)->driver, &cfg, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  uint8_t mode = HMC5883L_LLD_MM_SINGLE;
  cfg.mode = mode;
  status |= hmc5883l_lld_write_config(((ut_hmc5883ldata_t*)ut->data)->driver, cfg, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  aosSysGetUptime(&start);
  aosSysGetUptime(&t);
  status |= hmc5883l_lld_read_data(((ut_hmc5883ldata_t*)ut->data)->driver, mdata, ((ut_hmc5883ldata_t*)ut->data)->timeout);
  while (t - start < MICROSECONDS_PER_SECOND) {
    event_mask = chEvtWaitOneTimeout(EVENT_MASK(0), US2ST(1000));
    if (event_mask == EVENT_MASK(0)) {
      status |= hmc5883l_lld_read_data(((ut_hmc5883ldata_t*)ut->data)->driver, mdata, ((ut_hmc5883ldata_t*)ut->data)->timeout);
      data_reads++;
      status = hmc5883l_lld_write_register(((ut_hmc5883ldata_t*)ut->data)->driver, HMC5883L_LLD_REGISTER_CONFIG_A+2, &mode, 1, ((ut_hmc5883ldata_t*)ut->data)->timeout);
    }
    aosSysGetUptime(&t);
  }
  chEvtUnregister(((ut_hmc5883ldata_t*)ut->data)->drdyEvtSrc, &el);
  aosUtInfoMsg(stream,"Read data %u times in a second\n", data_reads);

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(HMC5883LDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_HMC5883L) */
