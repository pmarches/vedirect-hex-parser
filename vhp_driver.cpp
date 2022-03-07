#include <memory.h>

#include <deque>
#include <string>
#include <sstream>
#include <iostream>

#include "vhp_parser.h"

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
      printf("ERROR: The payload must include the command as the first byte\n");
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

void onVHSentenceReceived(); //User implements this signature as callback?

class VHPDriver {
  const static size_t READ_BUFFER_SIZE=256;
//  void onRegisterValueReceived(uint16_t registerId, uint32_t registerValue); //byte,short values are also encoded in this 32bit int.
  void onNonHexSentenceReceived(const char* nonHexSentence);
public:
  VHPSerial* serial;
  VHPDriver(VHPSerial* serial) : serial(serial), onAsyncHandler(nullptr) {
  }
  void registerHandler();
  void sendPing() {
    uint8_t pingCommandBytes[]={0x01};
    serial->sendPayload(pingCommandBytes, 1);

    while(true){
      VHParsedSentence* sentence=readSentence();
      if(sentence->sentenceType==VHParsedSentence::PING){
        delete sentence;
        return;
      }
      else{
        printf("Ignoring parsed sentence because we are looking for a PONG\n");
        delete sentence;
      }
    }

  }

  void requestModelName(){
    printf("ANSWER: :70A010061626364B9\n");
    VHParsedSentence* sentence=getRegisterValue(0x010A);
    delete sentence;
  }

  void requestProductId(){
    printf("ANSWER: :700010048A065\n");
    VHParsedSentence* sentence=getRegisterValue(0x0100);
    delete sentence;
  }

  VHParsedSentence* readSentence(){
    std::string hexLine=serial->readLine();
    VHParsedSentence* sentence=parseHexLine(hexLine.c_str());
    if(sentence && sentence->isAsync){
      this->onAsyncHandler(sentence);
    }
    return sentence;
  }

  /***
   * This will ignore sentences of other registers
   */
  VHParsedSentence* getSentenceForRegister(uint16_t registerIdWanted){
    while(true){
      VHParsedSentence* sentence=readSentence();
      if(sentence->isRegister() && sentence->registerId==registerIdWanted){
        return sentence;
      }
      else{
        printf("Ignoring parsed sentence\n");
        delete sentence;
      }
    }
  }

  VHParsedSentence* getRegisterValue(uint16_t registerToGet){
    uint8_t payloadBytes[4];
    uint8_t nbPayloadBytes;
    VHPBuildGetRegisterPayload(registerToGet, 0, payloadBytes);
    serial->sendPayload(payloadBytes, sizeof(payloadBytes));

    return getSentenceForRegister(registerToGet);
  }

  int32_t getRegisterValueSigned(uint16_t registerToGet){
    VHParsedSentence* sentence=getRegisterValue(registerToGet);
    int32_t ret=sentence->sentence.signedRegister->registerValueSigned;
    delete sentence;
    return ret;
  }

  std::string getModelName(); //Synchronous

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
    serialFd=open(SERIAL_DEVICE_PATH, O_RDWR|O_NONBLOCK);
    if(serialFd<0){
      perror("Failed to open serial device, using stdout");
      serialFd=1;
      exit(0);
    }
  }

  int serialFd;

  virtual const std::string readLine(){
    int readBufferLen=0;
    char readBuffer[256];
    memset(readBuffer, 0, sizeof(readBuffer));
    while(true){
      int nbBytesRead=read(serialFd, readBuffer, sizeof(readBuffer)-1);
      if(nbBytesRead<0){
        continue;
      }
      readBuffer[nbBytesRead]=0;
      if(readBuffer[0]==':' && readBuffer[nbBytesRead-1]=='\n'){
        printf("Read %d bytes, %.*s", nbBytesRead, nbBytesRead, readBuffer);
        return std::string(readBuffer, nbBytesRead);
      }
      else{
        printf("Read some bytes that are not HEX lines, ignoring this: %.*s\n", nbBytesRead, readBuffer);
      }
    }
  }

  virtual void writeHexLine(const char* hexLine, const uint16_t hexLineLen){
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
      printf("Missing a mock response. You need to add a mocked response to the MockSerial class.\n");
      exit(1);
    }
    std::string ret=responseQueue.front();
//    printf("Poped %s\n", ret.c_str());
    responseQueue.pop_front();
    return ret;
  }

  virtual void writeHexLine(const char* hexLine, const uint16_t hexLineLen){
    printf("Mock->writeHexLine(%s,%d)", hexLine, hexLineLen);
  }
};
