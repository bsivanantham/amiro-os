/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2018  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY) without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/.
*/

#ifndef _AMIROOS_TIME_H_
#define _AMIROOS_TIME_H_

#include <stdint.h>

/**
 * @brief   Generic time to represent long time frames at high precision.
 * @note    By definition the temporal resolution is 1us.
 */
typedef uint64_t aos_timestamp_t;

/**
 * @brief   generic time to represent medium length time frames at high precision.
 * @note    By definition the temporal resolution is 1us.
 */
typedef uint32_t aos_interval_t;

/**
 * @brief   Generic time to represent long time frames at high precision.
 * @note    By definition the temporal resolution is 1us.
 */
typedef aos_timestamp_t aos_longinterval_t;

#define MICROSECONDS_PER_MICROSECOND  ((uint8_t)  (1))
#define MILLISECONDS_PER_MILLISECOND  ((uint8_t)  (1))
#define SECONDS_PER_SECOND            ((uint8_t)  (1))
#define MINUTES_PER_MINUTE            ((uint8_t)  (1))
#define HOURS_PER_HOUR                ((uint8_t)  (1))
#define DAYS_PER_DAY                  ((uint8_t)  (1))
#define WEEKS_PER_WEEK                ((uint8_t)  (1))
#define MONTHS_PER_MONTH              ((uint8_t)  (1))
#define YEARS_PER_YEAR                ((uint8_t)  (1))
#define DECADES_PER_DECADE            ((uint8_t)  (1))
#define CENTURIES_PER_CENTURY         ((uint8_t)  (1))
#define MILLENIUMS_PER_MILLENIUM      ((uint8_t)  (1))

#define MICROSECONDS_PER_MILLISECOND  ((uint16_t) (1000))
#define MILLISECONDS_PER_SECOND       ((uint16_t) (1000))
#define SECONDS_PER_MINUTE            ((uint8_t)  (60))
#define MINUTES_PER_HOUR              ((uint8_t)  (60))
#define HOURS_PER_DAY                 ((uint8_t)  (24))
#define DAYS_PER_WEEK                 ((uint8_t)  (7))
#define DAYS_PER_YEAR                 ((float)    (365.25f))
#define MONTHS_PER_YEAR               ((uint8_t)  (12))
#define YEARS_PER_DECADE              ((uint8_t)  (10))
#define DECADES_PER_CENTURY           ((uint8_t)  (10))
#define CENTURIES_PER_MILLENIUM       ((uint8_t)  (10))

#define MICROSECONDS_PER_SECOND       ((uint32_t) ((uint32_t)MICROSECONDS_PER_MILLISECOND * (uint32_t)MILLISECONDS_PER_SECOND))
#define MILLISECONDS_PER_MINUTE       ((uint16_t) ((uint16_t)MILLISECONDS_PER_SECOND * (uint16_t)SECONDS_PER_MINUTE))
#define SECONDS_PER_HOUR              ((uint16_t) ((uint16_t)SECONDS_PER_MINUTE * (uint16_t)MINUTES_PER_HOUR))
#define MINUTES_PER_DAY               ((uint16_t) ((uint16_t)MINUTES_PER_HOUR * (uint16_t)HOURS_PER_DAY))
#define HOURS_PER_WEEK                ((uint8_t)  (HOURS_PER_DAY * DAYS_PER_WEEK))
#define HOURS_PER_YEAR                ((uint16_t) ((float)HOURS_PER_DAY * DAYS_PER_YEAR))
#define DAYS_PER_MONTH                ((float)    (DAYS_PER_YEAR / (float)MONTHS_PER_YEAR))
#define DAYS_PER_DECADE               ((float)    (DAYS_PER_YEAR * (float)YEARS_PER_DECADE))
#define WEEKS_PER_YEAR                ((float)    (DAYS_PER_YEAR / (float)DAYS_PER_WEEK))
#define MONTHS_PER_DECADE             ((uint8_t)  (MONTHS_PER_YEAR * YEARS_PER_DECADE))
#define YEARS_PER_CENTURY             ((uint8_t)  (YEARS_PER_DECADE * DECADES_PER_CENTURY))
#define DECADES_PER_MILLENIUM         ((uint8_t)  (DECADES_PER_CENTURY * CENTURIES_PER_MILLENIUM))

