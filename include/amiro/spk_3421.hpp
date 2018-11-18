#ifndef spk_3421_HPP_
#define spk_3421_HPP_

#include <hal.h>
#include <ch.hpp>

namespace amiro {

  class spk_3421 : public chibios_rt::BaseStaticThread<256> {
  public:



    spk_3421();
    virtual ~spk_3421();

    chibios_rt::EvtSource* getEventSource();

    float getSound();

  protected:
    virtual msg_t main();

  private:
    float rawsound;
    chibios_rt::EvtSource eventSource;
  };
}

#endif /* AMIRO_SPK_3421_H_ */
