#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>

#include "vhp_registers.h"
#include "vhp_parser.h"

uint8_t computeChecksum(const uint8_t* binaryPayload, const uint8_t nbBytesPayload){
  uint8_t checksum=0x55;
  for(int i=0; i<nbBytesPayload; i++){
    checksum -= binaryPayload[i];
  }
  return checksum;
}

void byteToHex(uint8_t byte, char* hexStr){
  sprintf(hexStr, "%02X", byte);
}

void bytesToHex(const uint8_t* bytes, const uint16_t nbBytes, char* hexOut){
  for(int i=0; i<nbBytes; i++){
    byteToHex(bytes[i], hexOut+(i*2));
  }
}

uint8_t hexCharToNibble(const char hexChar){
  if(hexChar>='0' && hexChar<='9') return hexChar-'0';
  if(hexChar>='A' && hexChar<='F') return hexChar-'A'+10;
  return 0;
}

void hexToBytes(const char* hexLine, uint16_t hexLineLen, uint8_t* outputBytes){
  if(hexLineLen%2!=0){
    printf("Warning, converting a odd length hex string. There may be data loss.\n");
  }
  uint16_t outputBytesLen=hexLineLen/2;
  for(int i=0; i<outputBytesLen; i++){
    uint8_t nibbleHigh=hexCharToNibble(hexLine[i*2]);
    uint8_t nibbleLow=hexCharToNibble(hexLine[(i*2)+1]);
    outputBytes[i]=nibbleLow|(nibbleHigh<<4);
//    printf("hexLine[i*2]=%c outputBytes=0x%02X nibleLow=0x%02X nibleHigh=0x%02X \n", hexLine[i*2], outputBytes[i], nibleLow, nibleHigh);
  }
}

void parseHistoryTotalRecord(const uint8_t* payloadBytes, uint16_t payloadBytesLen, VHParsedSentence* sentence){
  sentence->sentenceType=VHParsedSentence::HISTORY_TOTAL_REGISTER;
  sentence->sentence.historyTotal=new HistoryTotalRecord();

  sentence->sentence.historyTotal->totalYieldUser=le32toh(*((uint32_t*) payloadBytes));
  sentence->sentence.historyTotal->totalYieldSystem=le32toh(*((uint32_t*) payloadBytes+4));
//  printf("\ttotalYieldSystem=%d\n", totalYieldSystem);
  sentence->sentence.historyTotal->maxPanelVoltage=le16toh(*((uint32_t*) payloadBytes+8));
  sentence->sentence.historyTotal->maxBatteryVoltage=le16toh(*((uint32_t*) payloadBytes+10));
  sentence->sentence.historyTotal->numberOfDaysAvailable=le16toh(*((uint32_t*) payloadBytes+12));
}

void parseHistoryDayRecord(const uint8_t* payloadBytes, uint16_t payloadBytesLen, VHParsedSentence* sentence){
  sentence->sentenceType=VHParsedSentence::HISTORY_DAILY_REGISTER;
  sentence->sentence.historyDaily=new HistoryDailyRecord();
  sentence->sentence.historyDaily->reservedByte=(*((uint32_t*) payloadBytes+0));
  sentence->sentence.historyDaily->yield=le32toh(*((uint32_t*) payloadBytes+1));
  sentence->sentence.historyDaily->consumed=le32toh(*((uint32_t*) payloadBytes+5));//consumed by load output

  sentence->sentence.historyDaily->maxBattVoltage=le16toh(*((uint32_t*) payloadBytes+7));
  sentence->sentence.historyDaily->minBattVoltage=le16toh(*((uint32_t*) payloadBytes+9));

  sentence->sentence.historyDaily->timeBulk=le16toh(*((uint32_t*) payloadBytes+11));
  sentence->sentence.historyDaily->timeAbsorbtion=le16toh(*((uint32_t*) payloadBytes+13));
  sentence->sentence.historyDaily->timeFloat=le16toh(*((uint32_t*) payloadBytes+15));

  sentence->sentence.historyDaily->maxPower=le32toh(*((uint32_t*) payloadBytes+17));
  sentence->sentence.historyDaily->maxBattCurrent=le16toh(*((uint32_t*) payloadBytes+21));
  sentence->sentence.historyDaily->maxPanelVoltage=le16toh(*((uint32_t*) payloadBytes+23));
  sentence->sentence.historyDaily->daySequenceNumber=le16toh(*((uint32_t*) payloadBytes+25));
}