#define MICROSECONDS_PER_MINUTE       ((uint32_t) ((uint32_t)MICROSECONDS_PER_MILLISECOND * (uint32_t)MILLISECONDS_PER_MINUTE))
#define MILLISECONDS_PER_HOUR         ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * (uint32_t)SECONDS_PER_HOUR))
#define SECONDS_PER_DAY               ((uint32_t) ((uint32_t)SECONDS_PER_MINUTE * (uint32_t)MINUTES_PER_DAY))
#define MINUTES_PER_WEEK              ((uint16_t) ((uint16_t)MINUTES_PER_HOUR * (uint16_t)HOURS_PER_WEEK))
#define HOURS_PER_MONTH               ((float)    ((float)HOURS_PER_DAY * DAYS_PER_MONTH))
#define HOURS_PER_DECADE              ((uint32_t) ((float)HOURS_PER_DAY * DAYS_PER_DECADE))
#define MINUTES_PER_YEAR              ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * (uint32_t)HOURS_PER_YEAR))
#define DAYS_PER_CENTURY              ((uint16_t) (DAYS_PER_YEAR * (float)YEARS_PER_CENTURY))
#define WEEKS_PER_MONTH               ((float)    (DAYS_PER_MONTH / (float)DAYS_PER_WEEK))
#define WEEKS_PER_DECADE              ((float)    (DAYS_PER_DECADE / (float)DAYS_PER_WEEK))
#define MONTHS_PER_CENTURY            ((uint16_t) ((uint16_t)MONTHS_PER_YEAR * (uint16_t)YEARS_PER_CENTURY))
#define YEARS_PER_MILLENIUM           ((uint16_t) ((uint16_t)YEARS_PER_DECADE * (uint16_t)DECADES_PER_MILLENIUM))

#define MICROSECONDS_PER_HOUR         ((uint32_t) ((uint32_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_HOUR))
#define MILLISECONDS_PER_DAY          ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * SECONDS_PER_DAY))
#define SECONDS_PER_WEEK              ((uint32_t) (SECONDS_PER_MINUTE * (uint32_t)MINUTES_PER_WEEK))
#define SECONDS_PER_YEAR              ((uint32_t) (SECONDS_PER_MINUTE * MINUTES_PER_YEAR))
#define MINUTES_PER_MONTH             ((uint16_t) ((float)MINUTES_PER_HOUR * HOURS_PER_MONTH))
#define MINUTES_PER_DECADE            ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * HOURS_PER_DECADE))
#define HOURS_PER_CENTURY             ((uint32_t) ((uint32_t)HOURS_PER_DAY * (uint32_t)DAYS_PER_CENTURY))
#define DAYS_PER_MILLENIUM            ((uint32_t) (DAYS_PER_YEAR * (float)YEARS_PER_MILLENIUM))
#define WEEKS_PER_CENTURY             ((float)    ((float)DAYS_PER_CENTURY / (float)DAYS_PER_WEEK))
#define MONTHS_PER_MILLENIUM          ((uint16_t) ((uint16_t)MONTHS_PER_YEAR * YEARS_PER_MILLENIUM))

