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

#include <ut_alld_pca9544a.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_PCA9544A)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_pca9544a.h>

aos_utresult_t utAlldPca9544aFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_pca9544adata_t*)ut->data)->driver != NULL);

  // local variables
  aos_utresult_t result = {0,0};
  uint32_t status;
  uint8_t ctrlreg;
  pca9544a_lld_intstatus_t interrupt;
  pca9544a_lld_chid_t channel;
  uint8_t test_mask = 0x00u;

  chprintf(stream, "reading control register...\n");
  status = pca9544a_lld_read(((ut_pca9544adata_t*)ut->data)->driver, &ctrlreg, ((ut_pca9544adata_t*)ut->data)->timeout);
  if (status == APAL_STATUS_OK || status == APAL_STATUS_WARNING) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "writing control register...\n");
  status = APAL_STATUS_OK;
  status |= pca9544a_lld_write(((ut_pca9544adata_t*)ut->data)->driver, (uint8_t)(PCA9544A_LLD_CTRLREG_EN), ((ut_pca9544adata_t*)ut->data)->timeout);
  status |= pca9544a_lld_read(((ut_pca9544adata_t*)ut->data)->driver, &ctrlreg, ((ut_pca9544adata_t*)ut->data)->timeout);
  if ((status == APAL_STATUS_OK  || status == APAL_STATUS_WARNING) && ctrlreg == PCA9544A_LLD_CTRLREG_EN) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X, 0x%X\n", status, ctrlreg);
  }

  chprintf(stream, "reading interrupt status...\n");
  status = pca9544a_lld_getintstatus(((ut_pca9544adata_t*)ut->data)->driver, &interrupt, ((ut_pca9544adata_t*)ut->data)->timeout);
  if (status == APAL_STATUS_OK || status == APAL_STATUS_WARNING) {
    aosUtPassedMsg(stream, &result, "0x%08X\n", interrupt);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "reading current channel...\n");
  status = pca9544a_lld_getcurrentchannel(((ut_pca9544adata_t*)ut->data)->driver, &channel, ((ut_pca9544adata_t*)ut->data)->timeout);
  if (status == APAL_STATUS_OK || status == APAL_STATUS_WARNING) {
    aosUtPassedMsg(stream, &result, "0x%08X\n", channel);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "setting current channel...\n");
  status = APAL_STATUS_OK;
  status |= pca9544a_lld_setchannel(((ut_pca9544adata_t*)ut->data)->driver, PCA9544A_LLD_CH0, ((ut_pca9544adata_t*)ut->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((ut_pca9544adata_t*)ut->data)->driver, &channel, ((ut_pca9544adata_t*)ut->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH0) ? 0x01u : 0x00u;
  status |= pca9544a_lld_setchannel(((ut_pca9544adata_t*)ut->data)->driver, PCA9544A_LLD_CH1, ((ut_pca9544adata_t*)ut->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((ut_pca9544adata_t*)ut->data)->driver, &channel, ((ut_pca9544adata_t*)ut->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH1) ? 0x02u : 0x00u;
  status |= pca9544a_lld_setchannel(((ut_pca9544adata_t*)ut->data)->driver, PCA9544A_LLD_CH2, ((ut_pca9544adata_t*)ut->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((ut_pca9544adata_t*)ut->data)->driver, &channel, ((ut_pca9544adata_t*)ut->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH2) ? 0x04u : 0x00u;
  status |= pca9544a_lld_setchannel(((ut_pca9544adata_t*)ut->data)->driver, PCA9544A_LLD_CH3, ((ut_pca9544adata_t*)ut->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((ut_pca9544adata_t*)ut->data)->driver, &channel, ((ut_pca9544adata_t*)ut->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH3) ? 0x08u : 0x00u;
  status |= pca9544a_lld_setchannel(((ut_pca9544adata_t*)ut->data)->driver, PCA9544A_LLD_CH_NONE, ((ut_pca9544adata_t*)ut->data)->timeout);
  status |= pca9544a_lld_getcurrentchannel(((ut_pca9544adata_t*)ut->data)->driver, &channel, ((ut_pca9544adata_t*)ut->data)->timeout);
  test_mask |= (channel != PCA9544A_LLD_CH_NONE) ? 0x10u : 0x00u;
  if ((status == APAL_STATUS_OK || status == APAL_STATUS_WARNING) && test_mask == 0x00u) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X, 0x%X\n", status, test_mask);
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(PCA9544ADriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_PCA9544A) */

