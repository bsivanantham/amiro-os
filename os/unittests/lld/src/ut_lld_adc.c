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

#include <ut_lld_adc.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && (HAL_USE_ADC == TRUE))|| defined(__DOXYGEN__)

#include <aos_debug.h>
#include <aos_thread.h>
#include <chprintf.h>

/**
 * @brief   Event mask of the ADC analog watchdog event.
 */
#define _adcWdgEventmask                        EVENT_MASK(31)

/**
 * @brief   ADC analog watchdog threshold (9.0V).
 */
#define _adcWdgThreshold                        (uint16_t)(9.0f / 5.0f / 3.3f * ((1 << 12) - 1) + 0.5f)

/**
 * @brief   Pointer to the thread listening for an ADC analog watchdog event.
 */
static thread_t* _listener;

/**
 * @brief   Helper function to convert ADC sample value to volts.
 *
 * @param[in] adc   ADC sample value.
 *
 * @return          Converted value in volts.
 */
static inline float _adc2volt(adcsample_t adc) {
  return (float)adc / ((1 << 12) - 1) * 3.3f * 5.0f;
}

/**
 * @brief   ADC analog watchdog callback function.
 *
 * @param[in] adcp  ADC driver.
 * @param[in] err   ADC error value.
 */
static void _adcAwdCb(ADCDriver* adcp, adcerror_t err)
{
  (void)adcp;

  if (err == ADC_ERR_WATCHDOG) {
    chSysLockFromISR();
    if (_listener != NULL) {
      chEvtSignalI(_listener, _adcWdgEventmask);
      _listener = NULL;
    }
    chSysUnlockFromISR();
  }

  return;
}

/**
 * @brief   ADC unit test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAdcFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_adcdata_t*)(ut->data))->driver != NULL && ((ut_adcdata_t*)(ut->data))->convgroup != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  adcsample_t buffer[1] = {0};
  bool wdgpassed = false;
  eventmask_t eventmask = 0;
  ADCConversionGroup conversionGroup = *(((ut_adcdata_t*)(ut->data))->convgroup);
  conversionGroup.circular = true;
  conversionGroup.end_cb = NULL;
  conversionGroup.end_cb = NULL;
  conversionGroup.htr = ADC_HTR_HT;
  conversionGroup.ltr = 0;

  chprintf(stream, "reading voltage for five seconds...\n");
  adcStartConversion(((ut_adcdata_t*)(ut->data))->driver, &conversionGroup, buffer, 1);
  for (uint8_t s = 0; s < 5; ++s) {
    aosThdSSleep(1);
    chprintf(stream, "\tVSYS = %fV\n", _adc2volt(buffer[0]));
  }
  adcStopConversion(((ut_adcdata_t*)(ut->data))->driver);
  if (buffer[0] != 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "detecting external power...\n");
  conversionGroup.error_cb = _adcAwdCb;
  conversionGroup.htr = _adcWdgThreshold;
  conversionGroup.ltr = _adcWdgThreshold;
  _listener = chThdGetSelfX();
  adcStartConversion(((ut_adcdata_t*)(ut->data))->driver, &conversionGroup, buffer, 1);
  eventmask = chEvtWaitOneTimeout(_adcWdgEventmask, LL_S2ST(5));
  if (eventmask == _adcWdgEventmask) {
    aosUtPassedMsg(stream, &result, "%fV %c 9V\n", _adc2volt(buffer[0]), (buffer[0] > _adcWdgThreshold) ? '>' : '<');
    wdgpassed = true;
  } else {
    adcStopConversion(((ut_adcdata_t*)(ut->data))->driver);
    aosUtFailed(stream, &result);
    wdgpassed = false;
  }

  if (wdgpassed) {
    for (uint8_t i = 0; i < 2; ++i) {
      if (buffer[0] > _adcWdgThreshold) {
        chprintf(stream, "Remove external power within ten seconds.\n");
        conversionGroup.htr = ADC_HTR_HT;
        conversionGroup.ltr = _adcWdgThreshold;
      } else {
        chprintf(stream, "Connect external power within ten seconds.\n");
        conversionGroup.htr = _adcWdgThreshold;
        conversionGroup.ltr = 0;
      }
      aosThdMSleep(100); // wait some time so the ADC wil trigger again immediately due to noise
      _listener = chThdGetSelfX();
      adcStartConversion(((ut_adcdata_t*)(ut->data))->driver, &conversionGroup, buffer, 1);
      eventmask = chEvtWaitOneTimeout(_adcWdgEventmask, LL_S2ST(10));
      if (eventmask == _adcWdgEventmask) {
        aosUtPassedMsg(stream, &result, "%fV %c 9V\n", _adc2volt(buffer[0]), (buffer[0] > _adcWdgThreshold) ? '>' : '<');
      } else {
        adcStopConversion(((ut_adcdata_t*)(ut->data))->driver);
        aosUtFailed(stream, &result);
        break;
      }
    }
  }

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && (HAL_USE_ADC == TRUE) */
