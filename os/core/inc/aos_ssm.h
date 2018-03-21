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

#ifndef _AMIROOS_SSM_H_
#define _AMIROOS_SSM_H_

#include <hal.h>
#include <aos_types.h>
#include <aos_debug.h>

/**
 * @brief   Sequential Stream Multiplexer output structure.
 */
typedef struct ssm_output {
  /**
   * @brief   Pointer to a BaseSequentialStream object.
   */
  BaseSequentialStream* stream;

  /**
   * @brief   Pointer to the next element in a singly linked list.
   */
  struct ssm_output* next;

  /**
   * @brief   Flags related to the output.
   */
  uint8_t flags;
} ssm_output_t;

/**
 * @brief   Output flag to indicate whether the output is attached to a SSM list.
 */
#define SSM_OUTPUT_FLAG_ATTACHED                (1 << 0)

/**
 * @brief   Output flag to indicate whether the output is enabled.
 */
#define SSM_OUTPUT_FLAG_ENABLED                 (1 << 1)

/**
 * @brief   Sequential Stream Multiplexer specific methods.
 */
#define _sequential_stream_mux_methods          \
  _base_sequential_stream_methods               \

/**
 * @brief   Sequential Stream Multiplexer specific data.
 */
#define _sequential_stream_mux_data             \
  _base_sequential_stream_data                  \
  ssm_output_t* outputs;                        \
  BaseSequentialStream* input;                  \

/**
 * @brief   Sequential Stream Multiplexer virtual methods table.
 */
struct SequentialStreamMuxVMT {
  _sequential_stream_mux_methods
};

/**
 * @brief   Sequential Stream Multiplexer structure.
 */
typedef struct {
  /**
   * @brief   Virtual methods table.
   */
  const struct SequentialStreamMuxVMT *vmt;

  /**
   * @brief   Data fields.
   */
  _sequential_stream_mux_data
} SequentialStreamMux;

#ifdef __cplusplus
extern "C" {
#endif
  void ssmObjectInit(SequentialStreamMux* ssmp);
  void ssmOutputInit(ssm_output_t* op, BaseSequentialStream* stream);
  void ssmAddOutput(SequentialStreamMux* ssmp, ssm_output_t* output);
  aos_status_t ssmRemoveOutput(SequentialStreamMux* ssmp, BaseSequentialStream* stream, ssm_output_t** removed);
  aos_status_t ssmEnableOutput(SequentialStreamMux* ssmp, BaseSequentialStream* stream);
  aos_status_t ssmDisableOutput(SequentialStreamMux* ssmp, BaseSequentialStream* stream);
#ifdef __cplusplus
}
#endif

/**
 * @brief   Set the input stream
 *
 * @param[in] ssmp    pointer to the @p SequentialStreamMux object
 * @param[in] stream  pointer to a @p BaseSequentialStream or NULL to disable input
 */
static inline void ssmSetInput(SequentialStreamMux* ssmp, BaseSequentialStream* stream)
{
  aosDbgCheck(ssmp != NULL);

  ssmp->input = stream;

  return;
}

#endif /* _AMIROOS_SSM_H_ */
