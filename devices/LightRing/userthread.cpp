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
const Color colors[] = {Color::RED,Color::ORANGE,Color::YELLOW,Color::GREEN,Color::BLUE,Color::INDIGO,Color::VIOLET};

void light_on_n_m(int from, int to, UserThread& u, const Color& color);
void light_on_n_m_100ms(int from, int to, UserThread& u,const Color& color);
void light_off_n(int n, UserThread& u);
void light_off_n_100ms(int n, UserThread& u);
void light_show(UserThread& u, double abs_fft, double frequency, int i);
void light_show(UserThread& u);
void fourier_analysis(UserThread& u);

UserThread::UserThread() :
  chibios_rt::BaseStaticThread<USER_THREAD_STACK_SIZE>()
{
}

UserThread::~UserThread()
{
}

void light_on_n_m(int from, int to, UserThread& u, const Color& color){
  for(int i = from; i <= to; i++){
    global.robot.setLightColor(i, Color(colors[i]));
  }
}

void light_on_n_m_100ms(int from, int to, UserThread& u,const Color& color){
  for(int i = from; i <= to; i++){
    u.sleep(MS2ST(100));
    global.robot.setLightColor(i, Color(colors[i]));
  }
}

void light_off_n(int n, UserThread& u){
  for(int i = 7; i >= 8-n; i--){
    global.robot.setLightColor(i, Color(Color::BLACK));
  }
}

void light_off_n_100ms(int n, UserThread& u){
  for(int i = 7; i >= 8-n; i--){
    u.sleep(MS2ST(100));
    global.robot.setLightColor(i, Color(Color::BLACK));
  }
}

bool Motor_Dance_Schema_1(int i,bool is_negative){
  int32_t highWheelSpeed = 30 * 1000000;
  int32_t lowWheelSpeed = 10 * 1000000;
  if(is_negative){
    highWheelSpeed *= -1;
    lowWheelSpeed *= -1;
  }
  if(i%21 == 0){
    if(i != 0 && is_negative){
      is_negative = false;
    } else if(i != 0 && !is_negative){
      is_negative = true;
    }
    global.robot.setTargetSpeed(highWheelSpeed,lowWheelSpeed);

  } else if(i%14 == 0){
    global.robot.setTargetSpeed(highWheelSpeed,lowWheelSpeed);
  } else if(i%7 == 0){
    global.robot.setTargetSpeed(lowWheelSpeed,highWheelSpeed);
  }
   return is_negative;
}

void Motor_Dance_Schema_2(UserThread& u){
  int32_t highWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(highWheelSpeed,0);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // global.robot.setTargetSpeed(0,0);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
}

void Motor_Dance_Schema_3(UserThread& u){
  int32_t highWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(0,highWheelSpeed);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // global.robot.setTargetSpeed(0,0);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
}

void Motor_Dance_Schema_4(UserThread& u){
  int32_t MoveWheelSpeed = 20 * 1000000;
  int32_t RotateWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(MoveWheelSpeed,MoveWheelSpeed);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  global.robot.setTargetSpeed(RotateWheelSpeed,0);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // global.robot.setTargetSpeed(0,0);
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
  // u.sleep(MS2ST(1000));
}

