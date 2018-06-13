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

#ifndef _AOSCONF_H_
#define _AOSCONF_H_

/*
 * compatibility guards
 */
#define _AMIRO_OS_CFG_
#define _AMIRO_OS_CFG_VERSION_MAJOR_            2
#define _AMIRO_OS_CFG_VERSION_MINOR_            0

#include <stdbool.h>

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

/*
 * Include an external configuration file to override the following default settings only if required.
 */
#if defined(AMIRO_APPS) && (AMIRO_APPS == true)
  #include <osconf.h>
#endif

/**
 * @brief   Flag to set the module as SSSP master.
 */
#if !defined(OS_CFG_SSSP_MASTER)
  #define AMIROOS_CFG_SSSP_MASTER               true
#else
  #define AMIROOS_CFG_SSSP_MASTER               OS_CFG_SSSP_MASTER
#endif

/**
 * @brief   Time boundary for robot wide clock synchronization in microseconds.
 * @details Whenever the SSSP S (snychronization) signal gets logically deactivated,
 *          All modules need to align their local uptime to the nearest multiple of this value.
 */
#if !defined(OS_CFG_SSSP_SYSSYNCPERIOD)
  #define AMIROOS_CFG_SSSP_SYSSYNCPERIOD        1000000
#else
  #define AMIROOS_CFG_SSSP_SYSSYNCPERIOD        OS_CFG_SSSP_SYSSYNCPERIOD
#endif

/**
 * @brief   Flag to enable/disable debug API.
 */
#if !defined(OS_CFG_DBG)
  #define AMIROOS_CFG_DBG                       true
#else
  #define AMIROOS_CFG_DBG                       OS_CFG_DBG
#endif

/**
 * @brief   Flag to enable/disable unit tests.
 */
#if !defined(OS_CFG_TESTS_ENABLE)
  #define AMIROOS_CFG_TESTS_ENABLE              true
#else
  #define AMIROOS_CFG_TESTS_ENABLE              OS_CFG_TESTS_ENABLE
#endif

/**
 * @brief   Flag to enable/disable profiling API.
 */
#if !defined(OS_CFG_PROFILE)
  #define AMIROOS_CFG_PROFILE                   true
#else
  #define AMIROOS_CFG_PROFILE                   OS_CFG_PROFILE
#endif

/**
 * @brief   Timeout value when waiting for events in the main loop in microseconds.
 * @details A value of 0 deactivates the timeout.
 */
#if !defined(OS_CFG_MAIN_LOOP_TIMEOUT)
  #define AMIROOS_CFG_MAIN_LOOP_TIMEOUT         0
#else
  #define AMIROOS_CFG_MAIN_LOOP_TIMEOUT         OS_CFG_MAIN_LOOP_TIMEOUT
#endif

/** @} */

/*===========================================================================*/
/**
 * @name System shell options
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Shell enable flag.
 */
#if (AMIROOS_CFG_TESTS_ENABLE == false) && !defined(OS_CFG_SHELL_ENABLE)
  #define AMIROOS_CFG_SHELL_ENABLE              true
#elif (AMIROOS_CFG_TESTS_ENABLE == true)
  #define AMIROOS_CFG_SHELL_ENABLE              true
#else
  #define AMIROOS_CFG_SHELL_ENABLE              OS_CFG_SHELL_ENABLE
#endif

/**
 * @brief   Shell thread stack size.
 */
#if !defined(OS_CFG_SHELL_STACKSIZE)
  #define AMIROOS_CFG_SHELL_STACKSIZE           1024
#else
  #define AMIROOS_CFG_SHELL_STACKSIZE           OS_CFG_SHELL_STACKSIZE
#endif

/**
 * @brief   Shell thread priority.
 */
#if !defined(OS_CFG_SHELL_THREADPRIO)
  #define AMIROOS_CFG_SHELL_THREADPRIO          THD_NORMALPRIO_MIN
#else
  #define AMIROOS_CFG_SHELL_THREADPRIO          OS_CFG_SHELL_THREADPRIO
#endif

/**
 * @brief   Shell maximum input line length.
 */
#if !defined(OS_CFG_SHELL_LINEWIDTH)
  #define AMIROOS_CFG_SHELL_LINEWIDTH           64
#else
  #define AMIROOS_CFG_SHELL_LINEWIDTH           OS_CFG_SHELL_LINEWIDTH
#endif

/**
 * @brief   Shell maximum number of arguments.
 */
#if !defined(OS_CFG_SHELL_MAXARGS)
  #define AMIROOS_CFG_SHELL_MAXARGS             4
#else
  #define AMIROOS_CFG_SHELL_MAXARGS             OS_CFG_SHELL_MAXARGS
#endif

/** @} */

#endif // _AOSCONF_H_

