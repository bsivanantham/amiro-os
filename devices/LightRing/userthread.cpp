// #include <iostream>
// #include <fstream>
#include "userthread.hpp"
#include <chprintf.h>

#include <hal.h>
#include <board.h>
#include <complex.h>
#include <valarray>
#include <math.h>
#include "global.hpp"

using namespace amiro;

extern Global global;
types::kinematic kinematic;

typedef std::complex<double> Complex;
#define I2S_fft_BUF_SIZE      1000
double i2s_fft_buf[I2S_fft_BUF_SIZE];

UserThread::UserThread() :
  chibios_rt::BaseStaticThread<USER_THREAD_STACK_SIZE>()
{
}

UserThread::~UserThread()
{
}

// we should use float not double
  double fourier_analysis_one_channel(int16_t x,size_t i) {
  double PI = 3.141592653589793238460;
  Complex fft_val = std::polar(1.0, -2 * PI * i / (I2S_fft_BUF_SIZE)) * (x*1.0);
  double absolute_fft_val = sqrt(pow(real(fft_val),2.0) + pow(imag(fft_val),2.0));
  return absolute_fft_val;

}

void fourier_analysis_two_channel() {
double PI = 3.141592653589793238460;
for (size_t i = 1, k = 0; i < I2S_BUF_SIZE; i += 2) {
  uint32_t raw1 = global.i2s_rx_buf[i] & 0x0000FFFF;
  int16_t d1 = raw1 & 0x0000FFFF;

  double fft_val_real = 0.0;
  double fft_val_imag = 0.0;
  for (size_t j = 1, n = 0; j < I2S_BUF_SIZE; j += 2) {
    uint32_t raw = global.i2s_rx_buf[j];
    int16_t d = raw & 0x0000FFFF;
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"k= %d, n= %d, data= %d\n", k, n, d);
    Complex fft_val = std::polar(1.0, -2 * PI * k * n / (999)) * (d*1.0);
    fft_val_real += real(fft_val);
    fft_val_imag += imag(fft_val);
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"real= %f, imag= %f\n", real(fft_val), imag(fft_val));
    n++;
  }
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"real= %f, imag= %f\n", fft_val_real, fft_val_imag);
   double absolute = sqrt(pow(fft_val_real, 2.0) + pow(fft_val_imag, 2.0));
  i2s_fft_buf[k] = absolute;
  chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%f\n", k, d1, absolute, i2s_fft_buf[k]);
  k++;
}
//double absolute_fft_val ;
//return absolute_fft_val;
}

msg_t
UserThread::main()
{
  //global.robot.setLightColor(0, Color(Color::GREEN));

  /**
  * continuous record sound and print
  */
  chprintf((BaseSequentialStream*)&global.sercanmux1,"starting robot \n");
  this->sleep(MS2ST(1000));
  chprintf((BaseSequentialStream*)&global.sercanmux1,"before init \n");
  i2sInit();
  while (!this->shouldTerminate())
  {
    chprintf((BaseSequentialStream*)&global.sercanmux1,"inside While \n");
     //chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
     //kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};;

     chprintf((BaseSequentialStream*)&global.sercanmux1,"after init - before sart i2s \n");
     i2sStart(&I2SD2, &global.i2scfg);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"afterstart I2S -> sleep 1s \n");
     this->sleep(MS2ST(1000));
     chprintf((BaseSequentialStream*)&global.sercanmux1,"before start DMA \n");
     i2sStartExchange(&I2SD2);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"after start DMA \n");
     for(int i=0; i<56; i++){
       chprintf((BaseSequentialStream*)&global.sercanmux1,"sleep %ds \n",i+1);
       this->sleep(MS2ST(1000));
     }
     chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop DMA \n");
     i2sStopExchange(&I2SD2);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"after stop DMA -> sleep 1s \n");
     this->sleep(MS2ST(1000));
     chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
     i2sStop(&I2SD2);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
     this->sleep(MS2ST(1000));

     chprintf((BaseSequentialStream*)&global.sercanmux1,"print data \n");
     fourier_analysis_two_channel();
     // for (size_t i = 0; i < I2S_BUF_SIZE; i++) {
        // uint32_t raw = global.i2s_rx_buf[i];
        // uint32_t a = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
        // int32_t b = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
        // uint16_t c = raw & 0x0000FFFF;
        // int16_t d = raw & 0x0000FFFF;
        // double fa = fourier_analysis_one_channel(d,i);
        // double fb = fourier_analysis_two_channel(d,i);

        //uint32_t a_24 = ((raw & 0x0000FFFF) << 8) | ((raw & 0xFF000000) >> 24);

        // chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%f\n", i, d, fa, fb);
     // }
     this->sleep(MS2ST(1000));
     this->sleep(MS2ST(1000));
     this->sleep(MS2ST(1000));
  }

  /**
  * record sound and print
  */
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"starting robot \n");
  // this->sleep(MS2ST(1000));
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"before init \n");
  // i2sInit();
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"init \n");
  // i2sStart(&I2SD2, &global.i2scfg);
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"start I2S -> sleep 1s \n");
  // this->sleep(MS2ST(1000));
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"before start DMA \n");
  // i2sStartExchange(&I2SD2);
  // for(int i=0; i<56; i++){
  //   chprintf((BaseSequentialStream*)&global.sercanmux1,"sleep %ds \n",i+1);
  //   this->sleep(MS2ST(1000));
  // }
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop DMA \n");
  // i2sStopExchange(&I2SD2);
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"stop DMA -> sleep 1s \n");
  // this->sleep(MS2ST(1000));
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
  // i2sStop(&I2SD2);
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
  // this->sleep(MS2ST(1000));
  // while (!this->shouldTerminate())
  // {
  //   chprintf((BaseSequentialStream*)&global.sercanmux1,"Print data \n");
  //   this->sleep(MS2ST(1000));
  //    //chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
  //    //kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};;
  //    for (size_t i = 0; i < I2S_BUF_SIZE; i++) {
  //       uint32_t raw = global.i2s_rx_buf[i];
  //       uint32_t a = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
  //       //uint32_t a_24 = ((raw & 0x0000FFFF) << 8) | ((raw & 0xFF000000) >> 24);
  //
  //       chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%08X\n", i, a, a);
  //    }
  // }

  return RDY_OK;
}
