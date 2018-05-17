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

#ifndef _AMIROOS_H_
#define _AMIROOS_H_

/**
 * @brief   AMiRo-OS identification macro.
 */
#define _AMIRO_OS_

/**
 * @name   AMiRo-OS version and relase information.
 * @{
 */

/**
 * @brief   The type of this version.
 * @note    Is one of "pre-alpha", "alpha", "beta", "release candidate", "release".
 */
#define AMIROOS_RELEASE_TYPE          "beta"

/**
 * @brief   The operating system major version.
 * @note    Changes of the major version imply incompatibilities.
 */
#define AMIROOS_VERSION_MAJOR         2

/**
 * @brief   The operating system minor version.
 * @note    A higher minor version implies new functionalty, but all old interfaces are still available.
 */
#define AMIROOS_VERSION_MINOR         0

/**
 * @brief   The operating system patch level.
 */
#define AMIROOS_VERSION_PATCH         0

/** @} */

/* core headers */
#include <aosconf.h>
#include <module.h>

#if !defined(_AMIRO_OS_CFG_)
#error "invalid AMiRo-OS configuration file"
#endif
#if (_AMIRO_OS_CFG_VERSION_MAJOR_ != AMIROOS_VERSION_MAJOR) || (_AMIRO_OS_CFG_VERSION_MINOR_ < AMIROOS_VERSION_MINOR)
#error "incompatible AMiRo-OS configuration file"
#endif

#include "core/inc/aos_confcheck.h"
#include "core/inc/aos_debug.h"
#include <core/inc/aos_iostream.h>
#include "core/inc/aos_shell.h"
#include "core/inc/aos_system.h"
#include "core/inc/aos_thread.h"
#include "core/inc/aos_time.h"
#include "core/inc/aos_timer.h"
#include "core/inc/aos_types.h"
#include "core/inc/aos_unittest.h"

#endif /* _AMIROOS_H_ */
