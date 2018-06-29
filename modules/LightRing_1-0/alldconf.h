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
 * @brief   Enable flag for the AT24C01BN-SH-B EEPROM.
 */
#define AMIROLLD_CFG_USE_AT24C01BN

/**
 * @brief   Enable flag for the TLC5947 LED driver.
 */
#define AMIROLLD_CFG_USE_TLC5947

/**
 * @brief   Enable flag for the TPS2051BDBV power switch.
 */
#define AMIROLLD_CFG_USE_TPS2051BDBV

#endif /* _ALLDCONF_H_ */
