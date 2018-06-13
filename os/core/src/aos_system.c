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

#include <aos_system.h>

#include <chprintf.h>
#include <amiroos.h>
#include <amiroblt.h>
#include <string.h>
#include <stdarg.h>
#if (AMIROOS_CFG_TESTS_ENABLE == true)
#include <ch_test.h>
#endif

/**
 * @brief   Period of the system timer.
 */
#define SYSTIMER_PERIOD               (TIME_MAXIMUM - CH_CFG_ST_TIMEDELTA)

/**
 * @brief   Width of the printable system info text.
 */
#define SYSTEM_INFO_WIDTH             70

/**
 * @brief   Width of the name column of the system info table.
 */
#define SYSTEM_INFO_NAMEWIDTH         14

/* forward declarations */
static void _printSystemInfo(BaseSequentialStream* stream);
#if (AMIROOS_CFG_SHELL_ENABLE == true)
static int _shellcmd_configcb(BaseSequentialStream* stream, int argc, char* argv[]);
static int _shellcmd_infocb(BaseSequentialStream* stream, int argc, char* argv[]);
static int _shellcmd_shutdowncb(BaseSequentialStream* stream, int argc, char* argv[]);
#endif /* AMIROOS_CFG_SHELL_ENABLE == true */
#if (AMIROOS_CFG_TESTS_ENABLE == true)
static int _shellcmd_kerneltestcb(BaseSequentialStream* stream, int argc, char* argv[]);
#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/**
 * @brief   PD signal GPIO.
 */
static apalControlGpio_t* _gpioPd;

/**
 * @brief   Sync signal GPIO.
 */
static apalControlGpio_t* _gpioSync;

/**
 * @brief   Timer to accumulate system uptime.
 */
static virtual_timer_t _systimer;

/**
 * @brief   Accumulated system uptime.
 */
static aos_timestamp_t _uptime;

/**
 * @brief   Timer register value of last accumulation.
 */
static systime_t _synctime;

#if (AMIROOS_CFG_SSSP_MASTER == true) || defined(__DOXYGEN__)
/**
 * @brief   Timer to drive the SYS_SYNC signal for system wide time synchronization according to SSSP.
 */
static virtual_timer_t _syssynctimer;

/**
 * @brief   Last uptime of system wide time synchronization.
 */
static aos_timestamp_t _syssynctime;
#endif

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   System shell.
 */
static aos_shell_t _shell;

/**
 * @brief   Shell thread working area.
 */
THD_WORKING_AREA(_shell_wa, AMIROOS_CFG_SHELL_STACKSIZE);

/**
 * @brief   Shell input buffer.
 */
static char _shell_line[AMIROOS_CFG_SHELL_LINEWIDTH];

/**
 * @brief   Shell argument buffer.
 */
static char* _shell_arglist[AMIROOS_CFG_SHELL_MAXARGS];

/**
 * @brief   Shell command to retrieve system information.
 */
static aos_shellcommand_t _shellcmd_info = {
  /* name     */ "module:info",
  /* callback */ _shellcmd_infocb,
  /* next     */ NULL,
};

/**
 * @brief   Shell command to set or retrieve system configuration.
 */
static aos_shellcommand_t _shellcmd_config = {
  /* name     */ "module:config",
  /* callback */ _shellcmd_configcb,
  /* next     */ NULL,
};

/**
 * @brief   Shell command to shutdown the system.
 */
static aos_shellcommand_t _shellcmd_shutdown = {
  /* name     */ "system:shutdown",
  /* callback */ _shellcmd_shutdowncb,
  /* next     */ NULL,
};
#endif /* AMIROOS_CFG_SHELL_ENABLE == true */

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Shell kommand to run a test of the ChibiOS/RT kernel.
 */
static aos_shellcommand_t _shellcmd_kerneltest = {
  /* name     */ "kernel:test",
  /* callback */ _shellcmd_kerneltestcb,
  /* next     */ NULL,
};
#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/**
 * @brief   Global system object.
 */
aos_system_t aos = {
  /* SSSP stage       */ AOS_SSSP_STARTUP_2_1,
  /* I/O stream       */ {
    /* channel          */ NULL,
  },
  /* event            */ {
    /* I/O              */ {
      /* source           */ {
        /* next listener    */ NULL,
      },
      /* flagsSignalPd    */ 0,
      /* flagsSignalSync  */ 0,
    },
    /* OS               */ {
      /* source           */ {
        /* next listener    */ NULL,
      }
    },
  },
#if (AMIROOS_CFG_SHELL_ENABLE == true)
  /* shell            */ &_shell,
#endif
};

