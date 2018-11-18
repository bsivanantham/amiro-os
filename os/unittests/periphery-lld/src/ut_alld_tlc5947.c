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

#include <ut_alld_tlc5947.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TLC5947)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <alld_tlc5947.h>
#include <string.h>
#include <aos_thread.h>

/**
 * @brief   TLC9547 unit test function
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldTlc5947Func(BaseSequentialStream *stream, aos_unittest_t *ut)
{
  aosDbgCheck(ut->data != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status = 0;
  tlc5947_lld_blank_t blank;
  tlc5947_lld_buffer_t buffer;

  chprintf(stream, "reading blank pin...\n");
  status = tlc5947_lld_getBlank((TLC5947Driver*)ut->data, &blank);
  if (status == APAL_STATUS_OK) {
    aosUtPassedMsg(stream, &result, "blank is %s\n", (blank == TLC5947_LLD_BLANK_ENABLE) ? "enabled" : "disabled");
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  // enabling/disabling blank pin
  for (uint8_t i = 0; i < 2; ++i) {
    blank = (blank == TLC5947_LLD_BLANK_ENABLE) ? TLC5947_LLD_BLANK_DISABLE : TLC5947_LLD_BLANK_ENABLE;
    tlc5947_lld_blank_t blank_after = (blank == TLC5947_LLD_BLANK_ENABLE) ? TLC5947_LLD_BLANK_DISABLE : TLC5947_LLD_BLANK_ENABLE;
    chprintf(stream, "%s blank pin...\n", (blank == TLC5947_LLD_BLANK_ENABLE) ? "enabling" : "disabling");
    status = tlc5947_lld_setBlank((TLC5947Driver*)ut->data, blank);
    status |= tlc5947_lld_getBlank((TLC5947Driver*)ut->data, &blank_after);
    if (status == APAL_STATUS_OK && blank_after == blank) {
      aosUtPassed(stream, &result);
    } else {
      aosUtFailedMsg(stream, &result, "0x%08X\n", status);
    }
  }

  chprintf(stream, "setting all black and dimming white...\n");
  status = tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_ENABLE);
  memset(buffer.data, 0, TLC5947_LLD_BUFFER_SIZE);
  status |= tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
  status |= tlc5947_lld_update((TLC5947Driver*)ut->data);
  status |= tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_DISABLE);
  for (uint8_t bit = 0; bit < TLC5947_LLD_PWM_RESOLUTION_BITS; ++bit) {
    for (uint8_t channel = 0; channel < TLC5947_LLD_NUM_CHANNELS; ++channel) {
      tlc5947_lld_setBuffer(&buffer, channel, (1 << (bit+1)) - 1);
    }
    status |= tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
    status |= tlc5947_lld_update((TLC5947Driver*)ut->data);
    aosThdMSleep(100);
  }
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "NOT dimming black again...\n");
  status = APAL_STATUS_OK;
  for (uint8_t bit = TLC5947_LLD_PWM_RESOLUTION_BITS; bit > 0; --bit) {
    for (uint8_t channel = 0; channel < TLC5947_LLD_NUM_CHANNELS; ++channel) {
      tlc5947_lld_setBuffer(&buffer, channel, (1 << bit) - 1);
    }
    status |= tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_ENABLE);
    status |= tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
    status |= tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_ENABLE);
    // Grayscale data is not updated this time (tlc5947_lld_update() not called).
    // TODO: For some reason the PWMs are updated nevertheless.
    aosThdMSleep(100);
  }
  status |= tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_ENABLE);
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "setting each channel incrementally...\n");
  status = APAL_STATUS_OK;
  memset(buffer.data, 0, TLC5947_LLD_BUFFER_SIZE);
  status |= tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
  status |= tlc5947_lld_update((TLC5947Driver*)ut->data);
  status |= tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_DISABLE);
  for (uint8_t channel = 0; channel < TLC5947_LLD_NUM_CHANNELS; ++channel) {
    uint16_t value = 0x0000u;
    for (uint8_t byte = 0; byte < 3; ++byte) {
      switch (byte) {
        case 0:
          value |= 0x00Fu;
          break;
        case 1:
          value |= 0x0F0u;
          break;
        case 2:
          value |= 0xF00u;
          break;
      }
      tlc5947_lld_setBuffer(&buffer, channel, value);
      status |= tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
      status |= tlc5947_lld_update((TLC5947Driver*)ut->data);
      aosThdSleep(1.0f / 3.0f);
      tlc5947_lld_setBuffer(&buffer, channel, 0);
    }
  }
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "setting one color after another...\n");
  status = APAL_STATUS_OK;
  for (int8_t color = 0; color < 3; ++color) {
    for (uint8_t channel = color; channel < TLC5947_LLD_NUM_CHANNELS; channel += 3) {
      const uint16_t val = 0xAF5u; // some deterministic value with high entropy
      tlc5947_lld_setBuffer(&buffer, channel, val);
      status |= tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
      status |= tlc5947_lld_update((TLC5947Driver*)ut->data);
      status |= (tlc5947_lld_getBuffer(&buffer, channel) != val) ? (1 << 31) : 0;
      if (status != APAL_STATUS_OK) {
        break;
      } else {
        aosThdSleep(1.0f / 8.0f);
      }
    }
    for (uint8_t channel = 0; channel < TLC5947_LLD_NUM_CHANNELS; ++channel) {
      tlc5947_lld_setBuffer(&buffer, channel, 0);
    }
  }
  if (status == APAL_STATUS_OK) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  // turn LEDs off
  tlc5947_lld_setBlank((TLC5947Driver*)ut->data, TLC5947_LLD_BLANK_ENABLE);
  memset(buffer.data, 0, TLC5947_LLD_BUFFER_SIZE);
  tlc5947_lld_write((TLC5947Driver*)ut->data, &buffer);
  tlc5947_lld_update((TLC5947Driver*)ut->data);

  aosUtInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(TLC5947Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_TLC5947) */
