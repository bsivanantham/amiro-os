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

/*
 * @brief Makro to store data in the core coupled memory (ccm).
 *        Example:
 *        int compute_buffer[128] CCM_RAM;
 *
 * @note The ccm is not connected to any bus system.
 */
#define CCM_RAM                                 __attribute__((section(".ram4"), aligned(4)))

/*
 * @brief Makro to store data in the ethernet memory (eth).
 *        Example:
 *        int dma_buffer[128] ETH_RAM;
 *
 * @note The eth is a dedicated memory block with its own DMA controller.
 */
#define ETH_RAM                                 __attribute__((section(".ram2"), aligned(4)))

/*
 * @brief Makro to store data in the backup memory (bckp).
 *        Example:
 *        int backup_buffer[128] BCKP_RAM;
 *
 * @note The eth is a dedicated memory block with its own DMA controller.
 */
#define BCKP_RAM                                __attribute__((section(".ram5"), aligned(4)))

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/
#include <hal.h>

/**
 * @brief   ADC driver for reading the system voltage.
 */
#define MODULE_HAL_ADC_VSYS                     ADCD1

/**
 * @brief   Configuration for the ADC.
 */
extern ADCConversionGroup moduleHalAdcVsysConversionGroup;

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
 * @brief   I2C driver to access multiplexer, proximity sensors 1 to 4, power monitors for VIO1.8 and VIO 3.3, and fuel gauge (rear battery).
 */
#define MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR I2CD1

/**
 * @brief   Configuration for the multiplexer, proximity sensors 1 to 4, power monitors for VIO1.8 and VIO 3.3, and fuel gauge (rear battery) I2C driver.
 */
extern I2CConfig moduleHalI2cProxPm18Pm33GaugeRearConfig;

/**
 * @brief   I2C driver to access multiplexer, proximity sensors 5 to 8, power monitors for VSYS4.2, VIO 5.0 and VDD, EEPROM, touch sensor, and fuel gauge (front battery).
 */
#define MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT I2CD2

/**
 * @brief   Configuration for the multiplexer, proximity sensors 1 to 4, power monitors for VIO1.8 and VIO 3.3, and fuel gauge (rear battery) I2C driver.
 */
extern I2CConfig moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig;

/**
 * @brief   PWM driver for the buzzer.
 */
#define MODULE_HAL_PWM_BUZZER                   PWMD3

/**
 * @brief   Configuration of the PWM driver.
 */
extern PWMConfig moduleHalPwmBuzzerConfig;

/**
 * @brief   PWM channeö for the buzzer.
 */
#define MODULE_HAL_PWM_BUZZER_CHANNEL           1

/**
 * @brief   Serial driver of the programmer interface.
 */
#define MODULE_HAL_PROGIF                       SD1

/**
 * @brief   Configuration for the programmer serial interface driver.
 */
extern SerialConfig moduleHalProgIfConfig;

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/
#include <amiro-lld.h>

/**
 * @brief   Interrupt channel for the IR_INT1 and CHARGE_STAT1A signals.
 */
#define MODULE_GPIO_EXTCHANNEL_IRINT1           ((expchannel_t)0)

/**
 * @brief   Interrupt channel for the GAUGE_BATLOW1 signal.
 */
#define MODULE_GPIO_EXTCHANNEL_GAUGEBATLOW1     ((expchannel_t)1)

/**
 * @brief   Interrupt channel for the GAUGE_BATGD1 signal.
 */
#define MODULE_GPIO_EXTCHANNEL_GAUGEBATGD1      ((expchannel_t)2)

/**
 * @brief   Interrupt channel for the SYS_UART_DN signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSUARTDN        ((expchannel_t)3)

/**
 * @brief   Interrupt channel for the IR_INT2 and CHARGE_STAT2A signals.
 */
#define MODULE_GPIO_EXTCHANNEL_IRINT2           ((expchannel_t)4)

/**
 * @brief   Interrupt channel for the TOUCH_INT signal.
 */
