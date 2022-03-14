#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>

#include <string>
#include <string_view>

#include "vhp_registers.h"
#include "vhp_parser.h"
#include "vhp_traces.h"
#include <endian.h>

#define TAG __FILE__

VHParsedSentence::VHParsedSentence(uint16_t registerId) : registerId(registerId), isAsync(false), type(NONE) {
}

VHParsedSentence::~VHParsedSentence(){
  if(type==SIGNED_REGISTER){
    delete sentence.signedRegister;
  }
  else if(type==UNSIGNED_REGISTER){
    delete sentence.unsignedRegister;
  }
  else if(type==STRING){
    delete sentence.stringValue;
  }
  else if(type==HISTORY_DAILY_REGISTER){
    delete sentence.historyDaily;
  }
  else if(type==HISTORY_TOTAL_REGISTER){
    delete sentence.historyTotal;
  }
}

bool VHParsedSentence::isRegister() const {
  return type==SIGNED_REGISTER ||
      type==UNSIGNED_REGISTER ||
      type==HISTORY_TOTAL_REGISTER ||
      type==HISTORY_DAILY_REGISTER ||
      type==STRING;
}

uint8_t computeChecksum(const std::basic_string<unsigned char>& binaryPayload){
//  ESP_LOG_BUFFER_HEX_LEVEL(TAG, binaryPayload.c_str(), binaryPayload.size(), ESP_LOG_DEBUG);

  uint8_t checksum=0x55;
  for(int i=0; i<binaryPayload.size(); i++){
    checksum -= binaryPayload[i];
  }
  DEBUG_PRINTD(TAG, "checksum=0x%02X", checksum);
  return checksum;
}

uint8_t hexCharToNibble(const char hexChar){
  if(hexChar>='0' && hexChar<='9') return hexChar-'0';
  if(hexChar>='A' && hexChar<='F') return hexChar-'A'+10;
  return 0;
}

void hexToBytes(const char* hexLine, uint16_t hexLineLen, uint8_t* outputBytes){
  if(hexLineLen%2!=0){
    DEBUG_PRINTD(TAG,"Warning, converting a odd length hex string. There may be data loss.\n");
  }
  uint16_t outputBytesLen=hexLineLen/2;
  for(int i=0; i<outputBytesLen; i++){
    uint8_t nibbleHigh=hexCharToNibble(hexLine[i*2]);
    uint8_t nibbleLow=hexCharToNibble(hexLine[(i*2)+1]);
    outputBytes[i]=nibbleLow|(nibbleHigh<<4);
//    DEBUG_PRINTD(TAG,"hexLine[i*2]=%c outputBytes=0x%02X nibleLow=0x%02X nibleHigh=0x%02X \n", hexLine[i*2], outputBytes[i], nibleLow, nibleHigh);
  }
}

void parseProductId(const uint8_t* payloadBytes, const uint16_t payloadBytesLen, VHParsedSentence* sentence){
  DEBUG_PRINTD(TAG,"\tparseProductId\n");
//  hexdump("payloadBytes", payloadBytes, payloadBytesLen);

  sentence->type=VHParsedSentence::UNSIGNED_REGISTER;
  sentence->sentence.unsignedRegister=new UnSignedRegister();
  payloadBytes++; //Instance
  sentence->sentence.unsignedRegister->value=*((uint16_t*)payloadBytes);
  payloadBytes+=2; //ProductId
  payloadBytes++; //Reserved
}

void parseHistoryTotalRecord(const uint8_t* payloadBytes, uint16_t payloadBytesLen, VHParsedSentence* sentence){
  sentence->type=VHParsedSentence::HISTORY_TOTAL_REGISTER;
  sentence->sentence.historyTotal=new HistoryTotalRecord();

  sentence->sentence.historyTotal->totalYieldUser=le32toh(*((uint32_t*) payloadBytes));
  sentence->sentence.historyTotal->totalYieldSystem=le32toh(*((uint32_t*) payloadBytes+4));
//  DEBUG_PRINTD(TAG,"\ttotalYieldSystem=%d\n", totalYieldSystem);
  sentence->sentence.historyTotal->maxPanelVoltage=le16toh(*((uint32_t*) payloadBytes+8));
  sentence->sentence.historyTotal->maxBatteryVoltage=le16toh(*((uint32_t*) payloadBytes+10));
  sentence->sentence.historyTotal->numberOfDaysAvailable=le16toh(*((uint32_t*) payloadBytes+12));
}

