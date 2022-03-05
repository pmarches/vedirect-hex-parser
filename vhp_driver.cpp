#include <deque>
#include <string>
#include <memory.h>

#include "vhp_parser.h"

#define SERIAL_DEVICE_PATH "/dev/ttyUSB0"


class VHPSerial {
public:
  virtual void configure()=0;
  virtual void writeHexLine(char* hexLine, uint16_t hexLineLen)=0;
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
  VHPDriver(VHPSerial* serial) : serial(serial) {
  }
  void registerHandler();
  void sendPing() {
    printf("Ping ANSWER: :51641F9\n");
    uint8_t pingCommandBytes[]={0x01};
    serial->sendPayload(pingCommandBytes, 1);

    VHParsedSentence sentence;
    getSentenceOfType(VHParsedSentence::SentenceType::PING, &sentence);
//    sentence.PingResponse;
  }

  void requestModelName(){
    printf("ANSWER: :70A010061626364B9\n");
    VHParsedSentence sentence;
    getRegisterValue(0x010A, &sentence);
  }

  void requestProductId(){
    printf("ANSWER: :700010048A065\n");
    VHParsedSentence sentence;
    getRegisterValue(0x0100, &sentence);
  }

  /***
   * This will ignore sentences of other types.
   */
  void getSentenceOfType(VHParsedSentence::SentenceType typeWanted, VHParsedSentence* sentence){
    while(true){
      std::string hexLine=serial->readLine();
      parseHexLine(hexLine.c_str(), sentence);
      if(sentence->sentenceType==typeWanted){
        return;
      }
      else{
        printf("Ignoring hexline %s\n", hexLine.c_str());
      }
    }
  }

  void getRegisterValue(uint16_t registerToGet, VHParsedSentence* sentence){
    uint8_t payloadBytes[4];
    uint8_t nbPayloadBytes;
    VHPBuildGetRegisterPayload(registerToGet, 0, payloadBytes);
    serial->sendPayload(payloadBytes, sizeof(payloadBytes));

    getSentenceOfType(VHParsedSentence::SentenceType::GET_REGISTER, sentence);
  }

  int32_t getRegisterValueSigned(uint16_t registerToGet){
    VHParsedSentence sentence;
    getRegisterValue(registerToGet, &sentence);
    int32_t ret=sentence.sentence.getRegisterResponse->registerValueSigned;
    return ret;
  }

  std::string getModelName(); //Synchronous
};


class LinuxSerial : public VHPSerial {
public:
  LinuxSerial();
  void configureSerialPort(){
    system("stty -F " SERIAL_DEVICE_PATH " 19200 cs8 -cstopb -parenb");
    serialFd=open(SERIAL_DEVICE_PATH, O_RDWR|O_NONBLOCK);
    if(serialFd<0){
      perror("Failed to open serial device, using stdout");
      serialFd=1;
  //    exit(0);
    }
  }

  int serialFd;
#if 0
  int readBufferLen=0;
  char readBuffer[256];

  size_t readLine(){
    memset(readBuffer, 0, sizeof(readBuffer));
    while(true){
      int nbBytesRead=read(serialFd, readBuffer, sizeof(readBuffer)-1);
      if(nbBytesRead<0){
        continue;
      }
      readBuffer[nbBytesRead]=0;
      if(readBuffer[0]==':' && readBuffer[nbBytesRead-1]=='\n'){
        //      printf("Read %d bytes, %.*s", nbBytesRead, nbBytesRead, readBuffer);
        return nbBytesRead;
      }
      else{
        printf("Read some bytes that are not HEX lines, ignoring this: %.*s\n", nbBytesRead, readBuffer);
      }
    }
  }
#endif

  const std::string readLine(){
    return "";
  }

  void writeHexLine(const char* hexLine, const uint16_t hexLineLen){
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
      printf("Missing a mock response\n");
      exit(1);
    }
    std::string ret=responseQueue.front();
    responseQueue.pop_front();
    return ret;
  }
  virtual void writeHexLine(char* hexLine, uint16_t hexLineLen){
    printf("Mock->writeHexLine(%s,%d)", hexLine, hexLineLen);
  }
};
