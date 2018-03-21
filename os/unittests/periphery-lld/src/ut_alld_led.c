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

#include <ut_alld_led.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_LED)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <alld_led.h>
#include <aos_thread.h>

/**
 * @brief   LED unit test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldLedFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status = AOS_OK;

  chprintf(stream, "lighting up for two seconds...\n");
  led_lld_state_t state = LED_LLD_STATE_ON;
  status = led_lld_set((LEDDriver*)ut->data, state);
  aosThdSSleep(2);
  status |= led_lld_get((LEDDriver*)ut->data, &state);
  if (status == APAL_STATUS_OK && state == LED_LLD_STATE_ON) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "turning off for two seconds...\n");
  state = LED_LLD_STATE_OFF;
  status = led_lld_set((LEDDriver*)ut->data, state);
  aosThdSSleep(2);
  status |= led_lld_get((LEDDriver*)ut->data, &state);
  if (status == APAL_STATUS_OK && state == LED_LLD_STATE_OFF) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "toggling for two seconds...\n");
  status = led_lld_set((LEDDriver*)ut->data, LED_LLD_STATE_ON);
  for (uint32_t i = 0; i < 2000/100; ++i) {
    status |= led_lld_toggle((LEDDriver*)ut->data);
    status |= led_lld_get((LEDDriver*)ut->data, &state);
    status |= ((uint8_t)state != (i & 0x01u)) ? APAL_STATUS_ERROR : APAL_STATUS_OK;
    aosThdMSleep(100);
  }
  status = led_lld_set((LEDDriver*)ut->data, LED_LLD_STATE_OFF);
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(LEDDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_LED) */
