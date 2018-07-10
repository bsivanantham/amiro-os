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

#ifndef _AMIROOS_MODULE_H_
#define _AMIROOS_MODULE_H_

/*===========================================================================*/
/**
 * @name Module specific functions
 * @{
 */
/*===========================================================================*/

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/
#include <hal.h>

/**
 * @brief   CAN driver to use.
 */
#define MODULE_HAL_CAN                          CAND1

/**
 * @brief   Configuration for the CAN driver.
 */
extern CANConfig moduleHalCanConfig;

/**
 * @brief   Interrupt driver to use.
 */
#define MODULE_HAL_EXT                          EXTD1

/**
 * @brief   Interrupt driver configuration.
 */
extern EXTConfig moduleHalExtConfig;

/**
 * @brief   I2C driver to access the EEPROM.
 */
#define MODULE_HAL_I2C_EEPROM                   I2CD2

/**
 * @brief   Configuration for the EEPROM I2C driver.
 */
extern I2CConfig moduleHalI2cEepromConfig;

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_PROGIF                       SD1

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern SerialConfig moduleHalProgIfConfig;

/**
 * @brief   SPI interface driver for the motion sensors (gyroscope and accelerometer).
 */
#define MODULE_HAL_SPI_LIGHT                    SPID1

/**
 * @brief   Configuration for the SPI interface driver to communicate with the LED driver.
 */
extern SPIConfig moduleHalSpiLightConfig;

/**
 * @brief   Real-Time Clock driver.
 */
#define MODULE_HAL_RTC                          RTCD1

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/
#include <amiro-lld.h>

/**
 * @brief   Interrupt channel for the SYS_SYNC signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSSYNC          ((expchannel_t)2)

/**
 * @brief   Interrupt channel for the LASER_OC signal.
 */
#define MODULE_GPIO_EXTCHANNEL_LASEROC          ((expchannel_t)5)

/**
 * @brief   Interrupt channel for the SYS_UART_DN signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSUARTDN        ((expchannel_t)6)

/**
 * @brief   Interrupt channel for the WL_GDO2 signal.
 */
#define MODULE_GPIO_EXTCHANNEL_WLGDO2           ((expchannel_t)8)

/**
 * @brief   Interrupt channel for the WL_GDO0 signal.
 */
#define MODULE_GPIO_EXTCHANNEL_WLGDO0           ((expchannel_t)9)

/**
 * @brief   Interrupt channel for the SYS_PD signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSPD            ((expchannel_t)14)

/**
 * @brief   LIGHT_BANK output signal GPIO.
 */
extern apalGpio_t moduleGpioLightBlank;

/**
 * @brief   LASER_EN output signal GPIO.
 */
extern apalGpio_t moduleGpioLaserEn;

/**
 * @brief   LASER_OC input signal GPIO.
 */
extern apalGpio_t moduleGpioLaserOc;

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysUartDn;

/**
 * @brief   WL_GDO2 input signal GPIO.
 */
extern apalGpio_t moduleGpioWlGdo2;

/**
 * @brief   WL_GDO0 input signal GPIO.
 */
extern apalGpio_t moduleGpioWlGdo0;

/**
 * @brief   LIGHT_XLAT output signal GPIO.
 */
extern apalGpio_t moduleGpioLightXlat;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysSync;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a LASER_OC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_LASEROC          ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_LASEROC))

/**
 * @brief   Event flag to be set on a SYS_UART_DN interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSUARTDN        ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSUARTDN))

/**
 * @brief   Event flag to be set on a WL_GDO2 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_WLGDO2           ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_WLGDO2))

/**
 * @brief   Event flag to be set on a WL_GDO0 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_WLGDO0           ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_WLGDO0))

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSPD            ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSPD))

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSSYNC          ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSSYNC))

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell prompt text.
 */
extern const char* moduleShellPrompt;
#endif

/**
 * @brief   Unit test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                 \
  /* add unit-test shell commands */                                          \
  aosShellAddCommand(&aos.shell, &moduleUtAlldAt24c01bn.shellcmd);            \
  aosShellAddCommand(&aos.shell, &moduleUtAlldTlc5947.shellcmd);              \
  aosShellAddCommand(&aos.shell, &moduleUtAlldTps2051bdbv.shellcmd);          \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_COMM() {                                        \
  /* serial driver */                                                         \
  sdStart(&MODULE_HAL_PROGIF, &moduleHalProgIfConfig);                        \
  /* I2C */                                                                   \
  moduleHalI2cEepromConfig.clock_speed = (AT24C01BN_LLD_I2C_MAXFREQUENCY < moduleHalI2cEepromConfig.clock_speed) ? AT24C01BN_LLD_I2C_MAXFREQUENCY : moduleHalI2cEepromConfig.clock_speed; \
  moduleHalI2cEepromConfig.duty_cycle = (moduleHalI2cEepromConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;  \
  i2cStart(&MODULE_HAL_I2C_EEPROM, &moduleHalI2cEepromConfig);                \
  /* SPI */                                                                   \
  spiStart(&MODULE_HAL_SPI_LIGHT, &moduleHalSpiLightConfig);                  \
}

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_COMM() {                                    \
  /* SPI */                                                                   \
  spiStop(&MODULE_HAL_SPI_LIGHT);                                             \
  /* I2C */                                                                   \
  i2cStop(&MODULE_HAL_I2C_EEPROM);                                            \
  /* don't stop the serial driver so messages can still be printed */         \
}

/** @} */

/*===========================================================================*/
/**
 * @name Startup Shutdown Synchronization Protocol (SSSP)
 * @{
 */
/*===========================================================================*/

/**
 * @brief   PD signal GPIO.
 */
extern apalControlGpio_t moduleSsspGpioPd;

/**
 * @brief   SYNC signal GPIO.
 */
extern apalControlGpio_t moduleSsspGpioSync;

/**
 * @brief   Event flags for PD signal events.
 */
#define MODULE_SSSP_EVENTFLAGS_PD               MODULE_OS_IOEVENTFLAGS_SYSPD

/**
 * @brief   Event flags for Sync signal events.
 */
#define MODULE_SSSP_EVENTFLAGS_SYNC             MODULE_OS_IOEVENTFLAGS_SYSSYNC

/**
 * @brief   Hook to handle IO events during SSSP startup synchronization.
 */
#define MODULE_SSSP_STARTUP_OSINIT_OUTRO_IOEVENT_HOOK(mask, flags) {          \
  /* ignore all events */                                                     \
  (void)mask;                                                                 \
  (void)flags;                                                                \
}

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_at24c01bn-sh-b.h>
#include <alld_tlc5947.h>
#include <alld_tps2051bdbv.h>

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BNDriver moduleLldEeprom;

/**
 * @brief   LED PWM driver.
 */
extern TLC5947Driver moduleLldLedPwm;

/**
 * @brief   Power switch driver for the laser supply power.
 */
extern TPS2051BDriver moduleLldPowerSwitchLaser;

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <ut_alld_at24c01bn-sh-b.h>
#include <ut_alld_tlc5947.h>
#include <ut_alld_tps2051bdbv.h>

/**
 * @brief   EEPROM unit test object.
 */
extern aos_unittest_t moduleUtAlldAt24c01bn;

/**
 * @brief   LED PWM driver unit test object.
 */
extern aos_unittest_t moduleUtAlldTlc5947;

/**
 * @brief   Current-limited power switch (Laser output)
 */
extern aos_unittest_t moduleUtAlldTps2051bdbv;

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */

#endif /* _AMIROOS_MODULE_H_ */
