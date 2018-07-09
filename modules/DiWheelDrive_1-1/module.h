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
#include <hal_qei.h>

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
 * @brief   I2C driver to access the compass.
 */
#define MODULE_HAL_I2C_COMPASS                  I2CD1

/**
 * @brief   Configuration for the compass I2C driver.
 */
extern I2CConfig moduleHalI2cCompassConfig;

/**
 * @brief   I2C driver to access multiplexer, proximity sensors, EEPROM and power monitor.
 */
#define MODULE_HAL_I2C_PROX_EEPROM_PWRMTR       I2CD2

/**
 * @brief   Configuration for the multiplexer, proximity, EEPROM and power monitor I2C driver.
 */
extern I2CConfig moduleHalI2cProxEepromPwrmtrConfig;

/**
 * @brief   PWM driver to use.
 */
#define MODULE_HAL_PWM_DRIVE                    PWMD2

/**
 * @brief   Configuration for the PWM driver.
 */
extern PWMConfig moduleHalPwmDriveConfig;

/**
 * @brief   Drive PWM channel for the left wheel forward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_FORWARD     0

/**
 * @brief   Drive PWM channel for the left wheel backward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_BACKWARD    1

/**
 * @brief   Drive PWM channel for the right wheel forward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_FORWARD    2

/**
 * @brief   Drive PWM channel for the right wheel backward direction.
 */
#define MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_BACKWARD   3

/**
 * @brief   Quadrature encooder for the left wheel.
 */
#define MODULE_HAL_QEI_LEFT_WHEEL               QEID3

/**
 * @brief   Quadrature encooder for the right wheel.
 */
#define MODULE_HAL_QEI_RIGHT_WHEEL              QEID4

/**
 * @brief   Configuration for both quadrature encoders.
 */
extern QEIConfig moduleHalQeiConfig;

/**
 * @brief   QEI increments per wheel revolution.
 * @details 2 signal edges per pulse * 2 signals * 16 pulses per motor revolution * 22:1 gearbox
 */
#define MODULE_HAL_QEI_INCREMENTS_PER_REVOLUTION  (apalQEICount_t)(2 * 2 * 16 * 22)

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
#define MODULE_HAL_SPI_MOTION                   SPID1

/**
 * @brief   Configuration for the motion sensor SPI interface  driver to communicate with the accelerometer.
 */
extern SPIConfig moduleHalSpiAccelerometerConfig;

/**
 * @brief   Configuration for the motion sensor SPI interface  driver to communicate with the gyroscope.
 */
extern SPIConfig moduleHalSpiGyroscopeConfig;

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
#define MODULE_GPIO_EXTCHANNEL_SYSSYNC          ((expchannel_t)1)

/**
 * @brief   Interrupt channel for the SYS_WARMRST signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSWARMRST       ((expchannel_t)2)

/**
 * @brief   Interrupt channel for the PATH_DCSTAT signal.
 */
#define MODULE_GPIO_EXTCHANNEL_PATHDCSTAT       ((expchannel_t)3)

/**
 * @brief   Interrupt channel for the COMPASS_DRDY signal.
 */
#define MODULE_GPIO_EXTCHANNEL_COMPASSDRDY      ((expchannel_t)5)

/**
 * @brief   Interrupt channel for the SYS_PD signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSPD            ((expchannel_t)7)

/**
 * @brief   Interrupt channel for the SYS_REG_EN signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSREGEN         ((expchannel_t)8)

/**
 * @brief   Interrupt channel for the IR_INT signal.
 */
#define MODULE_GPIO_EXTCHANNEL_IRINT            ((expchannel_t)12)

/**
 * @brief   Interrupt channel for the GYRO_DRDY signal.
 */
#define MODULE_GPIO_EXTCHANNEL_GYRODRDY         ((expchannel_t)13)

/**
 * @brief   Interrupt channel for the SYS_UART_UP signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSUARTUP        ((expchannel_t)14)

/**
 * @brief   Interrupt channel for the ACCEL_INT signal.
 */
#define MODULE_GPIO_EXTCHANNEL_ACCELINT         ((expchannel_t)15)

/**
 * @brief   LED output signal GPIO.
 */
extern apalGpio_t moduleGpioLed;

/**
 * @brief   POWER_EN output signal GPIO.
 */
extern apalGpio_t moduleGpioPowerEn;

/**
 * @brief   COMPASS_DRDY input signal GPIO.
 */
extern apalGpio_t moduleGpioCompassDrdy;

/**
 * @brief   IR_INT input signal GPIO.
 */
extern apalGpio_t moduleGpioIrInt;

/**
 * @brief   GYRO_DRDY input signal GPIO.
 */
extern apalGpio_t moduleGpioGyroDrdy;

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysUartUp;

/**
 * @brief   ACCEL_INT input signal GPIO.
 */
extern apalGpio_t moduleGpioAccelInt;

/**
 * @brief   SYS_SNYC bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysSync;

/**
 * @brief   PATH_DCSTAT input signal GPIO.
 */