/**
 * @brief   Print a separator line.
 *
 * @param[in] stream    Stream to print to or NULL to print to all system streams.
 * @param[in] c         Character to use.
 * @param[in] n         Length of the separator line.
 *
 * @return  Number of characters printed.
 */
static unsigned int _printSystemInfoSeparator(BaseSequentialStream* stream, const char c, const unsigned int n)
{
  aosDbgCheck(stream != NULL);

  // print the specified character n times
  for (unsigned int i = 0; i < n; ++i) {
    streamPut(stream, c);
  }
  streamPut(stream, '\n');

  return n+1;
}

/**
 * @brief   Print a system information line.
 * @details Prints a system information line with the following format:
 *            "<name>[spaces]fmt"
 *          The combined width of "<name>[spaces]" can be specified in order to align <fmt> on multiple lines.
 *          Note that there is not trailing newline added implicitely.
 *
 * @param[in] stream      Stream to print to or NULL to print to all system streams.
 * @param[in] name        Name of the entry/line.
 * @param[in] namewidth   Width of the name column.
 * @param[in] fmt         Formatted string of information content.
 *
 * @return  Number of characters printed.
 */
static unsigned int _printSystemInfoLine(BaseSequentialStream* stream, const char* name, const unsigned int namewidth, const char* fmt, ...)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(name != NULL);

  unsigned int n = 0;
  va_list ap;

  va_start(ap, fmt);
  n += chprintf(stream, name);
  while (n < namewidth) {
    streamPut(stream, ' ');
    ++n;
  }
  n += chvprintf(stream, fmt, ap);
  va_end(ap);

  return n;
}

/**
 * @brief   Prints information about the system.
 *
 * @param[in] stream    Stream to print to.
 */
