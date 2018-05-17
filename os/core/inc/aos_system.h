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
#define AOS_SYSTEM_SSSP_MAJOR                   1

/**
 * @brief   Minor version of the implemented SSSP.
 */
#define AOS_SYSTEM_SSSP_MINOR                   3

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
 * @brief   Enumerator of the several stages of SSSP
 */
typedef enum aos_ssspstage {
  AOS_SSSP_STARTUP_1_1  = 0x0000, /**< Identifier of SSSP startup phase stage 1-1. */
  AOS_SSSP_STARTUP_1_2  = 0x0001, /**< Identifier of SSSP startup phase stage 1-2. */
  AOS_SSSP_STARTUP_1_3  = 0x0002, /**< Identifier of SSSP startup phase stage 1-3. */
  AOS_SSSP_STARTUP_2_1  = 0x0004, /**< Identifier of SSSP startup phase stage 2-1. */
  AOS_SSSP_STARTUP_2_2  = 0x0005, /**< Identifier of SSSP startup phase stage 2-2. */
  AOS_SSSP_STARTUP_3_1  = 0x0008, /**< Identifier of SSSP startup phase stage 3-1. */
  AOS_SSSP_STARTUP_3_2  = 0x0009, /**< Identifier of SSSP startup phase stage 3-2. */
  AOS_SSSP_STARTUP_3_3  = 0x000A, /**< Identifier of SSSP startup phase stage 3-3. */
  AOS_SSSP_STARTUP_3_4  = 0x000B, /**< Identifier of SSSP startup phase stage 3-4. */
  AOS_SSSP_OPERATION    = 0x0100, /**< Identifier of SSSP operation pahse. */
  AOS_SSSP_SHUTDOWN_1_1 = 0x0200, /**< Identifier of SSSP shutdown phase stage 1-1. */
  AOS_SSSP_SHUTDOWN_1_2 = 0x0201, /**< Identifier of SSSP shutdown phase stage 1-2. */
  AOS_SSSP_SHUTDOWN_1_3 = 0x0202, /**< Identifier of SSSP shutdown phase stage 1-3. */
  AOS_SSSP_SHUTDOWN_2_1 = 0x0204, /**< Identifier of SSSP shutdown phase stage 2-1. */
  AOS_SSSP_SHUTDOWN_2_2 = 0x0205, /**< Identifier of SSSP shutdown phase stage 2-2. */
  AOS_SSSP_SHUTDOWN_3   = 0x0208, /**< Identifier of SSSP shutdown phase stage 3. */
} aos_ssspstage_t;

/**
 * @brief   AMiRo-OS base system structure.
 */
typedef struct aos_system {
  /**
   * @brief   Current SSSP stage of the system.
   */
  aos_ssspstage_t ssspStage;

  /**
   * @brief   System I/O stream.
   */
  AosIOStream iostream;

  /**
   * @brief   Event structure.
   */
  struct {

    /**
     * @brief   I/O related events.
     */
    struct {
      /**
       * @brief   I/O event source.
       */
      event_source_t source;

      /**
       * @brief   Event flags emitted when a PD signal interrupt occurs.
       */
      eventflags_t flagsSignalPd;

      /**
       * @brief   Event flags emitted when a Sync signal interrupt occurs.
       */
      eventflags_t flagsSignalSync;
    } io;

    /**
     * @brief   Operating system events.
     */
    struct {
      /**
       * @brief   OS event source.
       */
      event_source_t source;
    } os;
  } events;

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
  /**
   * @brief   Pointer to the shell object.
   */
  aos_shell_t* shell;
#endif

} aos_system_t;

/**
 * @brief   Global system object.
 */
extern aos_system_t aos;

/**
 * @brief   Printf function that uses the default system I/O stream.
 */
#define aosprintf(fmt, ...)                     chprintf((BaseSequentialStream*)&aos.iostream, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
  void aosSysInit(EXTDriver* extDrv, EXTConfig* extCfg, apalControlGpio_t* gpioPd, apalControlGpio_t* gpioSync, eventflags_t flagsSignalPd, eventflags_t evtFlagsSync, const char* shellPrompt);
  void aosSysStart(void);
  eventmask_t aosSysSsspStartupOsInitSyncCheck(event_listener_t* syncEvtListener);
  void aosSysGetUptimeX(aos_timestamp_t* ut);
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
