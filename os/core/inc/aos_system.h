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

#ifndef _AMIROOS_SYSTEM_H_
#define _AMIROOS_SYSTEM_H_

#include <aos_iostream.h>
#include <amiro-lld.h>
#include <aos_shell.h>
#include <aos_time.h>
#include <chprintf.h>

/**
 * @brief   Resolution of the system time measurement.
 */
#define AOS_SYSTEM_TIME_RESOLUTION              ((MICROSECONDS_PER_SECOND + CH_CFG_ST_FREQUENCY - 1) / CH_CFG_ST_FREQUENCY)

/**
 * @brief   System event flag which is emitted when a shutdown was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_SHUTDOWN          (eventflags_t)(1 << 0)

/**
 * @brief   System event flag which is emitted when a shutdown to transportation mode was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_TRANSPORTATION    (AOS_SYSTEM_EVENTFLAGS_SHUTDOWN | (eventflags_t)(1 << 1))

/**
 * @brief   System event flag which is emitted when a shutdown to deepsleep mode was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_DEEPSLEEP         (AOS_SYSTEM_EVENTFLAGS_SHUTDOWN | (eventflags_t)(1 << 2))

/**
 * @brief   System event flag which is emitted when a shutdown to hibernate mode was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_HIBERNATE         (AOS_SYSTEM_EVENTFLAGS_SHUTDOWN | (eventflags_t)(1 << 3))

/**
 * @brief   System event flag which is emitted when a system restart was initiated.
 */
#define AOS_SYSTEM_EVENTFLAGS_RESTART           (AOS_SYSTEM_EVENTFLAGS_SHUTDOWN | (eventflags_t)(1 << 4))

/**
 * @brief   Major version of the implemented SSSP.
 */
#define AOS_SYSTEM_SSSP_VERSION_MAJOR           1

/**
 * @brief   Minor version of the implemented SSSP.
 */
#define AOS_SYSTEM_SSSP_VERSION_MINOR           4

/**
 * @brief   Timeout delay according to SSSP.
 * @details SSSP defines timeouts to be ten times longer than the signal delay time.
 */
#define AOS_SYSTEM_SSSP_TIMEOUT                 (10 * AMIROOS_CFG_SSSP_SIGNALDELAY)

/**
 * @brief   Enumerator to identify shutdown types.
 */
typedef enum aos_shutdown {
  AOS_SHUTDOWN_NONE,            /**< Default value if no shutdown action was initiated */
  AOS_SHUTDOWN_PASSIVE,         /**< Passive shutdown (initiated by another module). */
  AOS_SHUTDOWN_HIBERNATE,       /**< Active shutdown to hibernate mode. */
  AOS_SHUTDOWN_DEEPSLEEP,       /**< Active shutdown to deepsleep mode. */
  AOS_SHUTDOWN_TRANSPORTATION,  /**< Active shutdown to transportation mode. */
  AOS_SHUTDOWN_RESTART,         /**< Active saystem restart request. */
} aos_shutdown_t;

/**
 * @brief   Enumerator of the several stages of SSSP.
 */