static void _printSystemInfo(BaseSequentialStream* stream)
{
  aosDbgCheck(stream != NULL);

  _printSystemInfoSeparator(stream, '=', SYSTEM_INFO_WIDTH);
  _printSystemInfoLine(stream, "Module", SYSTEM_INFO_NAMEWIDTH, "%s (v%s)\n", BOARD_NAME, BOARD_VERSION);
#ifdef PLATFORM_NAME
  _printSystemInfoLine(stream, "Platform", SYSTEM_INFO_NAMEWIDTH, "%s\n", PLATFORM_NAME);
#endif
#ifdef PORT_CORE_VARIANT_NAME
  _printSystemInfoLine(stream, "Core Variant", SYSTEM_INFO_NAMEWIDTH, "%s\n", PORT_CORE_VARIANT_NAME);
#endif
  _printSystemInfoLine(stream, "Architecture", SYSTEM_INFO_NAMEWIDTH, "%s\n", PORT_ARCHITECTURE_NAME);
  _printSystemInfoSeparator(stream, '-', SYSTEM_INFO_WIDTH);
  _printSystemInfoLine(stream, "AMiRo-OS" ,SYSTEM_INFO_NAMEWIDTH, "%u.%u.%u %s (SSSP %u.%u)\n", AMIROOS_VERSION_MAJOR, AMIROOS_VERSION_MINOR, AMIROOS_VERSION_PATCH, AMIROOS_RELEASE_TYPE, AOS_SYSTEM_SSSP_MAJOR, AOS_SYSTEM_SSSP_MINOR);
  _printSystemInfoLine(stream, "AMiRo-LLD" ,SYSTEM_INFO_NAMEWIDTH, "%u.%u.%u %s (periphAL %u.%u)\n", AMIRO_LLD_VERSION_MAJOR, AMIRO_LLD_VERSION_MINOR, AMIRO_LLD_VERSION_PATCH, AMIRO_LLD_RELEASE_TYPE, PERIPHAL_VERSION_MAJOR, PERIPHAL_VERSION_MINOR);
  _printSystemInfoLine(stream, "ChibiOS/RT" ,SYSTEM_INFO_NAMEWIDTH, "%u.%u.%u %s\n", CH_KERNEL_MAJOR, CH_KERNEL_MINOR, CH_KERNEL_PATCH, (CH_KERNEL_STABLE == 1) ? "stable" : "non-stable");
  _printSystemInfoLine(stream, "ChibiOS/HAL",SYSTEM_INFO_NAMEWIDTH, "%u.%u.%u %s\n", CH_HAL_MAJOR, CH_HAL_MINOR, CH_HAL_PATCH, (CH_HAL_STABLE == 1) ? "stable" : "non-stable");
  _printSystemInfoLine(stream, "build type",SYSTEM_INFO_NAMEWIDTH,"%s\n", (AMIROOS_CFG_DBG == true) ? "debug" : "release");
  _printSystemInfoLine(stream, "Compiler" ,SYSTEM_INFO_NAMEWIDTH, "%s %u.%u.%u\n", "GCC", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__); // TODO: support other compilers than GCC
  _printSystemInfoLine(stream, "Compiled" ,SYSTEM_INFO_NAMEWIDTH, "%s - %s\n", __DATE__, __TIME__);
  _printSystemInfoSeparator(stream, '-', SYSTEM_INFO_WIDTH);
  if (BL_CALLBACK_TABLE_ADDRESS->magicNumber == BL_MAGIC_NUMBER) {
    _printSystemInfoLine(stream, "AMiRo-BLT", SYSTEM_INFO_NAMEWIDTH, "%u.%u.%u %s (SSSP %u.%u)\n", BL_CALLBACK_TABLE_ADDRESS->vBootloader.major, BL_CALLBACK_TABLE_ADDRESS->vBootloader.minor, BL_CALLBACK_TABLE_ADDRESS->vBootloader.patch,
                         (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_Release) ? "stable" :
                         (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_ReleaseCandidate) ? "release candidate" :
                         (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_Beta) ? "beta" :
                         (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_Alpha) ? "alpha" :
                         (BL_CALLBACK_TABLE_ADDRESS->vBootloader.identifier == BL_VERSION_ID_AMiRoBLT_PreAlpha) ? "pre-alpha" :
                         "<release type unknown>",
                         BL_CALLBACK_TABLE_ADDRESS->vSSSP.major, BL_CALLBACK_TABLE_ADDRESS->vSSSP.minor);
    if (BL_CALLBACK_TABLE_ADDRESS->vSSSP.major != AOS_SYSTEM_SSSP_MAJOR) {
      if (stream) {
        chprintf(stream, "WARNING: Bootloader and AMiRo-OS implement incompatible SSSP versions!\n");
      } else {
        aosprintf("WARNING: Bootloader and AMiRo-OS implement incompatible SSSP versions!\n");
      }
    }
    _printSystemInfoLine(stream, "Compiler", SYSTEM_INFO_NAMEWIDTH, "%s %u.%u.%u\n", (BL_CALLBACK_TABLE_ADDRESS->vCompiler.identifier == BL_VERSION_ID_GCC) ? "GCC" : "<compiler unknown>", BL_CALLBACK_TABLE_ADDRESS->vCompiler.major, BL_CALLBACK_TABLE_ADDRESS->vCompiler.minor, BL_CALLBACK_TABLE_ADDRESS->vCompiler.patch); // TODO: support other compilers than GCC
  } else {
    if (stream) {
      chprintf(stream, "Bootloader incompatible or not available.\n");
    } else {
      aosprintf("Bootloader incompatible or not available.\n");
    }
  }
  _printSystemInfoSeparator(stream, '=', SYSTEM_INFO_WIDTH);

  return;
}

#if (AMIROOS_CFG_SHELL_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Callback function for the system:config shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return              An exit status.
 * @retval  AOS_OK                  The command was executed successfuly.
 * @retval  AOS_INVALID_ARGUMENTS   There was an issue with the arguemnts.
 */
