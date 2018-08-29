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

#ifndef _AMIROOS_INTERRUPTS_H_
#define _AMIROOS_INTERRUPTS_H_

#include <hal.h>

/**
 * @brief   Interrupt configuration flag to autostart the interrupt when the interrupt system is started.
 */
#define AOS_INTERRUPT_AUTOSTART 0x1u

/**
 * @brief   Interrupt configuration structure.
 */
typedef struct {
  ioportid_t    port;      /**< Gpio port id. */
  uint8_t       pad;       /**< Gpio pad.     */
  uint8_t       flags;     /**< Interrupt configuration flags. */
  uint8_t       eventmode; /**< Pal event mode. */
  palcallback_t cb;        /**< Pal callback function. */
  uint8_t       cb_arg;    /**< Argument for the callback function. */
} aos_interrupt_cfg_t;

/**
 * @brief   Interrupt driver structure.
 */
typedef struct {
  aos_interrupt_cfg_t *interrupts; /**< Array of interrupt configurations. */
  uint8_t len;                     /**< Length of the interrupt configuration array. */
} aos_interrupt_driver_t;

#ifdef __cplusplus
extern "C" {
#endif
  void aosIntDriverInit(aos_interrupt_driver_t *intd, aos_interrupt_cfg_t *interrupt_cfg);
  void aosIntDriverStart(aos_interrupt_driver_t *intd);
  void aosIntDriverStop(aos_interrupt_driver_t *intd);
  void aosIntEnable(aos_interrupt_driver_t *intd, uint8_t channel);
  void aosIntDisable(aos_interrupt_driver_t *intd, uint8_t channel);
#ifdef __cplusplus
}
#endif

#endif /* _AMIROOS_INTERRUPTS_H_ */
