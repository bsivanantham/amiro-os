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
#include <amiro/Color.h>
#include <amiro/Constants.h>

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
  chprintf((BaseSequentialStream*)&global.sercanmux1,"k, d1, absolute, i2s_fft_buf[k]\n");
  double PI = 3.141592653589793238460;
  for (size_t i = 1, k = 0; i < I2S_BUF_SIZE; i += 2) {
    uint32_t raw1 = global.i2s_rx_buf[i] & 0x0000FFFF;
    int16_t d1 = raw1 & 0x0000FFFF;

    double fft_val_real = 0.0;
    double fft_val_imag = 0.0;
    for (size_t j = 1, n = 0; j < I2S_BUF_SIZE; j += 2) {
      uint32_t raw = global.i2s_rx_buf[j];
      int16_t d = raw & 0x0000FFFF;
      Complex fft_val = std::polar(1.0, -2 * PI * k * n / (999)) * (d*1.0);
      fft_val_real += real(fft_val);
      fft_val_imag += imag(fft_val);
      n++;
    }
    double absolute = sqrt(pow(fft_val_real, 2.0) + pow(fft_val_imag, 2.0));
    i2s_fft_buf[k] = absolute;
    chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%f\n", k, d1, absolute, i2s_fft_buf[k]); //(k->value, raw(d1)->data, absolute or i2i2s_fft_buf -> complex)
    k++;
  }
}

void light_up(){
  chprintf((BaseSequentialStream*)&global.sercanmux1,"in light up \n");
  for (size_t i = 1; i < I2S_fft_BUF_SIZE; i++) {
    chprintf((BaseSequentialStream*)&global.sercanmux1,"in for loop \n");
    double abs_fft = i2s_fft_buf[i];

    // set the front LEDs to blue for one second
    global.robot.setLightColor(constants::LightRing::LED_SSW, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_WSW, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_WNW, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_NNE, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_ENE, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_ESE, Color(Color::WHITE));
    global.robot.setLightColor(constants::LightRing::LED_SSE, Color(Color::WHITE));

    if(abs_fft > 5000){
      chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 5000 amplitude \n");
      double frequency = (32000/999) * i;

      if(frequency > 0 && frequency <= 4000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 0 \n");
        global.robot.setLightColor(constants::LightRing::LED_SSW, Color(Color::GREEN));
      }
      if(frequency > 4000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 4k \n");
        global.robot.setLightColor(constants::LightRing::LED_WSW, Color(Color::GREEN));
      }
      if(frequency > 8000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 8k \n");
        global.robot.setLightColor(constants::LightRing::LED_WNW, Color(Color::GREEN));
      }
      if(frequency > 12000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 12k \n");
        global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::GREEN));
      }
      if(frequency > 16000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 16k \n");
        global.robot.setLightColor(constants::LightRing::LED_NNE, Color(Color::GREEN));
      }
      if(frequency > 20000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 20k \n");
        global.robot.setLightColor(constants::LightRing::LED_ENE, Color(Color::GREEN));
      }
      if(frequency > 24000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 24k \n");
        global.robot.setLightColor(constants::LightRing::LED_ESE, Color(Color::GREEN));
      }
      if(frequency > 28000 && frequency <= 32000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 28k \n");
        global.robot.setLightColor(constants::LightRing::LED_SSE, Color(Color::GREEN));
      }
    }
  }
}

msg_t
UserThread::main()
{
  //global.robot.setLightColor(0, Color(Color::GREEN));
  global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::ORANGE));

  /**
  * continuous record sound and print
  */
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"starting robot \n");
  // this->sleep(MS2ST(1000));
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"before init \n");
  // i2sInit();
  while (!this->shouldTerminate())
  {
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"inside While \n");
     //chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
     //kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};;

//***///
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"after init - before sart i2s \n");
     // i2sStart(&I2SD2, &global.i2scfg);
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"afterstart I2S -> sleep 1s \n");
     // this->sleep(MS2ST(1000));
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"before start DMA \n");
     // i2sStartExchange(&I2SD2);
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"after start DMA \n");
     // for(int i=0; i<56; i++){
     //   chprintf((BaseSequentialStream*)&global.sercanmux1,"sleep %ds \n",i+1);
     //   this->sleep(MS2ST(1000));
     // }
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop DMA \n");
     // i2sStopExchange(&I2SD2);
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"after stop DMA -> sleep 1s \n");
     // this->sleep(MS2ST(1000));
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
     // i2sStop(&I2SD2);
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
     // this->sleep(MS2ST(1000));
     //
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"print data \n");
     //**///
     //fourier_analysis_two_channel();
     //light_up();
     // global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::ORANGE));
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"print data: %d\n",constants::LightRing::LED_NNW);

     // for (size_t i = 1; i < I2S_BUF_SIZE; i=i+2) {
     //    uint32_t raw = global.i2s_rx_buf[i];
     //    // uint32_t a = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
     //    // int32_t b = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
     //    uint16_t c = raw & 0x0000FFFF;
     //    // int16_t d = raw & 0x0000FFFF;
     //
     //    chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d\n",i,c);
     // }
     // double absolute = sqrt(pow(4, 2.0) + pow(0, 2.0));
     // chprintf((BaseSequentialStream*)&global.sercanmux1,"square_rooz: %f\n",absolute);
     // this->sleep(MS2ST(1000));
     // this->sleep(MS2ST(1000));
     // this->sleep(MS2ST(1000));
     // this->sleep(MS2ST(1000));
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
