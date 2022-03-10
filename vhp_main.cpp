#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sstream>
#include <iostream>

#include "main/vhp_command.h"
#include "vhp_parser.h"
#include "vhp_driver.h"
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

void printSignedRegister(VHParsedSentence* sentence, std::stringstream& ss){
  ss<<"\t"<<"registerId="<<sentence->registerId<<std::endl;
  ss<<"\t"<<"registerValueSigned="<<sentence->sentence.signedRegister->value<<std::endl;
}

void printUnsignedRegister(VHParsedSentence* sentence, std::stringstream& ss){
  ss<<"\t"<<"registerId="<<sentence->registerId<<std::endl;
  ss<<"\t"<<"registerValueUnsigned="<<sentence->sentence.unsignedRegister->value<<std::endl;

  const RegisterDesc* registerDesc=lookupRegister(sentence->registerId);
  if(NULL==registerDesc){
    printf("Did not find a description of this register\n");
    return;
  }
  printf("\tRegister for '%s'\n", registerDesc->desc);
  if(registerDesc->byteLen==1){
    printf("\tSimple byte value %0.2f%s\n", sentence->sentence.unsignedRegister->value*registerDesc->scale, registerDesc->unit);
  }
  else if(registerDesc->byteLen==2){
    printf("\tShort value %0.2f%s\n", sentence->sentence.unsignedRegister->value*registerDesc->scale, registerDesc->unit);
  }
  else if(registerDesc->byteLen==4){
    printf("\tInt value %0.2f%s\n", sentence->sentence.unsignedRegister->value*registerDesc->scale, registerDesc->unit);
  }
}

void printStringRegister(VHParsedSentence* sentence, std::stringstream& ss){
  ss<<"\t"<<"registerId="<<sentence->registerId<<std::endl;
  ss<<"\t"<<"registerValueUnsigned="<<sentence->sentence.unsignedRegister->value<<std::endl;

  const RegisterDesc* registerDesc=lookupRegister(sentence->registerId);
  if(NULL==registerDesc){
    printf("Did not find a description of this register\n");
    return;
  }
  printf("\tRegister for '%s'\n", registerDesc->desc);
  printf("\tString value %s\n", sentence->sentence.stringValue->c_str());
}

void printHistoryTotal(VHParsedSentence* sentence, std::stringstream& ss){
  ss<<"\t"<<"totalYieldUser="<<(sentence->sentence.historyTotal->totalYieldUser*0.01)<<std::endl;
  ss<<"\t"<<"totalYieldSystem="<<(sentence->sentence.historyTotal->totalYieldSystem*0.01)<<std::endl;
  ss<<"\t"<<"maxPanelVoltage="<<(sentence->sentence.historyTotal->maxPanelVoltage*0.01)<<std::endl;
  ss<<"\t"<<"maxBatteryVoltage="<<(sentence->sentence.historyTotal->maxBatteryVoltage*0.01)<<std::endl;
  ss<<"\t"<<"numberOfDaysAvailable="<<(sentence->sentence.historyTotal->numberOfDaysAvailable)<<std::endl;
}

void printHistoryDaily(VHParsedSentence* sentence, std::stringstream& ss){
  ss<<"\t"<<"yield="<<(sentence->sentence.historyDaily->yield*0.01)<<std::endl;
  if(sentence->sentence.historyDaily->consumed!=0xFFFFFFFF){
    ss<<"\tconsumed="<<sentence->sentence.historyDaily->consumed*0.01<<std::endl;
  }
  ss<<"\tmaxBattVoltage="<<sentence->sentence.historyDaily->maxBattVoltage*0.01<<std::endl;
  ss<<"\tminBattVoltage="<<sentence->sentence.historyDaily->minBattVoltage*0.01<<std::endl;
  ss<<"\ttimeBulk="<<sentence->sentence.historyDaily->timeBulk<<std::endl;
  ss<<"\ttimeAbsorbtion="<<sentence->sentence.historyDaily->timeAbsorbtion<<std::endl;
  ss<<"\ttimeFloat="<<sentence->sentence.historyDaily->timeFloat<<std::endl;
  ss<<"\tmaxPower="<<sentence->sentence.historyDaily->maxPower<<std::endl;
  ss<<"\tmaxBattCurrent="<<sentence->sentence.historyDaily->maxBattCurrent*0.1<<std::endl;
  ss<<"\tmaxPanelVoltage="<<sentence->sentence.historyDaily->maxPanelVoltage*0.01<<std::endl;
  ss<<"\tdaySequenceNumber="<<sentence->sentence.historyDaily->daySequenceNumber<<std::endl;
}

