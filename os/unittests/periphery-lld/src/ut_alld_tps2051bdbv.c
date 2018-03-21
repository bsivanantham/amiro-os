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

#include <ut_alld_tps2051bdbv.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TPS2051BDBV)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>

/**
 * @brief   TPS2051BDBV unit test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldTps2051bdbvFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status = AOS_OK;
  tps2051b_lld_enable_t en;
  tps2051b_lld_overcurrent_t oc;

  chprintf(stream, "write laser enable...\n");
  status = tps2051b_lld_set_enable((TPS2051BDriver*)ut->data, TPS2051B_LLD_ENABLE);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read laser enable...\n");
  status = tps2051b_lld_read_enable((TPS2051BDriver*)ut->data, &en);
  if (status == APAL_STATUS_SUCCESS && en == TPS2051B_LLD_ENABLE) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read laser oc...\n");
  status = tps2051b_lld_read_overcurrent((TPS2051BDriver*)ut->data, &oc);
  if (status == APAL_STATUS_SUCCESS && oc == TPS2051B_LLD_NO_OVERCURRENT) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "disable laser...\n");
  status = tps2051b_lld_set_enable((TPS2051BDriver*)ut->data, TPS2051B_LLD_DISABLE);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(TPS2051BDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TPS2051BDBV) */
