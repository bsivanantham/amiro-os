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

#ifndef _AMIROOS_SHELL_H_
#define _AMIROOS_SHELL_H_

#include <hal.h>
#include <aos_types.h>

/**
 * @brief   Shell event flag that is emitted when the thread starts.
 */
#define AOS_SHELL_EVTFLAG_START                 ((eventflags_t)(1 << 0))

/**
 * @brief   Shell event flag that is emitted when a command is executed.
 */
#define AOS_SHELL_EVTFLAG_EXEC                  ((eventflags_t)(1 << 1))

/**
 * @brief   Shell event flag that is emitted when a command execution finished.
 */
#define AOS_SHELL_EVTFLAG_DONE                  ((eventflags_t)(1 << 2))

/**
 * @brief   Shell event flag that is emitted when the shread stops.
 */
#define AOS_SHELL_EVTFLAG_EXIT                  ((eventflags_t)(1 << 3))

/**
 * @brief   Shell event flag that is emitted when an I/O error occurred.
 */
#define AOS_SHELL_EVTFLAG_IOERROR               ((eventflags_t)(1 << 4))

/**
 * @brief   Shell input configuration for replacing content by user input.
 */
#define AOS_SHELL_CONFIG_INPUT_OVERWRITE        (1 << 0)

/**
 * @brief   Shell prompt configuration print a minimalistic prompt.
 */
#define AOS_SHELL_CONFIG_PROMPT_MINIMAL         (1 << 1)

/**
 * @brief   Shell prompt configuration to additionally print the system uptime with the prompt.
 */
#define AOS_SHELL_CONFIG_PROMPT_UPTIME          (1 << 2)

/**
 * @brief   Shell prompt configuration to additionally print the system uptime with the prompt.
 */
#define AOS_SHELL_CONFIG_MATCH_CASE             (1 << 3)

/**
 * @brief   Shell command calback type.
 */
typedef int (*aos_shellcmdcb_t)(BaseSequentialStream* stream, int argc, char* argv[]);

/**
 * @brief   Shell command structure.
 */
typedef struct aos_shellcommand {
  /**
   * @brief   Command name.
   */
  const char* name;

  /**
   * @brief   Callback function.
   */
  aos_shellcmdcb_t callback;

  /**
   * @brief   Pointer to next command in a singly linked list.
   */
  struct aos_shellcommand* next;
} aos_shellcommand_t;

/**
 * @brief   Execution status of a shell command.
 */
typedef struct aos_shellexecstatus {
  aos_shellcommand_t* command;  /**< Pointer to the command that was executed. */
  int retval;                   /**< Return value of the executed command. */
} aos_shellexecstatus_t;

/**
 * @brief   Shell structure.
 */
typedef struct aos_shell {
  /**
   * @brief   Pointer to the thread object.
   */
  thread_t* thread;

  /**
   * @brief   Event source.
   */
  event_source_t eventSource;

  /**
   * @brief   Stream for user I/O.
   */
  BaseSequentialStream* stream;

  /**
   * @brief   String to printed as prompt.
   */
  const char* prompt;

  /**
   * @brief   Pointer to the first element of the singly linked list of commands.
   * @details Commands are ordered alphabetically in the list.
   */
  aos_shellcommand_t* commands;

  /**
   * @brief   Execution status of the most recent command.
   */
  aos_shellexecstatus_t execstatus;

  /**
   * @brief   Input buffer.
   */
  char* line;

  /**
   * @brief   Size of the input buffer.
   */
  size_t linesize;

  /**
   * @brief   Argument buffer.
   */
  char** arglist;

  /**
   * @brief   Size of the argument buffer.
   */
  size_t arglistsize;

  /**
   * @brief   Configuration flags.
   */
  uint8_t config;
} aos_shell_t;

#ifdef __cplusplus
extern "C" {
#endif
  void aosShellInit(aos_shell_t* shell, BaseSequentialStream* stream, const char* prompt, char* line, size_t linesize, char** arglist, size_t arglistsize);
  aos_status_t aosShellAddCommand(aos_shell_t* shell, aos_shellcommand_t* cmd);
  aos_status_t aosShellRemoveCommand(aos_shell_t* shell, char* cmd, aos_shellcommand_t** removed);
  THD_FUNCTION(aosShellThread, shell);
#ifdef __cplusplus
}
#endif

#endif /* _AMIROOS_SHELL_H_ */