static int _shellcmd_configcb(BaseSequentialStream* stream, int argc, char* argv[])
{
  aosDbgCheck(stream != NULL);

  // local variables
  int retval = AOS_INVALID_ARGUMENTS;

  // if there are additional arguments
  if (argc > 1) {
    // if the user wants to set or retrieve the shell configuration
    if (strcmp(argv[1], "--shell") == 0) {
      // if the user wants to modify the shell configuration
      if (argc > 2) {
        // if the user wants to modify the prompt
        if (strcmp(argv[2], "prompt") == 0) {
          // there must be a further argument
          if (argc > 3) {
            // handle the option
            if (strcmp(argv[3], "text") == 0) {
              aos.shell->config &= ~AOS_SHELL_CONFIG_PROMPT_MINIMAL;
              retval = AOS_OK;
            }
            else if (strcmp(argv[3], "minimal") == 0) {
              aos.shell->config |= AOS_SHELL_CONFIG_PROMPT_MINIMAL;
              retval = AOS_OK;
            }
            else if (strcmp(argv[3], "notime") == 0) {
              aos.shell->config &= ~AOS_SHELL_CONFIG_PROMPT_UPTIME;
              retval = AOS_OK;
            }
            else if (strcmp(argv[3], "uptime") == 0) {
              aos.shell->config |= AOS_SHELL_CONFIG_PROMPT_UPTIME;
              retval = AOS_OK;
            }
          }
        }
        // if the user wants to modify the string matching
        else if (strcmp(argv[2], "match") == 0) {
          // there must be a further argument
          if (argc > 3) {
            if (strcmp(argv[3], "casesensitive") == 0) {
              aos.shell->config |= AOS_SHELL_CONFIG_MATCH_CASE;
              retval = AOS_OK;
            }
            else if (strcmp(argv[3], "caseinsensitive") == 0) {
              aos.shell->config &= ~AOS_SHELL_CONFIG_MATCH_CASE;
              retval = AOS_OK;
            }
          }
        }
      }
      // if the user wants to retrieve the shell configuration
      else {
        chprintf(stream, "current shell configuration:\n");
        chprintf(stream, "  prompt text:   %s\n",
                 (aos.shell->prompt != NULL) ? aos.shell->prompt : "n/a");
        chprintf(stream, "  prompt style:  %s, %s\n",
                 (aos.shell->config & AOS_SHELL_CONFIG_PROMPT_MINIMAL) ? "minimal" : "text",
                 (aos.shell->config & AOS_SHELL_CONFIG_PROMPT_UPTIME) ? "system uptime" : "no time");
        chprintf(stream, "  input method:  %s\n",
                 (aos.shell->config & AOS_SHELL_CONFIG_INPUT_OVERWRITE) ? "replace" : "insert");
        chprintf(stream, "  text matching: %s\n",
                 (aos.shell->config & AOS_SHELL_CONFIG_MATCH_CASE) ? "case sensitive" : "case insensitive");
        retval = AOS_OK;
      }
    }
  }

  // print help, if required
  if (retval == AOS_INVALID_ARGUMENTS) {
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --help\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "  --shell [OPT [VAL]]\n");
    chprintf(stream, "    Set or retrieve shell configuration.\n");
    chprintf(stream, "    Possible OPTs and VALs are:\n");
    chprintf(stream, "      prompt text|minimal|uptime|notime\n");
    chprintf(stream, "        Configures the prompt.\n");
    chprintf(stream, "      match casesensitive|caseinsenitive\n");
    chprintf(stream, "        Configures string matching.\n");
  }

  return (argc > 1 && strcmp(argv[1], "--help") == 0) ? AOS_OK : retval;
}

/**
 * @brief   Callback function for the system:info shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return            An exit status.
 * @retval  AOS_OK    The command was executed successfully.
 */
static int _shellcmd_infocb(BaseSequentialStream* stream, int argc, char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  // print system information
  _printSystemInfo(stream);

  // print time measurement precision
  chprintf(stream, "system time resolution: %uus\n", AOS_SYSTEM_TIME_RESOLUTION);

  // print system uptime
  aos_timestamp_t uptime;
  aosSysGetUptime(&uptime);
  chprintf(stream, "The system is running for\n");
  chprintf(stream, "%10u days\n", (uint32_t)(uptime / MICROSECONDS_PER_DAY));
  chprintf(stream, "%10u hours\n", (uint8_t)(uptime % MICROSECONDS_PER_DAY / MICROSECONDS_PER_HOUR));
  chprintf(stream, "%10u minutes\n", (uint8_t)(uptime % MICROSECONDS_PER_HOUR / MICROSECONDS_PER_MINUTE));
  chprintf(stream, "%10u seconds\n", (uint8_t)(uptime % MICROSECONDS_PER_MINUTE / MICROSECONDS_PER_SECOND));
  chprintf(stream, "%10u milliseconds\n", (uint16_t)(uptime % MICROSECONDS_PER_SECOND / MICROSECONDS_PER_MILLISECOND));
  chprintf(stream, "%10u microseconds\n", (uint16_t)(uptime % MICROSECONDS_PER_MILLISECOND / MICROSECONDS_PER_MICROSECOND));

  return AOS_OK;
}

