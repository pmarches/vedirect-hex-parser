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

#define SERIAL_DEVICE_PATH "/dev/ttyUSB0"

int serialFd;
int readBufferLen=0;
char readBuffer[256];

void configureSerialPort(){
  system("stty -F " SERIAL_DEVICE_PATH " 19200 cs8 -cstopb -parenb");
  serialFd=open(SERIAL_DEVICE_PATH, O_RDWR|O_NONBLOCK);
  if(serialFd<0){
    perror("Failed to open serial device, using stdout");
    serialFd=1;
//    exit(0);
  }
}

size_t readHexLine(){
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
      printf("Read did not finish at a line ending, ignoring\n");
    }
  }
}

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

void parseHistoryTotalRecord(const uint8_t* payloadBytes){
  typedef struct __attribute__((__packed__)) {
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
  HistoryTotalRecord* totalRecord=(HistoryTotalRecord*) payloadBytes;

  uint32_t totalYieldUser=le32toh(totalRecord->totalYieldUser);
  uint32_t totalYieldSystem=le32toh(totalRecord->totalYieldSystem);
  uint16_t maxPanelVoltage=le16toh(totalRecord->maxPanelVoltage);
  uint16_t maxBatteryVoltage=le16toh(totalRecord->maxBatteryVoltage);
  uint16_t numberOfDaysAvailable=le16toh(totalRecord->numberOfDaysAvailable);
}

void parseHistoryDayRecord(const uint8_t* payloadBytes){
  typedef struct __attribute__((__packed__)) {
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
  HistoryDailyRecord* dailyRecord=(HistoryDailyRecord*) payloadBytes;
  printf("\tparseHistoryDayRecord\n");
//  uint8_t reservedByte=parseHexByte(hexLine+0);
  uint32_t yield=le32toh(dailyRecord->yield);
  printf("\tyield=%f\n", yield*0.01);
  uint32_t consumed=le32toh(dailyRecord->consumed);//consumed by load output
  if(consumed!=0xFFFFFFFF) printf("\tconsumed=%f\n", consumed*0.01);

  uint16_t maxBattVoltage=le16toh(dailyRecord->maxBattVoltage);
  printf("\tmaxBattVoltage=%f\n", maxBattVoltage*0.01);
  uint16_t minBattVoltage=le16toh(dailyRecord->minBattVoltage);
  printf("\tminBattVoltage=%f\n", minBattVoltage*0.01);

  uint16_t timeBulk=le16toh(dailyRecord->timeBulk);
  printf("\ttimeBulk=%d\n", timeBulk);
  uint16_t timeAbsorbtion=le16toh(dailyRecord->timeAbsorbtion);
  printf("\ttimeAbsorbtion=%d\n", timeAbsorbtion);
  uint16_t timeFloat=le16toh(dailyRecord->timeFloat);
  printf("\ttimeFloat=%d\n", timeFloat);

  uint32_t maxPower=le32toh(dailyRecord->maxPower);
  printf("\tmaxPower=%d\n", maxPower);
  uint16_t maxBattCurrent=le16toh(dailyRecord->maxBattCurrent);
  printf("\tmaxBattCurrent=%f\n", maxBattCurrent*0.1);
  uint16_t maxPanelVoltage=le16toh(dailyRecord->maxPanelVoltage);
  printf("\tmaxPanelVoltage=%f\n", maxPanelVoltage*0.01);
  uint16_t daySequenceNumber=le16toh(dailyRecord->daySequenceNumber);
  printf("\tdaySequenceNumber=%d\n", daySequenceNumber);
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

void parseGet(const uint8_t* payloadBytes){
  printf("Parse GET response\n");
  uint16_t registerId=le16toh(*(uint16_t*) (payloadBytes+1));
  uint8_t flag=*(payloadBytes+3);
  printf("registerId=0x%04X flag=%d\n", registerId, flag);
  if(flag!=0){
    printf("Something is wrong with that flag");
    return;
  }
  const RegisterDesc* registerDesc=lookupRegister(registerId);
  if(NULL==registerDesc){
    printf("Did not find a description of this register\n");
    return;
  }
  printf("\tRegister for '%s'\n", registerDesc->desc);
  if(registerDesc->byteLen==1){
    uint8_t simpleByte=*(payloadBytes+4);
    printf("\tSimple byte value %0.2f%s\n", simpleByte*registerDesc->scale, registerDesc->unit);
  }
  else if(registerDesc->byteLen==2){
    uint16_t shortValue=le16toh(*(uint16_t*) (payloadBytes+4));
    printf("\tShort value %0.2f%s\n", shortValue*registerDesc->scale, registerDesc->unit);
  }
  else if(registerDesc->byteLen==4){
    uint32_t intValue=le32toh(*(uint32_t*) (payloadBytes+4));
    printf("\tInt value %0.2f%s\n", intValue*registerDesc->scale, registerDesc->unit);
  }

  if(0x0100==registerId){
    parseProductId(payloadBytes+4);
  }
  else if(0x0104==registerId){
    parseGroupId(payloadBytes+4);
  }
  else if(0x010A==registerId){
    parseSerialNumber(payloadBytes+4);
  }
  else if(0x010B==registerId){
    parseModelName(payloadBytes+4);
  }
  else if(0x0140==registerId){
    parseCapabilities(payloadBytes+4);
  }
  else if(0x0201==registerId){
    uint8_t deviceState=*(payloadBytes+4);
    printf("\tDevicestate=%d\n", deviceState);
  }
  else if(0x1050<=registerId && 0x106E>=registerId){
    parseHistoryDayRecord(payloadBytes+4);
  }
}

void parseAsync(const uint8_t* payloadBytes){
  printf("Parse ASYNC response\n");
  parseGet(payloadBytes);
}

void parseDone(const uint8_t* payloadBytes){
  printf("TODO Parse DONE response\n");
  //Parsing DONE messages depends on the last command we sent...
}

void parsePing(const uint8_t* payloadBytes){
  printf("Parse ping response\n");
  uint16_t typeAndVersion=le16toh(*(uint16_t*)(payloadBytes+1));
  printf("\ttypeAndVersion=0x%02X\n", typeAndVersion);
  uint8_t appType=typeAndVersion>>14;
  printf("\tappType=%d\n", appType);
  if(3==appType){
    uint8_t rcVersion=payloadBytes[1]&0b00111111;
    printf("\tbooloader RC=%d \n", rcVersion);
  }
  else{
    printf("\tversion=%02X\n", typeAndVersion&0b0011111111111111);
  }
}


void parseHexLine(const char* hexLine){
  if(hexLine==NULL) {
    return;
  }
  printf("Parse %s", hexLine);

  int hexLineLen=strlen(hexLine);
  if(hexLineLen<5){
    printf("Hexline is too short\n");
    return;
  }
  if(hexLine[0]!=':' || hexLine[hexLineLen-1]!='\n'){
    printf("The hexline length should start with : end with \n and be of odd length\n");
    return;
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
    return;
  }

  if(0x1==payloadBytes[0]) {
    parseDone(payloadBytes);
  }
  else if(0x3==payloadBytes[0]) {
    printf("Got UNKNOWN message\n");
  }
  else if(0x4==payloadBytes[0]) {
    printf("Got ERROR message\n");
  }
  else if(0x5==payloadBytes[0]) {
    parsePing(payloadBytes);
  }
  else if(0x7==payloadBytes[0]) {
    parseGet(payloadBytes);
  }
  else if(0x8==payloadBytes[0]) {
    printf("Got SET response\n");
  }
  else if(0xA==payloadBytes[0]) {
    parseAsync(payloadBytes);
  }

  free(payloadBytes);
}

void sendBytes(const uint8_t* payload, const uint16_t payloadLen){
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

  write(serialFd, hexLine, hexLineLen);
  free(hexLine);
}

void sendPing(){
  printf("ANSWER: :51641F9\n");
  uint8_t pingCommandBytes[]={0x01};
  sendBytes(pingCommandBytes, 1);
  size_t hexLineLen=readHexLine();
  parseHexLine(readBuffer);
}

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

void getRegisterValue(uint16_t registerToGet){
  const uint8_t flag=0;
#if 0
  uint8_t binaryPayload[]={
      GET,
      (uint8_t)  (registerToGet&0x00FF),
      (uint8_t) ((registerToGet&0xFF00)>>8),
      flag,
      0 //placeholder
  };
  binaryPayload[4]=computeChecksum(binaryPayload, 4);
  uint16_t nbBytesOfHexMessage=1+sizeof(binaryPayload)*2+1;
  char* hexMessage=(char*) malloc(nbBytesOfHexMessage+1);
  bytesToHex(binaryPayload, 4+2, hexMessage);
  hexMessage[0]=':';
  hexMessage[nbBytesOfHexMessage-2]='\n';
  hexMessage[nbBytesOfHexMessage-1]=0;

  printf("hexMessage=%s\n", hexMessage);
  free(hexMessage);
#endif
  uint8_t commandBytes[]={
      GET,
      (uint8_t)  (registerToGet&0x00FF),
      (uint8_t) ((registerToGet&0xFF00)>>8),
      flag,
  };
  sendBytes(commandBytes, 4);
  size_t hexLineLen=readHexLine();
  parseHexLine(readBuffer);
}

void requestModelName(){
  printf("ANSWER: :70A010061626364B9\n");
  getRegisterValue(0x010A);
}

void requestProductId(){
  printf("ANSWER: :700010048A065\n");
  getRegisterValue(0x0100);
}

void testParser(){
  parseHexLine(":51641F9\n");
  parseHexLine(":11641FD\n");
//  parseHexLine(":A4F10000100000000006E5A00006E5A00008E12F9051E9604FFFFFFFFFFFFFFFFFFFFFFFFFF12\n");
  parseHexLine(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
  parseHexLine(":A0102000543\n");
  parseHexLine(":8F0ED0064000C\n");
}

int main(int argc, char **argv) {
#if 0
  testParser();
  printf("ANSWER: :7F0ED009600DB\n");
  getRegisterValue(0xEDF0);
  return 0;
#else
  configureSerialPort();
	sendPing();
	requestProductId();
	requestModelName();

//	while(true){
//	  size_t hexLineLen=readHexLine();
//	  parseHexLine(readBuffer);
//	}
	return 0;
#endif
}
