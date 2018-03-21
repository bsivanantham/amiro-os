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

#ifndef _AMIROOS_THREAD_H_
#define _AMIROOS_THREAD_H_

#include <aos_time.h>
#include <aos_system.h>

/**
 * @brief   Minimum thread priority.
 */
#define THD_LOWPRIO_MIN     ((tprio_t)  (LOWPRIO))

/**
 * @brief   Maximum priority for background threads.
 */
#define THD_LOWPRIO_MAX     ((tprio_t)  (LOWPRIO + ((NORMALPRIO - LOWPRIO) / 2)))

/**
 * @brief   Minimum priority for normal/standard threads.
 */
#define THD_NORMALPRIO_MIN  ((tprio_t)  (THD_LOWPRIO_MAX + 1))

/**
 * @brief   Maximum priority for normal/standard threads.
 */
#define THD_NORMALPRIO_MAX  ((tprio_t)  (NORMALPRIO))

/**
 * @brief   Minimum priority for important threads.
 */
#define THD_HIGHPRIO_MIN    ((tprio_t)  (NORMALPRIO + 1))

/**
 * @brief   Maximum priority for important threads.
 */
#define THD_HIGHPRIO_MAX    ((tprio_t)  (NORMALPRIO + ((HIGHPRIO - NORMALPRIO) / 2)))

/**
 * @brief   Minimum priority for real-time threads.
 */
#define THD_RTPRIO_MIN      ((tprio_t)  (THD_HIGHPRIO_MAX + 1))

/**
 * @brief   Maximum priority for real-time threads.
 */
#define THD_RTPRIO_MAX      ((tprio_t)  (HIGHPRIO - 1))

/**
 * @brief   Priority for the system control thread.
 */
#define THD_CTRLPRIO     ((tprio_t)  (HIGHPRIO))

/**
 * @brief   Maximum timeframe that can be slept in system ticks.
 */
#define THD_MAX_SLEEP_ST    TIME_MAXIMUM

/**
 * @brief   Maximum timeframe that can be slept in seconds.
 */
#define THD_MAX_SLEEP_S     (ST2S(THD_MAX_SLEEP_ST) - 1)

/**
 * @brief   Maximum timeframe that can be slept in milliseconds.
 */
#define THD_MAX_SLEEP_MS    (ST2MS(THD_MAX_SLEEP_ST) - 1)

/**
 * @brief   Maximum timeframe that can be slept in microseconds.
 */
#define THD_MAX_SLEEP_US    (ST2US(THD_MAX_SLEEP_ST) - 1)

#ifdef __cplusplus
extern "C" {
#endif
  void aosThdSleepUntilS(const aos_timestamp_t* t);
#ifdef __cplusplus
}
#endif

/**
 * @brief   Lets the calling thread sleep the specified amount of microseconds.
 *
 * @param[in] us    Time to sleep in microseconds.
 */
static inline void aosThdUSleepS(const aos_interval_t us)
{
  aos_timestamp_t ut;

  aosSysGetUptimeX(&ut);
  ut += us;
  aosThdSleepUntilS(&ut);

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of milliseconds.
 *
 * @param[in] ms    Time to sleep in milliseconds.
 */
static inline void aosThdMSleepS(const uint32_t ms)
{
  aos_timestamp_t ut;

  aosSysGetUptimeX(&ut);
  ut += (aos_timestamp_t)ms * MICROSECONDS_PER_MILLISECOND;
  aosThdSleepUntilS(&ut);

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of seconds.
 *
 * @param[in] s     Time to sleep in seconds.
 */
static inline void aosThdSSleepS(const uint32_t s)
{
  aos_timestamp_t ut;

  aosSysGetUptimeX(&ut);
  ut += (aos_timestamp_t)s * MICROSECONDS_PER_SECOND;
  aosThdSleepUntilS(&ut);

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of seconds.
 *
 * @param[in] s     Time to sleep in seconds.
 */
static inline void aosThdSleepS(const float s)
{
  aos_timestamp_t ut;

  aosSysGetUptimeX(&ut);
  ut += (aos_timestamp_t)(s * MICROSECONDS_PER_SECOND);
  aosThdSleepUntilS(&ut);

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of microseconds.
 *
 * @param[in] us    Time to sleep in microseconds.
 */
static inline void aosThdUSleep(const uint32_t us)
{
  chSysLock();
  aosThdUSleepS(us);
  chSysUnlock();

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of milliseconds.
 *
 * @param[in] ms    Time to sleep in milliseconds.
 */
static inline void aosThdMSleep(const uint32_t ms)
{
  chSysLock();
  aosThdMSleepS(ms);
  chSysUnlock();

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of seconds.
 *
 * @param[in] s     Time to sleep in seconds.
 */
static inline void aosThdSSleep(const uint32_t s)
{
  chSysLock();
  aosThdSSleepS(s);
  chSysUnlock();

  return;
}

/**
 * @brief   Lets the calling thread sleep the specified amount of seconds.
 *
 * @param[in] s     Time to sleep in seconds.
 */
static inline void aosThdSleep(const float s)
{
  chSysLock();
  aosThdSleepS(s);
  chSysUnlock();

  return;
}

/**
 * @brief   Lets the calling thread sleep until the specifide system uptime.
 *
 * @param[in] t     Deadline until the thread will sleep.
 */
static inline void aosThdSleepUntil(const aos_timestamp_t* t)
{
  chSysLock();
  aosThdSleepUntilS(t);
  chSysUnlock();

  return;
}

#endif /* _AMIROOS_THREAD_H_ */
