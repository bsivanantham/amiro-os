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

#ifndef _AMIROOS_PERIPHAL_H_
#define _AMIROOS_PERIPHAL_H_

/*============================================================================*/
/* VERSION                                                                    */
/*============================================================================*/

/**
 * @brief   The periphery abstraction layer interface major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define PERIPHAL_VERSION_MAJOR    1

/**
 * @brief   The periphery abstraction layer interface minor version.
 * @note    A higher minor version implies new functionalty, but all old interfaces are still available.
 */
#define PERIPHAL_VERSION_MINOR    0

/*============================================================================*/
/* DEPENDENCIES                                                               */
/*============================================================================*/

#include <periphALtypes.h>

#include <hal.h>
#include <hal_qei.h>
#include <ch.h>
#include <aos_debug.h>

/*============================================================================*/
/* GENERAL                                                                    */
/*============================================================================*/

/**
 * @brief Delay execution by a specific number of microseconds.
 *
 * @param[in]   us    Time to sleep until execution continues in microseconds.
 */
static inline void usleep(apalTime_t us)
{
  // check if the specified time can be represented by the system
  aosDbgCheck(us <= LL_ST2US(TIME_INFINITE));

  const systime_t st = LL_US2ST(us);
  // TIME_IMMEDIATE makes no sense and would even cause system halt
  if (st != TIME_IMMEDIATE) {
    chThdSleep(st);
  }
  return;
}

/*============================================================================*/
/* GPIO                                                                       */
/*============================================================================*/

#if HAL_USE_PAL || defined (__DOXYGEN__)

/**
 * @brief GPIO driver type.
 */
struct apalGpio_t {
  /*
   * Workaround, since GPIOv2 (STM32F4XX) uses a different type than GPIOv1 (STM32F1XX).
   */
  #if defined(STM32F4XX)
  stm32_gpio_t* port;
  #elif defined(STM32F1XX)
  GPIO_TypeDef* port;
  #else
  void* port;
  #endif

  uint8_t pad;
} PACKED_VAR;

/**
 * @brief Read the current value of a GPIO pin.
 *
 * @param[in]   gpio  GPIO to read.
 * @param[out]  val   Current value of the GPIO.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalGpioRead(apalGpio_t* gpio, apalGpioState_t* const val)
{
  aosDbgCheck(gpio != NULL);
  aosDbgCheck(val != NULL);

  *val = (palReadPad(gpio->port, gpio->pad) == PAL_HIGH) ? APAL_GPIO_HIGH : APAL_GPIO_LOW;
  return APAL_STATUS_OK;
}

/**
 * @brief Set the value of a GPIO pin.
 *
 * @param[in] gpio  GPIO to write.
 * @param[in] val   Value to set for the GPIO.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalGpioWrite(apalGpio_t* gpio, const apalGpioState_t val)
{
  aosDbgCheck(gpio != NULL);

  // palWritePad() is not guaranteed to be atomic, thus the scheduler is locked.
  syssts_t sysstatus = chSysGetStatusAndLockX();
  palWritePad(gpio->port, gpio->pad, (val == APAL_GPIO_HIGH) ? PAL_HIGH : PAL_LOW);
  chSysRestoreStatusX(sysstatus);
  return APAL_STATUS_OK;
}

/**
 * @brief Toggle the output of a GPIO.
 *
 * @param[in] gpio  GPIO to toggle.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalGpioToggle(apalGpio_t* gpio)
{
  aosDbgCheck(gpio != NULL);

  // palWritePad() is not guaranteed to be atomic, thus the scheduler is locked.
  syssts_t sysstatus = chSysGetStatusAndLockX();
  palWritePad(gpio->port, gpio->pad, (palReadPad(gpio->port, gpio->pad) == PAL_HIGH) ? PAL_LOW : PAL_HIGH);
  chSysRestoreStatusX(sysstatus);
  return APAL_STATUS_OK;
}

/**
 * @brief Get the current on/off state of a control GPIO.
 *
 * @param[in]   gpio  Control GPIO to read.
 * @param[out]  val   Current activation status of the control GPIO.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalControlGpioGet(const apalControlGpio_t* const cgpio, apalControlGpioState_t* const val)
{
  aosDbgCheck(cgpio != NULL);
  aosDbgCheck(cgpio->gpio != NULL);
  aosDbgCheck(val != NULL);

  *val = ((palReadPad(cgpio->gpio->port, cgpio->gpio->pad) == PAL_HIGH) ^ (cgpio->meta.active == APAL_GPIO_ACTIVE_HIGH)) ? APAL_GPIO_OFF : APAL_GPIO_ON;
  return APAL_STATUS_OK;
}

/**
 * @brief Turn a control GPIO 'on' or 'off' respectively.
 *
 * @param[in] gpio  Control GPIO to set.
 * @param[in] val   Activation value to set for the control GPIO.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalControlGpioSet(const apalControlGpio_t* const cgpio, const apalControlGpioState_t val)
{
  aosDbgCheck(cgpio != NULL);
  aosDbgCheck(cgpio->gpio != NULL);
  aosDbgCheck(cgpio->meta.direction == APAL_GPIO_DIRECTION_OUTPUT || cgpio->meta.direction == APAL_GPIO_DIRECTION_BIDIRECTIONAL);

  // palWritePad() is not guaranteed to be atomic, thus the scheduler is locked.
  syssts_t sysstatus = chSysGetStatusAndLockX();
  palWritePad(cgpio->gpio->port, cgpio->gpio->pad, ((cgpio->meta.active == APAL_GPIO_ACTIVE_HIGH) ^ (val == APAL_GPIO_ON)) ? PAL_LOW : PAL_HIGH);
  chSysRestoreStatusX(sysstatus);
  return APAL_STATUS_OK;
}

#endif

#if HAL_USE_EXT || defined(__DOXYGEN__)

/**
 * @brief   Converts an apalGpioEdge_t to an ChibiOS EXT edge.
 */
