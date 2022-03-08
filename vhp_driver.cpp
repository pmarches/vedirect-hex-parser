#include <memory.h>

#include <deque>
#include <string>
#include <sstream>
#include <iostream>

#include "vhp_parser.h"
#include "vhp_registers.h"
#include "vhp_traces.h"

#define SERIAL_DEVICE_PATH "/dev/ttyUSB0"


class VHPSerial {
public:
  virtual ~VHPSerial() {}
  virtual void configure()=0;
  virtual void writeHexLine(const char* hexLine, const uint16_t hexLineLen)=0;
  virtual const std::string readLine()=0;

  /***
   * Payload is the command and it's parameters. No leading ':' or checksum
   */
  void sendPayload(const uint8_t* payload, const uint16_t payloadLen){
    if(payloadLen<1){
      DEBUG("ERROR: The payload must include the command as the first byte\n");
      return;
    }
    uint16_t hexLineLen=payloadLen*2+2+1; //Add checksum, \n
    char* hexLine=(char*) malloc(hexLineLen+1); //Add \0
    memset(hexLine, 0, hexLineLen+1);
    byteToHex(payload[0], hexLine);
    hexLine[0]=':';
    bytesToHex(payload+1, payloadLen-1, hexLine+2);
    sprintf(hexLine+hexLineLen-3, "%02X\n", computeChecksum(payload, payloadLen));

    writeHexLine(hexLine, hexLineLen);
    free(hexLine);
  }
};

class VHPDriver {
  const static size_t READ_BUFFER_SIZE=256;
  const static int NB_SENTENCES_TO_WAIT_UNTIL_RETRY=30;
//  void onRegisterValueReceived(uint16_t registerId, uint32_t registerValue); //byte,short values are also encoded in this 32bit int.
  void onNonHexSentenceReceived(const char* nonHexSentence);
public:
  VHPSerial* serial;
  VHPDriver(VHPSerial* serial) : serial(serial), onAsyncHandler(nullptr) {
  }

  VHParsedSentence* discardSentencesUntilType(VHParsedSentence::SentenceType typeLookedFor){
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
  VHParsedSentence* discardSentencesUntilRegister(uint16_t registerIdWanted){
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

  void sendPingWaitPong() {
    //:154
    uint8_t pingCommandBytes[]={0x01};
    serial->sendPayload(pingCommandBytes, 1);
    VHParsedSentence *sentence=discardSentencesUntilType(VHParsedSentence::PONG);
    delete sentence;
  }

  const ProductDescription* getProductId(){
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

  void getGroupId(){
    VHParsedSentence* sentence=getRegisterValue(0x0104);
    delete sentence;
  }

  void getSerialNumber(){
    VHParsedSentence* sentence=getRegisterValue(0x010A);
    delete sentence;
  }

  void getModelName(){
    VHParsedSentence* sentence=getRegisterValue(0x010B);
    delete sentence;
  }

  void getCapabilities(){
    VHParsedSentence* sentence=getRegisterValue(0x0140);
    delete sentence;
  }

  VHParsedSentence* readSentence(){
    std::string hexLine=serial->readLine();
    VHParsedSentence* sentence=parseHexLine(hexLine.c_str());
    if(this->onAsyncHandler && sentence && sentence->isAsync){
      this->onAsyncHandler(sentence);
    }
    return sentence;
  }


  VHParsedSentence* getRegisterValue(uint16_t registerToGet){
    uint8_t payloadBytes[4];
    uint8_t nbPayloadBytes;
    VHPBuildGetRegisterPayload(registerToGet, 0, payloadBytes);

    while(true){
      serial->sendPayload(payloadBytes, sizeof(payloadBytes));
      VHParsedSentence* sentence=discardSentencesUntilRegister(registerToGet);
      if(sentence!=NULL){
        return sentence;
      }
      DEBUG("Our command seems to have been ignored, or we missed the response. Retrying...\n");
    }
  }

  int32_t getRegisterValueSigned(uint16_t registerToGet){
    VHParsedSentence* sentence=getRegisterValue(registerToGet);
    int32_t ret=sentence->sentence.signedRegister->value;
    delete sentence;
    return ret;
  }

  void (*onAsyncHandler)(VHParsedSentence*);
  void registerAsyncSentenceHandler(void (*onAsyncHandler)(VHParsedSentence*)){
    this->onAsyncHandler=onAsyncHandler;
  }

};


class LinuxSerial : public VHPSerial {
public:
  LinuxSerial() :serialFd(0) {}
  virtual ~LinuxSerial() {}
  virtual void configure(){
    system("stty -F " SERIAL_DEVICE_PATH " 19200 cs8 -cstopb -parenb");
    serialFd=open(SERIAL_DEVICE_PATH, O_RDWR);
    if(serialFd<0){
      perror("Failed to open serial device, using stdout");
      serialFd=1;
      exit(0);
    }
  }

  int serialFd;

  virtual const std::string readLine(){
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

  virtual void writeHexLine(const char* hexLine, const uint16_t hexLineLen){
    DEBUG("writeHexLine: hexLine=>'%.*s'\n", hexLineLen, hexLine);
    write(serialFd, hexLine, hexLineLen);
  }
};


class MultiplexedSerial : public VHPSerial {
public:
  MultiplexedSerial();
  void configureSerialPort();
  void selectPort(uint8_t portNumber);
};

class MockSerial  : public VHPSerial {
  std::deque<std::string> responseQueue;
public:
  MockSerial(){}
  void queueResponse(const std::string& newResponse) {
    responseQueue.push_back(newResponse);
  }
  virtual void configure(){
  }

  virtual const std::string readLine(){
    if(responseQueue.empty()){
      DEBUG("Missing a mock response. You need to add a mocked response to the MockSerial class.\n");
      exit(1);
    }
    std::string ret=responseQueue.front();
//    DEBUG("Poped %s\n", ret.c_str());
    responseQueue.pop_front();
    return ret;
  }

  virtual void writeHexLine(const char* hexLine, const uint16_t hexLineLen){
    DEBUG("Mock->writeHexLine(%s,%d)", hexLine, hexLineLen);
  }
};
