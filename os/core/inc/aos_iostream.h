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

#ifndef _AMIROOS_IOSTREAM_H_
#define _AMIROOS_IOSTREAM_H_

#include <hal.h>
#include <aos_types.h>
#include <stdarg.h>



/**
 * @brief   Channel flag to indicate whether the channel is attached to a stream.
 */
#define AOS_IOCHANNEL_ATTACHED                  (1 << 0)

/**
 * @brief   Channel flag to indicate whether the channel is set as input.
 */
#define AOS_IOCHANNEL_INPUT_ENABLE              (1 << 1)

/**
 * @brief   Channel flag to indicate whether the channel is set as output.
 */
#define AOS_IOCHANNEL_OUTPUT_ENABLE             (1 << 2)

/*
 * forward definitions
 */
typedef struct aos_iochannel AosIOChannel;
typedef struct aos_ostream AosIOStream;

/**
 * @brief   AosI=Channel specific methods.
 */
#define _aos_iochannel_methods                                              \
  _base_asynchronous_channel_methods

/**
 * @brief   AosIOChannel specific data.
 */
#define _aos_iochannel_data                                                 \
  /* pointer to a BaseAsynchronousChannel object */                         \
  BaseAsynchronousChannel* asyncchannel;                                    \
  /* pointer to the next channel in a AosIOStream */                        \
  AosIOChannel* next;                                                       \
  /* flags related to the channel */                                        \
  uint8_t flags;

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   AosIOChannel virtual methods table.
 */
struct AosIOChannelVMT {
  _aos_iochannel_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   I/O Channel class.
 * @details This class implements an asynchronous I/O channel.
 */
struct aos_iochannel {
  /** @brief Virtual Methods Table. */
  const struct AosIOChannelVMT* vmt;
  _aos_iochannel_data
};

/**
 * @brief   AosIOStream methods.
 */
#define _aos_iostream_methods                                               \
  _base_sequential_stream_methods

/**
 * @brief   AosIOStream data.
 */
#define _aos_iostream_data                                                  \
  /* Pointer to the first channel in a list. */                             \
  AosIOChannel* channel;

/**
 * @extends BaseSequentialStream
 *
 * @brief   AosIOStream virtual methods table.
 */
struct AosIOStreamVMT {
  _aos_iostream_methods
};

/**
 * @extends BaseSequentialStream
 *
 * @brief   I/O Stream class.
 * @details This class implements an base sequential stream.
 * @todo    So far only output but no input is supported.
 */
struct aos_ostream {
  const struct AosIOStreamVMT* vmt;
  _aos_iostream_data
};

#ifdef __cplusplus
extern "C" {
#endif
  void aosIOStreamInit(AosIOStream* stream);
  void aosIOChannelInit(AosIOChannel* channel, BaseAsynchronousChannel* asyncchannel);
  void aosIOStreamAddChannel(AosIOStream* stream, AosIOChannel* channel);
  aos_status_t aosIOStreamRemoveChannel(AosIOStream* stream, AosIOChannel* channel);
  void aosIOChannelInputEnable(AosIOChannel* channel);
  void aosIOChannelInputDisable(AosIOChannel* channel);
  void aosIOChannelOutputEnable(AosIOChannel* channel);
  void aosIOChannelOutputDisable(AosIOChannel* channel);
#ifdef __cplusplus
}
#endif

#endif /* _AMIROOS_IOSTREAM_H_ */