#define APAL2CH_EDGE(edge)                                        \
  ((edge == APAL_GPIO_EDGE_RISING) ? EXT_CH_MODE_RISING_EDGE :    \
    (edge == APAL_GPIO_EDGE_FALLING) ? EXT_CH_MODE_FALLING_EDGE : \
     (edge == APAL_GPIO_EDGE_BOTH) ? EXT_CH_MODE_BOTH_EDGES : 0)

#endif

/*============================================================================*/
/* PWM                                                                        */
/*============================================================================*/

#if HAL_USE_PWM || defined (__DOXYGEN__)

/**
 * @brief PWM driver type.
 */
typedef PWMDriver apalPWMDriver_t;

/**
 * @brief   Set the PWM with given parameters.
 *
 * @param[in] pwm       PWM driver to set.
 * @param[in] channel   Channel of the PWM driver to set.
 * @param[in] width     Width to set the channel to.
 *
 * @return  The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalPWMSet(apalPWMDriver_t* pwm, const apalPWMchannel_t channel, const apalPWMwidth_t width)
{
  aosDbgCheck(pwm != NULL);

  pwmEnableChannel(pwm, (pwmchannel_t)channel, pwm->period * ((float)width / (float)APAL_PWM_WIDTH_MAX) + 0.5f);
  return APAL_STATUS_OK;
}

/**
 * @brief   Retrieve the current frequency of the PWM.
 *
 * @param[in]  pwm        PWM driver to read.
 * @param[out] frequency  The currently set frequency.
 *
 * @return  The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalPWMGetFrequency(apalPWMDriver_t* pwm, apalPWMfrequency_t* const frequency)
{
  aosDbgCheck(pwm != NULL);
  aosDbgCheck(frequency != NULL);

  *frequency = pwm->config->frequency;
  return APAL_STATUS_OK;
}

/**
 * @brief   Retrieve the current period of the PWM.
 *
 * @param[in]   pwm     PWM driver to read.
 * @param[out]  period  The currently set period.
 *
 * @return  The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalPWMGetPeriod(apalPWMDriver_t* pwm, apalPWMperiod_t* const period)
{
  aosDbgCheck(pwm != NULL);
  aosDbgCheck(period != NULL);

  *period = pwm->period;
  return APAL_STATUS_OK;
}

#endif

/*============================================================================*/
/* QEI                                                                        */
/*============================================================================*/

#if HAL_USE_QEI || defined (__DOXYGEN__)

/**
 * @brief QEI driver type.
 */
typedef QEIDriver apalQEIDriver_t;

