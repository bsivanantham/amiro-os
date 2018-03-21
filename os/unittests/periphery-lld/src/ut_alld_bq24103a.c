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

#include <ut_alld_bq24103a.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ24103A)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_bq24103a.h>

aos_utresult_t utAlldBq24103aFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  bq24103a_lld_enable_t en[3];
  bq24103a_lld_charge_state_t charge;

  chprintf(stream, "read enable pin...\n");
  status = bq24103a_lld_get_enabled((BQ24103ADriver*)ut->data, &en[0]);
  chprintf(stream, "\t\tcurrently %s\n", (en[0] == BQ24103A_LLD_ENABLED) ? "enabled" : "disabled");
  if(status == APAL_STATUS_SUCCESS){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "write enable pin...\n");
  status = bq24103a_lld_set_enabled((BQ24103ADriver*)ut->data, (en[0] == BQ24103A_LLD_ENABLED) ? BQ24103A_LLD_DISABLED : BQ24103A_LLD_ENABLED);
  aosThdMSleep(1);
  status |= bq24103a_lld_get_enabled((BQ24103ADriver*)ut->data, &en[1]);
  status |= bq24103a_lld_set_enabled((BQ24103ADriver*)ut->data, en[0]);
  aosThdMSleep(1);
  status |= bq24103a_lld_get_enabled((BQ24103ADriver*)ut->data, &en[2]);
  if(status == APAL_STATUS_SUCCESS && en[0] != en[1] && en[0] == en[2] && en[1] != en[2]){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X, %u-%u\n", status, en[1], en[2]);
  }

  chprintf(stream, "read status pin...\n");
  aosThdMSleep(500);
  status = bq24103a_lld_get_charge_status((BQ24103ADriver*)ut->data, &charge);
  chprintf(stream, "\t\tcharge status: %scharging\n", (charge == BQ24103A_LLD_CHARGING) ? "" : "not ");
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(BQ24103ADriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ24103A) */
