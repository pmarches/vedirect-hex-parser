#ifndef VHP_PARSER_H_
#define VHP_PARSER_H_

enum VEDIRECT_HEX_COMMAND {
    ENTER_BOOT=0,
    PING=1,
    APP_VERSION=3,
    PRODUCT_ID=4,
    RESTART=6,
    GET=7,
    SET=8,
    ASYNC=0xA,
};

typedef struct {
  uint16_t registerId;
  uint32_t registerValueUnsigned;
  int32_t registerValueSigned;
} ParsedSentenceGetRegister;

typedef struct {
} ParsedSentenceModelName;

typedef struct {
} ParsedSentencePingResponse;

typedef struct {
} ParsedSentenceAppVersion;

typedef struct {
} ParsedSentenceProductId;

class VHParsedSentence {
public:
  VHParsedSentence() : sentenceType(NONE) {
  }

  ~VHParsedSentence(){
    if(sentenceType==GET_REGISTER){
      delete sentence.getRegisterResponse;
    }
  }
  enum SentenceType {
    NONE,
    GET_REGISTER,
    PING,
  } sentenceType;
  union {
    ParsedSentenceGetRegister* getRegisterResponse;
    ParsedSentencePingResponse* pingResponse;
  } sentence;

};


void testParser();
void VHPBuildGetRegisterPayload(uint16_t registerToGet, uint8_t flag, uint8_t* payloadBytes);
void parseHexLine(const char* hexLine, VHParsedSentence* sentence);
uint8_t computeChecksum(const uint8_t* binaryPayload, const uint8_t nbBytesPayload);
void byteToHex(uint8_t byte, char* hexStr);
void bytesToHex(const uint8_t* bytes, const uint16_t nbBytes, char* hexOut);

#endif /* VHP_PARSER_H_ */
