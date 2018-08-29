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

#include <aos_iostream.h>

#include <aos_debug.h>
#include <chprintf.h>

/**
 * @brief   Implementation of the BaseAsynchronousChannel write() method (inherited from BaseSequentialStream).
 */
static size_t _channelwrite(void *instance, const uint8_t *bp, size_t n)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT_ENABLE) {
    return streamWrite(((AosIOChannel*)instance)->asyncchannel, bp, n);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel read() method (inherited from BaseSequentialStream).
 */
static size_t _channelread(void *instance, uint8_t *bp, size_t n)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT_ENABLE) {
    return streamRead(((AosIOChannel*)instance)->asyncchannel, bp, n);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel put() method (inherited from BaseSequentialStream).
 */
static msg_t _channelput(void *instance, uint8_t b)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT_ENABLE) {
    return streamPut(((AosIOChannel*)instance)->asyncchannel, b);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel get() method (inherited from BaseSequentialStream).
 */
static msg_t _channelget(void *instance)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT_ENABLE) {
    return streamGet(((AosIOChannel*)instance)->asyncchannel);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel putt() method.
 */
static msg_t _channelputt(void *instance, uint8_t b, systime_t time)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT_ENABLE) {
    return chnPutTimeout(((AosIOChannel*)instance)->asyncchannel, b, time);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel gett() method.
 */
static msg_t _channelgett(void *instance, systime_t time)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT_ENABLE) {
    return chnGetTimeout(((AosIOChannel*)instance)->asyncchannel, time);
  } else {
    return MSG_RESET;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel writet() method.
 */
static size_t _channelwritet(void *instance, const uint8_t *bp, size_t n, systime_t time)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_OUTPUT_ENABLE) {
    return chnWriteTimeout(((AosIOChannel*)instance)->asyncchannel, bp, n, time);
  } else {
    return 0;
  }
}

/**
 * @brief   Implementation of the BaseAsynchronousChannel readt() method.
 */
static size_t _channelreadt(void *instance, uint8_t *bp, size_t n, systime_t time)
{
  if (((AosIOChannel*)instance)->flags & AOS_IOCHANNEL_INPUT_ENABLE) {
    return chnReadTimeout(((AosIOChannel*)instance)->asyncchannel, bp, n, time);
  } else {
    return 0;
  }
}

/**
 * @brief   Virtual methods table for all AosIOChannel objects.
 */
static const struct AosIOChannelVMT _channelvmt = {
  (size_t) 0,
  _channelwrite,
  _channelread,
  _channelput,
  _channelget,
  _channelputt,
  _channelgett,
  _channelwritet,
  _channelreadt,
};

/**
 * @brief   Implementation of the BaseSequentialStream write() method.
 */
static size_t _streamwrite(void *instance, const uint8_t *bp, size_t n)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosIOChannel* channel = ((AosIOStream*)instance)->channel;
  size_t bytes;
  size_t maxbytes = 0;

  // iterate through the list of channels
  while (channel != NULL) {
    bytes = streamWrite(channel, bp, n);
    maxbytes = (bytes > maxbytes) ? bytes : maxbytes;
    channel = channel->next;
  }

  return maxbytes;
}

/**
 * @brief   Implementation of the BaseSequentialStream read() method.
 */
static size_t _stremread(void *instance, uint8_t *bp, size_t n)
{
  (void)instance;
  (void)bp;
  (void)n;

  return 0;
}

/**
 * @brief   Implementation of the BaseSequentialStream put() method.
 */
static msg_t _streamput(void *instance, uint8_t b)
{
  aosDbgCheck(instance != NULL);

  // local variables
  AosIOChannel* channel = ((AosIOStream*)instance)->channel;
  msg_t ret = MSG_OK;

  // iterate through the list of channels
  while (channel != NULL) {
    msg_t ret_ = streamPut(channel, b);
    ret = (ret_ < ret) ? ret_ : ret;
    channel = channel->next;
  }

  return ret;
}

/**
 * @brief   Implementation of the BaseSequentialStream get() method.
 */
static msg_t _streamget(void *instance)
{
  (void)instance;

  return 0;
}

/**
 * @brief   Virtual methods table for all AosIOStream objects.
 */
