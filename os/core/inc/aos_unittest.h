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

#ifndef _AMIROOS_UNITTEST_H_
#define _AMIROOS_UNITTEST_H_

#include <aosconf.h>
#include <hal.h>
#include <aos_shell.h>

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)

/*
 * Forward declarations.
 */
typedef struct aos_unittest aos_unittest_t;
typedef struct aos_utresult aos_utresult_t;

/**
 * @brief   Unit test interface function definition.
 *
 * @param[in] stream  The stream to use for printing messages.
 * @param[in] ut      The object to run the test on.
 *
 * @return    A result containing the number of passed and failed tests.
 */
typedef aos_utresult_t (*aos_utfunction_t)(BaseSequentialStream* stream, aos_unittest_t* ut);

/**
 * @brief   Unit test result struct.
 */
struct aos_utresult {
  /**
   * @brief   Number of passed tests.
   */
  uint32_t passed;

  /**
   * @brief   Number of failed tests.
   */
  uint32_t failed;
};

/**
 * @brief   Unit test definition struct.
 */
struct aos_unittest {
  /**
   * @brief   Name of the unit test.
   */
  const char* name;

  /**
   * @brief   Further information about the test.
   */
  const char* info;

  /**
   * @brief   Callback function to run that executes the unit test.
   */
  aos_utfunction_t testfunc;

  /**
   * @brief   Shell command to add to an shell command list.
   */
  aos_shellcommand_t shellcmd;

  /**
   * @brief   Further test specific data.
   */
  void* data;
};

#ifdef __cplusplus
extern "C" {
#endif
  uint32_t aosUtResultTotal(aos_utresult_t* result);
  float aosUtResultRatio(aos_utresult_t* result);
  void aosUtResultPrintSummary(BaseSequentialStream* stream, aos_utresult_t* result, char* heading);
  void aosUtObjectInit(aos_unittest_t* ut, char* name, char* info, aos_utfunction_t func, void* data, char* shellname, aos_shellcmdcb_t shellcb);
  aos_utresult_t aosUtRun(BaseSequentialStream* stream, aos_unittest_t* ut, char* note);
  void aosUtPassed(BaseSequentialStream* stream, aos_utresult_t* result);
  void aosUtPassedMsg(BaseSequentialStream* stream, aos_utresult_t* result, const char* fmt, ...);
  void aosUtFailed(BaseSequentialStream* stream, aos_utresult_t* result);
  void aosUtFailedMsg(BaseSequentialStream* stream, aos_utresult_t* result, const char* fmt, ...);
  void aosUtInfoMsg(BaseSequentialStream* stream, const char* fmt, ...);
#ifdef __cplusplus
}
#endif

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */

#endif /* _AMIROOS_UNITTEST_H_ */