void parseProductId(const uint8_t* payloadBytes){
  printf("\tparseProductId\n");
  uint16_t productId=le16toh(*(uint16_t*) payloadBytes);
  printf("\tproductId=0x%X\n",productId);
  const ProductDescription* productDesc=lookupProductId(productId);
  if(productDesc==NULL){
    printf("Product id not found\n");
    return;
  }
  printf("\tProduct name is %s\n", productDesc->productName);
}

void parseGroupId(const uint8_t* payloadBytes){
  printf("\tparseGroupId\n");
  printf("\tgroupId=%d\n", *payloadBytes);
}

void parseSerialNumber(const uint8_t* payloadBytes){
  printf("\tparseSerialNumber\n");
  char* serialNumber=(char*)payloadBytes;
  int serialNumberLen=4;
  printf("\tserialNumber=%.*s\n", serialNumberLen, serialNumber);
}

void parseModelName(const uint8_t* payloadBytes){
  printf("\tparseModelName\n");
  char* modelName=(char*) payloadBytes;
  printf("\tmodelName=%s\n", modelName);
}

void parseCapabilities(const uint8_t* payloadBytes){
  uint32_t capabilities=le32toh(*(uint32_t*)payloadBytes);
  printf("\tparseCapabilities\n");
  printf("\tcapabilities=0x%X\n", capabilities);
}