#define MODULE_GPIO_EXTCHANNEL_TOUCHINT         ((expchannel_t)5)

/**
 * @brief   Interrupt channel for the GAUGE_BATLOW2 signal.
 */
#define MODULE_GPIO_EXTCHANNEL_GAUGEBATLOW2     ((expchannel_t)6)

/**
 * @brief   Interrupt channel for the GAUGE_BATGD2 signal.
 */
#define MODULE_GPIO_EXTCHANNEL_GAUGEBATGD2      ((expchannel_t)7)

/**
 * @brief   Interrupt channel for the PATH_DC signal.
 */
#define MODULE_GPIO_EXTCHANNEL_PATHDC           ((expchannel_t)8)

/**
 * @brief   Interrupt channel for the SYS_SPI_DIR signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSSPIDIR        ((expchannel_t)9)

/**
 * @brief   Interrupt channel for the SYS_SYNC signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSSYNC          ((expchannel_t)12)

/**
 * @brief   Interrupt channel for the SYS_PD signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSPD            ((expchannel_t)13)

/**
 * @brief   Interrupt channel for the SYS_WARMRST signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSWARMRST       ((expchannel_t)14)

/**
 * @brief   Interrupt channel for the SYS_UART_UP signal.
 */
#define MODULE_GPIO_EXTCHANNEL_SYSUARTUP        ((expchannel_t)15)

/**
 * @brief   SYS_REG_EN output signal GPIO.
 */
extern apalGpio_t moduleGpioSysRegEn;

/**
 * @brief   IR_INT1 input signal GPIO.
 */
extern apalGpio_t moduleGpioIrInt1;

/**
 * @brief   POWER_EN output signal GPIO.
 */
extern apalGpio_t moduleGpioPowerEn;

/**
 * @brief   SYS_UART_DN bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysUartDn;

/**
 * @brief   CHARGE_STAT2A input signal GPIO.
 */
extern apalGpio_t moduleGpioChargeStat2A;

/**
 * @brief   GAUGE_BATLOW2 input signal GPIO.
 */
extern apalGpio_t moduleGpioGaugeBatLow2;

/**
 * @brief   GAUGE_BATGD2 input signal GPIO.
 */
extern apalGpio_t moduleGpioGaugeBatGd2;

/**
 * @brief   LED output signal GPIO.
 */
extern apalGpio_t moduleGpioLed;

/**
 * @brief   SYS_UART_UP bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysUartUp;

/**
 * @brief   CHARGE_STAT1A input signal GPIO.
 */
extern apalGpio_t moduleGpioChargeStat1A;

/**
 * @brief   GAUGE_BATLOW1 input signal GPIO.
 */
extern apalGpio_t moduleGpioGaugeBatLow1;

/**
 * @brief   GAUGE_BATGD1 input signal GPIO.
 */
extern apalGpio_t moduleGpioGaugeBatGd1;

/**
 * @brief   CHARG_EN1 output signal GPIO.
 */
extern apalGpio_t moduleGpioChargeEn1;

/**
 * @brief   IR_INT2 input signal GPIO.
 */
extern apalGpio_t moduleGpioIrInt2;

/**
 * @brief   TOUCH_INT input signal GPIO.
 */
extern apalGpio_t moduleGpioTouchInt;

/**
 * @brief   SYS_DONE input signal GPIO.
 */
extern apalGpio_t moduleGpioSysDone;

/**
 * @brief   SYS_PROG output signal GPIO.
 */
extern apalGpio_t moduleGpioSysProg;

/**
 * @brief   PATH_DC input signal GPIO.
 */
extern apalGpio_t moduleGpioPathDc;

/**
 * @brief   SYS_SPI_DIR bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysSpiDir;

/**
 * @brief   SYS_SYNC bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysSync;

/**
 * @brief   SYS_PD bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysPd;

/**
 * @brief   SYS_WARMRST bidirectional signal GPIO.
 */
extern apalGpio_t moduleGpioSysWarmrst;

