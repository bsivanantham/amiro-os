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

#include <aosconf.h>
#if (AMIROOS_CFG_SHELL_ENABLE == true)

#include <hal.h>
#include <aos_types.h>
//#include <aos_iostream.h>

/**
 * @brief   Shell event flag that is emitted when the thread starts.
 */
#define AOS_SHELL_EVTFLAG_START                   ((eventflags_t)(1 << 0))

/**
 * @brief   Shell event flag that is emitted when a command is executed.
 */
#define AOS_SHELL_EVTFLAG_EXEC                    ((eventflags_t)(1 << 1))

/**
 * @brief   Shell event flag that is emitted when a command execution finished.
 */
#define AOS_SHELL_EVTFLAG_DONE                    ((eventflags_t)(1 << 2))

/**
 * @brief   Shell event flag that is emitted when the shread stops.
 */
#define AOS_SHELL_EVTFLAG_EXIT                    ((eventflags_t)(1 << 3))

/**
 * @brief   Shell event flag that is emitted when an I/O error occurred.
 */
#define AOS_SHELL_EVTFLAG_IOERROR                 ((eventflags_t)(1 << 4))

/**
 * @brief   Shell input configuration for replacing content by user input.
 */
#define AOS_SHELL_CONFIG_INPUT_OVERWRITE          (1 << 0)

/**
 * @brief   Shell prompt configuration print a minimalistic prompt.
 */
#define AOS_SHELL_CONFIG_PROMPT_MINIMAL           (1 << 1)

/**
 * @brief   Shell prompt configuration to additionally print the system uptime with the prompt.
 */
#define AOS_SHELL_CONFIG_PROMPT_UPTIME            (1 << 2)

/**
 * @brief   Shell prompt configuration to additionally print the system uptime with the prompt.
 */
#define AOS_SHELL_CONFIG_MATCH_CASE               (1 << 3)

/**
 * @brief   Shell I/O channel flag whether the channel is attached to a list.
 */
#define AOS_SHELLCHANNEL_ATTACHED                 (1 << 0)

/**
 * @brief   Shell I/O channel flag whether the channel is enabled as input.
 */
#define AOS_SHELLCHANNEL_INPUT_ENABLED            (1 << 1)

/**
 * @brief   Shell I/O channel flag whether the channel is enabled as output.
 */
#define AOS_SHELLCHANNEL_OUTPUT_ENABLED           (1 << 2)

/*
 * forward definitions
 */
typedef struct aos_shellchannel AosShellChannel;
typedef struct aos_shellstream AosShellStream;

/**
 * @brief   AosShellChannel specific methods.
 */
#define _aos_shell_channel_methods                                          \
  _base_asynchronous_channel_methods

/**
 * @brief   AosShellChannel specific data.
 */
#define _aos_shell_channel_data                                             \
  /* pointer to a BaseAsynchronousChannel object */                         \
  BaseAsynchronousChannel* asyncchannel;                                    \
  /* event listener for the associated BaseAsynchronousChannel */           \
  event_listener_t listener;                                                \
  /* pointer to the next chennal in a AosShellStream */                     \
  AosShellChannel* next;                                                    \
  /* flags related to the channel */                                        \
  uint8_t flags;

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   AosShellChannel virtual methods table.
 */
struct AosShellChannelVMT {
  _aos_shell_channel_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   Shell channel class.
 * @details This class implements an asynchronous I/O channel.
 */
struct aos_shellchannel {
  /** @brief Virtual Methods Table. */
  const struct AosShellChannelVMT* vmt;
  _aos_shell_channel_data
};

/**
 * @brief   AosShellStream methods.
 */
#define _aos_shellstream_methods                                            \
  _base_sequential_stream_methods

/**
 * @brief   AosShellStream data.
 */
#define _aos_shellstream_data                                               \
  /* Pointer to the first channel in a list. */                             \
  AosShellChannel* channel;

/**
 * @extends BaseSequentialStream
 *
 * @brief   AosShellStream virtual methods table.
 */
struct AosShellStreamVMT {
  _aos_shellstream_methods
};

/**
 * @extends BaseSequentialStream
 *
 * @brief   Shell Stream class.
 * @details This class implements an base sequential stream.
 * @todo    So far only output but no input is supported.
 */
struct aos_shellstream {
  const struct AosShellStreamVMT* vmt;
  _aos_shellstream_data
};

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
  /**
   * @brief   Pointer to the command that was executed.
   */
  aos_shellcommand_t* command;

