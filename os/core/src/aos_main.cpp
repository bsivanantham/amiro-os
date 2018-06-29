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

#include <amiroos.h>
#include <module.h>

/*
 * hook to add further includes
 */
#if defined(AMIROOS_CFG_MAIN_EXTRA_INCLUDE)
AMIROOS_CFG_MAIN_EXTRA_INCLUDE
#endif

/**
 * @brief   Event mask to identify I/O events.
 */
#define IOEVENT_MASK                            EVENT_MASK(0)

/**
 * @brief   Event mask to identify OS events.
 */
#define OSEVENT_MASK                            EVENT_MASK(1)

/**
 * @brief   Listener object for I/O events.
 */
static event_listener_t _eventListenerIO;

/**
 * @brief   Listener object for OS events.
 */
static event_listener_t _eventListenerOS;

#if defined(MODULE_HAL_PROGIF) || defined(__DOXYGEN__)
/**
 * @brief   I/O channel for the programmer interface.
 */
static AosIOChannel _stdiochannel;

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   I/O shell channel for the programmer interface.
 */
static AosShellChannel _stdshellchannel;
#endif
#endif

/*
 * hook to add further static variables
 */
#if defined(AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES)
AMIROOS_CFG_MAIN_EXTRA_STATIC_VARIABLES
#endif

/**
 * @brief   Prints an error message about an unexpected event.
 *
 * @param[in] mask    The event mask.
 * @param[in] flags   The event flags.
 */
static inline void _unexpectedEventError(eventmask_t mask, eventflags_t flags)
{
  aosprintf("unexpected/unknown event recieved. mask: 0x%08X; flags: 0x%08X\n", mask, flags);
  return;
}

/**
 * @brief   Application entry point.
 */
