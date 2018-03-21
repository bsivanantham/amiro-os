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

#include <aos_timer.h>

#include <aos_system.h>

/*
 * Forward declarations.
 */
static inline void _setupTimer(aos_timer_t* timer);
static inline void _setupPeriodicTimer(aos_periodictimer_t* ptimer);
static void _intermediateCb(void* timer);
static void _fireCb(void* timer);
static void _periodicCb(void* ptimer);

/**
 * @brief   Setup a timer according to its configuration.
 *
 * @param[in] timer   Pointer to the timer to setup.
 */
static inline void _setupTimer(aos_timer_t* timer)
{
  aos_timestamp_t uptime;

  // get current system uptime
  aosSysGetUptimeX(&uptime);

  // if the wakeup time is more than TIME_IMMEDIATE in the future
  if ( (timer->wkuptime > uptime) && ((timer->wkuptime - uptime) > TIME_IMMEDIATE) ) {
    // split the time delta if necessary
    if ((timer->wkuptime - uptime) > TIMER_MAX_INTERVAL_US) {
      chVTSetI(&(timer->vt), US2ST(TIMER_MAX_INTERVAL_US), _intermediateCb, timer);
    } else {
      chVTSetI(&(timer->vt), US2ST(timer->wkuptime - uptime), _fireCb, timer);
    }
  } else {
    vtfunc_t fn = timer->callback;
    timer->callback = NULL;
    fn(timer->cbparam);
  }

  return;
}

/**
 * @brief   Setup a periodic timer according to its configuration.
 *
 * @param[in] ptimer  Pointer to the periodic timer to setup.
 */
static inline void _setupPeriodicTimer(aos_periodictimer_t *ptimer)
{
  // if the periodic timer is being initialized
  if (ptimer->timer.wkuptime == 0) {
    aos_timestamp_t uptime;

    // get current system uptime
    aosSysGetUptimeX(&uptime);
    ptimer->timer.wkuptime = uptime + ptimer->interval;
  }
  // if the peridic timer is reactivated after it has fired
  else {
    ptimer->timer.wkuptime += ptimer->interval;
  }

  // set the callback and parameters
  ptimer->timer.callback = _periodicCb;
  ptimer->timer.cbparam = ptimer;

  // setup the timer
  _setupTimer(&(ptimer->timer));

  return;
}

/**
 * @brief   Callback function for intermediate interrupts.
 * @details This is required if the desired time to fire is too far in the future so that the interval must be split.
 *
 * @param[in] timer   Pointer to a aos_timer_t to reactivate.
 */
static void _intermediateCb(void* timer)
{
  chSysLockFromISR();
  _setupTimer((aos_timer_t*)timer);
  chSysUnlockFromISR();
}

/**
 * @brief   Callback function for the final fire event of the timer.
 *
 * @param[in] timer   Pointer to a aos_timer_t to call its callback.
 */
static void _fireCb(void *timer)
{
  chSysLockFromISR();
  ((aos_timer_t*)timer)->callback(((aos_timer_t*)timer)->cbparam);
  chSysUnlockFromISR();
}

/**
 * @brief   Callback function for periodic timer interrupts.
 * @details This function calls the original callback and reenables the timer afterwards.
 *
 * @param[in] ptimer  Pointer to a aos_periodictimer_t to reactivate.
 */
static void _periodicCb(void* ptimer)
{
  ((aos_periodictimer_t*)ptimer)->callback(((aos_periodictimer_t*)ptimer)->cbparam);
  _setupPeriodicTimer((aos_periodictimer_t*)ptimer);
}

/**
 * @brief   Initialize a aos_timer_t object.
 *
 * @param[in] timer   The timer to initialize.
 */
void aosTimerInit(aos_timer_t* timer)
{
  aosDbgAssert(timer != NULL);

  chVTObjectInit(&(timer->vt));
  timer->wkuptime = 0;
  timer->callback = NULL;
  timer->cbparam = NULL;

  return;
}

/**
 * @brief   Set timer to fire at an absolute system time.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] uptime  Pointer to the absolute system time for the timer to fire.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    In contrast to ChibiOS callback functions, this gets called from an already ISR locked context.
 *                    Thus it may not contain any chSysLockX() or chSysUnlockX() calls and so forth.
 * @param[in] par     Pointer to a parameter fpr the callback function (may be NULL).
 */