  /**
   * @brief   Return value of the executed command.
   */
  int retval;
} aos_shellexecstatus_t;

/**
 * @brief   Enumerator to encode shell actions.
 */
typedef enum aos_shellaction {
  AOS_SHELL_ACTION_NONE,
  AOS_SHELL_ACTION_READCHAR,
  AOS_SHELL_ACTION_AUTOFILL,
  AOS_SHELL_ACTION_SUGGEST,
  AOS_SHELL_ACTION_INSERTTOGGLE,
  AOS_SHELL_ACTION_DELETEFORWARD,
  AOS_SHELL_ACTION_DELETEBACKWARD,
  AOS_SHELL_ACTION_RECALLLAST,
  AOS_SHELL_ACTION_CLEAR,
  AOS_SHELL_ACTION_CURSOR2START,
  AOS_SHELL_ACTION_CURSOR2END,
  AOS_SHELL_ACTION_CURSORLEFT,
  AOS_SHELL_ACTION_CURSORRIGHT,
  AOS_SHELL_ACTION_EXECUTE,
  AOS_SHELL_ACTION_ESCSTART,
} aos_shellaction_t;

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
   * @brief   Struct for OS related events
   */
  struct {
    /**
     * @brief   Pointer to the OS' event source.
     */
    event_source_t* eventSource;

    /**
     * @brief   Listener for OS related events.
     */
    event_listener_t eventListener;
  } os;

  /**
     * @brief   Pointer to the first I/O channel.
     */
  AosShellStream stream;

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
   * @brief   Structure containing data for internal input parsing.
   */
  struct {
    /**
     * @brief   The last action executed by the shell.
     */
    aos_shellaction_t lastaction;

    /**
     * @brief   Number of character in the current escape sequence.
     */
    uint8_t escp;

    /**
     * @brief   Buffer to store an escape sequence.
     */
    char escseq[5];

    /**
     * @brief   Current curso position.
     */
    size_t cursorpos;

    /**
     * @brief   Current line width.
     */
    size_t lineend;

    /**
     * @brief   Flag whether there was input since the prompt was printed the last time.
     */
    bool noinput;
  } inputdata;

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
  void aosShellInit(aos_shell_t* shell, event_source_t* oseventsource, const char* prompt, char* line, size_t linesize, char** arglist, size_t arglistsize);
  void aosShellStreamInit(AosShellStream* stream);
  void aosShellChannelInit(AosShellChannel* channel, BaseAsynchronousChannel* asyncchannel);
  aos_status_t aosShellAddCommand(aos_shell_t* shell, aos_shellcommand_t* cmd);
  aos_status_t aosShellRemoveCommand(aos_shell_t* shell, char* cmd, aos_shellcommand_t** removed);
  void aosShellStreamAddChannel(AosShellStream* stream, AosShellChannel* channel);
  aos_status_t aosShellStreamRemoveChannel(AosShellStream* stream, AosShellChannel* channel);
  void aosShellChannelInputEnable(AosShellChannel* channel);
  void aosShellChannelInputDisable( AosShellChannel* channel);
  void aosShellChannelOutputEnable(AosShellChannel* channel);
  void aosShellChannelOutputDisable(AosShellChannel* channel);
  THD_FUNCTION(aosShellThread, shell);
#ifdef __cplusplus
}
#endif

#endif /* AMIROOS_CFG_SHELL_ENABLE == true */

#endif /* _AMIROOS_SHELL_H_ */
