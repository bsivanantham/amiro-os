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
// double i2s_fft_buf[I2S_fft_BUF_SIZE];

UserThread::UserThread() :
  chibios_rt::BaseStaticThread<USER_THREAD_STACK_SIZE>()
{
}

UserThread::~UserThread()
{
}

// void fourier_analysis() {
//   // chprintf((BaseSequentialStream*)&global.sercanmux1,"i, k, frequency, d1, absolute, i2s_fft_buf[k]\n");
//   double PI = 3.141592653589793238460;
//   for (size_t i = 0, k = 0; i < I2S_BUF_SIZE; i++) {
//     // uint32_t raw1 = global.i2s_rx_buf[i] & 0x0000FFFF;
//     // int16_t d1 = raw1 & 0x0000FFFF;
//
//     // double fft_val_real = 0.0;
//     // double fft_val_imag = 0.0;
//     // if(d1 != 0){
//     //   double absolute = 0.0;
//     //   if(k != 0 && k != I2S_fft_BUF_SIZE-1){
//     //     for (size_t j = 0, n = 0; j < I2S_BUF_SIZE; j ++) {
//     //       uint32_t raw = global.i2s_rx_buf[j];
//     //       int16_t d = raw & 0x0000FFFF;
//     //       if(d != 0){
//     //         Complex fft_val = std::polar(1.0, -2 * PI * k * n / (999)) * (d*1.0);
//     //         fft_val_real += real(fft_val);
//     //         fft_val_imag += imag(fft_val);
//     //         n++;
//     //       }
//     //     }
//     //     absolute = sqrt(pow(fft_val_real, 2.0) + pow(fft_val_imag, 2.0));
//     //   }
//     //   i2s_fft_buf[k] = absolute;
//     //   double frequency = (32000/999) * k;
//     //   chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%d,%f,%f\n", i, k, frequency, d1, absolute, i2s_fft_buf[k]);
//     //   k++;
//     // }
//     // chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d\n", i, d1);
//   }
// }

// void light_on(int from, int to, UserThread& u){
//   chprintf((BaseSequentialStream*)&global.sercanmux1,"Light off \n");
//   for(int i = from; i <= to; i++){
//     chprintf((BaseSequentialStream*)&global.sercanmux1,"%d \n", i);
//     u.sleep(MS2ST(100));
//     global.robot.setLightColor(i, Color(Color::BLACK));
//   }
// }
//
// void light_off(int n, UserThread& u){
//   chprintf((BaseSequentialStream*)&global.sercanmux1,"Light off \n");
//   for(int i = 7; i >= 8-n; i--){
//     chprintf((BaseSequentialStream*)&global.sercanmux1,"%d \n", i);
//     u.sleep(MS2ST(100));
//     global.robot.setLightColor(i, Color(Color::BLACK));
//   }
// }

// void light_show(UserThread& u){
//   chprintf((BaseSequentialStream*)&global.sercanmux1,"in light up \n");
//   for (size_t i = 1; i < I2S_fft_BUF_SIZE; i++) {
//     chprintf((BaseSequentialStream*)&global.sercanmux1,"in for loop \n");
//     double abs_fft = i2s_fft_buf[i];
//
//     // set the front LEDs to blue for one second
//     // global.robot.setLightColor(constants::LightRing::LED_SSW, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_WSW, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_WNW, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_NNE, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_ENE, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_ESE, Color(Color::WHITE));
//     // global.robot.setLightColor(constants::LightRing::LED_SSE, Color(Color::WHITE));
//
//     if(abs_fft > 5000){
//       chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 5000 amplitude \n");
//       double frequency = (32000/999) * i;
//       int light_no = 0;
//       if(frequency > 0 && frequency <= 4000){
//         light_off(7, u);
//         light_on(light_no,0,u);
//         light_no = 0;
//         // u.sleep(MS2ST(100));
//         chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 0 \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//       }
//       if(frequency > 4000){
//         light_off(6, u);
//         light_on(light_no,1,u);
//         light_no = 1;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 4k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//       }
//       if(frequency > 8000){
//         light_off(5, u);
//         light_on(light_no,2,u);
//         light_no = 2;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 8k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//         // global.robot.setLightColor(2, Color(Color::GREEN));
//       }
//       if(frequency > 12000){
//         light_off(4, u);
//         light_on(light_no,3,u);
//         light_no = 3;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 12k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//         // global.robot.setLightColor(2, Color(Color::GREEN));
//         // global.robot.setLightColor(3, Color(Color::GREEN));
//       }
//       if(frequency > 16000){
//         light_off(3, u);
//         light_on(light_no,4,u);
//         light_no = 4;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 16k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//         // global.robot.setLightColor(2, Color(Color::GREEN));
//         // global.robot.setLightColor(3, Color(Color::GREEN));
//         // global.robot.setLightColor(4, Color(Color::GREEN));
//       }
//       if(frequency > 20000){
//         light_off(2, u);
//         light_on(light_no,5,u);
//         light_no = 5;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 20k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//         // global.robot.setLightColor(2, Color(Color::GREEN));
//         // global.robot.setLightColor(3, Color(Color::GREEN));
//         // global.robot.setLightColor(4, Color(Color::GREEN));
//         // global.robot.setLightColor(5, Color(Color::GREEN));
//       }
//       if(frequency > 24000){
//         light_off(1, u);
//         light_on(light_no,6,u);
//         light_no = 7;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 24k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//         // global.robot.setLightColor(2, Color(Color::GREEN));
//         // global.robot.setLightColor(3, Color(Color::GREEN));
//         // global.robot.setLightColor(4, Color(Color::GREEN));
//         // global.robot.setLightColor(5, Color(Color::GREEN));
//         // global.robot.setLightColor(6, Color(Color::GREEN));
//       }
//       if(frequency > 28000 && frequency <= 32000){
//         light_off(0, u);
//         light_on(light_no,7,u);
//         light_no = 7;
//         // u.sleep(MS2ST(100));
//         // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 28k \n");
//         // global.robot.setLightColor(0, Color(Color::GREEN));
//         // global.robot.setLightColor(1, Color(Color::GREEN));
//         // global.robot.setLightColor(2, Color(Color::GREEN));
//         // global.robot.setLightColor(3, Color(Color::GREEN));
//         // global.robot.setLightColor(4, Color(Color::GREEN));
//         // global.robot.setLightColor(5, Color(Color::GREEN));
//         // global.robot.setLightColor(6, Color(Color::GREEN));
//         // global.robot.setLightColor(7, Color(Color::GREEN));
//       }
//     }
//   }
// }

msg_t
UserThread::main()
{

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

// //      //chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
// //      //kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};;

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

     chprintf((BaseSequentialStream*)&global.sercanmux1,"print data: \n");
     for (size_t i = 1; i < I2S_BUF_SIZE; i=i+2) {
        uint32_t raw = global.i2s_rx_buf[i];
        // uint32_t a_u32 = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
        // uint16_t d_u16 = raw & 0x0000FFFF;
        chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%08X\n",i,raw,raw);
     }

     this->sleep(MS2ST(100));
     this->sleep(MS2ST(100));
     this->sleep(MS2ST(100));
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
  // // this->sleep(MS2ST(1000));
  // // chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
  // // i2sStop(&I2SD2);
  // // chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
  // this->sleep(MS2ST(1000));
  // while (!this->shouldTerminate())
  // {
  //   chprintf((BaseSequentialStream*)&global.sercanmux1,"Print data \n");
  //   this->sleep(MS2ST(1000));
  //    //chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
  //    //kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};;
  //    for (size_t i = 1, j = 0; i < I2S_BUF_SIZE; i=i+2) {
  //       uint32_t raw = global.i2s_rx_buf[i];
  //       uint32_t a_32 = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
  //       // uint16_t c = a & 0x0000FFFF;
  //       // uint32_t raw1 = global.i2s_rx_buf[i] & 0x0000FFFF;
  //       uint16_t d1 = raw & 0x0000FFFF;
  //       // uint16_t d1 = a_32;
  //       //uint32_t a_24 = ((raw & 0x0000FFFF) << 8) | ((raw & 0xFF000000) >> 24);
  //
  //       chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%d,%d\n", i, j, a_32, d1);
  //       j++;
  //    }
  // }

  return RDY_OK;
}
