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

#include "module.h"

/*===========================================================================*/
/**
 * @name Module specific functions
 * @{
 */
/*===========================================================================*/
#include <amiroos.h>

/**
 * @brief   Interrupt service routine callback for I/O interrupt signals.
 *
 * @param   extp      EXT driver to handle the ISR.
 * @param   channel   Channel on which the interrupt was encountered.
 */
static void _moduleIsrCallback(EXTDriver* extp, expchannel_t channel) {
  (void)extp;

  chSysLockFromISR();
  chEvtBroadcastFlagsI(&aos.events.io.source, (1 << channel));
  chSysUnlockFromISR();

  return;
}

/** @} */

/*===========================================================================*/
/**
 * @name ChibiOS/HAL configuration
 * @{
 */
/*===========================================================================*/

CANConfig moduleHalCanConfig = {
  /* mcr  */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /* btr  */ CAN_BTR_SJW(1) | CAN_BTR_TS2(2) | CAN_BTR_TS1(13) | CAN_BTR_BRP(1),
};

EXTConfig moduleHalExtConfig = {
  /* channel configrations */ {
    /* channel  0 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  1 */ { // SYS_INT_N/SYS_SYNC_N: automatic interrupt on event
      /* mode     */ EXT_MODE_GPIOC | EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  2 */ { // SYS_WARMRST_N: automatic interrupt when activated
      /* mode     */ EXT_MODE_GPIOD | EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  3 */ { // PATH_DCSTAT: must be enabled explicitely when charging is in progress to detect unexpected voltage drop
      /* mode     */ EXT_MODE_GPIOC | EXT_CH_MODE_FALLING_EDGE,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  4 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  5 */ { // COMPASS_DRDY: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | APAL2CH_EDGE(HMC5883L_LLD_INT_EDGE),
      /* callback */ _moduleIsrCallback,
    },
    /* channel  6 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  7 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  8 */ { // SYS_PD_N: automatic interrupt when activated
      /* mode     */ EXT_MODE_GPIOC | EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  9 */ { // SYS_REG_EN: automatic interrupt when activated
      /* mode     */ EXT_MODE_GPIOC | EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback,
    },
    /* channel 10 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel 11 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel 12 */ { // IR_INT: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | APAL2CH_EDGE(VCNL4020_LLD_INT_EDGE),
      /* callback */ _moduleIsrCallback,
    },
    /* channel 13 */ { // GYRO_DRDY: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | APAL2CH_EDGE(L3G4200D_LLD_INT_EDGE),
      /* callback */ _moduleIsrCallback,
    },
    /* channel 14 */ { // SYS_UART_UP: automatic interrupt on event
      /* mode     */ EXT_MODE_GPIOB | EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback,
    },
    /* channel 15 */ { // ACCEL_INT_N: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | APAL2CH_EDGE(LIS331DLH_LLD_INT_EDGE),
      /* callback */ _moduleIsrCallback,
    },
    /* channel 16 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel 17 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel 18 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
  },
};

I2CConfig moduleHalI2cCompassConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000,
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

I2CConfig moduleHalI2cProxEepromPwrmtrConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000,
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

PWMConfig moduleHalPwmDriveConfig = {
  /* frequency              */ 7200000,
  /* period                 */ 360,
  /* callback               */ NULL,
  /* channel configurations */ {
    /* channel 0              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 1              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 2              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
    /* channel 3              */ {
      /* mode                   */ PWM_OUTPUT_ACTIVE_HIGH,
      /* callback               */ NULL
    },
  },
  /* TIM CR2 register       */ 0,
#if STM32_PWM_USE_ADVANCED
  /* TIM BDTR register      */ 0,
#endif
  /* TIM DIER register      */ 0
};

QEIConfig moduleHalQeiConfig = {
  /* mode           */ QEI_COUNT_BOTH,
  /* channel config */ {
    /* channel 0 */ {
      /* input mode */ QEI_INPUT_NONINVERTED,
    },
    /* channel 1 */ {
      /* input mode */ QEI_INPUT_NONINVERTED,
    },
  },
  /* encoder range  */  0x10000u,
};

SerialConfig moduleHalProgIfConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

SPIConfig moduleHalSpiAccelerometerConfig = {
  /* callback function pointer    */ NULL,
  /* chip select line port        */ GPIOC,
  /* chip select line pad number  */ GPIOC_ACCEL_SS_N,
  /* CR1                          */ SPI_CR1_BR_0,
  /* CR2                          */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,
};

SPIConfig moduleHalSpiGyroscopeConfig = {
  /* callback function pointer    */ NULL,
  /* chip select line port        */ GPIOC,
  /* chip select line pad number  */ GPIOC_GYRO_SS_N,
  /* CR1                          */ SPI_CR1_BR_0,
  /* CR2                          */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,
};

/** @} */

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

apalGpio_t moduleGpioLed = {
  /* port */ GPIOA,
  /* pad  */ GPIOA_LED,
};

apalGpio_t moduleGpioPowerEn = {
  /* port */GPIOB,
  /* pad  */  GPIOB_POWER_EN,
};

apalGpio_t moduleGpioCompassDrdy = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_COMPASS_DRDY,
};

 apalGpio_t moduleGpioIrInt = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_IR_INT,
};

apalGpio_t moduleGpioGyroDrdy = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_GYRO_DRDY,
};

apalGpio_t moduleGpioSysUartUp = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_SYS_UART_UP,
};

apalGpio_t moduleGpioAccelInt = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_ACCEL_INT_N,
};