/**
 * @brief   Callback function for the sytem:shutdown shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return              An exit status.
 * @retval  AOS_OK                  The command was executed successfully.
 * @retval  AOS_INVALID_ARGUMENTS   There was an issue with the arguments.
 */
static int _shellcmd_shutdowncb(BaseSequentialStream* stream, int argc, char* argv[])
{
  aosDbgCheck(stream != NULL);

  // print help text
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    chprintf(stream, "Usage: %s OPTION\n", argv[0]);
    chprintf(stream, "Options:\n");
    chprintf(stream, "  --help\n");
    chprintf(stream, "    Print this help text.\n");
    chprintf(stream, "  --hibernate, -h\n");
    chprintf(stream, "    Shutdown to hibernate mode.\n");
    chprintf(stream, "    Least energy saving, but allows charging via pins.\n");
    chprintf(stream, "  --deepsleep, -d\n");
    chprintf(stream, "    Shutdown to deepsleep mode.\n");
    chprintf(stream, "    Minimum energy consumption while allowing charging via plug.\n");
    chprintf(stream, "  --transportation, -t\n");
    chprintf(stream, "    Shutdown to transportation mode.\n");
    chprintf(stream, "    Minimum energy consumption with all interrupts disabled (no charging).\n");
    chprintf(stream, "  --restart, -r\n");
    chprintf(stream, "    Shutdown and restart system.\n");

    return (argc != 2) ? AOS_INVALID_ARGUMENTS : AOS_OK;
  }
  // handle argument
  else {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--hibernate") == 0) {
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_HIBERNATE);
      chThdTerminate(currp);
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--deepsleep") == 0) {
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_DEEPSLEEP);
      chThdTerminate(currp);
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--transportation") == 0) {
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_TRANSPORTATION);
      chThdTerminate(currp);
      return AOS_OK;
    }
    else if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--restart") == 0) {
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_RESTART);
      chThdTerminate(currp);
      return AOS_OK;
    }
    else {
      chprintf(stream, "unknown argument %s\n", argv[1]);
      return AOS_INVALID_ARGUMENTS;
    }
  }
}
#endif /* AMIROOS_CFG_SHELL_ENABLE == true */

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
/**
 * @brief   Callback function for the kernel:test shell command.
 *
 * @param[in] stream    The I/O stream to use.
 * @param[in] argc      Number of arguments.
 * @param[in] argv      List of pointers to the arguments.
 *
 * @return      An exit status.
 */
static int _shellcmd_kerneltestcb(BaseSequentialStream* stream, int argc, char* argv[])
{
  aosDbgCheck(stream != NULL);

  (void)argc;
  (void)argv;

  tprio_t prio = chThdGetPriorityX();
  chThdSetPriority(HIGHPRIO - 5); // some tests increase priorirty by 5, so this is the maximum priority permitted
  msg_t retval = test_execute(stream);
  chThdSetPriority(prio);

  return retval;
}
#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

/**
 * @brief   Callback function for the PD signal interrupt.
 *
 * @param[in] extp      Pointer to the interrupt driver object.
 * @param[in] channel   Interrupt channel identifier.
 */
static void _signalPdCallback(EXTDriver* extp, expchannel_t channel)
{
  (void)extp;
  (void)channel;

  chSysLockFromISR();
  chEvtBroadcastFlagsI(&aos.events.io.source, aos.events.io.flagsSignalPd);
  chSysUnlockFromISR();

  return;
}

/**
 * @brief   Callback function for the Sync signal interrupt.
 *
 * @param[in] extp      Pointer to the interrupt driver object.
 * @param[in] channel   Interrupt channel identifier.
 */
static void _signalSyncCallback(EXTDriver* extp, expchannel_t channel)
{
  (void)extp;
  (void)channel;

#if (AMIROOS_CFG_SSSP_MASTER == true)
  chSysLockFromISR();
  chEvtBroadcastFlagsI(&aos.events.io.source, aos.events.io.flagsSignalSync);
  chSysUnlockFromISR();
#else
  apalControlGpioState_t s_state;
  aos_timestamp_t uptime;

  chSysLockFromISR();
  // get current uptime
  aosSysGetUptimeX(&uptime);
  // read signal S
  apalControlGpioGet(_gpioSync, &s_state);
  // if S was toggled from on to off during SSSP operation phase
  if (aos.ssspStage == AOS_SSSP_OPERATION && s_state == APAL_GPIO_OFF) {
    // align the uptime with the synchronization period
    if (uptime % AMIROOS_CFG_SSSP_SYSSYNCPERIOD < AMIROOS_CFG_SSSP_SYSSYNCPERIOD / 2) {
      _uptime -= uptime % AMIROOS_CFG_SSSP_SYSSYNCPERIOD;
    } else {
      _uptime += AMIROOS_CFG_SSSP_SYSSYNCPERIOD - (uptime % AMIROOS_CFG_SSSP_SYSSYNCPERIOD);
    }
  }
  // broadcast event
  chEvtBroadcastFlagsI(&aos.events.io.source, aos.events.io.flagsSignalSync);
  chSysUnlockFromISR();
#endif

  return;
}

