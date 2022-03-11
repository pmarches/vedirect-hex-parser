#ifndef MAIN_VHP_DRIVER_H_
#define MAIN_VHP_DRIVER_H_

#include <memory.h>

#include <deque>
#include <string>
#include <sstream>
#include <iostream>

#include "vhp_command.h"
#include "vhp_parser.h"
#include "vhp_registers.h"
#include "vhp_traces.h"

#define SERIAL_DEVICE_PATH "/dev/ttyUSB0"


class VHPSerial {
public:
  virtual ~VHPSerial();
  virtual void configure()=0;
  virtual void writeHexLine(const std::string& hexLine)=0;
  virtual const std::string readLine()=0;
  void sendPayload(const std::basic_string<unsigned char>& payload);
};

class VHPDriver {
  const static size_t READ_BUFFER_SIZE=256;
  const static int NB_SENTENCES_TO_WAIT_UNTIL_RETRY=30;
//  void onRegisterValueReceived(uint16_t registerId, uint32_t registerValue); //byte,short values are also encoded in this 32bit int.
  void onNonHexSentenceReceived(const char* nonHexSentence);
public:
  VHPSerial* serial;
  VHPDriver(VHPSerial* serial);
  VHParsedSentence* discardSentencesUntilType(VHParsedSentence::SentenceType typeLookedFor);
  VHParsedSentence* discardSentencesUntilRegister(uint16_t registerIdWanted);
  void sendPingWaitPong();
  const ProductDescription* getProductId();
  std::string getGroupId();
  std::string getSerialNumber();
  std::string getModelName();
  void getCapabilities();
  VHParsedSentence* readSentence();
  VHParsedSentence* getRegisterValue(uint16_t registerToGet);
  int32_t getRegisterValueSigned(uint16_t registerToGet);
  void (*onAsyncHandler)(VHParsedSentence*);
  void registerAsyncSentenceHandler(void (*onAsyncHandler)(VHParsedSentence*));
};


#ifdef LINUX
class LinuxSerial : public VHPSerial {
public:
  LinuxSerial();
  virtual ~LinuxSerial();
  virtual void configure();
  int serialFd;
  virtual const std::string readLine();
  virtual void writeHexLine(const std::string& hexLine);
};

class MockSerial  : public VHPSerial {
  std::deque<std::string> responseQueue;
public:
  MockSerial();
  void queueResponse(const std::string& newResponse);
  virtual void configure();
  virtual const std::string readLine();
  virtual void writeHexLine(const std::string& hexLine);
};

#endif

void testDriver();

#endif /* MAIN_VHP_DRIVER_H_ */