VHParsedSentence* parseGet(const uint8_t* payloadBytes, uint16_t payloadBytesLen){
  printf("Parse GET response\n");
  if(payloadBytesLen<5){
    printf("Got a GET response but the payload length is wrong. %d bytes\n", payloadBytesLen);
    return NULL;
  }
  uint16_t registerId=le16toh(*(uint16_t*) (payloadBytes+1));
  const RegisterDesc* registerDesc=lookupRegister(registerId);
  if(registerDesc==NULL){
    printf("Don't know how to decode register 0x%X\n", registerId);
    return NULL;
  }

  uint8_t flag=*(payloadBytes+3);
  printf("registerId=0x%04X flag=%d\n", registerId, flag);
  if(flag!=0){
    printf("Something is wrong with that flag");
    return NULL;
  }

  VHParsedSentence* sentence=new VHParsedSentence(registerId);
  if(registerDesc->byteLen==1 && registerDesc->encoding==SIGNED){
    sentence->sentenceType=VHParsedSentence::SIGNED_REGISTER;
    sentence->sentence.signedRegister=new SignedRegister();
    sentence->registerId=registerId;
    sentence->sentence.signedRegister->registerValueSigned=*(payloadBytes+4);
  }
  else if(registerDesc->byteLen==1 && registerDesc->encoding==UNSIGNED){
    sentence->sentenceType=VHParsedSentence::UNSIGNED_REGISTER;
    sentence->sentence.unSignedRegister=new UnSignedRegister();
    sentence->registerId=registerId;
    sentence->sentence.unSignedRegister->registerValueUnsigned=*(payloadBytes+4);
  }
  else if(registerDesc->byteLen==2 && registerDesc->encoding==SIGNED){
    sentence->sentenceType=VHParsedSentence::SIGNED_REGISTER;
    sentence->sentence.signedRegister=new SignedRegister();
    sentence->registerId=registerId;
    sentence->sentence.signedRegister->registerValueSigned=le16toh(*(uint16_t*) (payloadBytes+4));
  }
  else if(registerDesc->byteLen==4 && registerDesc->encoding==SIGNED){
    sentence->sentenceType=VHParsedSentence::SIGNED_REGISTER;
    sentence->sentence.signedRegister=new SignedRegister();
    sentence->registerId=registerId;
    sentence->sentence.signedRegister->registerValueSigned=le32toh(*(uint32_t*) (payloadBytes+4));
  }
  else if(registerDesc->byteLen==2 && registerDesc->encoding==UNSIGNED){
    sentence->sentenceType=VHParsedSentence::UNSIGNED_REGISTER;
    sentence->sentence.unSignedRegister=new UnSignedRegister();
    sentence->registerId=registerId;
    sentence->sentence.unSignedRegister->registerValueUnsigned=le16toh(*(uint16_t*) (payloadBytes+4));
  }
  else if(registerDesc->byteLen==4 && registerDesc->encoding==UNSIGNED){
    sentence->sentenceType=VHParsedSentence::UNSIGNED_REGISTER;
    sentence->sentence.unSignedRegister=new UnSignedRegister();
    sentence->registerId=registerId;
    sentence->sentence.unSignedRegister->registerValueUnsigned=le32toh(*(uint32_t*) (payloadBytes+4));
  }
  else if(0x0100==sentence->registerId){
    parseProductId(payloadBytes+4);
  }
  else if(0x0104==sentence->registerId){
    parseGroupId(payloadBytes+4);
  }
  else if(0x010A==sentence->registerId){
    parseSerialNumber(payloadBytes+4);
  }
  else if(0x010B==sentence->registerId){
    parseModelName(payloadBytes+4);
  }
  else if(0x0140==sentence->registerId){
    parseCapabilities(payloadBytes+4);
  }
  else if(0x1050<=sentence->registerId && 0x106E>=sentence->registerId){
    parseHistoryDayRecord(payloadBytes+4, payloadBytesLen, sentence);
  }
  else if(0x104F==sentence->registerId){
    parseHistoryTotalRecord(payloadBytes+4, payloadBytesLen, sentence);
  }
  else{
    printf("Unhandeled combination of byte len %d encoding %d and register 0x%X\n", registerDesc->byteLen, registerDesc->encoding, sentence->registerId);
    exit(1);
  }
  return sentence;
}

VHParsedSentence* parseAsync(const uint8_t* payloadBytes, const uint16_t payloadBytesLen){
  printf("Parse ASYNC response\n");
  VHParsedSentence* sentence=parseGet(payloadBytes, payloadBytesLen);
  sentence->isAsync=true;
  return sentence;
}

void parseDone(const uint8_t* payloadBytes){
  printf("TODO Parse DONE response\n");
  //Parsing DONE messages depends on the last command we sent...
}

VHParsedSentence* parsePong(const uint8_t* payloadBytes){
  printf("Parse ping response\n");
  VHParsedSentence* sentence=new VHParsedSentence(0);
  sentence->sentenceType=VHParsedSentence::PING;
  sentence->sentence.pingResponse=new ParsedSentencePingResponse();

  sentence->sentence.pingResponse->typeAndVersion=le16toh(*(uint16_t*)(payloadBytes+1));
  sentence->sentence.pingResponse->appType=sentence->sentence.pingResponse->typeAndVersion>>14;
  if(3==sentence->sentence.pingResponse->appType){
    sentence->sentence.pingResponse->rcVersion=payloadBytes[1]&0b00111111;
  }
  return sentence;
}