/**
 * @brief   Callback function for the uptime accumulation timer.
 *
 * @param[in] par   Generic parameter.
 */
#include <module.h>
static void _uptimeCallback(void* par)
{
  (void)par;

  chSysLockFromISR();
  // read current time in system ticks
  register const systime_t st = chVTGetSystemTimeX();
  // update the uptime variables
  _uptime += LL_ST2US(st - _synctime);
  _synctime = st;
  // enable the timer again
  chVTSetI(&_systimer, SYSTIMER_PERIOD, &_uptimeCallback, NULL);
  chSysUnlockFromISR();

  return;
}

#if (AMIROOS_CFG_SSSP_MASTER == true) || defined (__DOXYGEN__)
/**
 * @brief   Periodic system synchronization callback function.
 * @details Toggles the SYS_SYNC signal and reconfigures the system synchronization timer.
 *
 * @param[in] par   Unuesed parameters.
 */
static void _sysSyncTimerCallback(void* par)
{
  (void)par;

  apalControlGpioState_t s_state;
  aos_timestamp_t uptime;

  chSysLockFromISR();
  // read and toggle signal S
  apalControlGpioGet(_gpioSync, &s_state);
  s_state = (s_state == APAL_GPIO_ON) ? APAL_GPIO_OFF : APAL_GPIO_ON;
  apalControlGpioSet(_gpioSync, s_state);
  // if S was toggled from off to on
  if (s_state == APAL_GPIO_ON) {
    // reconfigure the timer precisely, because the logically falling edge (next interrupt) snychronizes the system time
    _syssynctime += AMIROOS_CFG_SSSP_SYSSYNCPERIOD;
    aosSysGetUptimeX(&uptime);
    chVTSetI(&_syssynctimer, LL_US2ST(_syssynctime - uptime), _sysSyncTimerCallback, NULL);
  }
  // if S was toggled from on to off
  else /* if (s_state == APAL_GPIO_OFF) */ {
    // reconfigure the timer (lazy)
    chVTSetI(&_syssynctimer, LL_US2ST(AMIROOS_CFG_SSSP_SYSSYNCPERIOD / 2), _sysSyncTimerCallback, NULL);
  }
  chSysUnlockFromISR();

  return;
}
#endif

/**
 * @brief   AMiRo-OS system initialization.
 * @note    Must be called from the system control thread (usually main thread).
 *
 * @param[in] extDrv        Pointer to the interrupt driver.
 * @param[in] extCfg        Configuration for the interrupt driver.
 * @param[in] gpioPd        GPIO of the PD signal.
 * @param[in] gpioSync      GPIO of the Sync signal
 * @param[in] evtFlagsPd    Event flags to be set when a PD interrupt occurs.
 * @param[in] evtFlagsSync  Event flags to be set when a Sync interrupt occurs.
 * @param[in] shellPrompt   String to be printed as prompt of the system shell.
 * @param[in] stdio         Default (usually physically) interface for I/O like shell.
 */