apalGpio_t moduleGpioSysSync = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_INT_N,
};

apalGpio_t moduleGpioPathDcStat = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_PATH_DCSTAT,
};

apalGpio_t moduleGpioPathDcEn = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_PATH_DCEN,
};

apalGpio_t moduleGpioSysPd = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_PD_N,
};

apalGpio_t moduleGpioSysRegEn = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_REG_EN,
};

apalGpio_t moduleGpioSysWarmrst = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_SYS_WARMRST_N,
};

/** @} */

/*===========================================================================*/
/**
 * @name AMiRo-OS core configurations
 * @{
 */
/*===========================================================================*/

apalControlGpio_t moduleSsspPd = {
  /* GPIO */ &moduleGpioSysPd,
  /* meta */ {
    /* active state */ APAL_GPIO_ACTIVE_LOW,
    /* edge         */ APAL_GPIO_EDGE_FALLING,
    /* direction    */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
  },
};

apalControlGpio_t moduleSsspSync = {
  /* GPIO */ &moduleGpioSysSync,
  /* meta */ {
    /* active state */ APAL_GPIO_ACTIVE_LOW,
    /* edge         */ APAL_GPIO_EDGE_FALLING,
    /* direction    */ APAL_GPIO_DIRECTION_BIDIRECTIONAL,
  },
};

const char* moduleShellPrompt = "DiWheelDrive";

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