void parseHistoryDayRecord(const uint8_t* payloadBytes, uint16_t payloadBytesLen, VHParsedSentence* sentence){
  sentence->type=VHParsedSentence::HISTORY_DAILY_REGISTER;
  sentence->sentence.historyDaily=new HistoryDailyRecord();

  sentence->sentence.historyDaily->reservedByte=*payloadBytes;
  payloadBytes++;
  sentence->sentence.historyDaily->yield=le32toh(*((uint32_t*) payloadBytes));
  payloadBytes+=4;
  sentence->sentence.historyDaily->consumed=le32toh(*((uint32_t*) payloadBytes));//consumed by load output
  payloadBytes+=4;

  sentence->sentence.historyDaily->maxBattVoltage=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;
  sentence->sentence.historyDaily->minBattVoltage=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;

  sentence->sentence.historyDaily->timeBulk=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;
  sentence->sentence.historyDaily->timeAbsorbtion=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;
  sentence->sentence.historyDaily->timeFloat=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;

  sentence->sentence.historyDaily->maxPower=le32toh(*((uint32_t*) payloadBytes));
  payloadBytes+=4;
  sentence->sentence.historyDaily->maxBattCurrent=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;
  sentence->sentence.historyDaily->maxPanelVoltage=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;
  sentence->sentence.historyDaily->daySequenceNumber=le16toh(*((uint16_t*) payloadBytes));
  payloadBytes+=2;
}

void parseGroupId(const uint8_t* payloadBytes, const uint16_t payloadBytesLen, VHParsedSentence* sentence){
  DEBUG_PRINTD(TAG,"\tparseGroupId\n");
  DEBUG_PRINTD(TAG,"\tgroupId=%d\n", *payloadBytes);

  sentence->type=VHParsedSentence::UNSIGNED_REGISTER;
  sentence->sentence.unsignedRegister=new UnSignedRegister();
  sentence->sentence.unsignedRegister->value=*payloadBytes;
}

void parseSerialNumber(const uint8_t* payloadBytes, const uint16_t payloadBytesLen, VHParsedSentence* sentence){
  DEBUG_PRINTD(TAG,"\tparseSerialNumber\n");
  DEBUG_PRINTD(TAG,"\tserialNumber='%.*s'\n", payloadBytesLen, (char*) payloadBytes);
//  hexdump("payloadBytes", payloadBytes, payloadBytesLen);

  sentence->type=VHParsedSentence::STRING;
  sentence->sentence.stringValue=new std::string((const char*) payloadBytes, payloadBytesLen);
}

void parseModelName(const uint8_t* payloadBytes, const uint16_t payloadBytesLen, VHParsedSentence* sentence){
  DEBUG_PRINTD(TAG,"\tparseModelName\n");
  DEBUG_PRINTD(TAG,"\tmodelName=%.*s\n", payloadBytesLen, (char*) payloadBytes);
  sentence->type=VHParsedSentence::STRING;
  sentence->sentence.stringValue=new std::string((const char*) payloadBytes, payloadBytesLen);

}

void parseCapabilities(const uint8_t* payloadBytes, const uint16_t payloadBytesLen, VHParsedSentence* sentence){
  DEBUG_PRINTD(TAG,"\tparseCapabilities\n");
  sentence->type=VHParsedSentence::UNSIGNED_REGISTER;
  sentence->sentence.unsignedRegister=new UnSignedRegister();
  sentence->sentence.unsignedRegister->value=le32toh(*(uint32_t*)payloadBytes);

  DEBUG_PRINTD(TAG,"\tcapabilities=0x%X\n", sentence->sentence.unsignedRegister->value);
}

