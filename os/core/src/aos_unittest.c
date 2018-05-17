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

#include <aos_unittest.h>

#if (AMIROOS_CFG_TESTS_ENABLE == true) || defined(__DOXYGEN__)
#include <aos_debug.h>
#include <chprintf.h>
#include <string.h>

/**
 * @brief   Retrieve the total number of tests (passed and failed).
 *
 * @param[in] result    The result object to evaluate.
 *
 * @return  Number of total tests executed.
 */
inline uint32_t aosUtResultTotal(aos_utresult_t *result)
{
  aosDbgCheck(result != NULL);

  return result->passed + result->failed;
}

/**
 * @brief   Retrieve the ratio of passed tests.
 *
 * @param[in] result    The result object to evaluate.
 *
 * @return  Ratio of passed tests to total tests as float in range [0, 1].
 */
inline float aosUtResultRatio(aos_utresult_t *result)
{
  aosDbgCheck(result != NULL);

  if (aosUtResultTotal(result) > 0) {
    return (float)result->passed / (float)aosUtResultTotal(result);
  } else {
    return 1.0f;
  }
}

/**
 * @brief   Print the summary of a test.
 * @details The summary consists of:
 *          - total numer of tests executed
 *          - absolute number of passed tests
 *          - absolute number of failed tests
 *          - relative ratio of passed tests
 *
 * @param[in] stream    Stream to print the result to.
 * @param[in] result    Result to evaluate and print.
 * @param[in] heading   Optional heading (defaults to "summary").
 */
void aosUtResultPrintSummary(BaseSequentialStream *stream, aos_utresult_t *result, char* heading)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  chprintf(stream, "%s:\n", (heading != NULL) ? heading : "summary");
  chprintf(stream, "\ttotal:  %3u\n", aosUtResultTotal(result));
  chprintf(stream, "\tpassed: %3u\n", result->passed);
  chprintf(stream, "\tfailed: %3u\n", result->failed);
  chprintf(stream, "\tratio:  %3u%%\n", (uint8_t)(aosUtResultRatio(result) * 100.0f));

  return;
}

/**
 * @brief   Initialize a unit test object.
 *
 * @param[in] ut          The unit test object to initialize.
 * @param[in] name        name of the unit test.
 * @param[in] info        Optional information string.
 * @param[in] func        Unit test calback function.
 * @param[in] data        Optional data for the unit test.
 * @param[in] shellname   Name of the shell command
 * @param[in] shellcb     Callback for the shell command.
 */
void aosUtObjectInit(aos_unittest_t* ut, char* name, char* info, aos_utfunction_t func, void* data, char* shellname, aos_shellcmdcb_t shellcb)
{
  aosDbgCheck(ut != NULL);
  aosDbgCheck(name != NULL && strlen(name) > 0);
  aosDbgCheck(func != NULL);
  aosDbgCheck(shellname != NULL && strlen(shellname) > 0);
  aosDbgCheck(shellcb != NULL);

  ut->name = name;
  ut->info = info;
  ut->testfunc = func;
  ut->shellcmd.name = shellname;
  ut->shellcmd.callback = shellcb;
  ut->shellcmd.next = NULL;
  ut->data = data;

  return;
}

/**
 * @brief   Run an unit test.
 *
 * @param[in] stream  A stream for printing messages.
 * @param[in] ut      Unit test to execute.
 * @param[in] note    Optional note string.
 *
 * @return    Result of the test.
 */
aos_utresult_t aosUtRun(BaseSequentialStream *stream, aos_unittest_t *ut, char *note)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(ut != NULL);

  // print name heading
  {
    chprintf(stream, "\n");
    const int nchars = chprintf(stream, "%s unit test\n", ut->name);
    for (int c = 0; c < nchars-1; ++c) {
      chprintf(stream, "=");
    }
    chprintf(stream, "\n");
  }

  // print info (if any)
  if (ut->info != NULL) {
    chprintf(stream, "info: %s\n", ut->info);
  }
  // print note (if any)
  if (note != NULL) {
    chprintf(stream, "note: %s\n", note);
  }
  chprintf(stream, "\n");

  // run test
  aos_utresult_t result = ut->testfunc(stream, ut);

  // print summary
  aosUtResultPrintSummary(stream, &result, NULL);

  return result;
}

/**
 * @brief   Helper function for passed tests.
 * @details Prints a message that the test was passed and modifies the result accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 */
void aosUtPassed(BaseSequentialStream *stream, aos_utresult_t* result)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  ++result->passed;
  chprintf(stream, "\tPASSED\n");
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for passed tests.
 * @details Prints a message that the test was passed, an additional custom message, and modifies the result accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 * @param[in] fmt         Formatted message string.
 */
void aosUtPassedMsg(BaseSequentialStream *stream, aos_utresult_t* result, const char *fmt, ...)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  va_list ap;

  ++result->passed;
  chprintf(stream, "\tPASSED\t");
  va_start(ap, fmt);
  chvprintf(stream, fmt, ap);
  va_end(ap);
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for failed tests.
 * @details Prints a message that the test was failed and modifies the result accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 */
void aosUtFailed(BaseSequentialStream *stream, aos_utresult_t* result)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  ++result->failed;
  chprintf(stream, "\tFAILED\n");
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for failed tests.
 * @details Prints a message that the test was failed, an additional custom message, and modifies the result accordigly.
 *
 * @param[in] stream      Stream to print the message to.
 * @param[in,out] result  Result object to modify.
 * @param[in] fmt         Formatted message string.
 */
void aosUtFailedMsg(BaseSequentialStream *stream, aos_utresult_t* result, const char *fmt, ...)
{
  aosDbgCheck(stream != NULL);
  aosDbgCheck(result != NULL);

  va_list ap;

  ++result->failed;
  chprintf(stream, "\tFAILED\t");
  va_start(ap, fmt);
  chvprintf(stream, fmt, ap);
  va_end(ap);
  chprintf(stream, "\n");

  return;
}

/**
 * @brief   Helper function for information messages.
 *
 * @param[in] stream  Strean to rpint the message to.
 * @param[in] fmt     Formatted message string.
 */
void aosUtInfoMsg(BaseSequentialStream* stream, const char* fmt, ...)
{
  aosDbgCheck(stream != NULL);

  va_list ap;
  va_start(ap, fmt);
  chvprintf(stream, fmt, ap);
  va_end(ap);
  chprintf(stream, "\n");

  return;
}

#endif /* AMIROOS_CFG_TESTS_ENABLE == true */