#define MICROSECONDS_PER_DAY          ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * (uint64_t)MILLISECONDS_PER_DAY))
#define MILLISECONDS_PER_WEEK         ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * SECONDS_PER_WEEK))
#define MILLISECONDS_PER_YEAR         ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * (uint64_t)SECONDS_PER_YEAR))
#define SECONDS_PER_MONTH             ((uint32_t) ((uint32_t)SECONDS_PER_MINUTE * (uint32_t)MINUTES_PER_MONTH))
#define SECONDS_PER_DECADE            ((uint32_t) ((uint32_t)SECONDS_PER_MINUTE * MINUTES_PER_DECADE))
#define MINUTES_PER_CENTURY           ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * HOURS_PER_CENTURY))
#define HOURS_PER_MILLENIUM           ((uint32_t) ((uint32_t)HOURS_PER_DAY * DAYS_PER_MILLENIUM))
#define WEEKS_PER_MILLENIUM           ((float)    ((float)DAYS_PER_MILLENIUM / (float)DAYS_PER_WEEK))

#define MICROSECONDS_PER_WEEK         ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * (uint64_t)MILLISECONDS_PER_WEEK))
#define MICROSECONDS_PER_YEAR         ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_YEAR))
#define MILLISECONDS_PER_MONTH        ((uint32_t) ((uint32_t)MILLISECONDS_PER_SECOND * SECONDS_PER_MONTH))
#define MILLISECONDS_PER_DECADE       ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * (uint64_t)SECONDS_PER_DECADE))
#define SECONDS_PER_CENTURY           ((uint32_t) (SECONDS_PER_MINUTE * MINUTES_PER_CENTURY))
#define MINUTES_PER_MILLENIUM         ((uint32_t) ((uint32_t)MINUTES_PER_HOUR * HOURS_PER_MILLENIUM))

#define MICROSECONDS_PER_MONTH        ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * (uint64_t)MILLISECONDS_PER_MONTH))
#define MICROSECONDS_PER_DECADE       ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_DECADE))
#define MILLISECONDS_PER_CENTURY      ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * (uint64_t)SECONDS_PER_CENTURY))
#define SECONDS_PER_MILLENIUM         ((uint64_t) ((uint64_t)SECONDS_PER_MINUTE * (uint64_t)MINUTES_PER_MILLENIUM))

#define MICROSECONDS_PER_CENTURY      ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_CENTURY))
#define MILLISECONDS_PER_MILLENIUM    ((uint64_t) ((uint64_t)MILLISECONDS_PER_SECOND * SECONDS_PER_MILLENIUM))

#define MICROSECONDS_PER_MILLENIUM    ((uint64_t) ((uint64_t)MICROSECONDS_PER_MILLISECOND * MILLISECONDS_PER_MILLENIUM))

#define MILLISECONDS_PER_MICROSECOND  ((float)MILLISECONDS_PER_MILLISECOND / (float)MICROSECONDS_PER_MILLISECOND)

#define SECONDS_PER_MICROSECOND       ((float)SECONDS_PER_SECOND / (float)MICROSECONDS_PER_SECOND)
#define SECONDS_PER_MILLISECOND       ((float)SECONDS_PER_SECOND / (float)MILLISECONDS_PER_SECOND)

#define MINUTES_PER_MICROSECOND       ((float)MINUTES_PER_MINUTE / (float)MICROSECONDS_PER_MINUTE)
#define MINUTES_PER_MILLISECOND       ((float)MINUTES_PER_MINUTE / (float)MILLISECONDS_PER_MINUTE)
#define MINUTES_PER_SECOND            ((float)MINUTES_PER_MINUTE / (float)SECONDS_PER_MINUTE)

#define HOURS_PER_MICROSECOND         ((float)HOURS_PER_HOUR / (float)MICROSECONDS_PER_HOUR)
#define HOURS_PER_MILLISECOND         ((float)HOURS_PER_HOUR / (float)MILLISECONDS_PER_HOUR)
#define HOURS_PER_SECOND              ((float)HOURS_PER_HOUR / (float)SECONDS_PER_HOUR)
#define HOURS_PER_MINUTE              ((float)HOURS_PER_HOUR / (float)MINUTES_PER_HOUR)

