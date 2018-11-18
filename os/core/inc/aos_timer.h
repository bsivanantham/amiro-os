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

#ifndef _AMIROOS_TIMER_H_
#define _AMIROOS_TIMER_H_

#include <ch.h>
#include <aos_time.h>

/**
 * @brief   Maximum timer interval that can be set in system ticks.
 */
#define AOS_TIMER_MAX_INTERVAL_ST     TIME_MAX_INTERVAL

/**
 * @brief   Maximum timer interval that can be set in seconds.
 */
#define AOS_TIMER_MAX_INTERVAL_S      (TIME_I2S(AOS_TIMER_MAX_INTERVAL_ST) - 1)

/**
 * @brief   Maximum timer interval that can be set in milliseconds.
 */
#define AOS_TIMER_MAX_INTERVAL_MS     (TIME_I2MS(AOS_TIMER_MAX_INTERVAL_ST) - 1)

/**
 * @brief   Maximum timer interval that can be set in microseconds.
 */
#define AOS_TIMER_MAX_INTERVAL_US     (TIME_I2US(AOS_TIMER_MAX_INTERVAL_ST) - 1)

/**
 * @brief   Timer stucture.
 */
typedef struct aos_timer {
  /**
   * @brief   ChibiOS virtual timer.
   */
  virtual_timer_t vt;

  /**
   * @brief   Time to wake up.
   */
  aos_timestamp_t wkuptime;

  /**
   * @brief   Pointer to a callback function.
   */
  vtfunc_t callback;

  /**
   * @brief   Pointer to a parameter for the callback function.
   */
  void* cbparam;
} aos_timer_t;

/**
 * @brief   Periodic timer structure.
 */
typedef struct aos_periodictimer {
  /**
   * @brief   AMiRo-OS timer.
   */
  aos_timer_t timer;

  /**
   * @brief   Period interval.
   */
  aos_longinterval_t interval;

  /**
   * @brief   Pointer to a callback function.
   */
  vtfunc_t callback;

  /**
   * @brief   Pointer to a parameter for the callback function.
   */
  void* cbparam;
} aos_periodictimer_t;

#ifdef __cplusplus
extern "C" {
#endif
  void aosTimerInit(aos_timer_t* timer);
  void aosTimerSetAbsoluteI(aos_timer_t* timer, aos_timestamp_t* uptime, vtfunc_t cb, void* par);
  void aosTimerSetIntervalI(aos_timer_t* timer, aos_interval_t offset, vtfunc_t cb, void* par);
  void aosTimerSetLongIntervalI(aos_timer_t* timer, aos_longinterval_t* offset, vtfunc_t cb, void* par);
  void aosPeriodicTimerInit(aos_periodictimer_t* ptimer);
  void aosPeriodicTimerSetI(aos_periodictimer_t* ptimer, aos_interval_t interval, vtfunc_t cb, void* par);
  void aosPeriodicTimerSetLongI(aos_periodictimer_t* ptimer, aos_longinterval_t* interval, vtfunc_t cb, void* par);
#ifdef __cplusplus
}
#endif

/**
 * @brief   Set timer to fire at an absolute system time.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] uptime  Pointer to the absolute uptime for the timer to fire.
 * @param[in] cb      Pointer to a callback function to be called.
 * @param[in] par     Pointer to a parameter fpr the callback function (may be NULL).
 */
static inline void aosTimerSetAbsolute(aos_timer_t* timer, aos_timestamp_t* uptime, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerSetAbsoluteI(timer, uptime, cb, par);
  chSysUnlock();

  return;
}

/**
 * @brief   Set timer to fire after a relative interval.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] offset  Relative interval to set for the timer to fire.
 * @param[in] cb      Pointer to a callback function to be called.
 * @param[in] par     Pointer to a parameter fpr the callback function (may be NULL).
 */
static inline void aosTimerSetInterval(aos_timer_t* timer, aos_interval_t offset, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerSetIntervalI(timer, offset, cb, par);
  chSysUnlock();
}

/**
 * @brief   Set timer to fire after a long relative interval.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] offset  Pointer to a long interval value to set for the timer to fire.
 * @param[in] cb      Pointer to a callback function to be called.
 * @param[in] par     Pointer to a parameter fpr the callback function (may be NULL).
 */
static inline void aosTimerSetLongInterval(aos_timer_t* timer, aos_longinterval_t* offset, vtfunc_t cb, void* par)
{
  chSysLock();
  aosTimerSetLongIntervalI(timer, offset, cb, par);
  chSysUnlock();
}

/**
 * @brief   Reset a timer.
 *
 * @param[in] timer   Pointer to the timer to reset.
 */
static inline void aosTimerResetI(aos_timer_t* timer)
{
  chVTResetI(&(timer->vt));

  return;
}

/**
 * @brief   Reset a timer.
 *
 * @param[in] timer   Pointer to the timer to reset.
 */
static inline void aosTimerReset(aos_timer_t* timer)
{
  chSysLock();
  aosTimerResetI(timer);
  chSysUnlock();

  return;
}

/**
 * @brief   Determine whether a timer is armed.
 *
 * @param[in] timer   Pointer to the timer to check.
 *
 * @return    true if the timer is armed, false otherwise.
 */
static inline bool aosTimerIsArmedI(aos_timer_t* timer)
{
  return chVTIsArmedI(&(timer->vt));
}

/**
 * @brief   Determine whether a timer is armed.
 *
 * @param[in] timer   Pointer to the timer to check.
 *
 * @return    true if the timer is armed, false otherwise.
 */
static inline bool aosTimerIsArmed(aos_timer_t* timer)
{
  bool b;

  chSysLock();
  b = aosTimerIsArmedI(timer);
  chSysUnlock();

  return b;
}

/**
 * @brief   Set a periodic timer to fire in the specified interval.
 *
 * @param[in] ptimer    Pointer to the periodic timer to set.
 * @param[in] interval  Interval for the periodic timer to fire,
 * @param[in] cb        Pointer to a callback function to be called.
 *                      In contrast to other callback functions, this get called from an already ISR locked context.
 *                      This it may not contain any chSysLockX() or chSysUnlockX() calls.
 * @param[in] par       Pointer to a parameter for the callback function (may be NULL).
 */
static inline void aosPeriodicTimerSet(aos_periodictimer_t *ptimer, aos_interval_t interval, vtfunc_t cb, void *par)
{
  chSysLock();
  aosPeriodicTimerSetI(ptimer, interval, cb, par);
  chSysUnlock();

  return;
}

/**
 * @brief   Set a periodic timer to fire in the specified interval.
 *
 * @param[in] ptimer    Pointer to the periodic timer to set.
 * @param[in] interval  Pointer to a long interval value for the periodic timer to fire,
 * @param[in] cb        Pointer to a callback function to be called.
 *                      In contrast to other callback functions, this get called from an already ISR locked context.
 *                      This it may not contain any chSysLockX() or chSysUnlockX() calls.
 * @param[in] par       Pointer to a parameter for the callback function (may be NULL).
 */
static inline void aosPeriodicTimerSetLong(aos_periodictimer_t* ptimer, aos_longinterval_t* interval, vtfunc_t cb, void* par)
{
  chSysLock();
  aosPeriodicTimerSetLongI(ptimer, interval, cb, par);
  chSysUnlock();

  return;
}

#endif /* _AMIROOS_TIMER_H_ */
