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

#include <aos_interrupts.h>

/**
 * @brief   Initializes the interrupt driver.
 *
 * @param[out]  intd            Interrupt driver object.
 * @param[in]   interrupt_cfg   Interrupt configuration to be used to initialize the driver.
 */
void aosIntDriverInit(aos_interrupt_driver_t *intd, aos_interrupt_cfg_t *interrupt_cfg) {
  intd->interrupts = interrupt_cfg;
}

/**
 * @brief   Starts the interrupt system by setting the pad callbacks and starting the pad events.
 *
 * @param[in]   intd    Interrupt driver with an interrupt configuration that specifies which interrupts to start.
 */
void aosIntDriverStart(aos_interrupt_driver_t *intd) {
  irqInit();
  for (uint8_t i = 0; i < intd->len; i++) {
    chSysLock();
    palSetPadCallbackI(intd->interrupts[i].port, intd->interrupts[i].pad, intd->interrupts[i].cb, &intd->interrupts[i].cb_arg);
    if (intd->interrupts[i].flags & AOS_INTERRUPT_AUTOSTART) {
      palEnablePadEventI(intd->interrupts[i].port, intd->interrupts[i].pad, intd->interrupts[i].eventmode);
    }
    chSysUnlock();
  }
}

/**
 * @brief   Stops the interrupt system.
 *
 * @param[in]   intd   All events associated with this driver will be disabled.
 */
void aosIntDriverStop(aos_interrupt_driver_t *intd) {
  for (uint8_t i = 0; i < intd->len; i++) {
    palDisablePadEvent(intd->interrupts[i].port, intd->interrupts[i].pad);
  }
  irqDeinit();
}

/**
 * @brief   Enables the events on a specified channel.
 *
 * @param[in]   intd    Interrupt driver object.
 * @param[in]   channel Channel of the interrupt to enable.
 */
void aosIntEnable(aos_interrupt_driver_t *intd, uint8_t channel) {
  channel -= 1;
  palEnablePadEvent(intd->interrupts[channel].port, intd->interrupts[channel].pad, intd->interrupts[channel].eventmode);
}

/**
 * @brief   Disables the events on a specified channel.
 *
 * @param[in]   intd    Interrupt driver object.
 * @param[in]   channel Channel of the interrupt to disable.
 */
void aosIntDisable(aos_interrupt_driver_t *intd, uint8_t channel) {
  channel -= 1;
  palDisablePadEvent(intd->interrupts[channel].port, intd->interrupts[channel].pad);
}
