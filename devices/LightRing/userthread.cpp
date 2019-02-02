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
const Color colors[] = {Color::SLATEGREY,Color::BLUEVIOLET,Color::HONEYDEW,Color::DARKBLUE,Color::DARKGREEN,Color::GREENYELLOW,Color::DARKORANGE,Color::DARKRED};
const Color colors2[] = {Color::RED,Color::ORANGE,Color::YELLOW,Color::GREEN,Color::BLUE,Color::INDIGO,Color::VIOLET,Color::DARKGREY};

void light_on_n_m(int from, int to, UserThread& u, const Color& color);
void light_on_n_m2(int from, int to, UserThread& u, const Color& color);
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

void light_on_n_m2(int from, int to, UserThread& u, const Color& color){
  for(int i = from; i <= to; i++){
    global.robot.setLightColor(i, Color(colors2[i]));
  }
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

void Motor_Dance_Schema_1(UserThread& u){
  int32_t MoveWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(- MoveWheelSpeed,- MoveWheelSpeed);
}

void Motor_Dance_Schema_2(UserThread& u){
  int32_t highWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(highWheelSpeed,0);
}

void Motor_Dance_Schema_3(UserThread& u){
  int32_t highWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(0,highWheelSpeed);
}

void Motor_Dance_Schema_4(UserThread& u){
  int32_t MoveWheelSpeed = 30 * 1000000;
  global.robot.setTargetSpeed(MoveWheelSpeed,MoveWheelSpeed);
}

bool light_show(UserThread& u, double abs_fft, double frequency, int i, bool is_negative){
  int light_no = 0;
    if(abs_fft > 4300){
      if(frequency > 0 && frequency <= 100){
        global.robot.setLightBrightness(12);
        light_off_n(7, u);
        light_on_n_m2(light_no,0,u,Color::RED);
        Motor_Dance_Schema_1(u);
        light_no = 0;
      }
      if(frequency > 200){
        global.robot.setLightBrightness(12);
        light_off_n(6, u);
        light_on_n_m2(light_no,1,u,Color::ORANGE);
        Motor_Dance_Schema_1(u);
        light_no = 1;
      }
      if(frequency > 300){
        global.robot.setLightBrightness(12);
        light_off_n(5, u);
        light_on_n_m2(light_no,2,u,Color::YELLOW);
        Motor_Dance_Schema_1(u);
        light_no = 2;
      }
      if(frequency > 400){
        global.robot.setLightBrightness(12);
        light_off_n(4, u);
        light_on_n_m2(light_no,3,u,Color::GREEN);
        Motor_Dance_Schema_2(u);
        light_no = 3;
      }
      if(frequency > 500){
        global.robot.setLightBrightness(12);
        light_off_n(3, u);
        light_on_n_m2(light_no,4,u,Color::BLUE);
        Motor_Dance_Schema_2(u);
        light_no = 4;
      }
      if(frequency > 600){
        global.robot.setLightBrightness(12);
        light_off_n(2, u);
        light_on_n_m2(light_no,5,u,Color::INDIGO);
        Motor_Dance_Schema_2(u);
        light_no = 5;
      }
      if(frequency > 700){
        global.robot.setLightBrightness(12);
        light_off_n(1, u);
        light_on_n_m2(light_no,6,u,Color::VIOLET);
        Motor_Dance_Schema_2(u);
        light_no = 7;
      }
      if(frequency > 800){
        global.robot.setLightBrightness(12);
        light_off_n(0, u);
        light_on_n_m2(light_no,7,u,Color::DARKGREY);
        Motor_Dance_Schema_3(u);
        light_no = 7;
      }
      if(frequency > 1000){
        global.robot.setLightBrightness(12);
        light_off_n(7, u);
        light_on_n_m(light_no,0,u,Color::RED);
        Motor_Dance_Schema_3(u);
        light_no = 0;
      }
      if(frequency > 2000){
        global.robot.setLightBrightness(24);
        light_off_n(6, u);
        light_on_n_m(light_no,1,u,Color::ORANGE);
        Motor_Dance_Schema_3(u);
        light_no = 1;
      }
      if(frequency > 4000){
        global.robot.setLightBrightness(36);
        light_off_n(5, u);
        light_on_n_m(light_no,2,u,Color::YELLOW);
        Motor_Dance_Schema_3(u);
        light_no = 2;
      }
      if(frequency > 6000){
        global.robot.setLightBrightness(48);
        light_off_n(4, u);
        light_on_n_m(light_no,3,u,Color::GREEN);
        Motor_Dance_Schema_4(u);
        light_no = 3;
      }
      if(frequency > 8000){
        global.robot.setLightBrightness(60);
        light_off_n(3, u);
        light_on_n_m(light_no,4,u,Color::BLUE);
        Motor_Dance_Schema_4(u);
        light_no = 4;
      }
      if(frequency > 10000){
        global.robot.setLightBrightness(72);
        light_off_n(2, u);
        light_on_n_m(light_no,5,u,Color::INDIGO);
        Motor_Dance_Schema_4(u);
        light_no = 5;
      }
      if(frequency > 12000){
        global.robot.setLightBrightness(84);
        light_off_n(1, u);
        light_on_n_m(light_no,6,u,Color::VIOLET);
        Motor_Dance_Schema_4(u);
        light_no = 7;
      }
      if(frequency > 14000){
        global.robot.setLightBrightness(96);
        light_off_n(0, u);
        light_on_n_m(light_no,7,u,Color::DARKGREY);
        Motor_Dance_Schema_4(u);
        light_no = 7;
      }
    }
    return is_negative;
}

void fourier_analysis(UserThread& u) {
  // chprintf((BaseSequentialStream*)&global.sercanmux1,"i, k, frequency, d1, d1, i2s_fft_buf[k]\n");
  double PI = constants::PI;
  bool is_negative = false;
  int16_t i2s_buf_size = 1000;
  int16_t i2s_fft_buf_size = (i2s_buf_size/2)-1;
  for (size_t i = 1, k = 0; i < i2s_buf_size/2; i = i + 2) {
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
    // chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d,%f,%d,%08X,%f\n", i, k, frequency, d1, d1, i2s_fft_buf[k]);
    k++;
  }
  light_off_n(8,u);
}

msg_t
UserThread::main()
{

  /**
  * record sound and print
  */
  i2sInit();
  while (!this->shouldTerminate())
  {
    this->sleep(MS2ST(100));
    for(int i = 0; i < 8; i++){
      global.robot.setLightColor(i, Color(Color::DARKKHAKI));
    }
    i2sStart(&I2SD2, &global.i2scfg);
    this->sleep(MS2ST(100));
    i2sStartExchange(&I2SD2);
    int cycle = 0;
     this->sleep(MS2ST(1000));
     i2sStopExchange(&I2SD2);
     this->sleep(MS2ST(100));
     i2sStop(&I2SD2);
     light_off_n(8,*this);

     fourier_analysis(*this);
     cycle++;
     //stop robot movement
     global.robot.setTargetSpeed(0,0);

     //sleep before restart recording
     for(int i=0; i<3; i++){
       light_off_n(8,*this);
       this->sleep(MS2ST(100));
       light_on_n_m(0,7, *this,Color::SADDLEBROWN);
       this->sleep(MS2ST(100));
     }
     light_off_n(8,*this);
  }


  return RDY_OK;
}
