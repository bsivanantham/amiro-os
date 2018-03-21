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

#ifndef _ALLDCONF_H_
#define _ALLDCONF_H_

/*
 * compatibility guards
 */
#define _AMIRO_LLD_CFG_
#define AMIRO_LLD_CFG_VERSION_MAJOR         1
#define AMIRO_LLD_CFG_VERSION_MINOR         0

/**
 * @brief   Enable flag for the A3906 motor driver.
 */
#define AMIROLLD_CFG_USE_A3906

/**
 * @brief   Enable flag for the AT24C01BN-SH-B EEPROM.
 */
#define AMIROLLD_CFG_USE_AT24C01BN

/**
 * @brief   Enable flag for the MHC5883L compass.
 */
#define AMIROLLD_CFG_USE_HMC5883L

/**
 * @brief   Enable flag for the INA219 power monitor.
 */
#define AMIROLLD_CFG_USE_INA219

/**
 * @brief   Enable flag for the L3G4200D gyroscope.
 */
#define AMIROLLD_CFG_USE_L3G4200D

/**
 * @brief   Enable flag for the status LED.
 */
#define AMIROLLD_CFG_USE_LED

/**
 * @brief   Enable flag for the LIS331DLH accelerometer.
 */
#define AMIROLLD_CFG_USE_LIS331DLH

/**
 * @brief   Enable flag for the LTC4412 power path controller.
 */
#define AMIROLLD_CFG_USE_LTC4412

/**
 * @brief   Enable flag for the PCA9544A I2C multiplexer.
 */
#define AMIROLLD_CFG_USE_PCA9544A

/**
 * @brief   Enable flag for the TPS62113 step-down converter.
 */
#define AMIROLLD_CFG_USE_TPS62113

/**
 * @brief   Enable flag for the VCNL4020 proximity sensor.
 */
#define AMIROLLD_CFG_USE_VCNL4020

#endif /* _ALLDCONF_H_ */
