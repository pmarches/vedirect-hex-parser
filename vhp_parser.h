#ifndef VHP_PARSER_H_
#define VHP_PARSER_H_

enum VEDIRECT_HEX_COMMAND {
    HEXCMD_ENTER_BOOT=0x0,
    HEXCMD_DONE=0x1,
    HEXCMD_APP_VERSION=0x3,
    HEXCMD_PRODUCT_ID=0x4,
    HEXCMD_PING=0x5,
    HEXCMD_RESTART=0x6,
    HEXCMD_GET=0x7,
    HEXCMD_SET=0x8,
    HEXCMD_ASYNC=0xA,
};

typedef struct {
  uint32_t registerValueUnsigned;
} UnSignedRegister;

typedef struct {
  int32_t registerValueSigned;
} SignedRegister;

typedef struct {
  uint8_t reservedByte;
  uint8_t errorDatabase;
  uint8_t error0;
  uint8_t error1;
  uint8_t error2;
  uint8_t error3;
  uint32_t totalYieldUser;
  uint32_t totalYieldSystem;
  uint16_t maxPanelVoltage;
  uint16_t maxBatteryVoltage;
  uint16_t numberOfDaysAvailable;
} HistoryTotalRecord;

typedef struct {
  uint8_t reservedByte;
  uint32_t yield;
  uint32_t consumed;
  uint16_t maxBattVoltage;
  uint16_t minBattVoltage;
  uint8_t errorDatabase;
  uint8_t error0;
  uint8_t error1;
  uint8_t error2;
  uint8_t error3;
  uint16_t timeBulk;
  uint16_t timeAbsorbtion;
  uint16_t timeFloat;
  uint32_t maxPower;
  uint16_t maxBattCurrent;
  uint16_t maxPanelVoltage;
  uint16_t daySequenceNumber;
} HistoryDailyRecord;

typedef struct {
} ParsedSentenceModelName;

typedef struct {
  uint16_t typeAndVersion;
  uint8_t appType;
  uint8_t rcVersion;
} ParsedSentencePingResponse;

typedef struct {
} ParsedSentenceAppVersion;

typedef struct {
} ParsedSentenceProductId;

class VHParsedSentence {
public:
  VHParsedSentence(uint16_t registerId) : registerId(registerId), sentenceType(NONE), isAsync(false){
  }

  ~VHParsedSentence(){
    if(sentenceType==SIGNED_REGISTER){
      delete sentence.signedRegister;
    }
  }
  uint16_t registerId;
  bool isAsync;
  enum SentenceType {
    NONE,
    PING,
    SIGNED_REGISTER,
    UNSIGNED_REGISTER,
    HISTORY_TOTAL_REGISTER,
    HISTORY_DAILY_REGISTER,
  } sentenceType;
  union {
    SignedRegister* signedRegister;
    UnSignedRegister* unSignedRegister;
    ParsedSentencePingResponse* pingResponse;
    HistoryTotalRecord* historyTotal;
    HistoryDailyRecord* historyDaily;
  } sentence;

  bool isRegister(){
    return sentenceType==SIGNED_REGISTER ||
        sentenceType==UNSIGNED_REGISTER ||
        sentenceType==HISTORY_TOTAL_REGISTER ||
        sentenceType==HISTORY_DAILY_REGISTER;
  }

  void printHistoryDaily(VHParsedSentence* sentence){
    printf("\tyield=%f\n", sentence->sentence.historyDaily->yield*0.01);
    if(sentence->sentence.historyDaily->consumed!=0xFFFFFFFF) printf("\tconsumed=%f\n", sentence->sentence.historyDaily->consumed*0.01);
    printf("\tmaxBattVoltage=%f\n", sentence->sentence.historyDaily->maxBattVoltage*0.01);
    printf("\tminBattVoltage=%f\n", sentence->sentence.historyDaily->minBattVoltage*0.01);
    printf("\ttimeBulk=%d\n", sentence->sentence.historyDaily->timeBulk);
    printf("\ttimeAbsorbtion=%d\n", sentence->sentence.historyDaily->timeAbsorbtion);
    printf("\ttimeFloat=%d\n", sentence->sentence.historyDaily->timeFloat);
    printf("\tmaxPower=%d\n", sentence->sentence.historyDaily->maxPower);
    printf("\tmaxBattCurrent=%f\n", sentence->sentence.historyDaily->maxBattCurrent*0.1);
    printf("\tmaxPanelVoltage=%f\n", sentence->sentence.historyDaily->maxPanelVoltage*0.01);
    printf("\tdaySequenceNumber=%d\n", sentence->sentence.historyDaily->daySequenceNumber);
  }

  #if 0
  toString() {
    printf("\ttypeAndVersion=0x%02X\n", sentence->sentence.pingResponse->typeAndVersion);
    printf("\tappType=%d\n", sentence->sentence.pingResponse->appType);
    printf("\tbooloader RC=%d \n", sentence->sentence.pingResponse->rcVersion);
    printf("\tversion=%02X\n", sentence->sentence.pingResponse->typeAndVersion&0b0011111111111111);
  }
  #endif

};


void testParser();
void VHPBuildGetRegisterPayload(uint16_t registerToGet, uint8_t flag, uint8_t* payloadBytes);
VHParsedSentence* parseHexLine(const char* hexLine);
uint8_t computeChecksum(const uint8_t* binaryPayload, const uint8_t nbBytesPayload);
void byteToHex(uint8_t byte, char* hexStr);
void bytesToHex(const uint8_t* bytes, const uint16_t nbBytes, char* hexOut);

#endif /* VHP_PARSER_H_ */
