// #include <iostream>
// #include <fstream>
#include "userthread.hpp"
#include <chprintf.h>

#include <hal.h>
#include <board.h>
#include "global.hpp"

using namespace amiro;

extern Global global;
types::kinematic kinematic;

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
  // ofstream myfile ("example.txt");
  // if (myfile.is_open())
  // {
  //   myfile << "This is a line.\n";
  //   myfile << "This is another line.\n";
  //   myfile.close();
  // }
  // else
  //   cout << "Unable to open file";

  //i2sStartExchange(&I2SD2);
  while (!this->shouldTerminate())
  {

    /*
     * INSERT CUSTOM CODE HERE
     */
     chprintf((BaseSequentialStream*)&global.sercanmux1,"ROBOT MOVING \n");
     kinematic = {10000000, 10000000, 10000000, 10000000, 10000000, 10000000};

     chprintf((BaseSequentialStream*)&global.sercanmux1,"Starting \n");
     this->sleep(MS2ST(5000));
     i2sStartExchange(&I2SD2);
     for (size_t i = 1; i < I2S_BUF_SIZE; i+=2) {
       // global.robot.setTargetSpeed(global.i2s_rx_buf[i],global.i2s_rx_buf[i]);
       // this->sleep(MS2ST(500));
      chprintf((BaseSequentialStream*)&global.sercanmux1,"%d i2s_rx_f[%d]: 0x%08X \n",I2S_BUF_SIZE, i, global.i2s_rx_buf[i]);
     //  chprintf((BaseSequentialStream*)&global.sercanmux1,"%d,%d\n", global.i2s_rx_buf[i],i);
     }

     i2sStopExchange(&I2SD2);
     chprintf((BaseSequentialStream*)&global.sercanmux1,"Stoping \n");
     this->sleep(MS2ST(5000));

  }

  return RDY_OK;
}