extern apalGpio_t moduleGpioPathDcStat;

/**
 * @brief   PATH_DCEN output signal GPIO.
 */
extern apalGpio_t moduleGpioPathDcEn;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_REG_EN input signal GPIO.
 */
extern apalGpio_t moduleGpioSysRegEn;

/**
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysWarmrst;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSSYNC          ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSSYNC))

/**
 * @brief   Event flag to be set on a SYS_WARMRST interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSWARMRST       ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSWARMRST))

/**
 * @brief   Event flag to be set on a PATH_DCSTAT interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_PATHDCSTAT       ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_PATHDCSTAT))

/**
 * @brief   Event flag to be set on a COMPASS_DRDY interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_COMPASSDRDY      ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_COMPASSDRDY))

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSPD            ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSPD))

/**
 * @brief   Event flag to be set on a SYS_REG_EN interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSREGEN         ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSREGEN))

/**
 * @brief   Event flag to be set on a IR_INT interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_IRINT            ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_IRINT))

/**
 * @brief   Event flag to be set on a GYRO_DRDY interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_GYRODRDY         ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_GYRODRDY))

/**
 * @brief   Event flag to be set on a SYS_UART_UP interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSUARTUP        ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_SYSUARTUP))

/**
 * @brief   Event flag to be set on a ACCEL_INT interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_ACCELINT         ((eventflags_t)(1 << MODULE_GPIO_EXTCHANNEL_ACCELINT))

/**
 * @brief   PD signal for SSSP.
 */
extern apalControlGpio_t moduleSsspPd;

/**
 * @brief   SYNC signal for SSSP.
 */
extern apalControlGpio_t moduleSsspSync;

/**
 * @brief   Shell prompt text.
 */
extern const char* moduleShellPrompt;

/**
 * @brief   Additional HAL initialization hook.
 */
#define MODULE_INIT_HAL_EXTRA() {                                             \
  qeiInit();                                                                  \
}