VHParsedSentence* parseGet(const uint8_t* payloadBytes, uint16_t payloadBytesLen){
  DEBUG_PRINTD(TAG,"Parse GET response\n");
  if(payloadBytesLen<5){
    DEBUG_PRINTD(TAG,"Got a GET response but the payload length is wrong. %d bytes\n", payloadBytesLen);
    return NULL;
  }
  payloadBytes+=1; //Skip command

  uint16_t registerId=le16toh(*(uint16_t*) payloadBytes);
  payloadBytes+=2;
  const RegisterDesc* registerDesc=lookupRegister(registerId);
  if(registerDesc==NULL){
    DEBUG_PRINTD(TAG,"Don't know how to decode register 0x%04X\n", registerId);
    return NULL;
  }

  uint8_t flag=*payloadBytes;
  payloadBytes+=1;
  DEBUG_PRINTD(TAG,"registerId=0x%04X flag=%d\n", registerId, flag);
  if(flag!=0){
    DEBUG_PRINTD(TAG,"Something is wrong with that flag");
    return NULL;
  }
  payloadBytesLen-=4;

  VHParsedSentence* sentence=new VHParsedSentence(registerId);
  if(0x0100==sentence->registerId){
    parseProductId(payloadBytes, payloadBytesLen, sentence);
  }
  else if(0x0104==sentence->registerId){
    parseGroupId(payloadBytes, payloadBytesLen, sentence);
  }
  else if(0x010A==sentence->registerId){
    parseSerialNumber(payloadBytes, payloadBytesLen, sentence);
  }
  else if(0x010B==sentence->registerId){
    parseModelName(payloadBytes, payloadBytesLen, sentence);
  }
  else if(0x0140==sentence->registerId){
    parseCapabilities(payloadBytes, payloadBytesLen, sentence);
  }
  else if(0x1050<=sentence->registerId && 0x106E>=sentence->registerId){
    parseHistoryDayRecord(payloadBytes, payloadBytesLen, sentence);
  }
  else if(0x104F==sentence->registerId){
    parseHistoryTotalRecord(payloadBytes, payloadBytesLen, sentence);
  }
  else if(registerDesc->byteLen==1 && registerDesc->encoding==SIGNED){
    sentence->type=VHParsedSentence::SIGNED_REGISTER;
    sentence->sentence.signedRegister=new SignedRegister();
    sentence->sentence.signedRegister->value=*(payloadBytes);
    payloadBytes+=registerDesc->byteLen;
  }
  else if(registerDesc->byteLen==1 && registerDesc->encoding==UNSIGNED){
    sentence->type=VHParsedSentence::UNSIGNED_REGISTER;
    sentence->sentence.unsignedRegister=new UnSignedRegister();
    sentence->sentence.unsignedRegister->value=*(payloadBytes);
    payloadBytes+=registerDesc->byteLen;
  }
  else if(registerDesc->byteLen==2 && registerDesc->encoding==SIGNED){
    sentence->type=VHParsedSentence::SIGNED_REGISTER;
    sentence->sentence.signedRegister=new SignedRegister();
    sentence->sentence.signedRegister->value=le16toh(*(uint16_t*) (payloadBytes));
    payloadBytes+=registerDesc->byteLen;
  }
  else if(registerDesc->byteLen==4 && registerDesc->encoding==SIGNED){
    sentence->type=VHParsedSentence::SIGNED_REGISTER;
    sentence->sentence.signedRegister=new SignedRegister();
    sentence->sentence.signedRegister->value=le32toh(*(uint32_t*) (payloadBytes));
    payloadBytes+=registerDesc->byteLen;
  }
  else if(registerDesc->byteLen==2 && registerDesc->encoding==UNSIGNED){
    sentence->type=VHParsedSentence::UNSIGNED_REGISTER;
    sentence->sentence.unsignedRegister=new UnSignedRegister();
    sentence->sentence.unsignedRegister->value=le16toh(*(uint16_t*) (payloadBytes));
    payloadBytes+=registerDesc->byteLen;
  }
  else if(registerDesc->byteLen==4 && registerDesc->encoding==UNSIGNED){
    sentence->type=VHParsedSentence::UNSIGNED_REGISTER;
    sentence->sentence.unsignedRegister=new UnSignedRegister();
    sentence->sentence.unsignedRegister->value=le32toh(*(uint32_t*) (payloadBytes));
    payloadBytes+=registerDesc->byteLen;
  }
  else{
    DEBUG_PRINTD(TAG,"Unhandeled combination of byte len %d encoding %d and register 0x%X\n", registerDesc->byteLen, registerDesc->encoding, sentence->registerId);
    exit(1);
  }
  return sentence;
}

VHParsedSentence* parseAsync(const uint8_t* payloadBytes, const uint16_t payloadBytesLen){
  DEBUG_PRINTD(TAG,"Parse ASYNC response\n");
  VHParsedSentence* sentence=parseGet(payloadBytes, payloadBytesLen);
  if(sentence){
    sentence->isAsync=true;
  }
  return sentence;
}

VHParsedSentence* parseDone(const uint8_t* payloadBytes){
  DEBUG_PRINTD(TAG,"TODO Parse DONE response\n");
  //Parsing DONE messages depends on the last command we sent...
  VHParsedSentence* sentence=new VHParsedSentence(0);
  sentence->type=VHParsedSentence::DONE;
  return sentence;
}