typedef enum aos_ssspstage {
  AOS_SSSP_STARTUP_1_1  = 0x10, /**< Identifier of SSSP startup phase stage 1-1. */
  AOS_SSSP_STARTUP_1_2  = 0x11, /**< Identifier of SSSP startup phase stage 1-2. */
  AOS_SSSP_STARTUP_1_3  = 0x12, /**< Identifier of SSSP startup phase stage 1-3. */
  AOS_SSSP_STARTUP_2_1  = 0x14, /**< Identifier of SSSP startup phase stage 2-1. */
  AOS_SSSP_STARTUP_2_2  = 0x15, /**< Identifier of SSSP startup phase stage 2-2. */
  AOS_SSSP_STARTUP_3_1  = 0x18, /**< Identifier of SSSP startup phase stage 3-1. */
  AOS_SSSP_STARTUP_3_2  = 0x19, /**< Identifier of SSSP startup phase stage 3-2. */
  AOS_SSSP_STARTUP_3_3  = 0x1A, /**< Identifier of SSSP startup phase stage 3-3. */
  AOS_SSSP_STARTUP_3_4  = 0x1B, /**< Identifier of SSSP startup phase stage 3-4. */
  AOS_SSSP_OPERATION    = 0x20, /**< Identifier of SSSP operation pahse. */
  AOS_SSSP_SHUTDOWN_1_1 = 0x30, /**< Identifier of SSSP shutdown phase stage 1-1. */
  AOS_SSSP_SHUTDOWN_1_2 = 0x31, /**< Identifier of SSSP shutdown phase stage 1-2. */
  AOS_SSSP_SHUTDOWN_1_3 = 0x32, /**< Identifier of SSSP shutdown phase stage 1-3. */
  AOS_SSSP_SHUTDOWN_2_1 = 0x34, /**< Identifier of SSSP shutdown phase stage 2-1. */
  AOS_SSSP_SHUTDOWN_2_2 = 0x35, /**< Identifier of SSSP shutdown phase stage 2-2. */
  AOS_SSSP_SHUTDOWN_3   = 0x38, /**< Identifier of SSSP shutdown phase stage 3. */
} aos_ssspstage_t;

/**
 * @brief   Type to represent module IDs.
 */
typedef uint16_t aos_ssspmoduleid_t;

/**
 * @brief   AMiRo-OS base system structure.
 */
typedef struct aos_system {

  /**
   * @brief   SSSP relevant data.
   */
  struct {
    /**
     * @brief   Current SSSP stage of the system.
     */
    aos_ssspstage_t stage;

    /**
     * @brief   Module identifier.
     * @details A value of 0 indicates an uninitialized ID.
     *          The vlaues 0 and ~0 are reserved und must not be set.
     */
    aos_ssspmoduleid_t moduleId;
  } sssp;

  /**
   * @brief   System I/O stream.
   */
  AosIOStream iostream;

  /**
   * @brief   Event structure.
   */
  struct {

    /**
     * @brief   I/O event source.
     */
    event_source_t io;

    /**
     * @brief   OS event source.
     */
    event_source_t os;
  } events;

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
  /**
   * @brief   Pointer to the shell object.
   */
  aos_shell_t shell;
#endif

} aos_system_t;

/**
 * @brief   Global system object.
 */
extern aos_system_t aos;

/**
 * @brief   Printf function that uses the default system I/O stream.
 *
 * @param[in] fmt   Formatted string to print.
 */
#define aosprintf(fmt, ...)                     chprintf((BaseSequentialStream*)&aos.iostream, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
#if (AMIROOS_CFG_SHELL_ENABLE == true)
  void aosSysInit(const char* shellPrompt);
#else
  void aosSysInit(void);
#endif
  void aosSysStart(void);
  eventmask_t aosSysSsspStartupOsInitSyncCheck(event_listener_t* syncEvtListener);
  void aosSysGetUptimeX(aos_timestamp_t* ut);
  void aosSysGetDateTime(struct tm* dt);
  void aosSysSetDateTime(struct tm* dt);
  void aosSysShutdownInit(aos_shutdown_t shutdown);
  void aosSysStop(void);
  void aosSysDeinit(void);
  void aosSysShutdownFinal(EXTDriver* extDrv, aos_shutdown_t shutdown);
#ifdef __cplusplus
}
#endif

/**
 * @brief   Retrieves the system uptime.
 *
 * @param[out] ut   The system uptime.
 */
static inline void aosSysGetUptime(aos_timestamp_t* ut)
{
  aosDbgCheck(ut != NULL);

  chSysLock();
  aosSysGetUptimeX(ut);
  chSysUnlock();

  return;
}

#endif /* _AMIROOS_SYSTEM_H_ */