/**
 * @brief   Unit test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                 \
  /* add unit-test shell commands */                                          \
  aosShellAddCommand(aos.shell, &moduleUtAlldA3906.shellcmd);                 \
  aosShellAddCommand(aos.shell, &moduleUtAlldAt24c01bn.shellcmd);             \
  aosShellAddCommand(aos.shell, &moduleUtAlldHmc5883l.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldIna219.shellcmd);                \
  aosShellAddCommand(aos.shell, &moduleUtAlldL3g4200d.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldLed.shellcmd);                   \
  aosShellAddCommand(aos.shell, &moduleUtAlldLis331dlh.shellcmd);             \
  aosShellAddCommand(aos.shell, &moduleUtAlldLtc4412.shellcmd);               \
  aosShellAddCommand(aos.shell, &moduleUtAlldPca9544a.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldTps62113.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldVcnl4020.shellcmd);              \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_COMM() {                                        \
  /* serial driver */                                                         \
  sdStart(&MODULE_HAL_PROGIF, &moduleHalProgIfConfig);                        \
  /* I2C */                                                                   \
  moduleHalI2cCompassConfig.clock_speed = (HMC5883L_LLD_I2C_MAXFREQUENCY < moduleHalI2cCompassConfig.clock_speed) ? HMC5883L_LLD_I2C_MAXFREQUENCY : moduleHalI2cCompassConfig.clock_speed;  \
  moduleHalI2cCompassConfig.duty_cycle = (moduleHalI2cCompassConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;  \
  i2cStart(&MODULE_HAL_I2C_COMPASS, &moduleHalI2cCompassConfig);              \
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (PCA9544A_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? PCA9544A_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed; \
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (VCNL4020_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? VCNL4020_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed; \
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (AT24C01BN_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? AT24C01BN_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed; \
  moduleHalI2cProxEepromPwrmtrConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxEepromPwrmtrConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxEepromPwrmtrConfig.clock_speed; \
  moduleHalI2cProxEepromPwrmtrConfig.duty_cycle = (moduleHalI2cProxEepromPwrmtrConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;  \
  i2cStart(&MODULE_HAL_I2C_PROX_EEPROM_PWRMTR, &moduleHalI2cProxEepromPwrmtrConfig);  \
  /* SPI is shared between accelerometer and gyroscope and needs to be restarted for each transmission */ \
  /* PWM */                                                                   \
  pwmStart(&MODULE_HAL_PWM_DRIVE, &moduleHalPwmDriveConfig);                  \
  /* QEI */                                                                   \
  qeiStart(&MODULE_HAL_QEI_LEFT_WHEEL, &moduleHalQeiConfig);                  \
  qeiStart(&MODULE_HAL_QEI_RIGHT_WHEEL, &moduleHalQeiConfig);                 \
  qeiEnable(&MODULE_HAL_QEI_LEFT_WHEEL);                                      \
  qeiEnable(&MODULE_HAL_QEI_RIGHT_WHEEL);                                     \
}

/**
 * @brief   Hook to handle IO events during SSSP startup synchronization.
 */
#define MODULE_SSP_STARTUP_OUTRO_IO_EVENT(mask, flags) {                      \
  /* ignore all events */                                                     \
  (void)mask;                                                                 \
  (void)flags;                                                                \
}

/**
 * @brief   Periphery communication interface deinitialization hook.
 */
#define MODULE_SHUTDOWN_PERIPHERY_COMM() {                                    \
  /* PWM */                                                                   \
  pwmStop(&MODULE_HAL_PWM_DRIVE);                                             \
  /* QEI */                                                                   \
  qeiDisable(&MODULE_HAL_QEI_LEFT_WHEEL);                                     \
  qeiDisable(&MODULE_HAL_QEI_RIGHT_WHEEL);                                    \
  qeiStop(&MODULE_HAL_QEI_LEFT_WHEEL);                                        \
  qeiStop(&MODULE_HAL_QEI_RIGHT_WHEEL);                                       \
  /* I2C */                                                                   \
  i2cStop(&MODULE_HAL_I2C_COMPASS);                                           \
  i2cStop(&MODULE_HAL_I2C_PROX_EEPROM_PWRMTR);                                \
  /* don't stop the serial driver so messages can still be printed */         \
}

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_a3906.h>
#include <alld_at24c01bn-sh-b.h>
#include <alld_hmc5883l.h>
#include <alld_ina219.h>
#include <alld_l3g4200d.h>
#include <alld_led.h>
#include <alld_lis331dlh.h>
#include <alld_ltc4412.h>
#include <alld_pca9544a.h>
#include <alld_tps62113.h>
#include <alld_vcnl4020.h>

/**
 * @brief   Motor driver.
 */
extern A3906Driver moduleLldMotors;

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BNDriver moduleLldEeprom;

/**
 * @brief   Compass driver.
 */
extern HMC5883LDriver moduleLldCompass;

/**
 * @brief   Power monitor (VDD) driver.
 */
extern INA219Driver moduleLldPowerMonitorVdd;

/**
 * @brief   Gyroscope driver.
 */
extern L3G4200DDriver moduleLldGyroscope;

/**
 * @brief   Status LED driver.
 */
extern LEDDriver moduleLldStatusLed;

/**
 * @brief   Accelerometer driver.
 */
extern LIS331DLHDriver moduleLldAccelerometer;

/**
 * @brief   Power path controler (charging pins) driver.
 */
extern LTC4412Driver moduleLldPowerPathController;

/**
 * @brief   I2C multiplexer driver.
 */
extern PCA9544ADriver moduleLldI2cMultiplexer;

/**
 * @brief   Step down converter (VDRIVE) driver.
 */
extern TPS62113Driver moduleLldStepDownConverterVdrive;

/**
 * @brief   Proximity sensor driver.
 */
extern VCNL4020Driver moduleLldProximity;

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <ut_alld_a3906.h>
#include <ut_alld_at24c01bn-sh-b.h>
#include <ut_alld_hmc5883l.h>
#include <ut_alld_ina219.h>
#include <ut_alld_l3g4200d.h>
#include <ut_alld_led.h>
#include <ut_alld_lis331dlh.h>
#include <ut_alld_ltc4412.h>
#include <ut_alld_pca9544a.h>
#include <ut_alld_tps62113.h>
#include <ut_alld_vcnl4020.h>

/**
 * @brief   A3906 (motor driver) unit test object.
 */
extern aos_unittest_t moduleUtAlldA3906;

/**
 * @brief   AT24C01BN-SH-B (EEPROM) unit test object.
 */
extern aos_unittest_t moduleUtAlldAt24c01bn;

/**
 * @brief   HMC5883L (compass) unit test object.
 */
extern aos_unittest_t moduleUtAlldHmc5883l;

/**
 * @brief   INA219 (power monitor) unit test object.
 */
extern aos_unittest_t moduleUtAlldIna219;

/**
 * @brief   L3G4200D (gyroscope) unit test object.
 */
extern aos_unittest_t moduleUtAlldL3g4200d;

/**
 * @brief   Status LED unit test object.
 */
extern aos_unittest_t moduleUtAlldLed;

/**
 * @brief   LIS331DLH (accelerometer) unit test object.
 */
extern aos_unittest_t moduleUtAlldLis331dlh;

/**
 * @brief   LTC4412 (power path controller) unit test object.
 */
extern aos_unittest_t moduleUtAlldLtc4412;

/**
 * @brief   PCA9544A (I2C multiplexer) unit test object.
 */
extern aos_unittest_t moduleUtAlldPca9544a;

/**
 * @brief   TPS62113 (step-down converter) unit test object.
 */
extern aos_unittest_t moduleUtAlldTps62113;

/**
 * @brief   VCNL4020 (proximity sensor) unit test object.
 */
extern aos_unittest_t moduleUtAlldVcnl4020;

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */

#endif /* _AMIROOS_MODULE_H_ */
