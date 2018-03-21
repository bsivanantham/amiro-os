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

#include <ut_alld_at24c01bn-sh-b.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_AT24C01BN)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>

/**
 * @brief   EEPROM address for page write/read access.
 */
#define _pageAddress                  (AT24C01BN_LLD_SIZE_BYTES - 2*AT24C01BN_LLD_PAGE_SIZE_BYTES)

/**
 * @brief   EEPROM address for byte write/read access.
 */
#define _byteAddress                  (_pageAddress + AT24C01BN_LLD_PAGE_SIZE_BYTES - 1)

/**
 * @brief   Offset for overflow tests.
 */
#define _overflowOffset               3

/**
 * @brief   EEPROM address for page write/read access with overflow.
 */
#define _overflowAddress              (_pageAddress + _overflowOffset)

/**
 * @brief   Test data byte.
 */
#define _bytedata                     0xA5

/**
 * @brief   AT24C01BN-SH-B unit test function.
 *
 * @param[in] stream  Stream for inout/outout.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldAt24c01bnFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_at24c01bndata_t*)(ut->data))->driver != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status = 0;
  uint8_t page_data[AT24C01BN_LLD_PAGE_SIZE_BYTES];
  uint8_t original_data[AT24C01BN_LLD_PAGE_SIZE_BYTES];
  uint8_t read_data[AT24C01BN_LLD_PAGE_SIZE_BYTES];
  size_t errors = 0;
  for (size_t dataIdx = 0; dataIdx < AT24C01BN_LLD_PAGE_SIZE_BYTES; ++dataIdx) {
    page_data[dataIdx] = dataIdx;
  }

  chprintf(stream, "reading byte...\n");
  status = at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, original_data, 1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  if (status == APAL_STATUS_SUCCESS || status == APAL_STATUS_WARNING) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing byte...\n");
  status = at24c01bn_lld_write_byte(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, _bytedata, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  while (at24c01bn_lld_poll_ack(((ut_at24c01bndata_t*)(ut->data))->driver, ((ut_at24c01bndata_t*)(ut->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, &read_data[0], 1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  status |= (read_data[0] == _bytedata) ? 0x00 : 0x20;
  status = at24c01bn_lld_write_byte(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, (uint8_t)~_bytedata, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  while (at24c01bn_lld_poll_ack(((ut_at24c01bndata_t*)(ut->data))->driver, ((ut_at24c01bndata_t*)(ut->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, &read_data[1], 1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  status |= (read_data[1] == (uint8_t)~_bytedata) ? 0x00 : 0x40;
  status |= at24c01bn_lld_write_byte(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, original_data[0], ((ut_at24c01bndata_t*)(ut->data))->timeout);
  while (at24c01bn_lld_poll_ack(((ut_at24c01bndata_t*)(ut->data))->driver, ((ut_at24c01bndata_t*)(ut->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _byteAddress, &read_data[2], 1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  status |= (read_data[2] == original_data[0]) ? 0x00 : 0x80;
  if (status == APAL_STATUS_SUCCESS || status == APAL_STATUS_WARNING) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading page...\n");
  status = at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _pageAddress, original_data, AT24C01BN_LLD_PAGE_SIZE_BYTES, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing page...\n");
  status = at24c01bn_lld_write_page(((ut_at24c01bndata_t*)(ut->data))->driver, _pageAddress, page_data, AT24C01BN_LLD_PAGE_SIZE_BYTES, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  while (at24c01bn_lld_poll_ack(((ut_at24c01bndata_t*)(ut->data))->driver, ((ut_at24c01bndata_t*)(ut->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _pageAddress, read_data, AT24C01BN_LLD_PAGE_SIZE_BYTES, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  errors = 0;
  for (size_t dataIdx = 0; dataIdx < AT24C01BN_LLD_PAGE_SIZE_BYTES; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "reading page at current address...\n");
  // set address counter to UT_ALLD_AT24C01BN_PAGE_ADDRESS
  status = at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _pageAddress-1, read_data, 1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  if (status == APAL_STATUS_WARNING) {
    // on STM32F1 platform the address gets incremented by 2 after reading
    status = at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _pageAddress-2, read_data, 1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  }
  // read page from the current address
  status = at24c01bn_lld_read_current_address(((ut_at24c01bndata_t*)(ut->data))->driver, read_data, AT24C01BN_LLD_PAGE_SIZE_BYTES, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < 8; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "writing page with overflow (7 bytes)...\n");
  status = at24c01bn_lld_write_page(((ut_at24c01bndata_t*)(ut->data))->driver, _overflowAddress, page_data, AT24C01BN_LLD_PAGE_SIZE_BYTES-1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  while (at24c01bn_lld_poll_ack(((ut_at24c01bndata_t*)(ut->data))->driver, ((ut_at24c01bndata_t*)(ut->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _overflowAddress, read_data, AT24C01BN_LLD_PAGE_SIZE_BYTES-_overflowOffset, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _overflowAddress-_overflowOffset, read_data+(AT24C01BN_LLD_PAGE_SIZE_BYTES-_overflowOffset), _overflowOffset-1, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < AT24C01BN_LLD_PAGE_SIZE_BYTES-1; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "writing page with overflow (8 bytes)...\n");
  status = at24c01bn_lld_write_page(((ut_at24c01bndata_t*)(ut->data))->driver, _overflowAddress, page_data, AT24C01BN_LLD_PAGE_SIZE_BYTES, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  while (at24c01bn_lld_poll_ack(((ut_at24c01bndata_t*)(ut->data))->driver, ((ut_at24c01bndata_t*)(ut->data))->timeout) == APAL_STATUS_FAILURE) {
    continue;
  }
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _overflowAddress, read_data, AT24C01BN_LLD_PAGE_SIZE_BYTES-_overflowOffset, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  status |= at24c01bn_lld_read(((ut_at24c01bndata_t*)(ut->data))->driver, _overflowAddress-_overflowOffset, read_data+(AT24C01BN_LLD_PAGE_SIZE_BYTES-_overflowOffset), _overflowOffset, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < AT24C01BN_LLD_PAGE_SIZE_BYTES; dataIdx++) {
    if (read_data[dataIdx] != page_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_OK && errors == 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X; %u\n", status, errors);
  }

  chprintf(stream, "write back original data...\n");
  status = at24c01bn_lld_write_page(((ut_at24c01bndata_t*)(ut->data))->driver, _pageAddress, original_data, AT24C01BN_LLD_PAGE_SIZE_BYTES, ((ut_at24c01bndata_t*)(ut->data))->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(AT24C01BNDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_AT24C01BN) */