VHParsedSentence* parseHexLine(const char* hexLine){
  if(hexLine==NULL) {
    return NULL;
  }
  printf("Parsing %s", hexLine);

  int hexLineLen=strlen(hexLine);
  if(hexLineLen<5){
    printf("Hexline is too short\n");
    return NULL;
  }
  if(hexLine[0]!=':' || hexLine[hexLineLen-1]!='\n'){
    printf("The hexline length should start with : end with \n and be of odd length\n");
    return NULL;
  }

  uint16_t hexPayloadLen=hexLineLen-2; //The : and \n are not part of the payload
  uint16_t payloadBytesLen=(hexPayloadLen/2)+1; //The payload includes the command
  uint8_t* payloadBytes=(uint8_t*) malloc(payloadBytesLen);
  payloadBytes[0]=hexCharToNibble(hexLine[1]); //Convert the command (first low nibble)
  hexToBytes(hexLine+2, hexPayloadLen-1, payloadBytes+1); //Skip the command and the trailing \n

  uint8_t computedChecksum=computeChecksum(payloadBytes, payloadBytesLen-1); //Do not use the checksum byte to compute the the checksum
  if(payloadBytes[payloadBytesLen-1]!=computedChecksum){
    printf("Checksum error: Received checksum 0x%02X but computed 0x%02X\n", payloadBytes[payloadBytesLen-1], computedChecksum);
    free(payloadBytes);
    return NULL;
  }

  VHParsedSentence *sentence=NULL;
  if(HEXCMD_DONE==payloadBytes[0]) {
    parseDone(payloadBytes);
  }
  else if(HEXCMD_APP_VERSION==payloadBytes[0]) {
    printf("Got AppVersion message\n");
  }
  else if(HEXCMD_PRODUCT_ID==payloadBytes[0]) {
    printf("Got ERROR message\n");
  }
  else if(HEXCMD_PING==payloadBytes[0]) {
    sentence=parsePong(payloadBytes);
  }
  else if(HEXCMD_GET==payloadBytes[0]) {
    sentence=parseGet(payloadBytes, payloadBytesLen);
  }
  else if(HEXCMD_SET==payloadBytes[0]) {
    printf("Got SET response\n");
  }
  else if(HEXCMD_ASYNC==payloadBytes[0]) {
    sentence=parseAsync(payloadBytes, payloadBytesLen);
  }

  free(payloadBytes);
  if(sentence->sentenceType==VHParsedSentence::SentenceType::NONE){
    printf("Unknown HEX command\n");
    exit(1);
  }
  return sentence;
}

void VHPBuildGetRegisterPayload(uint16_t registerToGet, uint8_t flag, uint8_t* payloadBytes){
  payloadBytes[0]=HEXCMD_GET;
  payloadBytes[1]=(uint8_t)  (registerToGet&0x00FF); //TODO Convert to little endian properly
  payloadBytes[2]=(uint8_t) ((registerToGet&0xFF00)>>8); //TODO Convert to little endian properly
  payloadBytes[3]=flag;
}

void assertEquals(uint32_t expected, uint32_t actual, const char* failureMsg){
  if(expected!=actual){
    printf("%s\n", failureMsg);
    exit(0);
  }
}

void testParser(){
  VHParsedSentence* sentence;
  sentence=parseHexLine(":51641F9\n");
  assertEquals(VHParsedSentence::PING, sentence->sentenceType, "Should be ping");

  sentence=parseHexLine(":11641FD\n");
//  sentence=parseHexLine(":A4F10000100000000006E5A00006E5A00008E12F9051E9604FFFFFFFFFFFFFFFFFFFFFFFFFF12\n");
  sentence=parseHexLine(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
  sentence=parseHexLine(":A4F10000100000000005655010056550100EB36FC051E7500FFFFFFFFFFFFFFFFFFFFFFFFFFEB\n");
  sentence=parseHexLine(":A0102000543\n");
  sentence=parseHexLine(":8F0ED0064000C\n");
  sentence=parseHexLine(":ABCED002209000077\n");
  sentence=parseHexLine(":AD5ED0009057B\n");
  sentence=parseHexLine(":ABBED00081C7F\n");
  sentence=parseHexLine(":ABDED0002009F\n");
  sentence=parseHexLine(":A0702000000000042\n");
  sentence=parseHexLine(":A0202000200000045\n");
}