void aosSysInit(EXTDriver* extDrv,
                EXTConfig* extCfg,
                apalControlGpio_t* gpioPd,
                apalControlGpio_t* gpioSync,
                eventflags_t evtFlagsPd,
                eventflags_t evtFlagsSync,
                const char* shellPrompt)
{
  // check arguments
  aosDbgCheck(extDrv != NULL);
  aosDbgCheck(extCfg != NULL);
  aosDbgCheck(gpioPd != NULL);
  aosDbgCheck(gpioSync != NULL);

  // set control thread to maximum priority
  chThdSetPriority(THD_CTRLPRIO);

  // set local variables
  _gpioPd = gpioPd;
  _gpioSync = gpioSync;
  chVTObjectInit(&_systimer);
  _synctime = 0;
  _uptime = 0;
#if (AMIROOS_CFG_SSSP_MASTER == true)
  chVTObjectInit(&_syssynctimer);
  _syssynctime = 0;
#endif

  // set aos configuration
  aos.ssspStage = AOS_SSSP_STARTUP_2_1;
  aosIOStreamInit(&aos.iostream);
  chEvtObjectInit(&aos.events.io.source);
  chEvtObjectInit(&aos.events.os.source);
  aos.events.io.flagsSignalPd = evtFlagsPd;
  aos.events.io.flagsSignalSync = evtFlagsSync;

  // setup external interrupt system
  extCfg->channels[gpioPd->gpio->pad].cb = _signalPdCallback;
  extCfg->channels[gpioSync->gpio->pad].cb = _signalSyncCallback;
  extStart(extDrv, extCfg);

#if (AMIROOS_CFG_SHELL_ENABLE == true)
  // init shell
  aosShellInit(aos.shell,
               &aos.events.os.source,
               shellPrompt,
               _shell_line,
               AMIROOS_CFG_SHELL_LINEWIDTH,
               _shell_arglist,
               AMIROOS_CFG_SHELL_MAXARGS);
  // add system commands
  aosShellAddCommand(aos.shell, &_shellcmd_config);
  aosShellAddCommand(aos.shell, &_shellcmd_info);
  aosShellAddCommand(aos.shell, &_shellcmd_shutdown);
#if (AMIROOS_CFG_TESTS_ENABLE == true)
  aosShellAddCommand(aos.shell, &_shellcmd_kerneltest);
#endif
#else
  (void)shellPrompt;
#endif

  return;
}

/**
 * @brief   Starts the system and all system threads.
 */
inline void aosSysStart(void)
{
  // update the system SSSP stage
  aos.ssspStage = AOS_SSSP_OPERATION;

  // print system information;
  _printSystemInfo((BaseSequentialStream*)&aos.iostream);
  aosprintf("\n");

#if (AMIROOS_CFG_SHELL_ENABLE == true)
  // start system shell thread
  aos.shell->thread = chThdCreateStatic(_shell_wa, sizeof(_shell_wa), AMIROOS_CFG_SHELL_THREADPRIO, aosShellThread, aos.shell);
#endif

  return;
}

/**
 * @brief   Implements the SSSP startup synchronization step.
 *
 * @param[in] syncEvtListener   Event listener that receives the Sync event.
 *
 * @return    If another event that the listener is interested in was received, its mask is returned.
 *            Otherwise an empty mask (0) is returned.
 */
eventmask_t aosSysSsspStartupOsInitSyncCheck(event_listener_t* syncEvtListener)
{
  aosDbgCheck(syncEvtListener != NULL);

  // local variables
  eventmask_t m;
  eventflags_t f;
  apalControlGpioState_t s;

  // update the system SSSP stage
  aos.ssspStage = AOS_SSSP_STARTUP_2_2;

  // deactivate the sync signal to indicate that the module is ready (SSSPv1 stage 2.1 of startup phase)
  apalControlGpioSet(_gpioSync, APAL_GPIO_OFF);

  // wait for any event to occur (do not apply any filter in order not to miss any event)
  m = chEvtWaitOne(ALL_EVENTS);
  f = chEvtGetAndClearFlags(syncEvtListener);
  apalControlGpioGet(_gpioSync, &s);

  // if the event was a system event,
  //   and it was fired because of the SysSync control signal,
  //   and the SysSync control signal has been deactivated
  if (m & syncEvtListener->events &&
      f == aos.events.io.flagsSignalSync &&
      s == APAL_GPIO_OFF) {
    chSysLock();
#if (AMIROOS_CFG_SSSP_MASTER == true)
    // start the systen synchronization counter
    chVTSetI(&_syssynctimer, LL_US2ST(AMIROOS_CFG_SSSP_SYSSYNCPERIOD / 2), &_sysSyncTimerCallback, NULL);
#endif
    // start the uptime counter
    _synctime = chVTGetSystemTimeX();
    _uptime = 0;
    chVTSetI(&_systimer, SYSTIMER_PERIOD, &_uptimeCallback, NULL);
    chSysUnlock();

    return 0;
  }
  // an unexpected event occurred
  else {
    // reassign the flags to the event and return the event mask
    syncEvtListener->flags |= f;
    return m;
  }
}

/**
 * @brief   Retrieves the system uptime.
 *
 * @param[out] ut   The system uptime.
 */