/**
 * @brief   BT_RST output signal GPIO.
 */
extern apalGpio_t moduleGpioBtRst;

/**
 * @brief   CHARGE_EN2 output signal GPIO.
 */
extern apalGpio_t moduleGpioChargeEn2;

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

/**
 * @brief   Event flag to be set on a IR_INT1 / CHARGE_STAT1A interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_IRINT1           ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_IRINT1)

/**
 * @brief   Event flag to be set on a GAUGE_BATLOW1 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_GAUGEBATLOW1     ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_GAUGEBATLOW1)

/**
 * @brief   Event flag to be set on a GAUGE_BATGD1 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_GAUGEBATGD1      ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_GAUGEBATGD1)

/**
 * @brief   Event flag to be set on a SYS_UART_DN interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSUARTDN        ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_SYSUARTDN)

/**
 * @brief   Event flag to be set on a IR_INT2 / CHARGE_STAT2A interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_IRINT2           ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_IRINT2)

/**
 * @brief   Event flag to be set on a TOUCH_INT interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_TOUCHINT         ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_TOUCHINT)

/**
 * @brief   Event flag to be set on a GAUGE_BATLOW2 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_GAUGEBATLOW2     ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_GAUGEBATLOW2)

/**
 * @brief   Event flag to be set on a GAUGE_BATGD2 interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_GAUGEBATGD2      ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_GAUGEBATGD2)

/**
 * @brief   Event flag to be set on a PATH_DC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_PATHDC           ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_PATHDC)

/**
 * @brief   Event flag to be set on a SYS_SPI_DIR interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSSPIDIR        ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_SYSSPIDIR)

/**
 * @brief   Event flag to be set on a SYS_SYNC interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSSYNC          ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_SYSSYNC)

/**
 * @brief   Event flag to be set on a SYS_PD interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSPD            ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_SYSPD)

/**
 * @brief   Event flag to be set on a SYS_WARMRST interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSWARMRST       ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_SYSWARMRST)

/**
 * @brief   Event flag to be set on a SYS_UART_UP interrupt.
 */
#define MODULE_OS_IOEVENTFLAGS_SYSUARTUP        ((eventflags_t)1 << MODULE_GPIO_EXTCHANNEL_SYSUARTUP)

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
 * @brief   Unit test initialization hook.
 */
#define MODULE_INIT_TESTS() {                                                 \
  /* add unit-test shell commands */                                          \
  aosShellAddCommand(aos.shell, &moduleUtAdcVsys.shellcmd);                   \
  aosShellAddCommand(aos.shell, &moduleUtAlldAt24c01bn.shellcmd);             \
  aosShellAddCommand(aos.shell, &moduleUtAlldBq24103a.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldBq27500.shellcmd);               \
  aosShellAddCommand(aos.shell, &moduleUtAlldBq27500Bq24103a.shellcmd);       \
  aosShellAddCommand(aos.shell, &moduleUtAlldIna219.shellcmd);                \
  aosShellAddCommand(aos.shell, &moduleUtAlldMpr121.shellcmd);                \
  aosShellAddCommand(aos.shell, &moduleUtAlldPca9544a.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldPklcs1212e4001.shellcmd);        \
  aosShellAddCommand(aos.shell, &moduleUtAlldLed.shellcmd);                   \
  aosShellAddCommand(aos.shell, &moduleUtAlldTps62113.shellcmd);              \
  aosShellAddCommand(aos.shell, &moduleUtAlldTps62113Ina219.shellcmd);        \
  aosShellAddCommand(aos.shell, &moduleUtAlldVcnl4020.shellcmd);              \
}

/**
 * @brief   Periphery communication interfaces initialization hook.
 */
