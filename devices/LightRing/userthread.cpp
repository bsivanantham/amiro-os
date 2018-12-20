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
//Complex i2s_fft_buf[I2S_fft_BUF_SIZE];

UserThread::UserThread() :
  chibios_rt::BaseStaticThread<USER_THREAD_STACK_SIZE>()
{
}

UserThread::~UserThread()
{
}

// double normalization(int32_t x) {
//     int32_t temp = x;
//     if(temp > -2500 && temp < 2500){
//       return temp;
//     }
//   }

// we should use float not double
  Complex fourier_analysis_one_channel(int16_t x,size_t i) {
  double PI = 3.141592653589793238460;
  return std::polar(1.0, -2 * PI * i / (I2S_fft_BUF_SIZE)) * (x*1.0);
  // i2s_fft_buf[i / 2] = i2s_norm_rx_buf[i] + t;
	// i2s_fft_buf[(i + I2S_NORM_BUF_SIZE)/2] = i2s_norm_rx_buf[i] - t;
}

Complex fourier_analysis_two_channel(int16_t x,size_t i) {
double PI = 3.141592653589793238460;
return std::polar(1.0, -2 * PI * i / (I2S_BUF_SIZE)) * (x*1.0);
// i2s_fft_buf[i / 2] = i2s_norm_rx_buf[i] + t;
// i2s_fft_buf[(i + I2S_NORM_BUF_SIZE)/2] = i2s_norm_rx_buf[i] - t;
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
     for (size_t i = 0; i < I2S_BUF_SIZE; i++) {
        uint32_t raw = global.i2s_rx_buf[i];
        // uint32_t a = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
        // int32_t b = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
        // uint16_t c = raw & 0x0000FFFF;
        int16_t d = raw & 0x0000FFFF;
        Complex fa = fourier_analysis_one_channel(d,i);
        Complex fb = fourier_analysis_two_channel(d,i);

        //uint32_t a_24 = ((raw & 0x0000FFFF) << 8) | ((raw & 0xFF000000) >> 24);

        chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%fi,%f,%fi\n", i, d, real(fa), imag(fa), real(fb), imag(fb));
     }
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