bool light_show(UserThread& u, double abs_fft, double frequency, int i, bool is_negative){
  int light_no = 0;
  is_negative = Motor_Dance_Schema_1(i,is_negative);
    // set the front LEDs to blue for one second
    // global.robot.setLightColor(constants::LightRing::LED_SSW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_WSW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_WNW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_NNE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_ENE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_ESE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_SSE, Color(Color::WHITE));
    if(abs_fft > 4500){
      if(frequency > 0 && frequency <= 2000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 0K \n");
        global.robot.setLightBrightness(12);
        light_off_n(7, u);
        light_on_n_m(light_no,0,u,Color::RED);
        light_no = 0;
      }
      if(frequency > 2000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 2K \n");
        global.robot.setLightBrightness(24);
        light_off_n(6, u);
        light_on_n_m(light_no,1,u,Color::ORANGE);
        light_no = 1;
      }
      if(frequency > 4000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 4K \n");
        global.robot.setLightBrightness(36);
        light_off_n(5, u);
        light_on_n_m(light_no,2,u,Color::YELLOW);
        light_no = 2;
      }
      if(frequency > 6000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 6K \n");
        global.robot.setLightBrightness(48);
        light_off_n(4, u);
        light_on_n_m(light_no,3,u,Color::GREEN);
        light_no = 3;
      }
      if(frequency > 8000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 8K \n");
        global.robot.setLightBrightness(60);
        light_off_n(3, u);
        light_on_n_m(light_no,4,u,Color::BLUE);
        light_no = 4;
      }
      if(frequency > 10000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 10K \n");
        global.robot.setLightBrightness(72);
        light_off_n(2, u);
        light_on_n_m(light_no,5,u,Color::INDIGO);
        light_no = 5;
      }
      if(frequency > 12000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 12K \n");
        global.robot.setLightBrightness(84);
        light_off_n(1, u);
        light_on_n_m(light_no,6,u,Color::VIOLET);
        light_no = 7;
      }
      if(frequency > 14000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 14K \n");
        global.robot.setLightBrightness(96);
        light_off_n(0, u);
        light_on_n_m(light_no,7,u,Color::DARKGREY);
        light_no = 7;
      }
      if(frequency > 16000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 16K \n");
        global.robot.setLightBrightness(96);
        light_off_n(0, u);
        light_on_n_m(light_no,7,u,Color::DARKGREY);
        light_no = 7;
      }
      if(frequency > 18000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 18K \n");
        global.robot.setLightBrightness(84);
        light_off_n(1, u);
        light_on_n_m(light_no,6,u,Color::VIOLET);
        light_no = 7;
      }
      if(frequency > 20000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 20K \n");
        global.robot.setLightBrightness(72);
        light_off_n(2, u);
        light_on_n_m(light_no,5,u,Color::INDIGO);
        light_no = 5;
      }
      if(frequency > 22000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 22K \n");
        global.robot.setLightBrightness(60);
        light_off_n(3, u);
        light_on_n_m(light_no,4,u,Color::BLUE);
        light_no = 4;
      }
      if(frequency > 24000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 24K \n");
        global.robot.setLightBrightness(48);
        light_off_n(4, u);
        light_on_n_m(light_no,3,u,Color::GREEN);
        light_no = 3;
      }
      if(frequency > 26000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 26K \n");
        global.robot.setLightBrightness(36);
        light_off_n(5, u);
        light_on_n_m(light_no,2,u,Color::YELLOW);
        light_no = 2;
      }
      if(frequency > 28000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 28K \n");
        global.robot.setLightBrightness(24);
        light_off_n(6, u);
        light_on_n_m(light_no,1,u,Color::ORANGE);
        light_no = 1;
      }

      if(frequency > 30000 && frequency <= 32000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"BOOL 30K \n");
        global.robot.setLightBrightness(12);
        light_off_n(7, u);
        light_on_n_m(light_no,0,u,Color::RED);
        light_no = 0;
      }
    }
    return is_negative;
}

