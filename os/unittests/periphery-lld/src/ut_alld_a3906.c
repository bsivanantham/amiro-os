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

#include <ut_alld_a3906.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_A3906)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <alld_a3906.h>
#include <aos_thread.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief   Interval to poll QEI in certain tests.
 */
#define QEI_POLL_INTERVAL_MS          100

/**
 * @brief   Threshold for QEI differences.
 * @details Differences smaller tahn or equal to this value are neglected (interpreted as zero).
 */
#define QEI_DIFF_THRESHOLD            4

/**
 * @brief   Enumerator to distinguish between left and right wheel.
 */
typedef enum {
  WHEEL_LEFT    = 0,  /**< left wheel identifier */
  WHEEL_RIGHT   = 1,  /**< right wheel identifier */
} wheel_t;

/**
 * @brief   Enumerator to distinguish directions.
 */
typedef enum {
  DIRECTION_FORWARD   = 0,  /**< forward direction identifier */
  DIRECTION_BACKWARD  = 1,  /**< backward direction identifier */
} direction_t;

/**
 * @brief   helper function to test each wheel and direction separately.
 *
 * @param[in] stream      Stream for input/output.
 * @param[in] data        Unit test meta data.
 * @param[in] wheel       Wheel to test.
 * @param[in] direction   Direction to test.
 * @param[in,out] result  Result variable to modify.
 */
