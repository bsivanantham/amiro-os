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

#include <ut_alld_bq27500_bq24103a.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ27500) && defined(AMIROLLD_CFG_USE_BQ24103A)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>

aos_utresult_t utAlldBq27500Bq24103aFunc(BaseSequentialStream *stream, aos_unittest_t *ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_bq27500bq24103adata_t*)ut->data)->bq27500 != NULL && ((ut_bq27500bq24103adata_t*)ut->data)->bq24103a != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  uint16_t dst;
  bq27500_lld_flags_t flags;
  bq27500_lld_batgood_t bg;
  bq24103a_lld_enable_t charger_enabled;
  uint32_t sleeptime_s;
  bq24103a_lld_charge_state_t charge;

  chprintf(stream, "check for battery...\n");
  status = bq27500_lld_std_command(((ut_bq27500bq24103adata_t*)ut->data)->bq27500, BQ27500_LLD_STD_CMD_Flags, &dst, ((ut_bq27500bq24103adata_t*)ut->data)->timeout);
  flags.value = dst;
  chprintf(stream, "\t\tbattery detected: %s\n", flags.content.bat_det ? "yes" : "no");
  chprintf(stream, "\t\tbattery fully charged: %s\n", flags.content.fc ? "yes" : "no");
  status |= bq27500_lld_read_batgood(((ut_bq27500bq24103adata_t*)ut->data)->bq27500, &bg);
  chprintf(stream, "\t\tbattery good: %s\n", (bg == BQ27500_LLD_BATTERY_GOOD) ? "yes" : "no");
  if (status == APAL_STATUS_SUCCESS) {
    if (!flags.content.bat_det) {
      aosUtPassedMsg(stream, &result, "no battery detected, aborting\n");
      return result;
    } else if (!bg) {
      aosUtPassedMsg(stream, &result, "battery damaged, aborting\n");
      return result;
    } else {
      aosUtPassed(stream, &result);
    }
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X; aborting\n", status);
    return result;
  }

  chprintf(stream, "get current charger setting...\n");
  status = bq24103a_lld_get_enabled(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, &charger_enabled);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassedMsg(stream, &result, "currently %s\n", (charger_enabled == BQ24103A_LLD_ENABLED) ? "enabled" : "disabled");
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  // disable/enable charger
  for (uint8_t iteration = 0; iteration < 2; ++iteration) {
    if (charger_enabled == BQ24103A_LLD_ENABLED) {
      sleeptime_s = 120;
      chprintf(stream, "disable charger...\n");
      status = bq24103a_lld_set_enabled(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, BQ24103A_LLD_DISABLED);
      aosThdSSleep(1);
      status |= bq24103a_lld_get_enabled(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, &charger_enabled);
      status |= (charger_enabled == BQ24103A_LLD_DISABLED) ? 0x0000 : 0x0100;
      for (uint32_t s = 0; s < sleeptime_s; ++s) {
        int nchars = chprintf(stream, "%us / %us", s, sleeptime_s);
        aosThdSSleep(1);
        for (int c = 0; c < nchars; ++c) {
          chprintf(stream, "\b \b");
        }
      }
      status |= bq24103a_lld_get_charge_status(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, &charge);
      chprintf(stream, "\t\tcharge status: %scharging\n", (charge == BQ24103A_LLD_CHARGING) ? "" : "not ");
      status |= bq27500_lld_std_command(((ut_bq27500bq24103adata_t*)ut->data)->bq27500, BQ27500_LLD_STD_CMD_TimeToFull, &dst, ((ut_bq27500bq24103adata_t*)ut->data)->timeout);
      if (status == APAL_STATUS_SUCCESS) {
        if (dst == 0xFFFF || dst == 0) {
          aosUtPassedMsg(stream, &result, "battery %sfull\n", flags.content.fc ? "" : "not ");
        } else {
          aosUtFailedMsg(stream, &result, "battery %sfull but charging (TTF = %umin)\n", flags.content.fc ? "" : "not ", dst);
        }
      } else {
        aosUtFailedMsg(stream, &result, "0x%08X\n", status);
      }
    }
    else { /* charger_enabled != BQ24103A_LLD_ENABLED */
      sleeptime_s = 90;
      chprintf(stream, "enable charger...\n");
      status = bq24103a_lld_set_enabled(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, BQ24103A_LLD_ENABLED);
      aosThdSSleep(1);
      status |= bq24103a_lld_get_enabled(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, &charger_enabled);
      status |= (charger_enabled == BQ24103A_LLD_ENABLED) ? 0x0000 : 0x0100;
      for (uint32_t s = 0; s < sleeptime_s; ++s) {
        int nchars = chprintf(stream, "%us / %us", s, sleeptime_s);
        aosThdSSleep(1);
        for (int c = 0; c < nchars; ++c) {
          chprintf(stream, "\b \b");
        }
      }
      status |= bq24103a_lld_get_charge_status(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, &charge);
      chprintf(stream, "\t\tcharge status: %scharging\n", (charge == BQ24103A_LLD_CHARGING) ? "" : "not ");
      status |= bq27500_lld_std_command(((ut_bq27500bq24103adata_t*)ut->data)->bq27500, BQ27500_LLD_STD_CMD_TimeToFull, &dst, ((ut_bq27500bq24103adata_t*)ut->data)->timeout);
      if (status == APAL_STATUS_SUCCESS) {
        if (((dst == 0xFFFF || dst == 0) && flags.content.fc) || (!(dst == 0xFFFF || dst == 0) && !flags.content.fc)) {
          aosUtPassedMsg(stream, &result, "battery %sfull and %scharging (TTF = %umin)\n", flags.content.fc ? "" : "not ", (dst != 0xFFFF || dst != 0) ? "" : "not ", dst);
        } else {
          aosUtFailedMsg(stream, &result, "battery %sfull and %scharging (TTF = %umin)\n", flags.content.fc ? "" : "not ", (dst != 0xFFFF || dst != 0) ? "" : "not ", dst);
        }
      } else {
        aosUtFailedMsg(stream, &result, "0x%08X\n", status);
      }
    }

    bq24103a_lld_get_enabled(((ut_bq27500bq24103adata_t*)ut->data)->bq24103a, &charger_enabled);
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ27500) && defined(AMIROLLD_CFG_USE_BQ24103A) */
