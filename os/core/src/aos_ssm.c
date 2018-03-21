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

#include <aos_ssm.h>

/**
 * @brief   Sequential Stream Multiplexer write.
 *
 * @param[in] ip      pointer to a @p SequentialStreamMux object
 * @param[in] bp      pointer to the data buffer
 * @param[in] n       the maximum amount of bytes to be transferred
 *
 * @return            The minimum number of bytes written to one of the BaseSequentialStreams.
 *                    If the return value is lower than n, one or multiple (or even all) streams could not write all bytes as requested.
 */
static size_t write(void *ip, const uint8_t *bp, size_t n)
{
  // initialize the number of written bytes
  size_t outbytes = ~0;
  // iterate over all output streams associated with te mux
  ssm_output_t* out = ((SequentialStreamMux*)ip)->outputs;
  while (out != NULL) {
    // if the output is enabled
    if (out->flags | SSM_OUTPUT_FLAG_ENABLED) {
      // write to the stream
      register const size_t ob = streamWrite(out->stream, bp, n);
      // retrieve the minimal number of bytes written across all streams
      outbytes = (ob < outbytes) ? ob : outbytes;
    }
    // iterate to the next stream
    out = out->next;
  }

  return (outbytes == (size_t)~0) ? 0 : outbytes;
}

/**
 * @brief   Sequential Stream Multiplexer read.
 *
 * @param[in]  ip     pointer to a @p SequentialStreamMux object
 * @param[out] bp     pointer to the data buffer
 * @param[in]  n      the maximum amount of data to be transferred
 *
 * @return            The number of bytes transferred.
 */
static size_t read(void *ip, uint8_t *bp, size_t n)
{
  // check whether a input stream is set
  if ( ((SequentialStreamMux*)ip)->input != NULL ) {
    // read from the input stream
    return streamRead(((SequentialStreamMux*)ip)->input, bp, n);
  } else {
    // return zero since
    return 0;
  }
}

/**
 * @brief   Sequential Stream Multiplexer blocking byte write
 *
 * @param[in] ip      pointer to a @p SequentialStreamMux object
 * @param[in] b       the byte value to be written
 *
 * @return            The operation status.
 *                    The operation status is ORed from all enabled output streams.
 *                    The operation status of each output stream depends on its implementation.
 */
static msg_t put(void *ip, uint8_t b)
{
  // initialize the return value
  msg_t retval = MSG_OK;
  // iterate over all output streams associated with the mux
  ssm_output_t* out = ((SequentialStreamMux*)ip)->outputs;
  while (out != NULL) {
    // if the output is enabled
    if (out->flags | SSM_OUTPUT_FLAG_ENABLED) {
      // put the given byte on the stream
      retval |= streamPut(out->stream, b);
    }
    // iterate to rhe next stream
    out = out->next;
  }

  return retval;
}

/**
 * @brief   Sequential Stream Multiplexer blocking byte read
 *
 * @param[in] ip      pointer to the @p SequentialStreamMux object
 *
 * @return            A byte value from the input.
 */
static msg_t get(void *ip)
{
  // check whether an input is set
  if ( ((SequentialStreamMux*)ip)->input != NULL) {
    // get a byte from the input
    return streamGet(((SequentialStreamMux*)ip)->input);
  } else {
    // return an error
    return MSG_RESET;
  }
}

static const struct SequentialStreamMuxVMT vmt = {
  write, read, put, get
};

/**
 * @brief   Initializes a @p SequentialStreamMux object
 *
 * @param[in] ssmp    pointer to the @p SequentialStreamMux object
 */
void ssmObjectInit(SequentialStreamMux *ssmp)
{
  aosDbgCheck(ssmp != NULL);

  // initialize object variables
  ssmp->vmt = &vmt;
  ssmp->outputs = NULL;
  ssmp->input = NULL;

  return;
}

/**
 * @brief   Initializes a @p ssm_output_t object
 *
 * @param[in] op    pointer to the @p ssm_output_t object
 */
void ssmOutputInit(ssm_output_t* op, BaseSequentialStream* stream)
{
  aosDbgCheck(op != NULL);

  // initialize object variables
  op->stream = stream;
  op->next = NULL;
  op->flags = 0;

  return;
}

