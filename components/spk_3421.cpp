#include <string.h>

#include <hal.h>
#include <amiro/util/util.h>
#include <amiro/spk_3421.hpp>
#include <chprintf.h>
#include <cmath>  // abs()
#include <amiro/Constants.h>
#include <global.hpp>
#include <board.h>

using namespace chibios_rt;

namespace amiro {

  extern Global global;

  spk_3421::spk_3421():
  BaseStaticThread<256>(),
  rawsound(0x00u){
  }

  spk_3421::~spk_3421() {
  }

  chibios_rt::EvtSource*
  spk_3421::getEventSource() {
    return &this->eventSource;
  }

  float spk_3421::getSound() {
    // const size_t buffer_size = offsetof(spk_3421::registers, out_z)
    //                            + 1;
    // uint8_t buffer[buffer_size];
    //
    // memset(buffer, 0xFFu, buffer_size);

    //buffer[0] = offsetof(spk_3421::registers, status_reg) | spk_3421::SPI_MULT | spk_3421::SPI_READ;
    //this->driver->exchange(buffer, buffer, buffer_size);

  }


  msg_t spk_3421::main(void) {
    this->setName("spk3421");

    while (!this->shouldTerminate()) {
      this->eventSource.broadcastFlags(0);
      //this->getSound();
      this->waitAnyEventTimeout(ALL_EVENTS, CAN::UPDATE_PERIOD);
    }
    return true;
  }
}