inline void aosSysGetUptimeX(aos_timestamp_t* ut)
{
  aosDbgCheck(ut != NULL);

  *ut = _uptime + LL_ST2US(chVTGetSystemTimeX() - _synctime);

  return;
}

/**
 * @brief   Initializes/Acknowledges a system shutdown/restart request.
 * @note    This functions should be called from the thread with highest priority.
 *
 * @param[in] shutdown    Type of shutdown.
 */
void aosSysShutdownInit(aos_shutdown_t shutdown)
{
  // check arguments
  aosDbgCheck(shutdown != AOS_SHUTDOWN_NONE);

#if (AMIROOS_CFG_SSSP_MASTER == true)
  // deactivate the system synchronization timer
  chVTReset(&_syssynctimer);
#endif

  // update the system SSSP stage
  aos.ssspStage = AOS_SSSP_SHUTDOWN_1_1;

  // activate the SYS_PD control signal only, if this module initiated the shutdown
  chSysLock();
  if (shutdown != AOS_SHUTDOWN_PASSIVE) {
    apalControlGpioSet(_gpioPd, APAL_GPIO_ON);
  }
  // activate the SYS_SYNC signal
  apalControlGpioSet(_gpioSync, APAL_GPIO_ON);
  chSysUnlock();

  switch (shutdown) {
    case AOS_SHUTDOWN_PASSIVE:
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_SHUTDOWN);
      aosprintf("shutdown request received...\n");
      break;
    case AOS_SHUTDOWN_HIBERNATE:
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_HIBERNATE);
      aosprintf("shutdown to hibernate mode...\n");
      break;
    case AOS_SHUTDOWN_DEEPSLEEP:
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_DEEPSLEEP);
      aosprintf("shutdown to deepsleep mode...\n");
      break;
    case AOS_SHUTDOWN_TRANSPORTATION:
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_TRANSPORTATION);
      aosprintf("shutdown to transportation mode...\n");
      break;
    case AOS_SHUTDOWN_RESTART:
      chEvtBroadcastFlags(&aos.events.os.source, AOS_SYSTEM_EVENTFLAGS_RESTART);
      aosprintf("restarting system...\n");
      break;
   // must never occur
   case AOS_SHUTDOWN_NONE:
   default:
      break;
  }

  // update the system SSSP stage
  aos.ssspStage = AOS_SSSP_SHUTDOWN_1_2;

  return;
}

/**
 * @brief   Stops the system and all related threads (not the thread this function is called from).
 */
void aosSysStop(void)
{
#if (AMIROOS_CFG_SHELL_ENABLE == true)
  chThdWait(aos.shell->thread);
#endif

  return;
}

/**
 * @brief   Deinitialize all system variables.
 */
void aosSysDeinit(void)
{
  return;
}

/**
 * @brief   Finally shuts down the system and calls the bootloader callback function.
 * @note    This function should be called from the thtead with highest priority.
 *
 * @param[in] extDrv      Pointer to the interrupt driver.
 * @param[in] shutdown    Type of shutdown.
 */
void aosSysShutdownFinal(EXTDriver* extDrv, aos_shutdown_t shutdown)
{
  // check arguments
  aosDbgCheck(extDrv != NULL);
  aosDbgCheck(shutdown != AOS_SHUTDOWN_NONE);

  // stop external interrupt system
  extStop(extDrv);

  // update the system SSSP stage
  aos.ssspStage = AOS_SSSP_SHUTDOWN_1_3;

  // call bootloader callback depending on arguments
  switch (shutdown) {
    case AOS_SHUTDOWN_PASSIVE:
      BL_CALLBACK_TABLE_ADDRESS->cbHandleShutdownRequest();
      break;
    case AOS_SHUTDOWN_HIBERNATE:
      BL_CALLBACK_TABLE_ADDRESS->cbShutdownHibernate();
      break;
    case AOS_SHUTDOWN_DEEPSLEEP:
      BL_CALLBACK_TABLE_ADDRESS->cbShutdownDeepsleep();
      break;
    case AOS_SHUTDOWN_TRANSPORTATION:
      BL_CALLBACK_TABLE_ADDRESS->cbShutdownTransportation();
      break;
    case AOS_SHUTDOWN_RESTART:
      BL_CALLBACK_TABLE_ADDRESS->cbShutdownRestart();
      break;
    // must never occur
    case AOS_SHUTDOWN_NONE:
    default:
      break;
  }

  return;
}