#if 0
toString() {
  ss<<"\ttypeAndVersion=0x%02X"<<sentence->sentence.pingResponse->typeAndVersion<<std::endl;
  ss<<"\tappType=%d"<<sentence->sentence.pingResponse->appType<<std::endl;
  ss<<"\tbooloader RC=%d "<<sentence->sentence.pingResponse->rcVersion<<std::endl;
  ss<<"\tversion=%02X"<<sentence->sentence.pingResponse->typeAndVersion&0b0011111111111111<<std::endl;
}
#endif

void toPrint(VHParsedSentence* sentence){
  if(sentence==NULL){
    std::cout<<"Nothing to print, sentence is NULL\n";
    return;
  }
  std::stringstream ss;
  ss<<"\t"<<"sentenceType="<<sentence->type<<std::endl;
  ss<<"\t"<<"isAsync="<<sentence->isAsync<<std::endl;
  if(sentence->type==VHParsedSentence::SIGNED_REGISTER){
    printSignedRegister(sentence, ss);
  }
  else if(sentence->type==VHParsedSentence::UNSIGNED_REGISTER){
    printUnsignedRegister(sentence, ss);
  }
  else if(sentence->type==VHParsedSentence::STRING){
    printStringRegister(sentence, ss);
  }
  else if(sentence->type==VHParsedSentence::HISTORY_DAILY_REGISTER){
    printHistoryDaily(sentence, ss);
  }
  else if(sentence->type==VHParsedSentence::HISTORY_TOTAL_REGISTER){
    printHistoryTotal(sentence, ss);
  }
  std::cout<<ss.str();
}

void onAsyncSentenceReceived(VHParsedSentence* asyncSentence){
  printf("Async sentence received..\n");
}

int main(int argc, char **argv) {
#if 0
  LinuxSerial serial;
  serial.configure();
  VHPDriver vhpDriver(&serial);
  vhpDriver.registerAsyncSentenceHandler(onAsyncSentenceReceived);
  vhpDriver.sendPingWaitPong();
  vhpDriver.getProductId();
  vhpDriver.getGroupId();
  vhpDriver.getSerialNumber();
  vhpDriver.getModelName();
  vhpDriver.getCapabilities();
  while(true){
    VHParsedSentence* sentence=vhpDriver.readSentence();
    if(sentence){
      toPrint(sentence);
      delete sentence;
    }
  }

#elif 1
  testParser();
  testCommandBuilder();

  MockSerial serial;
  VHPDriver vhpDriver(&serial);

  serial.queueResponse(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
  VHParsedSentence* sentence=vhpDriver.readSentence();
  toPrint(sentence);
  delete sentence;

  vhpDriver.registerAsyncSentenceHandler(onAsyncSentenceReceived);
  serial.queueResponse(":7F0ED009600DB\n");
  serial.queueResponse(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
//  serial.queueResponse(NON_HEX_PAYLOAD);
  vhpDriver.getRegisterValueSigned(0xEDF0);
  serial.queueResponse(":51641F9\n");
  vhpDriver.sendPingWaitPong();

  serial.queueResponse(":7F0ED009600DB\n");
  sentence=vhpDriver.getRegisterValue(0xEDF0);
  printRegister(sentence->registerId, sentence->sentence.signedRegister->value);

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