#define DAYS_PER_MICROSECOND          ((float)DAYS_PER_DAY / (float)MICROSECONDS_PER_DAY)
#define DAYS_PER_MILLISECOND          ((float)DAYS_PER_DAY / (float)MILLISECONDS_PER_DAY)
#define DAYS_PER_SECOND               ((float)DAYS_PER_DAY / (float)SECONDS_PER_DAY)
#define DAYS_PER_MINUTE               ((float)DAYS_PER_DAY / (float)MINUTES_PER_DAY)
#define DAYS_PER_HOUR                 ((float)DAYS_PER_DAY / (float)HOURS_PER_DAY)

#define WEEKS_PER_MICROSECOND         ((float)WEEKS_PER_WEEK / (float)MICROSECONDS_PER_WEEK)
#define WEEKS_PER_MILLISECOND         ((float)WEEKS_PER_WEEK / (float)MILLISECONDS_PER_WEEK)
#define WEEKS_PER_SECOND              ((float)WEEKS_PER_WEEK / (float)SECONDS_PER_WEEK)
#define WEEKS_PER_MINUTE              ((float)WEEKS_PER_WEEK / (float)MINUTES_PER_WEEK)
#define WEEKS_PER_HOUR                ((float)WEEKS_PER_WEEK / (float)HOURS_PER_WEEK)
#define WEEKS_PER_DAY                 ((float)WEEKS_PER_WEEK / (float)DAYS_PER_WEEK)

#define MONTHS_PER_MICROSECOND        ((float)MONTHS_PER_MONTH / (float)MICROSECONDS_PER_MONTH)
#define MONTHS_PER_MILLISECOND        ((float)MONTHS_PER_MONTH / (float)MILLISECONDS_PER_MONTH)
#define MONTHS_PER_SECOND             ((float)MONTHS_PER_MONTH / (float)SECONDS_PER_MONTH)
#define MONTHS_PER_MINUTE             ((float)MONTHS_PER_MONTH / (float)MINUTES_PER_MONTH)
#define MONTHS_PER_HOUR               ((float)MONTHS_PER_MONTH / (float)HOURS_PER_MONTH)
#define MONTHS_PER_DAY                ((float)MONTHS_PER_MONTH / (float)DAYS_PER_MONTH)
#define MONTHS_PER_WEEK               ((float)MONTHS_PER_MONTH / (float)WEEKS_PER_MONTH)

#define YEARS_PER_MICROSECOND         ((float)YEARS_PER_YEAR / (float)MICROSECONDS_PER_YEAR)
#define YEARS_PER_MILLISECOND         ((float)YEARS_PER_YEAR / (float)MILLISECONDS_PER_YEAR)
#define YEARS_PER_SECOND              ((float)YEARS_PER_YEAR / (float)SECONDS_PER_YEAR)
#define YEARS_PER_MINUTE              ((float)YEARS_PER_YEAR / (float)MINUTES_PER_YEAR)
#define YEARS_PER_HOUR                ((float)YEARS_PER_YEAR / (float)HOURS_PER_YEAR)
#define YEARS_PER_DAY                 ((float)YEARS_PER_YEAR / (float)DAYS_PER_YEAR)
#define YEARS_PER_WEEK                ((float)YEARS_PER_YEAR / (float)WEEKS_PER_YEAR)
#define YEARS_PER_MONTH               ((float)YEARS_PER_YEAR / (float)MONTHS_PER_YEAR)

#define DECADES_PER_MICROSECOND       ((float)DECADES_PER_DECADE / (float)MICROSECONDS_PER_DECADE)
#define DECADES_PER_MILLISECOND       ((float)DECADES_PER_DECADE / (float)MILLISECONDS_PER_DECADE)
#define DECADES_PER_SECOND            ((float)DECADES_PER_DECADE / (float)SECONDS_PER_DECADE)
#define DECADES_PER_MINUTE            ((float)DECADES_PER_DECADE / (float)MINUTES_PER_DECADE)
#define DECADES_PER_HOUR              ((float)DECADES_PER_DECADE / (float)HOURS_PER_DECADE)
#define DECADES_PER_DAY               ((float)DECADES_PER_DECADE / (float)DAYS_PER_DECADE)
#define DECADES_PER_WEEK              ((float)DECADES_PER_DECADE / (float)WEEKS_PER_DECADE)
#define DECADES_PER_MONTH             ((float)DECADES_PER_DECADE / (float)MONTHS_PER_DECADE)
#define DECADES_PER_YEAR              ((float)DECADES_PER_DECADE / (float)YEARS_PER_DECADE)