VHParsedSentence* parsePong(const uint8_t* payloadBytes){
  DEBUG_PRINTD(TAG,"Parse pong\n");
  VHParsedSentence* sentence=new VHParsedSentence(0);
  sentence->type=VHParsedSentence::PONG;
  sentence->sentence.pingResponse=new ParsedSentencePingResponse();

  sentence->sentence.pingResponse->typeAndVersion=le16toh(*(uint16_t*)(payloadBytes+1));
  DEBUG_PRINTD(TAG,"\ttypeAndVersion=0x%04X\n", sentence->sentence.pingResponse->typeAndVersion);
  sentence->sentence.pingResponse->appType=sentence->sentence.pingResponse->typeAndVersion>>14;
  if(3==sentence->sentence.pingResponse->appType){
    sentence->sentence.pingResponse->rcVersion=payloadBytes[1]&0b00111111;
  }
  return sentence;
}

#if 0
VHParsedSentence* parseAppVersion(const uint8_t* payloadBytes, uint16_t payloadBytesLen){
  DEBUG_PRINTD(TAG,"Got AppVersion message\n");
  uint16_t appversion=le16toh(*(uint16_t*)(payloadBytes+1));
  DEBUG_PRINTD(TAG,"\tFirmware version=0x%04X\n", appversion);
  VHParsedSentence* sentence=new VHParsedSentence(0);
  sentence->type=VHParsedSentence::FIRMWARE_VERSION;
  return sentence;
}
#endif

VHParsedSentence* parseHexLine(const char* hexLine){
  if(hexLine==NULL) {
    return NULL;
  }
  DEBUG_PRINTD(TAG,"Parsing '%s'", hexLine); //Do not output the trailing \n

  //Skip leading junk
  while(hexLine[0]!='\0' && hexLine[0]!=':'){
    hexLine++;
  }
  int hexLineLen=strlen(hexLine);
  if(hexLine[0]!=':' || hexLine[hexLineLen-1]!='\n'){
    DEBUG_PRINTD(TAG,"The hexline length should start with ':' end with '\\n' and be of odd length, ignoring.\n");
    return NULL;
  }
  if(hexLineLen<5){
    DEBUG_PRINTD(TAG,"Hexline is too short\n");
    return NULL;
  }

  uint16_t hexPayloadLen=hexLineLen-2; //The : and \n are not part of the payload
  uint16_t payloadBytesLen=(hexPayloadLen/2)+1; //The payload includes the command
  uint8_t* payloadBytes=(uint8_t*) malloc(payloadBytesLen);
  payloadBytes[0]=hexCharToNibble(hexLine[1]); //Convert the command (first low nibble)
  hexToBytes(hexLine+2, hexPayloadLen-1, payloadBytes+1); //Skip the command and the trailing \n

  std::basic_string<unsigned char> payloadNoChecksum(payloadBytes, payloadBytesLen-1);
  uint8_t computedChecksum=computeChecksum(payloadNoChecksum); //Do not use the checksum byte to compute the the checksum
  if(payloadBytes[payloadBytesLen-1]!=computedChecksum){
    DEBUG_PRINTE(TAG,"Checksum error: Received checksum 0x%02X but computed 0x%02X\n", payloadBytes[payloadBytesLen-1], computedChecksum);
    free(payloadBytes);
    return NULL;
  }

  VHParsedSentence *sentence=NULL;
  if(HEXRSP_GET==payloadBytes[0]) {
    sentence=parseGet(payloadBytes, payloadBytesLen);
  }
  else if(HEXRSP_ASYNC==payloadBytes[0]) {
    sentence=parseAsync(payloadBytes, payloadBytesLen);
  }
  else if(HEXRSP_DONE==payloadBytes[0]) {
    sentence=parseDone(payloadBytes);
  }
#if 0
  else if(HEXRSP_APP_VERSION==payloadBytes[0]) {
    sentence=parseAppVersion(payloadBytes, payloadBytesLen);
  }
#endif
  else if(HEXRSP_ERROR==payloadBytes[0]) {
    DEBUG_PRINTE(TAG,"Got HEXRSP_ERROR message. Checksum was wrong\n");
    sentence=new VHParsedSentence(0);
    sentence->type=VHParsedSentence::ERROR;
  }
  else if(HEXCMD_PING==payloadBytes[0]) {
    sentence=parsePong(payloadBytes);
  }

  free(payloadBytes);
  if(sentence==NULL || sentence->type==VHParsedSentence::SentenceType::NONE){
    DEBUG_PRINTD(TAG,"Unknown/Unhandled HEX command %s\n", hexLine);
//    exit(1);
  }
  return sentence;
}

void testParser(){
  VHParsedSentence* sentence;

//  sentence=parseHexLine(":AD5ED0009057B\n:ABBED00E32997\n:A07020000000");
  sentence=parseHexLine(":70401000A3F\n");
  assertEquals(VHParsedSentence::UNSIGNED_REGISTER, sentence->type, "Should be a signed register");
  assertEquals(10, sentence->sentence.signedRegister->value, "10");

  sentence=parseHexLine(":70001000062A0FF4C\n");

  sentence=parseHexLine(":70A01004851313834304D4145493400000000008D\n");
  assertEquals(VHParsedSentence::STRING, sentence->type, "Should be a STRING");
  assertEquals("HQ1840MAEI4", sentence->sentence.stringValue->c_str(), "");

  sentence=parseHexLine(":70B0100536D617274536F6C61722043686172676572204D505054203135302F363000B2\n");
  assertEquals(VHParsedSentence::STRING, sentence->type, "Should be a STRING");
  assertEquals("SmartSolar Charger MPPT 150/60", sentence->sentence.stringValue->c_str(), "");

  sentence=parseHexLine(":740010076EF2F0871\n"); //Capablilities

  sentence=parseHexLine(":A4E030000FA\n"); //Unknown from the documentation
  delete sentence;

  sentence=parseHexLine(":305004D\n"); //Firmware version
  delete sentence;

  sentence=parseHexLine(":51641F9\n");
  assertEquals(VHParsedSentence::PONG, sentence->type, "Should be ping");
  delete sentence;

  sentence=parseHexLine(":11641FD\n");
  assertEquals(VHParsedSentence::DONE, sentence->type, "Should be done");
  delete sentence;

  sentence=parseHexLine(":A4F10000100000000006E5A00006E5A00008E12F9051E9604FFFFFFFFFFFFFFFFFFFFFFFFFF12\n");
  sentence=parseHexLine(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
  assertEquals(VHParsedSentence::HISTORY_DAILY_REGISTER, sentence->type, "HISTORY_DAILY_REGISTER");
  assertEquals(0x1050, sentence->registerId, "Daily history");
  assertEquals(127, sentence->sentence.historyDaily->yield, "yield");
  assertEquals(0xFFFFFFFF, sentence->sentence.historyDaily->consumed, "consumed");
  assertEquals(1430, sentence->sentence.historyDaily->maxBattVoltage, "maxBattVoltage");
  assertEquals(1247, sentence->sentence.historyDaily->minBattVoltage, "minBattVoltage");

  assertEquals(0, sentence->sentence.historyDaily->error0, "error0");
  assertEquals(0, sentence->sentence.historyDaily->error1, "error1");
  assertEquals(0, sentence->sentence.historyDaily->error2, "error2");
  assertEquals(0, sentence->sentence.historyDaily->error3, "error3");

//  assertEquals(0, sentence->sentence.historyDaily->timeBulk, "timeBulk");
//  assertEquals(0, sentence->sentence.historyDaily->timeAbsorbtion, "timeAbsorbtion");
//  assertEquals(41728, sentence->sentence.historyDaily->timeFloat, "timeFloat");

//  assertEquals(1, sentence->sentence.historyDaily->maxPower, "maxPower");

//  assertEquals(0, sentence->sentence.historyDaily->daySequenceNumber, "daySequenceNumber");
//  assertEquals(60561, sentence->sentence.historyDaily->maxBattCurrent, "maxBattCurrent");
  delete sentence;

  sentence=parseHexLine(":A4F10000100000000005655010056550100EB36FC051E7500FFFFFFFFFFFFFFFFFFFFFFFFFFEB\n");
  sentence=parseHexLine(":A0102000543\n");
//  sentence=parseHexLine(":8F0ED0064000C\n"); //Out of scope
  sentence=parseHexLine(":ABCED002209000077\n");
  sentence=parseHexLine(":AD5ED0009057B\n");
  sentence=parseHexLine(":ABBED00081C7F\n");
  sentence=parseHexLine(":ABDED0002009F\n");
  sentence=parseHexLine(":A0702000000000042\n");
  sentence=parseHexLine(":A0202000200000045\n");
  sentence=parseHexLine("Checksum 0:4AAAAFD\n");
  assertEquals(sentence->type, 1, "Should handle leading garbage");

  sentence=parseHexLine(":70104D\n");
}