A3906Driver moduleLldMotors = {
  /* power enable GPIO  */ {
    /* GPIO */& moduleGpioPowerEn,
    /* meta */ {
      /* active state */ APAL_GPIO_ACTIVE_HIGH,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
};

AT24C01BNDriver moduleLldEeprom = {
  /* I2C driver   */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
  /* I²C address  */ AT24C01BN_LLD_I2C_ADDR_FIXED,
};

HMC5883LDriver moduleLldCompass = {
  /* I²C Driver */ &MODULE_HAL_I2C_COMPASS,
};

INA219Driver moduleLldPowerMonitorVdd = {
  /* I2C Driver       */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
  /* I²C address      */ INA219_LLD_I2C_ADDR_FIXED,
  /* current LSB (uA) */ 0x00u,
  /* configuration    */ NULL,
};

L3G4200DDriver moduleLldGyroscope = {
  /* SPI Driver */ &MODULE_HAL_SPI_MOTION,
};

LEDDriver moduleLldStatusLed = {
  /* LED enable Gpio */ {
    /* GPIO       */ &moduleGpioLed,
    /* GPIO meta  */ {
      /* active state */ APAL_GPIO_ACTIVE_LOW,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
};

LIS331DLHDriver moduleLldAccelerometer = {
  /* SPI Driver */ &MODULE_HAL_SPI_MOTION,
};

LTC4412Driver moduleLldPowerPathController = {
  /* Control GPIO */ {
    /* GPIO       */ &moduleGpioPathDcEn,
    /* GPIO meta  */ {
      /* active state */ APAL_GPIO_ACTIVE_HIGH,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
  /* Status GPIO */ {
    /* GPIO       */ &moduleGpioPathDcStat,
    /* GPIO meta  */ {
      /* active state */ APAL_GPIO_ACTIVE_HIGH,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_INPUT,
    },
  },
};

PCA9544ADriver moduleLldI2cMultiplexer = {
  /* I²C driver   */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
  /* I²C address  */ PCA9544A_LLD_I2C_ADDR_FIXED | PCA9544A_LLD_I2C_ADDR_A0 | PCA9544A_LLD_I2C_ADDR_A1 | PCA9544A_LLD_I2C_ADDR_A2,
};

TPS62113Driver moduleLldStepDownConverterVdrive = {
  /* Power enable Gpio */ {
    /* GPIO       */ &moduleGpioPowerEn,
    /* GPIO meta  */ {
      /* 'power on' state */ APAL_GPIO_ACTIVE_HIGH,
      /* edge             */ APAL_GPIO_EDGE_NONE,
      /* direction        */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
};

VCNL4020Driver moduleLldProximity = {
  /* I²C Driver */ &MODULE_HAL_I2C_PROX_EEPROM_PWRMTR,
};

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <string.h>
#include <chprintf.h>

/* A3906 (motor driver) */
static int _utShellCmdCb_AlldA3906(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldA3906, NULL);
  return AOS_OK;
}
static ut_a3906data_t _utA3906Data = {
  /* driver           */ &moduleLldMotors,
  /* PWM information  */ {
    /* driver   */ &MODULE_HAL_PWM_DRIVE,
    /* channels */ {
      /* left wheel forward   */ MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_FORWARD,
      /* left wheel backward  */ MODULE_HAL_PWM_DRIVE_CHANNEL_LEFT_BACKWARD,
      /* right wheel forward  */ MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_FORWARD,
      /* right wheel backward */ MODULE_HAL_PWM_DRIVE_CHANNEL_RIGHT_BACKWARD,
    },
  },
  /* QEI information  */ {
    /* left wheel               */ &MODULE_HAL_QEI_LEFT_WHEEL,
    /* right wheel              */ &MODULE_HAL_QEI_RIGHT_WHEEL,
    /* increment per revolution */ MODULE_HAL_QEI_INCREMENTS_PER_REVOLUTION,
  },
  /* Wheel diameter   */ 0.05571f,
  /* timeout          */ 10* MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldA3906  = {
  /* name           */ "A3906",
  /* info           */ "motor driver",
  /* test function  */ utAlldA3906Func,
  /* shell command  */ {
    /* name     */ "unittest:MotorDriver",
    /* callback */ _utShellCmdCb_AlldA3906,
    /* next     */ NULL,
  },
  /* data           */ &_utA3906Data,
};

/* AT24C01BN (EEPROM) */
static int _utShellCmdCb_AlldAt24c01bn(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldAt24c01bn, NULL);
  return AOS_OK;
}
static ut_at24c01bndata_t _utAt24c01bnData = {
  /* driver   */ &moduleLldEeprom,
  /* timeout  */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldAt24c01bn = {
  /* name           */ "AT24C01BN-SH-B",
  /* info           */ "1kbit EEPROM",
  /* test function  */ utAlldAt24c01bnFunc,
  /* shell command  */ {
    /* name     */ "unittest:EEPROM",
    /* callback */ _utShellCmdCb_AlldAt24c01bn,
    /* next     */ NULL,
  },
  /* data           */ &_utAt24c01bnData,
};

/* HMC5883L (compass) */
static int _utShellCmdCb_AlldHmc5883l(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  extChannelEnable(&MODULE_HAL_EXT, MODULE_GPIO_EXTCHANNEL_COMPASSDRDY);
  aosUtRun(stream, &moduleUtAlldHmc5883l, NULL);
  return AOS_OK;
}
static ut_hmc5883ldata_t _utHmc5883lData = {
  /* HMC driver   */ &moduleLldCompass,
  /* event source */ &aos.events.io.source,
  /* event flags  */ (1 << MODULE_GPIO_EXTCHANNEL_COMPASSDRDY),
  /* timeout      */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldHmc5883l = {
  /* name           */ "HMC5883L",
  /* info           */ "compass",
  /* test function  */ utAlldHmc5883lFunc,
  /* shell command  */ {
    /* name     */ "unittest:Compass",
    /* callback */ _utShellCmdCb_AlldHmc5883l,
    /* next     */ NULL,
  },
  /* data           */ &_utHmc5883lData,
};

/* INA219 (power monitor) */
static int _utShellCmdCb_AlldIna219(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldIna219, "VDD (3.3V)");
  return AOS_OK;
}
static ut_ina219data_t _utIna219Data = {
  /* driver           */ &moduleLldPowerMonitorVdd,
  /* expected voltage */ 3.3f,
  /* tolerance        */ 0.05f,
  /* timeout */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldIna219 = {
  /* name           */ "INA219",
  /* info           */ "power monitor",
  /* test function  */ utAlldIna219Func,
  /* shell command  */ {
    /* name     */ "unittest:PowerMonitor",
    /* callback */ _utShellCmdCb_AlldIna219,
    /* next     */ NULL,
  },
  /* data           */ &_utIna219Data,
};

/* L3G4200D (gyroscope) */
static int _utShellCmdCb_AlldL3g4200d(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  extChannelEnable(&MODULE_HAL_EXT, MODULE_GPIO_EXTCHANNEL_GYRODRDY);
  spiStart(((ut_l3g4200ddata_t*)moduleUtAlldL3g4200d.data)->l3gd->spid, ((ut_l3g4200ddata_t*)moduleUtAlldL3g4200d.data)->spiconf);
  aosUtRun(stream, &moduleUtAlldL3g4200d, NULL);
  spiStop(((ut_l3g4200ddata_t*)moduleUtAlldL3g4200d.data)->l3gd->spid);
  return AOS_OK;
}
static ut_l3g4200ddata_t _utL3g4200dData = {
  /* driver            */ &moduleLldGyroscope,
  /* SPI configuration */ &moduleHalSpiGyroscopeConfig,
  /* event source */ &aos.events.io.source,
  /* event flags  */ (1 << MODULE_GPIO_EXTCHANNEL_GYRODRDY),
};
aos_unittest_t moduleUtAlldL3g4200d = {
  /* name           */ "L3G4200D",
  /* info           */ "Gyroscope",
  /* test function  */ utAlldL3g4200dFunc,
  /* shell command  */ {
    /* name     */ "unittest:Gyroscope",
    /* callback */ _utShellCmdCb_AlldL3g4200d,
    /* next     */ NULL,
  },
  /* data           */ &_utL3g4200dData,
};

/* Status LED */
static int _utShellCmdCb_AlldLed(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldLed, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldLed = {
  /* name           */ "LED",
  /* info           */ NULL,
  /* test function  */ utAlldLedFunc,
  /* shell command  */ {
    /* name     */ "unittest:StatusLED",
    /* callback */ _utShellCmdCb_AlldLed,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldStatusLed,
};

/* LIS331DLH (accelerometer) */
static int _utShellCmdCb_AlldLis331dlh(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  extChannelEnable(&MODULE_HAL_EXT, MODULE_GPIO_EXTCHANNEL_ACCELINT);
  spiStart(((ut_lis331dlhdata_t*)moduleUtAlldLis331dlh.data)->lisd->spid, ((ut_lis331dlhdata_t*)moduleUtAlldLis331dlh.data)->spiconf);
  aosUtRun(stream, &moduleUtAlldLis331dlh, NULL);
  spiStop(((ut_lis331dlhdata_t*)moduleUtAlldLis331dlh.data)->lisd->spid);
  return AOS_OK;
}
static ut_lis331dlhdata_t _utLis331dlhData = {
  /* driver            */ &moduleLldAccelerometer,
  /* SPI configuration */ &moduleHalSpiAccelerometerConfig,
  /* event source */ &aos.events.io.source,
  /* event flags  */ (1 << MODULE_GPIO_EXTCHANNEL_ACCELINT),
};
aos_unittest_t moduleUtAlldLis331dlh = {
  /* name           */ "LIS331DLH",
  /* info           */ "Accelerometer",
  /* test function  */ utAlldLis331dlhFunc,
  /* shell command  */ {
    /* name     */ "unittest:Accelerometer",
    /* callback */ _utShellCmdCb_AlldLis331dlh,
    /* next     */ NULL,
  },
  /* data           */ &_utLis331dlhData,
};

/* LTC4412 (power path controller) */
static int _utShellCmdCb_AlldLtc4412(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldLtc4412, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldLtc4412 = {
  /* name           */ "LTC4412",
  /* info           */ "Power path controller",
  /* test function  */ utAlldLtc4412Func,
  /* shell command  */ {
    /* name     */ "unittest:PowerPathController",
    /* callback */ _utShellCmdCb_AlldLtc4412,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldPowerPathController,
};

/* PCA9544A (I2C multiplexer) */
static int _utShellCmdCb_AlldPca9544a(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldPca9544a, NULL);
  return AOS_OK;
}
static ut_pca9544adata_t _utPca9544aData = {
  /* driver  */ &moduleLldI2cMultiplexer,
  /* timeout */ MICROSECONDS_PER_SECOND,
};
aos_unittest_t moduleUtAlldPca9544a = {
  /* name           */ "PCA9544A",
  /* info           */ "I2C multiplexer",
  /* test function  */ utAlldPca9544aFunc,
  /* shell command  */ {
    /* name     */ "unittest:I2CMultiplexer",
    /* callback */ _utShellCmdCb_AlldPca9544a,
    /* next     */ NULL,
  },
  /* data           */ &_utPca9544aData,
};

/* TPS62113 (step-down converter) */
static int _utShellCmdCb_AlldTps62113(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldTps62113, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldTps62113 = {
  /* name           */ "TPS62113",
  /* info           */ "Step down converter",
  /* test function  */ utAlldTps62113Func,
  /* shell command  */ {
    /* name     */ "unittest:StepDownConverter",
    /* callback */ _utShellCmdCb_AlldTps62113,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldStepDownConverterVdrive,
};

/* VCNL4020 (proximity sensor) */
static void _utAlldVcnl4020_disableInterrupt(VCNL4020Driver* vcnl)
{
  uint8_t intstatus;
  vcnl4020_lld_writereg(vcnl, VCNL4020_LLD_REGADDR_INTCTRL, 0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
  vcnl4020_lld_readreg(vcnl, VCNL4020_LLD_REGADDR_INTSTATUS, &intstatus, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
  if (intstatus) {
    vcnl4020_lld_writereg(vcnl, VCNL4020_LLD_REGADDR_INTSTATUS, intstatus, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
  }
  return;
}
static int _utShellCmdCb_AlldVcnl4020(BaseSequentialStream* stream, int argc, char* argv[])
{
  enum {
    UNKNOWN,
    FL, FR, WL, WR,
  } sensor = UNKNOWN;
  // evaluate arguments
  if (argc == 2) {
    if (strcmp(argv[1], "--frontleft") == 0 || strcmp(argv[1], "-fl") == 0) {
      sensor = FL;
    } else if (strcmp(argv[1], "--frontright") == 0 || strcmp(argv[1], "-fr") == 0) {
      sensor = FR;
    } else if (strcmp(argv[1], "--wheelleft") == 0 || strcmp(argv[1], "-wl") == 0) {
      sensor = WL;
    } else if (strcmp(argv[1], "--wheelright") == 0 || strcmp(argv[1], "-wr") == 0) {
      sensor = WR;
    }
  }
  if (sensor != UNKNOWN) {
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH1, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH2, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH3, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
    _utAlldVcnl4020_disableInterrupt(((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->vcnld);
    extChannelEnable(&MODULE_HAL_EXT, MODULE_GPIO_EXTCHANNEL_IRINT);
    switch (sensor) {
      case FL:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH3, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "front left snesor");
        break;
      case FR:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH0, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "front right sensor");
        break;
      case WL:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH2, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "left wheel sensor");
        break;
      case WR:
        pca9544a_lld_setchannel(&moduleLldI2cMultiplexer, PCA9544A_LLD_CH1, ((ut_vcnl4020data_t*)moduleUtAlldVcnl4020.data)->timeout);
        aosUtRun(stream, &moduleUtAlldVcnl4020, "right wheel sensor");
        break;
      default:
        break;
    }
    return AOS_OK;
  }
  // print help
  chprintf(stream, "Usage: %s OPTION\n", argv[0]);
  chprintf(stream, "Options:\n");
  chprintf(stream, "  --frontleft, -fl\n");
  chprintf(stream, "    Test front left proximity sensor.\n");
  chprintf(stream, "  --frontrigt, -fr\n");
  chprintf(stream, "    Test front right proximity sensor.\n");
  chprintf(stream, "  --wheelleft, -wl\n");
  chprintf(stream, "    Test left wheel proximity sensor.\n");
  chprintf(stream, "  --wheelright, -wr\n");
  chprintf(stream, "    Test right wheel proximity sensor.\n");
  return AOS_INVALID_ARGUMENTS;
}
static ut_vcnl4020data_t _utVcnl4020Data = {
  /* driver       */ &moduleLldProximity,
  /* timeout      */ MICROSECONDS_PER_SECOND,
  /* event source */ &aos.events.io.source,
  /* event flags  */ (1 << MODULE_GPIO_EXTCHANNEL_IRINT),
};
aos_unittest_t moduleUtAlldVcnl4020 = {
  /* name           */ "VCNL4020",
  /* info           */ "proximity sensor",
  /* test function  */ utAlldVcnl4020Func,
  /* shell command  */ {
    /* name     */ "unittest:Proximity",
    /* callback */ _utShellCmdCb_AlldVcnl4020,
    /* next     */ NULL,
  },
  /* data           */ &_utVcnl4020Data,
};

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */
