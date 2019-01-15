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

void light_on_n_m(int from, int to, const Color& color){
  for(int i = from; i <= to; i++){
    global.robot.setLightColor(i, Color(color));
  }
}

void light_off_n(int n){
  for(int i = 7; i >= 8-n; i--){
    global.robot.setLightColor(i, Color(Color::BLACK));
  }
}

void fourier_analysis() {
  chprintf((BaseSequentialStream*)&global.sercanmux1,"i, k, frequency, d1, d1, i2s_fft_buf[k]\n");
  double PI = 3.141592653589793238460;
  bool color_switch = false;
  for (size_t i = 1, k = 0; i < I2S_BUF_SIZE; i = i + 2) {
    if(color_switch){
      color_switch = false;
      light_on_n_m(0, 7, Color::DARKRED);
    } else {
      color_switch = true;
      light_on_n_m(0, 7, Color::BLUE);
    }
    uint32_t raw1 = global.i2s_rx_buf[i] /*& 0x0000FFFF*/;
    int16_t d1 = raw1 & 0x0000FFFF;

    double fft_val_real = 0.0;
    double fft_val_imag = 0.0;
    if(d1 == 0){;
      d1 = (raw1 >> 16) & 0x0000FFFF;
    }
    double absolute = 0.0;
    if(k != 0 && k != I2S_fft_BUF_SIZE-1){
      for (size_t j = 1, n = 0; j < I2S_BUF_SIZE; j = j + 2) {
        uint32_t raw = global.i2s_rx_buf[j];
        int16_t d = raw & 0x0000FFFF;
        if(d == 0){
          d = (raw >> 16) & 0x0000FFFF;
          // chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%08X\n,", i, d);
        }
        Complex fft_val = std::polar(1.0, -2 * PI * k * n / (I2S_fft_BUF_SIZE-1)) * (d*1.0);
        fft_val_real += real(fft_val);
        fft_val_imag += imag(fft_val);
        n++;
      }
      absolute = sqrt(pow(fft_val_real, 2.0) + pow(fft_val_imag, 2.0));
    }
    i2s_fft_buf[k] = absolute;
    double frequency = (32000/(double)(I2S_fft_BUF_SIZE-1)) * (double)k;
    chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%d,%08X,%f\n", i, k, frequency, d1, d1, i2s_fft_buf[k]);
    k++;
    //chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d\n", i, d1);
  }
  light_off_n(8);
}

void light_on_n_m_100ms(int from, int to, UserThread& u,const Color& color){
  for(int i = from; i <= to; i++){
    u.sleep(MS2ST(100));
    global.robot.setLightColor(i, Color(color));
  }
}

void light_off_n_100ms(int n, UserThread& u){
  for(int i = 7; i >= 8-n; i--){
    u.sleep(MS2ST(100));
    global.robot.setLightColor(i, Color(Color::BLACK));
  }
}

void light_show(UserThread& u){
  for (size_t i = 0; i < I2S_fft_BUF_SIZE; i++) {
    double abs_fft = i2s_fft_buf[i];
    int32_t highWheelSpeed = 100000000;
    int32_t lowWheelSpeed = 60000000;
    if(i == 499){
      chprintf((BaseSequentialStream*)&global.sercanmux1,"go backward \n");
      highWheelSpeed *= -1;
      lowWheelSpeed *= -1;
    }

    // set the front LEDs to blue for one second
    // global.robot.setLightColor(constants::LightRing::LED_SSW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_WSW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_WNW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_NNE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_ENE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_ESE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_SSE, Color(Color::WHITE));
    if(abs_fft > 2000){
      // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 5000 amplitude \n");
      double frequency = (32000/(double)(I2S_fft_BUF_SIZE-1)) * (double)i;
      int light_no = 0;
      if(frequency > 0 && frequency <= 4000){
        light_off_n_100ms(7, u);
        light_on_n_m_100ms(light_no,0,u,Color::GREEN);
        global.robot.setTargetSpeed(highWheelSpeed,lowWheelSpeed);
        light_no = 0;
        // u.sleep(MS2ST(100));
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 0 \n");
      }
      if(frequency > 4000){
        light_off_n_100ms(6, u);
        light_on_n_m_100ms(light_no,1,u,Color::GREEN);
        global.robot.setTargetSpeed(lowWheelSpeed,highWheelSpeed);
        light_no = 1;
      }
      if(frequency > 8000){
        light_off_n_100ms(5, u);
        light_on_n_m_100ms(light_no,2,u,Color::GREEN);
        global.robot.setTargetSpeed(highWheelSpeed,lowWheelSpeed);
        light_no = 2;
        // u.sleep(MS2ST(100));
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 8k \n");
      }
      if(frequency > 12000){
        light_off_n_100ms(4, u);
        light_on_n_m_100ms(light_no,3,u,Color::GREEN);
        global.robot.setTargetSpeed(lowWheelSpeed,highWheelSpeed);
        light_no = 3;
        // u.sleep(MS2ST(100));
        chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 12k \n");
      }
      if(frequency > 16000){
        light_off_n_100ms(3, u);
        light_on_n_m_100ms(light_no,4,u,Color::GREEN);
        global.robot.setTargetSpeed(highWheelSpeed,lowWheelSpeed);
        light_no = 4;
        // u.sleep(MS2ST(100));
        // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 16k \n");
      }
      if(frequency > 20000){
        light_off_n_100ms(2, u);
        light_on_n_m_100ms(light_no,5,u,Color::GREEN);
        global.robot.setTargetSpeed(lowWheelSpeed,highWheelSpeed);
        light_no = 5;
        // u.sleep(MS2ST(100));
        // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 20k \n");
      }
      if(frequency > 24000){
        light_off_n_100ms(1, u);
        light_on_n_m_100ms(light_no,6,u,Color::GREEN);
        global.robot.setTargetSpeed(highWheelSpeed,lowWheelSpeed);
        light_no = 7;
        // u.sleep(MS2ST(100));
        // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 24k \n");
      }
      if(frequency > 28000 && frequency <= 32000){
        light_off_n_100ms(0, u);
        light_on_n_m_100ms(light_no,7,u,Color::GREEN);
        global.robot.setTargetSpeed(lowWheelSpeed,highWheelSpeed);
        light_no = 7;
        // u.sleep(MS2ST(100));
        // chprintf((BaseSequentialStream*)&global.sercanmux1,"more than 28k \n");
      }
    }
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"less than 5000 amplitude \n");
  }
}

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