#define MODULE_INIT_PERIPHERY_COMM() {                                        \
  /* serial driver */                                                         \
  sdStart(&MODULE_HAL_PROGIF, &moduleHalProgIfConfig);                        \
  /* I2C */                                                                   \
  moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed = (PCA9544A_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed) ? PCA9544A_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed;  \
  moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed = (VCNL4020_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed) ? VCNL4020_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed;  \
  moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed;  \
  moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed = (BQ27500_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed) ? BQ27500_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed;  \
  moduleHalI2cProxPm18Pm33GaugeRearConfig.duty_cycle = (moduleHalI2cProxPm18Pm33GaugeRearConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;  \
  i2cStart(&MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR, &moduleHalI2cProxPm18Pm33GaugeRearConfig); \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed = (PCA9544A_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed) ? PCA9544A_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed; \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed = (VCNL4020_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed) ? VCNL4020_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed; \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed = (INA219_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed) ? INA219_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed; \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed = (AT24C01BN_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed) ? AT24C01BN_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed; \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed = (MPR121_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed) ? MPR121_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed; \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed = (BQ27500_LLD_I2C_MAXFREQUENCY < moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed) ? BQ27500_LLD_I2C_MAXFREQUENCY : moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed; \
  moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.duty_cycle = (moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig.clock_speed <= 100000) ? STD_DUTY_CYCLE : FAST_DUTY_CYCLE_2;  \
  i2cStart(&MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT, &moduleHalI2cProxPm42Pm50PmVddEepromTouchGaugeFrontConfig);  \
  /* ADC */                                                                   \
  adcStart(&MODULE_HAL_ADC_VSYS, NULL);                                       \
  /* PWM */                                                                   \
  pwmStart(&MODULE_HAL_PWM_BUZZER, &moduleHalPwmBuzzerConfig);                \
  moduleHalPwmBuzzerConfig.frequency = MODULE_HAL_PWM_BUZZER.clock;           \
  pwmStop(&MODULE_HAL_PWM_BUZZER);                                            \
  moduleHalPwmBuzzerConfig.period = moduleHalPwmBuzzerConfig.frequency / PKLCS1212E4001_LLD_FREQUENCY_SPEC; \
  pwmStart(&MODULE_HAL_PWM_BUZZER, &moduleHalPwmBuzzerConfig);                \
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
  pwmStop(&MODULE_HAL_PWM_BUZZER);                                            \
  /* ADC */                                                                   \
  adcStop(&MODULE_HAL_ADC_VSYS);                                              \
  /* I2C */                                                                   \
  i2cStop(&MODULE_HAL_I2C_PROX_PM18_PM33_GAUGEREAR);                          \
  i2cStop(&MODULE_HAL_I2C_PROX_PM42_PM50_PMVDD_EEPROM_TOUCH_GAUGEFRONT);      \
  /* don't stop the serial driver so messages can still be printed */         \
}

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/
#include <alld_at24c01bn-sh-b.h>
#include <alld_bq24103a.h>
#include <alld_bq27500.h>
#include <alld_ina219.h>
#include <alld_led.h>
#include <alld_mpr121.h>
#include <alld_pca9544a.h>
#include <alld_pklcs1212e4001.h>
#include <alld_tps62113.h>
#include <alld_vcnl4020.h>

/**
 * @brief   EEPROM driver.
 */
extern AT24C01BNDriver moduleLldEeprom;

/**
 * @brief   Battery charger (front battery) driver.
 */
extern BQ24103ADriver moduleLldBatteryChargerFront;

/**
 * @brief   Battery charger (rear battery) driver.
 */
extern BQ24103ADriver moduleLldBatteryChargerRear;

/**
 * @brief   Fuel gauge (front battery) driver.
 */
extern BQ27500Driver moduleLldFuelGaugeFront;

/**
 * @brief   Fuel gauge (rear battery) driver.
 */
extern BQ27500Driver moduleLldFuelGaugeRear;

/**
 * @brief   Power monitor (VDD) driver.
 */
extern INA219Driver moduleLldPowerMonitorVdd;

/**
 * @brief   Power monitor (VIO 1.8) driver.
 */
extern INA219Driver moduleLldPowerMonitorVio18;