static const struct AosIOStreamVMT _streamvmt = {
  (size_t) 0,
  _streamwrite,
  _stremread,
  _streamput,
  _streamget,
};

/**
 * @brief   Initializes an aos_iostream_t object.
 *
 * @param[in] stream    Th aos_iostream_t object to initialize.
 */
void aosIOStreamInit(AosIOStream* stream)
{
  aosDbgCheck(stream != NULL);

  stream->vmt = &_streamvmt;
  stream->channel = NULL;

  return;
}

/**
 * @brief   Initializes an aos_iostream_channel_t object.
 *
 * @param[in] channel       The aos_iostream_channel_t to initialize.
 * @param[in] asyncchannel  The BaseAsynchronousChannel object to associate with te channel.
 */
void aosIOChannelInit(AosIOChannel *channel, BaseAsynchronousChannel *asyncchannel)
{
  aosDbgCheck(channel != NULL);
  aosDbgCheck(asyncchannel != NULL);

  channel->vmt = &_channelvmt;
  channel->asyncchannel = asyncchannel;
  channel->next = NULL;
  channel->flags = 0;

  return;
}

/**
 * @brief   Adds a channel to a stream.
 *
 * @param[in] stream    The stream to be expanded.
 * @param[in] channel   The channel to be added.
 */
void aosIOStreamAddChannel(AosIOStream *stream, AosIOChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL && channel->asyncchannel != NULL && channel->next == NULL && (channel->flags & AOS_IOCHANNEL_ATTACHED) == 0);

  // prepend the new channel
  chSysLock();
  channel->flags |= AOS_IOCHANNEL_ATTACHED;
  channel->next = stream->channel;
  stream->channel = channel;
  chSysUnlock();

  return;
}

/**
 * @brief   Removes a channel from a stream.
 *
 * @param[in] stream        The stream to remove the channel from.
 * @param[in] asyncchannel  The BaseAsynchronousChannel to remove.
 * @param[out] removed      Pointer to the removed aos_iostream_channel_t.
 * @return
 */
aos_status_t aosIOStreamRemoveChannel(AosIOStream* stream, AosIOChannel* channel)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(channel != NULL);

  // local variables
  AosIOChannel* prev = NULL;
  AosIOChannel* curr = stream->channel;

  // iterate through the list and search for the specified channel
  while (curr != NULL) {
    // if the channel was found, remove it
    if (curr == channel) {
      chSysLock();
      // special case: the first channel matches (prev is NULL)
      if (prev == NULL) {
        stream->channel = curr->next;
      } else {
        prev->next = curr->next;
      }
      curr->next = NULL;
      curr->flags &= ~AOS_IOCHANNEL_ATTACHED;
      chSysUnlock();
      return AOS_SUCCESS;
    }
    // iterate to the next channel
    else {
      prev = curr;
      curr = curr->next;
    }
  }

  // if the channel was not found, return an error
  return AOS_ERROR;
}

/**
 * @brief   Enable input for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to enable as input.
 */
void aosIOChannelInputEnable(AosIOChannel* channel)
{
  aosDbgCheck(channel != NULL);

  channel->flags |= AOS_IOCHANNEL_INPUT_ENABLE;

  return;
}

/**
 * @brief   Disable input for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to disable as input.
 */
void aosIOChannelInputDisable(AosIOChannel* channel)
{
  aosDbgCheck(channel != NULL);

  channel->flags &= ~AOS_IOCHANNEL_INPUT_ENABLE;

  return;
}

/**
 * @brief   Enable output for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to enable as output.
 */
void aosIOChannelOutputEnable(AosIOChannel* channel)
{
  aosDbgCheck(channel != NULL);

  channel->flags |= AOS_IOCHANNEL_OUTPUT_ENABLE;

  return;
}

/**
 * @brief   Disable output for a AosIOChannel.
 *
 * @param[in] channel   The AosIOChannel to disable as output.
 */
void aosIOChannelOutputDisable(AosIOChannel* channel)
{
  aosDbgCheck(channel != NULL);

  channel->flags &= ~AOS_IOCHANNEL_OUTPUT_ENABLE;

  return;
}