void _wheelDirectionTest(BaseSequentialStream* stream, ut_a3906data_t* data, wheel_t wheel, direction_t direction, aos_utresult_t* result)
{
  // local variables
  uint32_t status;
  bool qei_valid;
  apalQEICount_t qei_count[2];
  apalQEIDirection_t qei_direction;
  uint32_t timeout_counter;

  chprintf(stream, "%s wheel %s...\n", (wheel == WHEEL_LEFT) ? "left" : "right", (direction == DIRECTION_FORWARD) ? "forward" : "backward");
  qei_valid = false;
  status = apalQEIGetPosition((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_count[0]);
  // increase PWM incrementally and read QEI data
  for (apalPWMwidth_t pwm_width = APAL_PWM_WIDTH_MIN; pwm_width < APAL_PWM_WIDTH_MAX; ++pwm_width) {
    status |= a3906_lld_set_pwm(data->pwm.driver, (wheel == WHEEL_LEFT) ?
                                  ((direction == DIRECTION_FORWARD) ? data->pwm.channel.left_forward : data->pwm.channel.left_backward) :
                                  ((direction == DIRECTION_FORWARD) ? data->pwm.channel.right_forward : data->pwm.channel.right_backward),
                                pwm_width);
    status |= apalQEIGetPosition((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_count[1]);
    qei_valid = qei_valid || (qei_count[0] != qei_count[1]);
    aosThdUSleep(5 * MICROSECONDS_PER_SECOND / (APAL_PWM_WIDTH_MAX - APAL_PWM_WIDTH_MIN));
    qei_count[0] = qei_count[1];
  }
  status |= qei_valid ? 0x00 : 0x10;
  status |= apalQEIGetDirection((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_direction);
  status |= (qei_direction == ((direction == DIRECTION_FORWARD) ? APAL_QEI_DIRECTION_UP : APAL_QEI_DIRECTION_DOWN)) ? 0x00 : 0x20;

  // let the wheel spin free until it stops
  status |= a3906_lld_set_pwm(data->pwm.driver, (wheel == WHEEL_LEFT) ?
                                ((direction == DIRECTION_FORWARD) ? data->pwm.channel.left_forward : data->pwm.channel.left_backward) :
                                ((direction == DIRECTION_FORWARD) ? data->pwm.channel.right_forward : data->pwm.channel.right_backward),
                              0);
  qei_count[0] = 0;
  qei_count[1] = 0;
  timeout_counter = 0;
  do {
    status |= apalQEIGetPosition((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_count[0]);
    aosThdMSleep(1);
    status |= apalQEIGetPosition((wheel == WHEEL_LEFT) ? data->qei.left : data->qei.right, &qei_count[1]);
    ++timeout_counter;
  } while ((qei_count[0] != qei_count[1]) && (timeout_counter * MICROSECONDS_PER_MILLISECOND <= data->timeout));
  status |= (timeout_counter * MICROSECONDS_PER_MILLISECOND > data->timeout) ? APAL_STATUS_TIMEOUT : 0x00;

  // report result
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, result);
  } else {
    aosUtFailedMsg(stream, result, "0x%08X\n", status);
  }

  return;
}

/**
 * @brief   A3905 unit test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldA3906Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck((ut->data != NULL) &&
              (((ut_a3906data_t*)ut->data)->driver != NULL) &&
              (((ut_a3906data_t*)ut->data)->pwm.driver != NULL) &&
              (((ut_a3906data_t*)ut->data)->qei.left != NULL) &&
              (((ut_a3906data_t*)ut->data)->qei.right != NULL));



  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status = 0;
  a3906_lld_power_t power_state;
  apalQEICount_t qei_count[2][2];
  apalQEICount_t qei_range[2];
  uint32_t timeout_counter;
  apalQEICount_t qei_increments[2][2];
  uint32_t stable_counter;
  apalQEICount_t qei_increments_diff[2];

  chprintf(stream, "enable power...\n");
  power_state = A3906_LLD_POWER_ON;
  status = a3906_lld_set_power(((ut_a3906data_t*)ut->data)->driver, power_state);
  status |= a3906_lld_get_power(((ut_a3906data_t*)ut->data)->driver, &power_state);
  status |= (power_state != A3906_LLD_POWER_ON) ? 0x10 : 0x00;
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  _wheelDirectionTest(stream, (ut_a3906data_t*)ut->data, WHEEL_LEFT, DIRECTION_FORWARD, &result);

  _wheelDirectionTest(stream, (ut_a3906data_t*)ut->data, WHEEL_LEFT, DIRECTION_BACKWARD, &result);

  _wheelDirectionTest(stream, (ut_a3906data_t*)ut->data, WHEEL_RIGHT, DIRECTION_FORWARD, &result);

  _wheelDirectionTest(stream, (ut_a3906data_t*)ut->data, WHEEL_RIGHT, DIRECTION_BACKWARD, &result);

  chprintf(stream, "both wheels full speed forward...\n");
  status = APAL_STATUS_SUCCESS;
  status |= a3906_lld_set_pwm(((ut_a3906data_t*)ut->data)->pwm.driver, ((ut_a3906data_t*)ut->data)->pwm.channel.left_forward, APAL_PWM_WIDTH_MAX);
  status |= a3906_lld_set_pwm(((ut_a3906data_t*)ut->data)->pwm.driver, ((ut_a3906data_t*)ut->data)->pwm.channel.right_forward, APAL_PWM_WIDTH_MAX);
  aosThdMSleep(100);
  qei_count[WHEEL_LEFT][0] = 0;
  qei_count[WHEEL_LEFT][1] = 0;
  qei_count[WHEEL_RIGHT][0] = 0;
  qei_count[WHEEL_RIGHT][1] = 0;
  status |= apalQEIGetRange(((ut_a3906data_t*)ut->data)->qei.left, &qei_range[WHEEL_LEFT]);
  status |= apalQEIGetRange(((ut_a3906data_t*)ut->data)->qei.right, &qei_range[WHEEL_RIGHT]);
  timeout_counter = 0;
  qei_increments[WHEEL_LEFT][0] = 0;
  qei_increments[WHEEL_LEFT][1] = 0;
  qei_increments[WHEEL_RIGHT][0] = 0;
  qei_increments[WHEEL_RIGHT][1] = 0;
  stable_counter = 0;
  do {
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.left, &qei_count[WHEEL_LEFT][0]);
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.right, &qei_count[WHEEL_RIGHT][0]);
    aosThdMSleep(QEI_POLL_INTERVAL_MS);
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.left, &qei_count[WHEEL_LEFT][1]);
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.right, &qei_count[WHEEL_RIGHT][1]);
    timeout_counter += QEI_POLL_INTERVAL_MS;
    qei_increments[WHEEL_LEFT][0] = qei_increments[WHEEL_LEFT][1];
    qei_increments[WHEEL_LEFT][1] = (qei_count[WHEEL_LEFT][1] > qei_count[WHEEL_LEFT][0]) ? (qei_count[WHEEL_LEFT][1] - qei_count[WHEEL_LEFT][0]) : (qei_count[WHEEL_LEFT][1] + (qei_range[WHEEL_LEFT] - qei_count[WHEEL_LEFT][0]));
    qei_increments[WHEEL_RIGHT][0] = qei_increments[WHEEL_RIGHT][1];
    qei_increments[WHEEL_RIGHT][1] = (qei_count[WHEEL_RIGHT][1] > qei_count[WHEEL_RIGHT][0]) ? (qei_count[WHEEL_RIGHT][1] - qei_count[WHEEL_RIGHT][0]) : (qei_count[WHEEL_RIGHT][1] + (qei_range[WHEEL_RIGHT] - qei_count[WHEEL_RIGHT][0]));
    qei_increments_diff[WHEEL_LEFT] = abs((int32_t)(qei_increments[WHEEL_LEFT][0]) - (int32_t)qei_increments[WHEEL_LEFT][1]);
    qei_increments_diff[WHEEL_RIGHT] = abs((int32_t)(qei_increments[WHEEL_RIGHT][0]) - (int32_t)qei_increments[WHEEL_RIGHT][1]);
    stable_counter = (qei_increments_diff[WHEEL_LEFT] <= QEI_DIFF_THRESHOLD && qei_increments_diff[WHEEL_RIGHT] < QEI_DIFF_THRESHOLD) ? stable_counter+1 : 0;
  } while((stable_counter * QEI_POLL_INTERVAL_MS < MILLISECONDS_PER_SECOND) && (timeout_counter * MICROSECONDS_PER_MILLISECOND <= ((ut_a3906data_t*)ut->data)->timeout));
  status |= (timeout_counter * MICROSECONDS_PER_MILLISECOND > ((ut_a3906data_t*)ut->data)->timeout) ? APAL_STATUS_TIMEOUT : 0x00;
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
    const float tps[2] = {qei_increments[WHEEL_LEFT][1] * ((float)MILLISECONDS_PER_SECOND / (float)QEI_POLL_INTERVAL_MS),
                          qei_increments[WHEEL_RIGHT][1] * ((float)MILLISECONDS_PER_SECOND / (float)QEI_POLL_INTERVAL_MS)};
    const float rpm[2] = {tps[WHEEL_LEFT] * SECONDS_PER_MINUTE / (float)(((ut_a3906data_t*)ut->data)->qei.increments_per_revolution),
                          tps[WHEEL_RIGHT] * SECONDS_PER_MINUTE / (float)(((ut_a3906data_t*)ut->data)->qei.increments_per_revolution)};
    const float velocity[2] = {tps[WHEEL_LEFT] / (float)(((ut_a3906data_t*)ut->data)->qei.increments_per_revolution) * ((ut_a3906data_t*)ut->data)->wheel_diameter * acos(-1),
                               tps[WHEEL_RIGHT] / (float)(((ut_a3906data_t*)ut->data)->qei.increments_per_revolution) * ((ut_a3906data_t*)ut->data)->wheel_diameter * acos(-1)};
    chprintf(stream, "left wheel:\n");
    chprintf(stream, "\t%f tps\n", tps[WHEEL_LEFT]);
    chprintf(stream, "\t%f RPM\n", rpm[WHEEL_LEFT]);
    chprintf(stream, "\t%f m/s\n", velocity[WHEEL_LEFT]);
    chprintf(stream, "right wheel:\n");
    chprintf(stream, "\t%f tps\n", tps[WHEEL_RIGHT]);
    chprintf(stream, "\t%f RPM\n", rpm[WHEEL_RIGHT]);
    chprintf(stream, "\t%f m/s\n", velocity[WHEEL_RIGHT]);
    chprintf(stream, "\n");
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "disable power... \n");
  power_state = A3906_LLD_POWER_OFF;
  status = a3906_lld_set_power(((ut_a3906data_t*)ut->data)->driver, power_state);
  status |= a3906_lld_get_power(((ut_a3906data_t*)ut->data)->driver, &power_state);
  status |= (power_state != A3906_LLD_POWER_OFF) ? 0x10 : 0x00;
  qei_count[WHEEL_LEFT][0] = 0;
  qei_count[WHEEL_LEFT][1] = 0;
  qei_count[WHEEL_RIGHT][0] = 0;
  qei_count[WHEEL_RIGHT][1] = 0;
  timeout_counter = 0;
  stable_counter = 0;
  do {
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.left, &qei_count[WHEEL_LEFT][0]);
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.right, &qei_count[WHEEL_RIGHT][0]);
    aosThdMSleep(1);
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.left, &qei_count[WHEEL_LEFT][1]);
    status |= apalQEIGetPosition(((ut_a3906data_t*)ut->data)->qei.right, &qei_count[WHEEL_RIGHT][1]);
    ++timeout_counter;
    stable_counter = (qei_count[WHEEL_LEFT][0] == qei_count[WHEEL_LEFT][1] && qei_count[WHEEL_RIGHT][0] == qei_count[WHEEL_RIGHT][1]) ? stable_counter+1 : 0;
  } while((stable_counter < 100) && (timeout_counter * MICROSECONDS_PER_MILLISECOND <= ((ut_a3906data_t*)ut->data)->timeout));
  status |= (timeout_counter * MICROSECONDS_PER_MILLISECOND > ((ut_a3906data_t*)ut->data)->timeout) ? APAL_STATUS_TIMEOUT : 0x00;
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  // stop the PWM
  a3906_lld_set_pwm(((ut_a3906data_t*)ut->data)->pwm.driver, ((ut_a3906data_t*)ut->data)->pwm.channel.left_forward, APAL_PWM_WIDTH_OFF);
  a3906_lld_set_pwm(((ut_a3906data_t*)ut->data)->pwm.driver, ((ut_a3906data_t*)ut->data)->pwm.channel.left_backward, APAL_PWM_WIDTH_OFF);
  a3906_lld_set_pwm(((ut_a3906data_t*)ut->data)->pwm.driver, ((ut_a3906data_t*)ut->data)->pwm.channel.right_forward, APAL_PWM_WIDTH_OFF);
  a3906_lld_set_pwm(((ut_a3906data_t*)ut->data)->pwm.driver, ((ut_a3906data_t*)ut->data)->pwm.channel.right_backward, APAL_PWM_WIDTH_OFF);

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(A3906Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_A3906) */
