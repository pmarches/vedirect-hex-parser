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
#include "vhp_registers.h"

void printRegister(uint16_t registerId, uint32_t registerValue){
  const RegisterDesc* registerDesc=lookupRegister(registerId);
  if(NULL==registerDesc){
    printf("Did not find a description of this register\n");
    return;
  }
  printf("\tRegister for '%s'\n", registerDesc->desc);
  if(registerDesc->byteLen==1){
    printf("\tSimple byte value %0.2f%s\n", registerValue*registerDesc->scale, registerDesc->unit);
  }
  else if(registerDesc->byteLen==2){
    printf("\tShort value %0.2f%s\n", registerValue*registerDesc->scale, registerDesc->unit);
  }
  else if(registerDesc->byteLen==4){
    printf("\tInt value %0.2f%s\n", registerValue*registerDesc->scale, registerDesc->unit);
  }
}

void onAsyncSentenceReceived(VHParsedSentence* asyncSentence){
  printf("Async sentence received..\n");
}

int main(int argc, char **argv) {
#if 0
  LinuxSerial serial;
  serial.configure();
#elif 1
//  testParser();

  MockSerial serial;
  VHPDriver vhpDriver(&serial);
  vhpDriver.registerAsyncSentenceHandler(onAsyncSentenceReceived);
  serial.queueResponse(":7F0ED009600DB\n");
  serial.queueResponse(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
//  serial.queueResponse(NON_HEX_PAYLOAD);
  vhpDriver.getRegisterValueSigned(0xEDF0);
  serial.queueResponse(":51641F9\n");
  vhpDriver.sendPing();

  serial.queueResponse(":7F0ED009600DB\n");
  VHParsedSentence* sentence=vhpDriver.getRegisterValue(0xEDF0);
  printRegister(sentence->registerId, sentence->sentence.signedRegister->registerValueSigned);

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
