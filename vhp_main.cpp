#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "vhp_parser.h"
#include "vhp_driver.cpp"

int main(int argc, char **argv) {
#if 0

#elif 1
//  testParser();

  MockSerial serial;
  VHPDriver vhpDriver(&serial);
  serial.queueResponse(":7F0ED009600DB\n");
  vhpDriver.getRegisterValueSigned(0xEDF0);
#else
  MultiplexedSerial multiSerial(gpioBit0,gpioBit1,gpioBit2,gpioEnable);
  multiSerial.configureSerialPort();

//  configureMQTT();
  const int NB_MPPT=1;
  VHPDriver* vhpDrivers[NB_MPPT];
  for(int i=0; i<NB_MPPT; i++){
    multiSerial.selectPort(i);
    vhpDrivers[i]=new VHPDriver(&multiSerial);
    vhpDrivers[i]->ping();
    vhpDrivers[i]->getModelName();
    vhpDrivers[i]->readConfiguration();
  }

  while(true){
    for(int i=0; i<NB_MPPT; i++){
      multiSerial.selectPort(i);
      vhpDrivers[i]->getRegisterValueS32(VHPREG_BATT_CURRENT);
      vhpDrivers[i]->getRegisterValueS32(VHPREG_YIELD_USER_TODAY);
    }
  }
#endif
  return 0;
}