/**
 * @brief Gets the direction of the last transition.
 *
 * @param[in]   qei         The QEI driver to use.
 * @param[out]  direction   The direction of the last transition.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalQEIGetDirection(apalQEIDriver_t* qei, apalQEIDirection_t* const direction)
{
  aosDbgCheck(qei != NULL);
  aosDbgCheck(direction != NULL);

  *direction = (qei_lld_get_direction(qei)) ? APAL_QEI_DIRECTION_DOWN : APAL_QEI_DIRECTION_UP;

  return APAL_STATUS_OK;
}

/**
 * @brief Gets the current position of the ecnoder.
 *
 * @param[in]   qei       The QEI driver to use.
 * @param[out]  position  The current position of the encoder.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalQEIGetPosition(apalQEIDriver_t* qei, apalQEICount_t* const position)
{
  aosDbgCheck(qei != NULL);
  aosDbgCheck(position != NULL);

  *position = qei_lld_get_position(qei);

  return APAL_STATUS_OK;
}

/**
 * @brief Gets the value range of the encoder.
 *
 * @param[in]   qei     The QEI driver to use.
 * @param[out]  range   The value range of the encoder.
 *
 * @return The status indicates whether the function call was successful.
 */
static inline apalExitStatus_t apalQEIGetRange(apalQEIDriver_t* qei, apalQEICount_t* const range)
{
  aosDbgCheck(qei != NULL);
  aosDbgCheck(range != NULL);

  *range = qei_lld_get_range(qei);

  return APAL_STATUS_OK;
}

#endif

/*============================================================================*/
/* I2C                                                                        */
/*============================================================================*/

#if HAL_USE_I2C || defined(__DOXYGEN__)

/**
 * @brief I2C driver type.
 */
typedef I2CDriver apalI2CDriver_t;

/**
 * @brief Transmit data and receive a response.
 *
 * @param[in]   i2cd      The I2C driver to use.
 * @param[in]   addr      Address to write to.
 * @param[in]   txbuf     Buffer containing data to send.
 * @param[in]   txbytes   Number of bytes to send.
 * @param[out]  rxbuf     Buffer to store a response to.
 * @param[in]   rxbytes   Number of bytes to receive.
 * @param[in]   timeout   Timeout for the function to return (in microseconds).
 *
 * @return The status indicates whether the function call was succesful or a timeout occurred.
 */
static inline apalExitStatus_t apalI2CMasterTransmit(apalI2CDriver_t* i2cd, const apalI2Caddr_t addr, const uint8_t* const txbuf, const size_t txbytes, uint8_t* const rxbuf, const size_t rxbytes, const apalTime_t timeout)
{
  aosDbgCheck(i2cd != NULL);

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  i2cAcquireBus(i2cd);
#endif

#if defined(STM32F1XX_I2C)
  // Due to a hardware limitation, for STM32F1 platform the minimum number of bytes that can be received is two.
  msg_t status = MSG_OK;
  if (rxbytes == 1) {
    uint8_t buffer[2];
    status = i2cMasterTransmitTimeout(i2cd, addr, txbuf, txbytes, buffer, 2, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : LL_US2ST(timeout)) );
    rxbuf[0] = buffer[0];
  } else {
    status = i2cMasterTransmitTimeout(i2cd, addr, txbuf, txbytes, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : LL_US2ST(timeout)) );
  }
#else
  const msg_t status = i2cMasterTransmitTimeout(i2cd, addr, txbuf, txbytes, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : LL_US2ST(timeout)) );
#endif

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  i2cReleaseBus(i2cd);
#endif

  switch (status)
  {
    case MSG_OK:
#if defined(STM32F1XX_I2C)
      return (rxbytes != 1) ? APAL_STATUS_OK : APAL_STATUS_WARNING;
#else
      return APAL_STATUS_OK;
#endif
    case MSG_TIMEOUT:
      return APAL_STATUS_TIMEOUT;
    case MSG_RESET:
    default:
      return APAL_STATUS_ERROR;
  }
}

/**
 * @brief Read data from a specific address.
 *
 * @param[in]   i2cd      The I2C driver to use.
 * @param[in]   addr      Address to read.
 * @param[out]  rxbuf     Buffer to store the response to.
 * @param[in]   rxbytes   Number of bytes to receive.
 * @param[in]   timeout   Timeout for the function to return (in microseconds).
 *
 * @return The status indicates whether the function call was succesful or a timeout occurred.
 */
