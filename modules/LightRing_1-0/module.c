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
    /* channel  1 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  2 */ { // SYS_SYNC_N: automatic interrupt on event
      /* mode     */ EXT_MODE_GPIOD | EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback
    },
    /* channel  3 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  4 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  5 */ { // LASER_OC_N: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | EXT_CH_MODE_FALLING_EDGE,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  6 */ { // SYS_UART_DN: automatic interrupt on event
      /* mode     */ EXT_MODE_GPIOB | EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  7 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel  8 */ { // WL_GDO0: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | EXT_CH_MODE_BOTH_EDGES,
      /* callback */ _moduleIsrCallback,
    },
    /* channel  9 */ { // WL_GDO2: must be enabled explicitely
      /* mode     */ EXT_MODE_GPIOB | EXT_CH_MODE_BOTH_EDGES,
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
    /* channel 12 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel 13 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
    },
    /* channel 14 */ { // SYS_PD_N: automatic interrupt when activated
      /* mode     */ EXT_MODE_GPIOC | EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART,
      /* callback */ _moduleIsrCallback
    },
    /* channel 15 */ {
      /* mode     */ EXT_CH_MODE_DISABLED,
      /* callback */ NULL,
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
  }
};

I2CConfig moduleHalI2cEepromConfig = {
  /* I²C mode   */ OPMODE_I2C,
  /* frequency  */ 400000, // TODO: replace with some macro (-> ChibiOS/HAL)
  /* duty cycle */ FAST_DUTY_CYCLE_2,
};

SerialConfig moduleHalProgIfConfig = {
  /* bit rate */ 115200,
  /* CR1      */ 0,
  /* CR1      */ 0,
  /* CR1      */ 0,
};

SPIConfig moduleHalSpiLightConfig = {
  /* callback function pointer   */ NULL,
  /* chip select line port       */ GPIOC,
  /* chip select line pad number */ GPIOC_LIGHT_XLAT,
  /* CR1                         */ SPI_CR1_BR_0 | SPI_CR1_BR_1,
  /* CR2                         */ SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN,
};

/*===========================================================================*/
/**
 * @name GPIO definitions
 * @{
 */
/*===========================================================================*/

apalGpio_t moduleGpioLightBlank = {
  /* port */ GPIOA,
  /* pad  */ GPIOA_LIGHT_BLANK,
};

apalGpio_t moduleGpioLaserEn = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_LASER_EN,
};

apalGpio_t moduleGpioLaserOc = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_LASER_OC_N,
};

apalGpio_t moduleGpioSysUartDn = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_SYS_UART_DN,
};

apalGpio_t moduleGpioWlGdo2 = {
  /* port */ GPIOB,
  /* pad  */ GPIOB_WL_GDO2,
};

apalGpio_t moduleGpioWlGdo0= {
  /* port */ GPIOB,
  /* pad  */ GPIOB_WL_GDO0,
};

apalGpio_t moduleGpioLightXlat = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_LIGHT_XLAT,
};

apalGpio_t moduleGpioSysPd = {
  /* port */ GPIOC,
  /* pad  */ GPIOC_SYS_PD_N,
};

apalGpio_t moduleGpioSysSync = {
  /* port */ GPIOD,
  /* pad  */ GPIOD_SYS_INT_N,
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

const char* moduleShellPrompt = "LightRing";

/** @} */

/*===========================================================================*/
/**
 * @name Low-level drivers
 * @{
 */
/*===========================================================================*/

AT24C01BNDriver moduleLldEeprom = {
  /* I2C driver   */ &MODULE_HAL_I2C_EEPROM,
  /* I2C address  */ 0x00u,
};

TLC5947Driver moduleLldLedPwm = {
  /* SPI driver         */ &MODULE_HAL_SPI_LIGHT,
  /* BLANK signal GPIO  */ {
    /* GPIO */ &moduleGpioLightBlank,
    /* meta */ {
      /* active state */ TLC5947_LLD_BLANK_ACTIVE_STATE,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
  /* XLAT signal GPIO   */ {
    /* GPIO */ &moduleGpioLightXlat,
    /* meta */ {
      /* active state */ TLC5947_LLD_XLAT_ACTIVE_STATE,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
};

TPS2051BDriver moduleLldPowerSwitchLaser = {
  /* laser enable gpio */ {
    /* GPIO */ &moduleGpioLaserEn,
    /* meta */ {
      /* active state */ APAL_GPIO_ACTIVE_HIGH,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_OUTPUT,
    },
  },
  /* laser overcurrent gpio */ {
    /* GPIO         */ &moduleGpioLaserOc,
    /* meta */ {
      /* active state */ APAL_GPIO_ACTIVE_LOW,
      /* edge         */ APAL_GPIO_EDGE_NONE,
      /* direction    */ APAL_GPIO_DIRECTION_INPUT,
    },
  },
};

/** @} */

/*===========================================================================*/
/**
 * @name Unit tests (UT)
 * @{
 */
/*===========================================================================*/
#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

/* EEPROM (AT24C01BN) */
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

/* LED PWM driver (TLD5947) */
static int _utShellCmdCb_Tlc5947(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream, &moduleUtAlldTlc5947, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldTlc5947 = {
  /* info           */ "TLC5947",
  /* name           */ "LED PWM driver",
  /* test function  */ utAlldTlc5947Func,
  /* shell command  */ {
    /* name     */ "unittest:Lights",
    /* callback */ _utShellCmdCb_Tlc5947,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldLedPwm,
};

/* power switch (Laser) */
static int _utShellCmdCb_Tps2051bdbv(BaseSequentialStream* stream, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  aosUtRun(stream,&moduleUtAlldTps2051bdbv, NULL);
  return AOS_OK;
}
aos_unittest_t moduleUtAlldTps2051bdbv = {
  /* info           */ "TPS2051BDBV",
  /* name           */ "current-limited power switch",
  /* test function  */ utAlldTps2051bdbvFunc,
  /* shell command  */ {
    /* name     */ "unittest:PowerSwitch",
    /* callback */ _utShellCmdCb_Tps2051bdbv,
    /* next     */ NULL,
  },
  /* data           */ &moduleLldPowerSwitchLaser,
};

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/** @} */
