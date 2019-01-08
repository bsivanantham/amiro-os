#ifndef AMIRO_GLOBAL_HPP_
#define AMIRO_GLOBAL_HPP_

#include <hal.h>

#include <board.h>
#include <amiro/bus/i2c/HWI2CDriver.hpp>
#include <amiro/bus/spi/HWSPIDriver.hpp>
#include <amiro/leds/tlc5947.hpp>
#include <amiro/eeprom/at24.hpp>
#include <amiro/FileSystemInputOutput/FSIOLightRing.hpp>
#include <LightRing.h>
#include <amiro/Lidar.h>
#include <amiro/serial_reset/serial_can_mux.hpp>
#include <userthread.hpp>
#include <complex>


namespace amiro {

class Global final
{
public:

  I2CConfig i2c2_config{
    /* I2C mode                 */ OPMODE_I2C,
    /* frequency                */ 400000,
    /* I2C fast mode duty cycle */ FAST_DUTY_CYCLE_2
  };

  SerialConfig sd1_config{
    /* speed        */ 115200,
    /* CR1 register */ 0,
    /* CR2 register */ 0,
    /* CR3 register */ 0
  };
  SerialConfig sd2_config{
    /* speed        */ 19200,
    /* CR1 register */ 0,
    /* CR2 register */ 0,
    /* CR3 register */ 0
  };

  SPIConfig spi1_config{
    /* callback function pointer   */ NULL,
    /* chip select line port       */ GPIOC,
    /* chip select line pad number */ GPIOC_LIGHT_XLAT,
    /* initialzation data          */ SPI_CR1_BR_0 | SPI_CR1_BR_1
  };


  /**
   * @brief I2C Bus 2
   * Conected devices:
   *   AT24Cxx
   */

  HWI2CDriver HW_I2C2;

  HWSPIDriver HW_SPI1;


  TLC5947 tlc5947;

  AT24 at24c01;
  fileSystemIo::FSIOLightRing memory;

  LightRing robot;

  SerialCanMux sercanmux1;

  Lidar lidar;

  UserThread userThread;

  uint8_t shellTermID;

  //I2S driver global implementation
  #define I2S_BUF_SIZE      2000
  //typedef std::complex<double> cplx;
  uint32_t i2s_rx_buf[I2S_BUF_SIZE];
  //uint32_t i2s_norm_rx_buf[I2S_NORM_BUF_SIZE];
  // uint32_t i2s_fft_buf[I2S_NORM_BUF_SIZE];

  //dummy function
  static void i2scallback(I2SDriver *i2sp, size_t offset, size_t n){
    (void)i2sp;
    (void)offset;
    (void)n;
    // chprintf((BaseSequentialStream*)&sercanmux1,"callback \n");
  }

  // (0 << 4) | (0 << 3) | (1 << 1) | (1 << 0),
  // 17<<0 | 1<<8

  I2SConfig i2scfg {
    NULL,
    i2s_rx_buf,
    I2S_BUF_SIZE,
    i2scallback,
    (0 << 11) |(0 << 10) | (0 << 9) | (0 << 8) | (0 << 7) | (0 << 5) | (0 << 4) | (1 << 3) | (1 << 2) | (0 << 1) | (1 << 0),
    (1 << 8) | (17 << 0)
  };


public:
  Global() :
    HW_I2C2(&I2CD2),
    HW_SPI1(&SPID1, &spi1_config),
    tlc5947(&HW_SPI1, GPIOA, GPIOA_LIGHT_BLANK),
    at24c01(0x400u / 0x08u, 0x08u, 500u, &HW_I2C2),
    memory(at24c01, /*BMSV*/ 1, /*bmsv*/ 2, /*HMV*/ 1, /*hmv*/ 0),
    robot(&CAND1, &tlc5947, &memory),
    sercanmux1(&SD1, &CAND1, CAN::LIGHT_RING_ID),
    lidar(CAN::LIGHT_RING_ID, Lidar::SETUP::POWER_ONLY),
    userThread()
  {
    return;
  }

  ~Global()
  {
    return;
  }
};

} // end of namespace amiro

#endif /* AMIRO_GLOBAL_HPP_ */
