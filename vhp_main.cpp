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

#if 0
toString() {
  printf("\ttypeAndVersion=0x%02X\n", sentence->sentence.pingResponse->typeAndVersion);
  printf("\tappType=%d\n", sentence->sentence.pingResponse->appType);
  printf("\tbooloader RC=%d \n", sentence->sentence.pingResponse->rcVersion);
  printf("\tversion=%02X\n", sentence->sentence.pingResponse->typeAndVersion&0b0011111111111111);
}
#endif

int main(int argc, char **argv) {
#if 1
  LinuxSerial serial;
  serial.configure();
#elif 0
//  testParser();

  MockSerial serial;
  VHPDriver vhpDriver(&serial);
  serial.queueResponse(":7F0ED009600DB\n");
  serial.queueResponse(":A0102000543\n");
//  serial.queueResponse(NON_HEX_PAYLOAD);
  vhpDriver.getRegisterValueSigned(0xEDF0);
  serial.queueResponse(":51641F9\n");
  vhpDriver.sendPing();

  VHParsedSentence sentence;
  serial.queueResponse(":7F0ED009600DB\n");
  vhpDriver.getRegisterValue(0xEDF0, &sentence);
  printRegister(sentence.sentence.getRegisterResponse->registerId, sentence.sentence.getRegisterResponse->registerValueSigned);

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