void light_show(UserThread& u){
  for (size_t i = 0; i < I2S_fft_BUF_SIZE; i++) {
    double abs_fft = i2s_fft_buf[i];

    // set the front LEDs to blue for one second
    // global.robot.setLightColor(constants::LightRing::LED_SSW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_WSW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_WNW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_NNW, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_NNE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_ENE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_ESE, Color(Color::WHITE));
    // global.robot.setLightColor(constants::LightRing::LED_SSE, Color(Color::WHITE));
    if(abs_fft > 4500){
      double frequency = (32000/(double)(I2S_fft_BUF_SIZE-1)) * (double)i;
      int light_no = 0;

      if(frequency > 0 && frequency <= 2000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 0K \n");
        global.robot.setLightBrightness(12);
        light_off_n_100ms(7, u);
        light_on_n_m_100ms(light_no,0,u,Color::RED);
        Motor_Dance_Schema_2(u);
        light_no = 0;
      }
      if(frequency > 2000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 2K \n");
        global.robot.setLightBrightness(24);
        light_off_n_100ms(6, u);
        light_on_n_m_100ms(light_no,1,u,Color::ORANGE);
        Motor_Dance_Schema_3(u);
        light_no = 1;
      }
      if(frequency > 4000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 4K \n");
        global.robot.setLightBrightness(36);
        light_off_n_100ms(5, u);
        light_on_n_m_100ms(light_no,2,u,Color::YELLOW);
        Motor_Dance_Schema_4(u);
        light_no = 2;
      }
      if(frequency > 6000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 6K \n");
        global.robot.setLightBrightness(48);
        light_off_n_100ms(4, u);
        light_on_n_m_100ms(light_no,3,u,Color::GREEN);
        Motor_Dance_Schema_2(u);
        light_no = 3;
      }
      if(frequency > 8000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 8K \n");
        global.robot.setLightBrightness(60);
        light_off_n_100ms(3, u);
        light_on_n_m_100ms(light_no,4,u,Color::BLUE);
        Motor_Dance_Schema_3(u);
        light_no = 4;
      }
      if(frequency > 10000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 10K \n");
        global.robot.setLightBrightness(72);
        light_off_n_100ms(2, u);
        light_on_n_m_100ms(light_no,5,u,Color::INDIGO);
        Motor_Dance_Schema_4(u);
        light_no = 5;
      }
      if(frequency > 12000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 12K \n");
        global.robot.setLightBrightness(84);
        light_off_n_100ms(1, u);
        light_on_n_m_100ms(light_no,6,u,Color::VIOLET);
        Motor_Dance_Schema_2(u);
        light_no = 7;
      }
      if(frequency > 14000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 14K \n");
        global.robot.setLightBrightness(96);
        light_off_n_100ms(0, u);
        light_on_n_m_100ms(light_no,7,u,Color::DARKGREY);
        Motor_Dance_Schema_3(u);
        light_no = 7;
      }
      if(frequency > 16000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 16K \n");
        global.robot.setLightBrightness(96);
        light_off_n_100ms(0, u);
        light_on_n_m_100ms(light_no,7,u,Color::DARKGREY);
        Motor_Dance_Schema_3(u);
        light_no = 7;
      }
      if(frequency > 18000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 18K \n");
        global.robot.setLightBrightness(84);
        light_off_n_100ms(1, u);
        light_on_n_m_100ms(light_no,6,u,Color::VIOLET);
        Motor_Dance_Schema_2(u);
        light_no = 7;
      }
      if(frequency > 20000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 20K \n");
        global.robot.setLightBrightness(72);
        light_off_n_100ms(2, u);
        light_on_n_m_100ms(light_no,5,u,Color::INDIGO);
        Motor_Dance_Schema_4(u);
        light_no = 5;
      }
      if(frequency > 22000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 22K \n");
        global.robot.setLightBrightness(60);
        light_off_n_100ms(3, u);
        light_on_n_m_100ms(light_no,4,u,Color::BLUE);
        Motor_Dance_Schema_3(u);
        light_no = 4;
      }
      if(frequency > 24000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 24K \n");
        global.robot.setLightBrightness(48);
        light_off_n_100ms(4, u);
        light_on_n_m_100ms(light_no,3,u,Color::GREEN);
        Motor_Dance_Schema_2(u);
        light_no = 3;
      }
      if(frequency > 26000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 26K \n");
        global.robot.setLightBrightness(36);
        light_off_n_100ms(5, u);
        light_on_n_m_100ms(light_no,2,u,Color::YELLOW);
        Motor_Dance_Schema_4(u);
        light_no = 2;
      }
      if(frequency > 28000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 28K \n");
        global.robot.setLightBrightness(24);
        light_off_n_100ms(6, u);
        light_on_n_m_100ms(light_no,1,u,Color::ORANGE);
        Motor_Dance_Schema_3(u);
        light_no = 1;
      }

      if(frequency > 30000 && frequency <= 32000){
        chprintf((BaseSequentialStream*)&global.sercanmux1,"VOID 30K \n");
        global.robot.setLightBrightness(12);
        light_off_n_100ms(7, u);
        light_on_n_m_100ms(light_no,0,u,Color::RED);
        Motor_Dance_Schema_2(u);
        light_no = 0;
      }
    }
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"less than 5000 amplitude \n");
  }
}

