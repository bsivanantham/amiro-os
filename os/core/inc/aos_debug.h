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

#ifndef _AMIROOS_DEBUG_H_
#define _AMIROOS_DEBUG_H_

#include <aosconf.h>
#include <ch.h>

#if (AMIROOS_CFG_DBG == true) || defined(__DOXYGEN__)

/**
 * @brief   Function parameters check.
 * @details If the condition check fails, the kernel panics and halts.
 *
 * @param[in] c The condition to be verified to be true.
 */
#define aosDbgCheck(c)                chDbgCheck(c)

/**
 * @brief   Condition assertion.
 * @details If the condition check fails, the kernel panics with a message and halts.
 * @note    As a limitation of ChibiOS, the remark string is not currently used.
 *
 * @param[in] c The condition to be verified to be true.
 */
#define aosDbgAssert(c)               chDbgAssert(c, "__FILE__(__LINE__): aosDbgAssert failed")

/**
 * @brief   Condition assertion.
 * @details If the condition check fails, the kernel panics with a message and halts.
 * @note    As a limitation of ChibiOS, the remark string is not currently used.
 *
 * @param[in] c The condition to be verified to be true.
 * @param[in] r A custom remark string.
 */
#define aosDbgAssertMsg(c, r) {                           \
  (void)r;                                                \
  chDbgAssert(c, r);                                      \
}

/**
 * @brief   Printf function for messages only printed in debug builds.
 *
 * @param[in] fmt   Formatted string to print.
 */
#define aosDbgPrintf(fmt, ...)           chprintf((BaseSequentialStream*)&aos.iostream, fmt, ##__VA_ARGS__)

#else /* (AMIROOS_CFG_DBG != true) */

#define aosDbgCheck(c) {                                  \
  (void)(c);                                              \
  }

#define aosDbgAssert(c) {                                 \
  (void)(c);                                              \
}

#define aosDbgAssertMsg(c, r) {                           \
  (void)(c);                                              \
  (void)(r);                                              \
}

#define aosDbgPrintf(fmt, ...) {                          \
  (void)(fmt);                                            \
}

#endif

#ifdef __cplusplus
extern "C" {
#endif
  void aosPrintHaltErrorCode(const char* reason);
#ifdef __cplusplus
}
#endif

#endif /* _AMIROOS_DEBUG_H_ */
