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

/**
 * @file    os/modules/PowerManagement/chconf.h
 * @brief   ChibiOS Configuration file for the PowerManagement v1.1 module.
 * @details Contains the application specific kernel settings.
 *
 * @addtogroup config
 * @details Kernel related settings and hooks.
 * @{
 */

#ifndef _CHCONF_H_
#define _CHCONF_H_

#include <aosconf.h>

/*===========================================================================*/
/**
 * @name System timers settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   System time counter resolution.
 * @note    Allowed values are 16 or 32 bits.
 */
#define CH_CFG_ST_RESOLUTION                32

/**
 * @brief   System tick frequency.
 * @details Frequency of the system timer that drives the system ticks. This
 *          setting also defines the system tick time unit.
 */
#define CH_CFG_ST_FREQUENCY                 1000000UL

/**
 * @brief   Time delta constant for the tick-less mode.
 * @note    If this value is zero then the system uses the classic
 *          periodic tick. This value represents the minimum number
 *          of ticks that is safe to specify in a timeout directive.
 *          The value one is not valid, timeouts are rounded up to
 *          this value.
 */
#define CH_CFG_ST_TIMEDELTA                 10

/** @} */

/*===========================================================================*/
/**
 * @name Port specific settings
 * @{
 */
/*===========================================================================*/

/**
 * @brief   NVIC VTOR initialization offset.
 * @details On initialization, the code at this address in the flash memory will be executed.
 */
#define CORTEX_VTOR_INIT 0x00008000U

/** @} */

#include <aos_chconf.h>

#endif  /* _CHCONF_H_ */

/** @} */