static inline apalExitStatus_t apalI2CMasterReceive(apalI2CDriver_t* i2cd, const apalI2Caddr_t addr, uint8_t* const rxbuf, const size_t rxbytes, const apalTime_t timeout)
{
  aosDbgCheck(i2cd != NULL);

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  i2cAcquireBus(i2cd);
#endif

#if defined(STM32F1XX_I2C)
  // Due to a hardware limitation, for STM32F1 platform the minimum number of bytes that can be received is two.
  msg_t status = MSG_OK;
  if (rxbytes == 1) {
    uint8_t buffer[2];
    status = i2cMasterReceiveTimeout(i2cd, addr, buffer, 2, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : LL_US2ST(timeout)) );
    rxbuf[0] = buffer[0];
  } else {
    status = i2cMasterReceiveTimeout(i2cd, addr, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : LL_US2ST(timeout)) );
  }
#else
  const msg_t status = i2cMasterReceiveTimeout(i2cd, addr, rxbuf, rxbytes, ((timeout >= TIME_INFINITE) ? TIME_INFINITE : LL_US2ST(timeout)) );
#endif

#if (I2C_USE_MUTUAL_EXCLUSION == TRUE)
  i2cReleaseBus(i2cd);
#endif

  switch (status)
  {
    case MSG_OK:
#if defined(STM32F1XX_I2C)
      return (rxbytes != 1) ? APAL_STATUS_OK : APAL_STATUS_WARNING;
#else
      return APAL_STATUS_OK;
#endif
    case MSG_TIMEOUT:
      return APAL_STATUS_TIMEOUT;
    case MSG_RESET:
    default:
      return APAL_STATUS_ERROR;
  }
}

#endif

/*============================================================================*/
/* SPI                                                                        */
/*============================================================================*/

#if HAL_USE_SPI || defined(__DOXYGEN__)

/**
 * @brief SPI driver type.
 */
typedef SPIDriver apalSPIDriver_t;

/**
 * @brief Transmit and receive data from SPI
 *
 * @param[in]   spid      The SPI driver to use.
 * @param[in]   txData    Buffer containing data to send.
 * @param[out]  rxData    Buffer to store.
 * @param[in]   length    Number of bytes to send.
 *
 * @return The status indicates whether the function call was succesful.
 */
static inline apalExitStatus_t apalSPIExchange(apalSPIDriver_t* spid, const uint8_t* const txData , uint8_t* const rxData, const size_t length)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION)
  spiAcquireBus(spid);
#endif
  spiSelect(spid);
  spiExchange(spid, length, txData, rxData);
  spiUnselect(spid);
#if (SPI_USE_MUTUAL_EXCLUSION)
  spiReleaseBus(spid);
#endif

  return APAL_STATUS_OK;
}

/**
 * @brief Receive data from SPI
 *
 * @param[in]   spid      The SPI driver to use.
 * @param[out]  data      Buffer to store.
 * @param[in]   length    Number of bytes to send.
 *
 * @return The status indicates whether the function call was succesful.
 */
static inline apalExitStatus_t apalSPIReceive(apalSPIDriver_t* spid, uint8_t* const data, const size_t length)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION)
  spiAcquireBus(spid);
#endif
  spiSelect(spid);
  spiReceive(spid, length, data);
  spiUnselect(spid);
#if (SPI_USE_MUTUAL_EXCLUSION)
  spiReleaseBus(spid);
#endif

  return APAL_STATUS_OK;
}

/**
 * @brief Transmit data to SPI
 *
 * @param[in]   spid      The SPI driver to use.
 * @param[in]   data      Buffer containing data to send.
 * @param[in]   length    Number of bytes to send.
 *
 * @return The status indicates whether the function call was succesful.
 */
static inline apalExitStatus_t apalSPITransmit(apalSPIDriver_t* spid, const uint8_t* const data, const size_t length)
{
  aosDbgCheck(spid != NULL);

#if (SPI_USE_MUTUAL_EXCLUSION)
  spiAcquireBus(spid);
#endif
  spiSelect(spid);
  spiSend(spid, length, data);
  spiUnselect(spid);
#if (SPI_USE_MUTUAL_EXCLUSION)
  spiReleaseBus(spid);
#endif

  return APAL_STATUS_OK;
}

#endif

/*============================================================================*/
/* DEBUG                                                                      */
/*============================================================================*/

/**
 * @brief Assert function to check a given condition.
 *
 * @param[in] c   The condition to check.
 */
#define apalDbgAssert(c)   aosDbgAssert(c)

#endif /* _AMIROOS_PERIPHAL_H_ */
