#include "userthread.hpp"
#include <chprintf.h>

#include <hal.h>
#include "global.hpp"

using namespace amiro;

extern Global global;

UserThread::UserThread() :
  chibios_rt::BaseStaticThread<USER_THREAD_STACK_SIZE>()
{
}

UserThread::~UserThread()
{
}

msg_t
UserThread::main()
{

  //i2sStartExchange(&I2SD2);
  while (!this->shouldTerminate())
  {
    /*
     * INSERT CUSTOM CODE HERE
     */
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"checking \n");
    i2sStartExchange(&I2SD2);
    //for (size_t i = 0; i < I2S_BUF_SIZE; i++) {
    chprintf((BaseSequentialStream*)&global.sercanmux1,"i2s_tx_buf: %u \n", global.i2s_tx_buf);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"i2s_rx_buf: %u \n", global.i2s_rx_buf);
    //}
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"0x%" PRIXPTR "\n", SPI_I2SCFGR_DATLEN);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"i2sp->spi->I2SCFGR: %u \n", I2SD2.spi->I2SCFGR);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"SPI_I2SCFGR_DATLEN_0: %u \n", SPI_I2SCFGR_DATLEN_0);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"SPI_I2SCFGR_DATLEN_1: %u \n", SPI_I2SCFGR_DATLEN_1);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"SPI_I2SPR_MCKOE: %u \n", SPI_I2SPR_MCKOE);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"SPI_I2SCFGR_I2SE: %u \n", SPI_I2SCFGR_I2SE);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"SPI_I2SCFGR_CKPOL: %u \n", SPI_I2SCFGR_CKPOL);
    this->sleep(MS2ST(10000));
    i2sStopExchange(&I2SD2);

  }

  // i2sStopExchange(&I2SD2);
  // i2sStop(&I2SD2)

  return RDY_OK;
}