/**
 * @brief   Adds an output to the SSM object.
 * @note    New outputs are always prepended to the list.
 *
 * @param[in] ssmp    pointer to the @p SequentialStreamMux object to be modified
 * @param[in] output  pointer to the @p ssm_output_t object to be inserted
 */
void ssmAddOutput(SequentialStreamMux* ssmp, ssm_output_t* output)
{
  aosDbgCheck(ssmp != NULL);
  aosDbgCheck(output != NULL);
  aosDbgCheck(output->next == NULL && output->flags ^ SSM_OUTPUT_FLAG_ATTACHED);

  // prepend output to the list
  output->flags = SSM_OUTPUT_FLAG_ATTACHED;
  output->next = ssmp->outputs;
  ssmp->outputs = output;

  return;
}

/**
 * @brief   Removes an output from the SSM object.
 *
 * @param[in] ssmp      pointer to the @p SequentialStreamMux object to be modified
 * @param[in] stream    pointer to the @p BaseSequentialStream to remove
 * @param[out] removed  pointer to the @p ssm_output_t that was removed, or NULL
 *
 * @return              Error code indicating any errors or warnings.
 * @retval AOS_SUCCESS  the stream was removed sucessfully
 * @retval AOS_ERROR    the stream was not found in the list
 */
aos_status_t ssmRemoveOutput(SequentialStreamMux* ssmp, BaseSequentialStream* stream, ssm_output_t** removed)
{
  aosDbgCheck(ssmp != NULL);
  aosDbgCheck(stream != NULL);

  // local varibales
  ssm_output_t* prev = NULL;
  ssm_output_t* curr = ssmp->outputs;

  // iterate through the list and search for the specified stream
  while (curr != NULL) {
    // if the stream was found, remove it
    if (curr->stream == stream) {
      // special case: the first stream matches
      if (prev == NULL) {
        ssmp->outputs->next = curr->next;
      } else {
        prev->next = curr->next;
      }
      curr->next = NULL;
      curr->flags &= ~SSM_OUTPUT_FLAG_ATTACHED;
      // set the optional output argument
      if (removed != NULL) {
        *removed = curr;
      }
      return AOS_SUCCESS;
    }
    // iterate through to the next item
    else {
      prev = curr;
      curr = curr->next;
    }
  }

  // if the stream was not found, return an error
  return AOS_ERROR;
}

/**
 * @brief   Enable an associated stream
 *
 * @param[in] ssmp    pointer to the @p SequentialStreamMux object
 * @param[in] stream  pointer to the @p BaseSequentialStream to enable
 *
 * @return            The operation status.
 * @retval AOS_SUCCESS  if the operation succeeded.
 * @retval AOS_ERROR    if the specified stream is not associated to the @p SequentialStreamMux.
 */
aos_status_t ssmEnableOutput(SequentialStreamMux *ssmp, BaseSequentialStream *stream)
{
  aosDbgCheck(ssmp != NULL);
  aosDbgCheck(stream != NULL);

  // iterate over all output streams associated to the given mux
  ssm_output_t* out = ssmp->outputs;
  while (out != NULL) {
    // if the output was found, activate the enabled flag
    if (out->stream == stream) {
      out->flags |= SSM_OUTPUT_FLAG_ENABLED;
      return AOS_SUCCESS;
    } else {
      out = out->next;
    }
  }
  // error if the stream is not associated with the mux
  return AOS_ERROR;
}

/**
 * @brief   Disable an associated stream
 *
 * @param[in] ssmp    pointer to the @p SequentialStreamMux or derived class
 * @param[in] stream  pointer to the stream to disable
 *
 *
 * @return            The operation status.
 * @retval AOS_OK     if the operation succeeded.
 * @retval AOS_ERROR  if the specified stream is not associated to the @p SequentialStreamMux.
 */
aos_status_t ssmDisableOutput(SequentialStreamMux *ssmp, BaseSequentialStream *stream)
{
  aosDbgCheck(ssmp != NULL);
  aosDbgCheck(stream != NULL);

  // iterate over all output streams associated to the given mux
  ssm_output_t* out = ssmp->outputs;
  while (out != NULL) {
    // if the output was found, activate the enabled flag
    if (out->stream == stream) {
      out->flags &= ~SSM_OUTPUT_FLAG_ENABLED;
      return AOS_SUCCESS;
    } else {
      out = out->next;
    }
  }
  // error if the stream is not associated with the mux
  return AOS_ERROR;
}