void fourier_analysis(UserThread& u) {
  chprintf((BaseSequentialStream*)&global.sercanmux1,"i, k, frequency, d1, d1, i2s_fft_buf[k]\n");
  double PI = 3.141592653589793238460;
  bool is_negative = false;
  int16_t i2s_buf_size = 1000;
  int16_t i2s_fft_buf_size = (i2s_buf_size/2)-1;
  for (size_t i = 1, k = 0; i < i2s_buf_size; i = i + 2) {
    // light_off_n(8);
    // if(color_switch){
    //   color_switch = false;
    //   light_on_n_m(0, 7, Color::DARKRED);
    // } else {
    //   color_switch = true;
    //   light_on_n_m(0, 7, Color::BLUE);
    // }
    uint32_t raw1 = global.i2s_rx_buf[i] /*& 0x0000FFFF*/;
    int16_t d1 = raw1 & 0x0000FFFF;

    double fft_val_real = 0.0;
    double fft_val_imag = 0.0;
    if(d1 == 0){;
      d1 = (raw1 >> 16) & 0x0000FFFF;
    }
    double absolute = 0.0;
    if(k != 0 && k != i2s_fft_buf_size){
      for (size_t j = 1, n = 0; j < i2s_buf_size; j = j + 2) {
        uint32_t raw = global.i2s_rx_buf[j];
        int16_t d = raw & 0x0000FFFF;
        if(d == 0){
          d = (raw >> 16) & 0x0000FFFF;
          // chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%08X\n,", i, d);
        }
        Complex fft_val = std::polar(1.0, -2 * PI * k * n / (i2s_fft_buf_size)) * (d*1.0);
        fft_val_real += real(fft_val);
        fft_val_imag += imag(fft_val);
        n++;
      }
      absolute = sqrt(pow(fft_val_real, 2.0) + pow(fft_val_imag, 2.0));
    }
    i2s_fft_buf[k] = absolute;
    double frequency = (32000/(double)i2s_fft_buf_size) * (double)k;
    is_negative = light_show(u, absolute, frequency,k,is_negative);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%d,%08X,%f\n", i, k, frequency, d1, d1, i2s_fft_buf[k]);
    k++;
    //chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d\n", i, d1);
  }
  light_off_n(8,u);
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
  while (!this->shouldTerminate())
  {
    chprintf((BaseSequentialStream*)&global.sercanmux1,"starting robot \n");
    this->sleep(MS2ST(1000));
    chprintf((BaseSequentialStream*)&global.sercanmux1,"before init \n");
    i2sInit();
    chprintf((BaseSequentialStream*)&global.sercanmux1,"init \n");
    i2sStart(&I2SD2, &global.i2scfg);
    chprintf((BaseSequentialStream*)&global.sercanmux1,"start I2S -> sleep 1s \n");
    this->sleep(MS2ST(1000));
    chprintf((BaseSequentialStream*)&global.sercanmux1,"before start DMA \n");
    i2sStartExchange(&I2SD2);
    int cycle = 0;
// //     for(int i=1,k=0,j = 0; i<=56; i++){
// //       if(j == 6){
// //         global.robot.setLightColor(k, Color(Color::INDIANRED));
// //       }else if(j%2 == 0){
// //         global.robot.setLightColor(k, Color(Color::GOLDENROD));
// //       } else{
// //         global.robot.setLightColor(k, Color(Color::BLACK));
// //       }
// //       chprintf((BaseSequentialStream*)&global.sercanmux1,"sleep %ds k:%d j:%d \n",i,k,j);
// //       k++;
// //       if(i%8 == 0){
// //         k =0;
// //         j++;
// //       }
// //       this->sleep(MS2ST(1000));
// //     }
//
// //     light_off_n(8);
     this->sleep(MS2ST(1000));
     chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop DMA \n");
     i2sStopExchange(&I2SD2);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"stop DMA -> sleep 1s \n");
     this->sleep(MS2ST(1000));
     chprintf((BaseSequentialStream*)&global.sercanmux1,"before stop I2S \n");
     i2sStop(&I2SD2);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"stop I2S -> 1s \n");
     this->sleep(MS2ST(1000));
     chprintf((BaseSequentialStream*)&global.sercanmux1,"Cycle %d\n", cycle);

     fourier_analysis(*this);
     cycle++;
     // //ligh show
     // // for(int i=0; i<5; i++){
       // light_show(*this);
     // // }
     //
     //stop robot movement
     global.robot.setTargetSpeed(0,0);

     //sleep before restart recording
     for(int i=0; i<5; i++){
       light_off_n(8,*this);
       this->sleep(MS2ST(500));
       light_on_n_m(0,7, *this,Color::SADDLEBROWN);
       this->sleep(MS2ST(500));
       chprintf((BaseSequentialStream*)&global.sercanmux1,"%ds\n", i+1);
     }
     light_off_n(8,*this);

  }


  return RDY_OK;
}
