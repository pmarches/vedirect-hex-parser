#include "vhp_driver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>

VHPSerial::~VHPSerial() {}

/***
 * Payload is the command and it's parameters. No leading ':' or checksum
 */
void VHPSerial::sendPayload(const std::basic_string<unsigned char>& payload){
  if(payload.size()<1){
    DEBUG("ERROR: The payload must include the command as the first byte\n");
    return;
  }
  std::string hexLine=bytesToHex(payload);
  hexLine[0]=':';

  std::stringstream ss;
  ss << hexLine
     << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << 0+computeChecksum(payload) //The 0+ here is a hack to get C++ to consider the uint8_t as integer a value, not a char.
     <<'\n';

  writeHexLine(ss.str());
}

VHPDriver::VHPDriver(VHPSerial* serial) : serial(serial), onAsyncHandler(nullptr) {
}

VHParsedSentence* VHPDriver::discardSentencesUntilType(VHParsedSentence::SentenceType typeLookedFor){
  for(int i=0; i<NB_SENTENCES_TO_WAIT_UNTIL_RETRY; i++){
    VHParsedSentence* sentence=readSentence();
    if(sentence==NULL){
      continue;
    }

    if(sentence->type==typeLookedFor){
      return sentence;
    }
    else{
      DEBUG("Ignoring parsed sentence because we are looking for %d\n", typeLookedFor);
      delete sentence;
    }
  }
  return NULL;
}

/***
 * This will ignore sentences of other registers
 */
VHParsedSentence* VHPDriver::discardSentencesUntilRegister(uint16_t registerIdWanted){
  for(int i=0; i<NB_SENTENCES_TO_WAIT_UNTIL_RETRY; i++){
    VHParsedSentence* sentence=readSentence();
    if(sentence==NULL){
      continue;
    }

    if(sentence->isRegister()){
      if(sentence->registerId==registerIdWanted){
        return sentence;
      }
      else{
        DEBUG("Ignoring parsed sentence because we are looking for register 0x%04X. This one was 0x%04X\n", registerIdWanted, sentence->registerId);
        delete sentence;
      }
    }
    else{
      DEBUG("Ignoring parsed sentence because we are looking for a register sentence. This one was a %d\n", sentence->type);
      delete sentence;
    }
  }
  //
  return NULL;
}

void VHPDriver::sendPingWaitPong() {
  //:154
  std::basic_string<unsigned char> pingCommandBytes({0x01}, 1);
  serial->sendPayload(pingCommandBytes);
  VHParsedSentence *sentence=discardSentencesUntilType(VHParsedSentence::PONG);
  delete sentence;
}

const ProductDescription* VHPDriver::getProductId(){
  VHParsedSentence* sentence=getRegisterValue(0x0100);

  uint16_t productId=sentence->sentence.unsignedRegister->value;
  DEBUG("\tproductId=0x%X\n", productId);
  const ProductDescription* productDesc=lookupProductId(productId);
  if(productDesc==NULL){
    DEBUG("Product id not found\n");
    return NULL;
  }
  DEBUG("\tProduct name is %s\n", productDesc->productName);

  delete sentence;
  return productDesc;
}

std::string VHPDriver::getGroupId(){
  VHParsedSentence* sentence=getRegisterValue(0x0104);
  delete sentence;
  return "X";
}

std::string VHPDriver::getSerialNumber(){
  VHParsedSentence* sentence=getRegisterValue(0x010A);
  std::string ret=*sentence->sentence.stringValue;
  delete sentence;
  return ret;
}

std::string VHPDriver::getModelName(){
  VHParsedSentence* sentence=getRegisterValue(0x010B);
  std::string ret=*sentence->sentence.stringValue;
  delete sentence;
  return ret;
}

void VHPDriver::getCapabilities(){
  VHParsedSentence* sentence=getRegisterValue(0x0140);
  delete sentence;
}

VHParsedSentence* VHPDriver::readSentence(){
  std::string hexLine=serial->readLine();
  VHParsedSentence* sentence=parseHexLine(hexLine.c_str());
  if(this->onAsyncHandler && sentence && sentence->isAsync){
    this->onAsyncHandler(sentence);
  }
  return sentence;
}


VHParsedSentence* VHPDriver::getRegisterValue(uint16_t registerToGet){
  std::basic_string<unsigned char> cmd=VHPCommandGetRegister(registerToGet, 0);

  while(true){
    serial->sendPayload(cmd);
    VHParsedSentence* sentence=discardSentencesUntilRegister(registerToGet);
    if(sentence!=NULL){
      return sentence;
    }
    DEBUG("Our command seems to have been ignored, or we missed the response. Retrying...\n");
  }
}

int32_t VHPDriver::getRegisterValueSigned(uint16_t registerToGet){
  VHParsedSentence* sentence=getRegisterValue(registerToGet);
  int32_t ret=sentence->sentence.signedRegister->value;
  delete sentence;
  return ret;
}

void (*onAsyncHandler)(VHParsedSentence*);
void VHPDriver::registerAsyncSentenceHandler(void (*onAsyncHandler)(VHParsedSentence*)){
  this->onAsyncHandler=onAsyncHandler;
}

#ifdef LINUX
LinuxSerial::LinuxSerial() :serialFd(0) {}
LinuxSerial::~LinuxSerial() {}
void LinuxSerial::configure(){
  system("stty -F " SERIAL_DEVICE_PATH " 19200 cs8 -cstopb -parenb");
  serialFd=open(SERIAL_DEVICE_PATH, O_RDWR);
  if(serialFd<0){
    perror("Failed to open serial device, using stdout");
    serialFd=1;
    exit(0);
  }
}

const std::string LinuxSerial::readLine(){
  char readBuffer[256];
  int nbBytesRead=0;
  while(true){
    if(read(serialFd, readBuffer+nbBytesRead, 1)<=0){
      perror("Error reading from linux serial");
      exit(1);
    }
    if('\n'==readBuffer[nbBytesRead]){
      return std::string(readBuffer, nbBytesRead+1);
    }
    if(nbBytesRead<sizeof(readBuffer)-1){ //Overflow will overwrite the last char until we reach a newline.
      nbBytesRead++;
    }
  }
}

void LinuxSerial::writeHexLine(const std::string& hexLine){
  DEBUG("writeHexLine: hexLine=>'%.*s'\n", (int) hexLine.size(), hexLine.c_str());
  write(serialFd, hexLine.c_str(), hexLine.size());
}


MockSerial::MockSerial(){}
void MockSerial::queueResponse(const std::string& newResponse) {
  responseQueue.push_back(newResponse);
}
void MockSerial::configure(){
}

const std::string MockSerial::readLine(){
  if(responseQueue.empty()){
    DEBUG("Missing a mock response. You need to add a mocked response to the MockSerial class.\n");
    exit(1);
  }
  std::string ret=responseQueue.front();
  //    DEBUG("Poped %s\n", ret.c_str());
  responseQueue.pop_front();
  return ret;
}

void MockSerial::writeHexLine(const std::string& hexLine){
  DEBUG("Mock->writeHexLine(%s,%ld)", hexLine.c_str(), hexLine.size());
}

void testDriver(){
  assertEquals(0x54, computeChecksum({0x01}), "computeChecksum 0x54");
  assertEquals(0xF9, computeChecksum({0x05, 0x16, 0x41}), "computeChecksum 0xF9");
  MockSerial serial;
  serial.sendPayload({0x01});
}

#endif