/**
 * @brief   Power monitor (VIO 3.3) driver.
 */
extern INA219Driver moduleLldPowerMonitorVio33;

/**
 * @brief   Power monitor (VSYS 4.2) driver.
 */
extern INA219Driver moduleLldPowerMonitorVsys42;

/**
 * @brief   Power monitor (VIO 5.0) driver.
 */
extern INA219Driver moduleLldPowerMonitorVio50;

/**
 * @brief   Status LED driver.
 */
extern LEDDriver moduleLldStatusLed;

/**
 * @brief   Touch sensor driver.
 */
extern MPR121Driver moduleLldTouch;

/**
 * @brief   I2C multiplexer (I2C 1) driver.
 */
extern PCA9544ADriver moduleLldI2cMultiplexer1;

/**
 * @brief   I2C multiplexer (I2C 2) driver.
 */
extern PCA9544ADriver moduleLldI2cMultiplexer2;

/**
 * @brief   Step down converter driver.
 * @note    Although there multiple TPS62113, those are completely identical from driver few (share the same signals).
 */
extern TPS62113Driver moduleLldStepDownConverter;

/**
 * @brief   Proximity sensor (I2C 1) driver.
 */
extern VCNL4020Driver moduleLldProximity1;

/**
 * @brief   Proximity sensor (I2C 2) driver.
 */
extern VCNL4020Driver moduleLldProximity2;

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <ut_lld_adc.h>
#include <ut_alld_at24c01bn-sh-b.h>
#include <ut_alld_bq24103a.h>
#include <ut_alld_bq27500.h>
#include <ut_alld_bq27500_bq24103a.h>
#include <ut_alld_ina219.h>
#include <ut_alld_led.h>
#include <ut_alld_mpr121.h>
#include <ut_alld_pca9544a.h>
#include <ut_alld_pklcs1212e4001.h>
#include <ut_alld_tps62113.h>
#include <ut_alld_tps62113_ina219.h>
#include <ut_alld_vcnl4020.h>

/**
 * @brief   ADC unit test object.
 */
extern aos_unittest_t moduleUtAdcVsys;

/**
 * @brief   AT24C01BN-SH-B (EEPROM) unit test object.
 */
extern aos_unittest_t moduleUtAlldAt24c01bn;

/**
 * @brief   BQ24103A (battery charger) unit test object.
 */
extern aos_unittest_t moduleUtAlldBq24103a;

/**
 * @brief   BQ27500 (fuel gauge) unit test object.
 */
extern aos_unittest_t moduleUtAlldBq27500;

/**
 * @brief   BQ27500 (fuela gauge) in combination with BQ24103A (battery charger) unit test object.
 */
extern aos_unittest_t moduleUtAlldBq27500Bq24103a;

/**
 * @brief   INA219 (power monitor) unit test object.
 */
extern aos_unittest_t moduleUtAlldIna219;

/**
 * @brief   Status LED unit test object.
 */
extern aos_unittest_t moduleUtAlldLed;

/**
 * @brief   MPR121 (touch sensor) unit test object.
 */
extern aos_unittest_t moduleUtAlldMpr121;

/**
 * @brief   PCA9544A (I2C multiplexer) unit test object.
 */
extern aos_unittest_t moduleUtAlldPca9544a;

/**
 * @brief   PKLCS1212E4001 (buzzer) unit test object.
 */
extern aos_unittest_t moduleUtAlldPklcs1212e4001;

/**
 * @brief   TPS62113 (step-down converter) unit test object.
 */
extern aos_unittest_t moduleUtAlldTps62113;

/**
 * @brief   TPS62113 (step-sown converter) in combination with INA219 (power monitor) unit test object.
 */
extern aos_unittest_t moduleUtAlldTps62113Ina219;

/**
 * @brief   VCNL4020 (proximity sensor) unit test object.
 */
extern aos_unittest_t moduleUtAlldVcnl4020;

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */

#endif /* _AMIROOS_MODULE_H_ */