#define CENTURIES_PER_MICROSECOND     ((float)CENTURIES_PER_CENTURY / (float)MICROSECONDS_PER_CENTURY)
#define CENTURIES_PER_MILLISECOND     ((float)CENTURIES_PER_CENTURY / (float)MILLISECONDS_PER_CENTURY)
#define CENTURIES_PER_SECOND          ((float)CENTURIES_PER_CENTURY / (float)SECONDS_PER_CENTURY)
#define CENTURIES_PER_MINUTE          ((float)CENTURIES_PER_CENTURY / (float)MINUTES_PER_CENTURY)
#define CENTURIES_PER_HOUR            ((float)CENTURIES_PER_CENTURY / (float)HOURS_PER_CENTURY)
#define CENTURIES_PER_DAY             ((float)CENTURIES_PER_CENTURY / (float)DAYS_PER_CENTURY)
#define CENTURIES_PER_WEEK            ((float)CENTURIES_PER_CENTURY / (float)WEEKS_PER_CENTURY)
#define CENTURIES_PER_MONTH           ((float)CENTURIES_PER_CENTURY / (float)MONTHS_PER_CENTURY)
#define CENTURIES_PER_YEAR            ((float)CENTURIES_PER_CENTURY / (float)YEARS_PER_CENTURY)
#define CENTURIES_PER_DECADE          ((float)CENTURIES_PER_CENTURY / (float)DECADES_PER_CENTURY)

#define MILLENIUMS_PER_MICROSECOND    ((float)MILLENIUMS_PER_MILLENIUM / (float)MICROSECONDS_PER_MILLENIUM)
#define MILLENIUMS_PER_MILLISECOND    ((float)MILLENIUMS_PER_MILLENIUM / (float)MILLISECONDS_PER_MILLENIUM)
#define MILLENIUMS_PER_SECOND         ((float)MILLENIUMS_PER_MILLENIUM / (float)SECONDS_PER_MILLENIUM)
#define MILLENIUMS_PER_MINUTE         ((float)MILLENIUMS_PER_MILLENIUM / (float)MINUTES_PER_MILLENIUM)
#define MILLENIUMS_PER_HOUR           ((float)MILLENIUMS_PER_MILLENIUM / (float)HOURS_PER_MILLENIUM)
#define MILLENIUMS_PER_DAY            ((float)MILLENIUMS_PER_MILLENIUM / (float)DAYS_PER_MILLENIUM)
#define MILLENIUMS_PER_WEEK           ((float)MILLENIUMS_PER_MILLENIUM / (float)WEEKS_PER_MILLENIUM)
#define MILLENIUMS_PER_MONTH          ((float)MILLENIUMS_PER_MILLENIUM / (float)MONTHS_PER_MILLENIUM)
#define MILLENIUMS_PER_YEAR           ((float)MILLENIUMS_PER_MILLENIUM / (float)YEARS_PER_MILLENIUM)
#define MILLENIUMS_PER_DECADE         ((float)MILLENIUMS_PER_MILLENIUM / (float)DECADES_PER_MILLENIUM)
#define MILLENIUMS_PER_CENTURY        ((float)MILLENIUMS_PER_MILLENIUM / (float)CENTURIES_PER_MILLENIUM)

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t aosTimeDayOfWeekFromDate(const uint16_t day, const uint8_t month, const uint16_t year);
#ifdef __cplusplus
}
#endif

#endif /* _AMIROOS_TIME_H_ */