//   while (!this->shouldTerminate())
//   {
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"inside While \n");
//
// // //      //chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
// // //      //kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};;
//
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"after init - before sart i2s \n");
//      i2sStart(&I2SD2, &global.i2scfg);
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"afterstart I2S -> sleep 1s \n");
//      this->sleep(MS2ST(1000));
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"before start DMA \n");
//      i2sStartExchange(&I2SD2);
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"after start DMA \n");
//      for(int i=0; i<56; i++){
//        chprintf((BaseSequentialStream*)&global.sercanmux1,"sleep %ds \n",i+1);
//        this->sleep(MS2ST(1000));
//      }
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop DMA \n");
//      i2sStopExchange(&I2SD2);
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"after stop DMA -> sleep 1s \n");
//      this->sleep(MS2ST(1000));
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
//      i2sStop(&I2SD2);
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
//      this->sleep(MS2ST(1000));
//
//      chprintf((BaseSequentialStream*)&global.sercanmux1,"print data: \n");
//      for (size_t i = 1; i < I2S_BUF_SIZE; i=i+2) {
//         uint32_t raw = global.i2s_rx_buf[i];
//         // uint32_t a_u32 = ((raw & 0x0000FFFF) << 16) | ((raw & 0xFFFF0000) >> 16);
//         // uint16_t d_u16 = raw & 0x0000FFFF;
//         chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%08X\n",i,raw,raw);
//      }
//
//      this->sleep(MS2ST(100));
//      this->sleep(MS2ST(100));
//      this->sleep(MS2ST(100));
//   }

  /**
  * record sound and print
  */
  chprintf((BaseSequentialStream*)&global.sercanmux1,"starting robot \n");
  this->sleep(MS2ST(1000));
  chprintf((BaseSequentialStream*)&global.sercanmux1,"before init \n");
  i2sInit();
  chprintf((BaseSequentialStream*)&global.sercanmux1,"init \n");
  int cycle = 0;
  while (!this->shouldTerminate())
  {
    i2sStart(&I2SD2, &global.i2scfg);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"start I2S -> sleep 1s \n");
    this->sleep(MS2ST(1000));
    chprintf((BaseSequentialStream*)&global.sercanmux1,"before start DMA \n");
    i2sStartExchange(&I2SD2);
    for(int i=1,k=0,j = 0; i<=56; i++){
      if(j == 6){
        global.robot.setLightColor(k, Color(Color::RED));
      }else if(j%2 == 0){
        global.robot.setLightColor(k, Color(Color::YELLOW));
      } else{
        global.robot.setLightColor(k, Color(Color::BLACK));
      }
      chprintf((BaseSequentialStream*)&global.sercanmux1,"sleep %ds k:%d j:%d \n",i,k,j);
      k++;
      if(i%8 == 0){
        k =0;
        j++;
      }
      this->sleep(MS2ST(1000));
    }
    light_off_n(8);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop DMA \n");
    i2sStopExchange(&I2SD2);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"stop DMA -> sleep 1s \n");
    this->sleep(MS2ST(1000));
    chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
    i2sStop(&I2SD2);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
    this->sleep(MS2ST(1000));
    chprintf((BaseSequentialStream*)&global.sercanmux1,"Cycle %d\n", cycle);

    fourier_analysis();
    cycle++;
    //ligh show
    for(int i=0; i<5; i++){
      light_show(*this);
    }

    //stop robot movement
    global.robot.setTargetSpeed(0,0);

    //sleep before restart recording
    for(int i=0; i<5; i++){
      light_off_n(8);
      this->sleep(MS2ST(500));
      light_on_n_m(0,7,Color::BISQUE);
      this->sleep(MS2ST(500));
      chprintf((BaseSequentialStream*)&global.sercanmux1,"%ds\n", i+1);
    }
  }

  return RDY_OK;
}
