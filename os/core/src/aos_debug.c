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

#include <aos_debug.h>

#include <hal.h>

/*
 * System_halt
 * ChibiOS error code (see chdebug.c)
 *
 *            - SV#1, misplaced @p chSysDisable().
 *            - SV#2, misplaced @p chSysSuspend()
 *            - SV#3, misplaced @p chSysEnable().
 *            - SV#4, misplaced @p chSysLock().
 *            - SV#5, misplaced @p chSysUnlock().
 *            - SV#6, misplaced @p chSysLockFromIsr().
 *            - SV#7, misplaced @p chSysUnlockFromIsr().
 *            - SV#8, misplaced @p CH_IRQ_PROLOGUE().
 *            - SV#9, misplaced @p CH_IRQ_EPILOGUE().
 *            - SV#10, misplaced I-class function.
 *            - SV#11, misplaced S-class function.
 */

#if ((CH_DBG_SYSTEM_STATE_CHECK == TRUE) && (HAL_USE_SERIAL == TRUE)) || defined(__DOXYGEN__)

/**
 * @brief   Actively waits for the serial driver to be empty.
 */
static inline void _serialWaitForEmpty(SerialDriver* sdp) {
  while (!((sdp->usart->SR) & USART_SR_TC)) {
    continue;
  }
  return;
}

/**
 * @brief   Prints the specified string without suspending the thread.
 *
 * @param[in] str   The string to print.
 *
 * @return          The number of characters printed.
 */
static inline unsigned int _printString(SerialDriver* sdp, const char* str)
{
  unsigned int i = 0;
  while (str[i] != '\0') {
    sdp->usart->DR = str[i];
    _serialWaitForEmpty(sdp);
    ++i;
  }
  return i;
}

/**
 * @brief   Prints the specified string to the given serial stream.
 *
 * @param[in] sdp       serial stream to print to
 * @param[in] reason    string to print
 */
static void _printError(SerialDriver* sdp, const char* reason)
{
  if (sdp->state != SD_STOP && sdp->state != SD_READY) {
    sd_lld_start(sdp, NULL);
    sdp->state = SD_READY;
  }
  if (sdp->state == SD_READY) {
    _printString(sdp, "\nSystem halt! error code / function name: ");
    _printString(sdp, reason);
    _printString(sdp, "\n");
  }

  return;
}

#endif /* ((CH_DBG_SYSTEM_STATE_CHECK == TRUE) && (HAL_USE_SERIAL == TRUE)) || defined(__DOXYGEN__) */

/**
 * @brief   Prints an error message.
 *
 * @param[in] reason  The string to print.
 */
void aosPrintHaltErrorCode(const char* reason)
{
#if (CH_DBG_SYSTEM_STATE_CHECK == TRUE) && (HAL_USE_SERIAL == TRUE)
  #if STM32_SERIAL_USE_USART1
  _printError(&SD1, reason);
  #endif
  #if STM32_SERIAL_USE_USART2
  _printError(&SD2, reason);
  #endif
  #if STM32_SERIAL_USE_USART3
  _printError(&SD3, reason);
  #endif
  #if STM32_SERIAL_USE_UART4
  _printError(&SD4, reason);
  #endif
  #if STM32_SERIAL_USE_UART5
  _printError(&SD5, reason);
  #endif
  #if STM32_SERIAL_USE_USART6
  _printError(&SD6, reason);
  #endif
  #if STM32_SERIAL_USE_UART7
  _printError(&SD7, reason);
  #endif
  #if STM32_SERIAL_USE_UART8
  _printError(&SD8, reason);
  #endif
#else
  (void)reason;
#endif /* (CH_DBG_SYSTEM_STATE_CHECK == TRUE) && (HAL_USE_SERIAL == TRUE) */

  return;
}