int main(void)
{
  // local variables
  eventmask_t eventmask = 0;
  eventflags_t eventflags = 0;
  aos_shutdown_t shutdown = AOS_SHUTDOWN_NONE;
#if defined(AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES)
  AMIROOS_CFG_MAIN_EXTRA_THREAD_VARIABLES
#endif

  /*
   * ##########################################################################
   * # system initialization                                                  #
   * ##########################################################################
   */

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_0)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_0_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_0(AMIROOS_CFG_MAIN_INIT_HOOK_0_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_0();
#endif
#endif

  /* hardware, kernel, and operating system initialization */
  // ChibiOS/HAL and custom hal additions (if any)
  halInit();
#ifdef MODULE_INIT_HAL_EXTRA
  MODULE_INIT_HAL_EXTRA();
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_1(AMIROOS_CFG_MAIN_INIT_HOOK_1_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_1();
#endif
#endif

  // ChibiOS/RT kernel and custom kernel additions (if any)
  chSysInit();
#ifdef MODULE_INIT_KERNEL_EXTRA
  MODULE_INIT_KERNEL_EXTRA();
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_2(AMIROOS_CFG_MAIN_INIT_HOOK_2_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_2();
#endif
#endif

  // AMiRo-OS and custom OS additions (if any)
  aosSysInit(&MODULE_HAL_EXT,
             &moduleHalExtConfig,
             &moduleSsspPd,
             &moduleSsspSync,
             MODULE_OS_IOEVENTFLAGS_SYSPD,
             MODULE_OS_IOEVENTFLAGS_SYSSYNC,
             moduleShellPrompt);
#ifdef MODULE_INIT_OS_EXTRA
  MODULE_INIT_OS_EXTRA();
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_3)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_3(AMIROOS_CFG_MAIN_INIT_HOOK_3_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_3();
#endif
#endif

#if (AMIROOS_CFG_TESTS_ENABLE == true)
#if defined(MODULE_INIT_TESTS)
  MODULE_INIT_TESTS();
#else
  #warning "MODULE_INIT_TESTS not defined"
#endif
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_4)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_4(AMIROOS_CFG_MAIN_INIT_HOOK_4_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_4();
#endif
#endif

  /* event associations */
  chEvtRegisterMask(&aos.events.io.source, &_eventListenerIO, IOEVENT_MASK);
  chEvtRegisterMask(&aos.events.os.source, &_eventListenerOS, OSEVENT_MASK);

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_5)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_5(AMIROOS_CFG_MAIN_INIT_HOOK_5_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_5();
#endif
#endif

  /* periphery communication initialization */
  // CAN (mandatory)
  canStart(&MODULE_HAL_CAN, &moduleHalCanConfig);
  // module specific initialization (if any)
#ifdef MODULE_INIT_PERIPHERY_COMM
  MODULE_INIT_PERIPHERY_COMM();
#endif
  // user interface (if any)
#ifdef MODULE_HAL_PROGIF
  aosIOChannelInit(&_stdiochannel, (BaseAsynchronousChannel*)&MODULE_HAL_PROGIF);
  aosIOChannelOutputEnable(&_stdiochannel);
  aosIOStreamAddChannel(&aos.iostream, &_stdiochannel);
#if (AMIROOS_CFG_SHELL_ENABLE == true)
  aosShellChannelInit(&_stdshellchannel, (BaseAsynchronousChannel*)&MODULE_HAL_PROGIF);
  aosShellChannelInputEnable(&_stdshellchannel);
  aosShellChannelOutputEnable(&_stdshellchannel);
  aosShellStreamAddChannel(&aos.shell->stream, &_stdshellchannel);
#endif
#endif

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_6)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_6_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_6(AMIROOS_CFG_MAIN_INIT_HOOK_6_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_6();
#endif
#endif

  /* module is ready -> print welcome prompt */
  aosprintf("\n");
  aosprintf("######################################################################\n");
  aosprintf("# AMiRo-OS is an operating system designed for the Autonomous Mini   #\n");
  aosprintf("# Robot (AMiRo) platform.                                            #\n");
  aosprintf("# Copyright (C) 2016..2018  Thomas Schöpping et al.                  #\n");
  aosprintf("#                                                                    #\n");
  aosprintf("# This is free software; see the source for copying conditions.      #\n");
  aosprintf("# There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR  #\n");
  aosprintf("# A PARTICULAR PURPOSE.                                              #\n");
  aosprintf("# The development of this software was supported by the Excellence   #\n");
  aosprintf("# Cluster EXC 227 Cognitive Interaction Technology. The Excellence   #\n");
  aosprintf("# Cluster EXC 227 is a grant of the Deutsche Forschungsgemeinschaft  #\n");
  aosprintf("# (DFG) in the context of the German Excellence Initiative.          #\n");
  aosprintf("######################################################################\n");
  aosprintf("\n");

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_7)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_7_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_7(AMIROOS_CFG_MAIN_INIT_HOOK_7_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_7();
#endif
#endif

  /* SSSP startup outro (end of startup stage 2) synchronization */
  while ((eventmask = aosSysSsspStartupOsInitSyncCheck(&_eventListenerIO)) != 0) {
    /*
     * This code is executed if the received event was not about the SYS_SYNC control signal.
     * The returned event could be caused by any listener (not only the argument).
     */
    // unexpected IO events
    if (eventmask & _eventListenerIO.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerIO);
#ifdef MODULE_SSP_STARTUP_OUTRO_IO_EVENT
      MODULE_SSP_STARTUP_OUTRO_IO_EVENT(eventmask, eventflags);
#else
      _unexpectedEventError(eventmask, eventflags);
#endif
    }
    // unexpected OS event
    else if (eventmask & _eventListenerOS.events) {
      eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
      _unexpectedEventError(eventmask, eventflags);
    }
#if (AMIROOS_CFG_DBG == true)
    // unknown event (must never occur, thus disabled for release builds)
    else {
      eventflags = 0;
      _unexpectedEventError(eventmask, eventflags);
    }
#endif
  }

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_8)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_8_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_8(AMIROOS_CFG_MAIN_INIT_HOOK_8_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_8();
#endif
#endif

  /* completely start AMiRo-OS */
  aosSysStart();

#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_9)
#if defined(AMIROOS_CFG_MAIN_INIT_HOOK_9_ARGS)
  AMIROOS_CFG_MAIN_INIT_HOOK_9(AMIROOS_CFG_MAIN_INIT_HOOK_9_ARGS);
#else
  AMIROOS_CFG_MAIN_INIT_HOOK_9();
#endif
#endif

  /*
   * ##########################################################################
   * # infinite loop                                                          #
   * ##########################################################################
   */

  // sleep until a shutdown event is received
  while (shutdown == AOS_SHUTDOWN_NONE) {
    // wait for an event
#if (AMIROOS_CFG_MAIN_LOOP_TIMEOUT != 0)
    eventmask = chEvtWaitOneTimeout(ALL_EVENTS, US2ST_LLD(AMIROOS_CFG_MAIN_LOOP_TIMEOUT));
#else
    eventmask = chEvtWaitOne(ALL_EVENTS);
#endif

#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_0)
#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_0_ARGS)
    AMIROOS_CFG_MAIN_LOOP_HOOK_0(AMIROOS_CFG_MAIN_LOOP_HOOK_0_ARGS);
#else
    AMIROOS_CFG_MAIN_LOOP_HOOK_0();
#endif
#endif

    switch (eventmask) {
      // if this was an I/O event
      case IOEVENT_MASK:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&_eventListenerIO);
        // PD event
        if (eventflags & MODULE_OS_IOEVENTFLAGS_SYSPD) {
          shutdown = AOS_SHUTDOWN_PASSIVE;
        }
        // all other events
#ifdef MODULE_MAIN_LOOP_IO_EVENT
        else {
          MODULE_MAIN_LOOP_IO_EVENT(eventmask, eventflags);
        }
#endif
        break;

      // if this was an OS event
      case OSEVENT_MASK:
        // evaluate flags
        eventflags = chEvtGetAndClearFlags(&_eventListenerOS);
        switch (eventflags) {
          case AOS_SYSTEM_EVENTFLAGS_HIBERNATE:
            shutdown = AOS_SHUTDOWN_HIBERNATE;
            break;
          case AOS_SYSTEM_EVENTFLAGS_DEEPSLEEP:
            shutdown = AOS_SHUTDOWN_DEEPSLEEP;
            break;
          case AOS_SYSTEM_EVENTFLAGS_TRANSPORTATION:
            shutdown = AOS_SHUTDOWN_TRANSPORTATION;
            break;
          case AOS_SYSTEM_EVENTFLAGS_RESTART:
            shutdown = AOS_SHUTDOWN_RESTART;
            break;
          default:
            _unexpectedEventError(eventmask, eventflags);
            break;
        }
        break;

      // if this was any other event (should be impossible to occur)
      default:
        eventflags = 0;
        _unexpectedEventError(eventmask, eventflags);
        break;
    }

#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS)
    AMIROOS_CFG_MAIN_LOOP_HOOK_1(AMIROOS_CFG_MAIN_LOOP_HOOK_1_ARGS);
#else
    AMIROOS_CFG_MAIN_LOOP_HOOK_1();
#endif
#endif
  }

  /*
   * ##########################################################################
   * # system shutdown                                                        #
   * ##########################################################################
   */

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_0();
#endif
#endif

  // initialize/acknowledge shutdown
  aosSysShutdownInit(shutdown);

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1();
#endif
#endif

  // stop system threads
  aosSysStop();

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_2();
#endif
#endif

  // deinitialize system
  aosSysDeinit();

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_3();
#endif
#endif

  /* stop all periphery communication */
  // CAN (mandatory)
  canStop(&MODULE_HAL_CAN);
#ifdef MODULE_SHUTDOWN_PERIPHERY_COMM
  MODULE_SHUTDOWN_PERIPHERY_COMM();
#endif

#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4)
#if defined(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS)
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4(AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4_ARGS);
#else
    AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_4();
#endif
#endif

  // finally hand over to bootloader
  aosSysShutdownFinal(&MODULE_HAL_EXT, shutdown);

  /*
   * ##########################################################################
   * # after shutdown/restart                                                 #
   * ##########################################################################
   *
   * NOTE: This code will not be executed, since the bootloader callbacks will stop/restart the MCU.
   *       It is included nevertheless for the sake of completeness and to explicitely indicate,
   *       which subsystems should NOT be shut down.
   */

  // return an error, since this code should not be executed
  return -1;
}