void aosTimerSetAbsoluteI(aos_timer_t *timer, aos_timestamp_t *uptime, vtfunc_t cb, void *par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(uptime != NULL);
  aosDbgCheck(cb != NULL);

  timer->wkuptime = *uptime;
  timer->callback = cb;
  timer->cbparam = par;
  _setupTimer(timer);

  return;
}

/**
 * @brief   Set timer to fire after a relative interval.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] offset  Relative interval to set for the timer to fire.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    In contrast to ChibiOS callback functions, this gets called from an already ISR locked context.
 *                    Thus it may not contain any chSysLockX() or chSysUnlockX() calls and so forth.
 * @param[in] par     Pointer to a parameter for the callback function (may be NULL).
 */
void aosTimerSetIntervalI(aos_timer_t *timer, aos_interval_t offset, vtfunc_t cb, void *par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(cb != NULL);

  aos_timestamp_t uptime;

  aosSysGetUptimeX(&uptime);
  timer->wkuptime = uptime + offset;
  timer->callback = cb;
  timer->cbparam = par;
  _setupTimer(timer);

  return;
}

/**
 * @brief   Set timer to fire after a long relative interval.
 *
 * @param[in] timer   Pointer to the timer to set.
 * @param[in] offset  Pointer to a long interval value to set for the timer to fire.
 * @param[in] cb      Pointer to a callback function to be called.
 *                    In contrast to ChibiOS callback functions, this gets called from an already ISR locked context.
 *                    Thus it may not contain any chSysLockX() or chSysUnlockX() calls and so forth.
 * @param[in] par     Pointer to a parameter for the callback function (may be NULL).
 */
void aosTimerSetLongIntervalI(aos_timer_t *timer, aos_longinterval_t *offset, vtfunc_t cb, void *par)
{
  aosDbgCheck(timer != NULL);
  aosDbgCheck(offset != NULL);
  aosDbgCheck(cb != NULL);

  aos_timestamp_t uptime;

  aosSysGetUptimeX(&uptime);
  timer->wkuptime = uptime + *offset;
  timer->callback = cb;
  timer->cbparam = par;
  _setupTimer(timer);

  return;
}

/**
 * @brief   Initialize a aos_periodictimer_t object.
 *
 * @param[in] timer   The periodic timer to initialize.
 */
void aosPeriodicTimerInit(aos_periodictimer_t *ptimer)
{
  aosDbgAssert(ptimer != NULL);

  aosTimerInit(&(ptimer->timer));
  ptimer->interval = 0;

  return;
}

/**
 * @brief   Set a periodic timer to fire in the specified interval.
 *
 * @param[in] ptimer    Pointer to the periodic timer to set.
 * @param[in] interval  Interval for the periodic timer to fire,
 * @param[in] cb        Pointer to a callback function to be called.
 *                      In contrast to ChibiOS callback functions, this gets called from an already ISR locked context.
 *                      Thus it may not contain any chSysLockX() or chSysUnlockX() calls and so forth.
 * @param[in] par       Pointer to a parameter for the callback function (may be NULL).
 */
void aosPeriodicTimerSetI(aos_periodictimer_t* ptimer, aos_interval_t interval, vtfunc_t cb, void* par)
{
  aosDbgCheck(ptimer != NULL);
  aosDbgCheck(interval > TIME_IMMEDIATE);
  aosDbgCheck(cb != NULL);

  ptimer->timer.wkuptime = 0;
  ptimer->interval = interval;
  ptimer->callback = cb;
  ptimer->cbparam = par;
  _setupPeriodicTimer(ptimer);

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
void aosPeriodicTimerSetLongI(aos_periodictimer_t* ptimer, aos_longinterval_t* interval, vtfunc_t cb, void* par)
{
  aosDbgCheck(ptimer != NULL);
  aosDbgCheck(*interval > TIME_IMMEDIATE);
  aosDbgCheck(cb != NULL);

  ptimer->timer.wkuptime = 0;
  ptimer->interval = *interval;
  ptimer->callback = cb;
  ptimer->cbparam = par;
  _setupPeriodicTimer(ptimer);

  return;
}
