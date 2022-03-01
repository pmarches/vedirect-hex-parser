#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>

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

void sendToVedirectSerial(char* hexBuffer, size_t hexBufferLen){
  if(hexBufferLen==0 || hexBuffer==NULL){
    return;
  }
  write(serialFd, ":", 1);
  write(serialFd, hexBuffer, hexBufferLen);
  write(serialFd, "\n", 1);
}

size_t readHexLine(){
  memset(readBuffer, 0, sizeof(readBuffer));
  while(true){
    int nbBytesRead=read(serialFd, readBuffer, sizeof(readBuffer));
    if(nbBytesRead<0){
      continue;
    }
    if(readBuffer[0]!=':' || readBuffer[nbBytesRead-1]!='\n'){
      printf("Read did not finish at line ending\n");
      return 0;
    }
    else{
//      printf("Read %d bytes, %.*s", nbBytesRead, nbBytesRead, readBuffer);
      return nbBytesRead;
    }
  }
}

uint16_t parseHexByte(const char* hexLine){
//  printf("parseHexByte=%s\n", hexLine);
  char hexByte[3];
  hexByte[0]=hexLine[0];
  hexByte[1]=hexLine[1];
  hexByte[2]=0;
  return strtoul(hexByte, NULL, 16);
}

uint16_t parseHexShort(const char* hexLine){
//  printf("parseHexShort=%s\n", hexLine);
  char hexShort[5];
  hexShort[0]=hexLine[2];
  hexShort[1]=hexLine[3];
  hexShort[2]=hexLine[0];
  hexShort[3]=hexLine[1];
  hexShort[4]=0;
  return strtoul(hexShort, NULL, 16);
}

uint32_t parseHexInteger(const char* hexLine){
//  printf("parseHexInteger=%s\n", hexLine);
  char hexInteger[9];
  hexInteger[0]=hexLine[6];
  hexInteger[1]=hexLine[7];
  hexInteger[2]=hexLine[4];
  hexInteger[3]=hexLine[5];
  hexInteger[4]=hexLine[2];
  hexInteger[5]=hexLine[3];
  hexInteger[6]=hexLine[0];
  hexInteger[7]=hexLine[1];
  hexInteger[8]=0;
  return strtoul(hexInteger, NULL, 16);
}

uint8_t hexCharToByte(const char hexChar){
  if(hexChar>='0' && hexChar<='9') return '9'-hexChar;
  if(hexChar>='A' && hexChar<='F') return 'A'-hexChar;
  return 0;
}

void parseHexString(const char* hexLine, char* destinationString, uint32_t maxDestinationLen){
  destinationString[maxDestinationLen-1]=0;
  for(uint8_t i=0; i<maxDestinationLen; i++){
    uint8_t nibleHigh=hexCharToByte(hexLine[i*2]);
    uint8_t nibleLow=hexCharToByte(hexLine[(i*2)+1]);
    destinationString[i]=nibleHigh<<4|nibleLow;
  }
}

void parseHistoryTotalRecord(const char* hexLine){
//  uint8_t reservedByte=parseHexByte(hexLine+0);
  uint8_t errorDatabase=parseHexByte(hexLine+2);
  uint8_t error0=parseHexByte(hexLine+4);
  uint8_t error1=parseHexByte(hexLine+6);
  uint8_t error2=parseHexByte(hexLine+8);
  uint8_t error3=parseHexByte(hexLine+10);

  uint32_t totalYieldUser=parseHexInteger(hexLine+12);
  uint32_t totalYieldSystem=parseHexInteger(hexLine+20);
  uint16_t maxPanelVoltage=parseHexShort(hexLine+28);
  uint16_t maxBatteryVoltage=parseHexShort(hexLine+32);
  uint16_t numberOfDaysAvailable=parseHexShort(hexLine+36);
}

void parseHistoryDayRecord(const char* hexLine){
  printf("\tparseHistoryDayRecord\n");
//  uint8_t reservedByte=parseHexByte(hexLine+0);
  uint32_t yield=parseHexInteger(hexLine+2);
  printf("\tyield=%f\n", yield*0.01);
  uint32_t consumed=parseHexInteger(hexLine+10);//consumed by load output
  if(consumed!=0xFFFFFFFF) printf("consumed=%f\n", consumed*0.01);

  uint16_t maxBattVoltage=parseHexShort(hexLine+18);
  printf("\tmaxBattVoltage=%f\n", maxBattVoltage*0.01);
  uint16_t minBattVoltage=parseHexShort(hexLine+22);
  printf("\tminBattVoltage=%f\n", minBattVoltage*0.01);

  uint8_t errorDatabase=parseHexByte(hexLine+26);
  uint8_t error0=parseHexByte(hexLine+28);
  uint8_t error1=parseHexByte(hexLine+30);
  uint8_t error2=parseHexByte(hexLine+32);
  uint8_t error3=parseHexByte(hexLine+34);

  uint16_t timeBulk=parseHexShort(hexLine+36);
  printf("\ttimeBulk=%d\n", timeBulk);
  uint16_t timeAbsorbtion=parseHexShort(hexLine+40);
  printf("\ttimeAbsorbtion=%d\n", timeAbsorbtion);
  uint16_t timeFloat=parseHexShort(hexLine+44);
  printf("\ttimeFloat=%d\n", timeFloat);

  uint32_t maxPower=parseHexInteger(hexLine+48);
  printf("\tmaxPower=%d\n", maxPower);
  uint16_t maxBattCurrent=parseHexShort(hexLine+56);
  printf("\tmaxBattCurrent=%f\n", maxBattCurrent*0.1);
  uint16_t maxPanelVoltage=parseHexShort(hexLine+60);
  printf("\tmaxPanelVoltage=%f\n", maxPanelVoltage*0.01);
  uint16_t daySequenceNumber=parseHexShort(hexLine+64);
  printf("\tdaySequenceNumber=%d\n", daySequenceNumber);
}

void parseProductId(const char* hexLine){
  printf("\tparseProductId\n");
  uint16_t productId=parseHexShort(hexLine);
  printf("\tproductId=0x%X\n",productId);
  if(0xA056==productId) {
    printf("\tSmartSolar MPPT 100|30");
  }
  else if(0xA062==productId) {
    printf("\tSmartSolar MPPT 150|60");
  }
  //TODO etc.. Get the list from the PDF
}

void parseGroupId(const char* hexLine){
  printf("\tparseGroupId\n");
  uint8_t groupId=parseHexByte(hexLine);
  printf("\tgroupId=%d\n", groupId);
}

void parseSerialNumber(const char* hexLine){
  printf("\tparseSerialNumber\n");
  int serialNumberLen=strlen(hexLine)-2;
  char* serialNumber=(char*) malloc(serialNumberLen+1);
  parseHexString(hexLine, serialNumber, serialNumberLen);
  printf("\tserialNumberLen=%s", serialNumber);
  free(serialNumber);
}

void parseModelName(const char* hexLine){
  printf("\tparseModelName\n");
  int modelNameLen=strlen(hexLine)-2;
  char* modelName=(char*) malloc(modelNameLen+1);
  parseHexString(hexLine, modelName, modelNameLen);
  printf("\tmodelNameLen=%s", modelName);
  free(modelName);
}

void parseCapabilities(const char* hexLine){
  uint32_t capabilities=parseHexInteger(hexLine);
  printf("\tparseCapabilities\n");
  printf("\tcapabilities=0x%X\n", capabilities);
}

void parseGet(const char* hexLine){
  printf("Parse GET response\n");
  uint16_t registerId=parseHexShort(hexLine);
  uint8_t flag=parseHexByte(hexLine+4);
  printf("registerId=0x%X flag=%d\n", registerId, flag);
  if(flag!=0){
    printf("Something is wrong with that flag");
    return;
  }

  if(0x0100==registerId){
    parseProductId(hexLine+6);
  }
  else if(0x0104==registerId){
    parseGroupId(hexLine+6);
  }
  else if(0x010A==registerId){
    parseSerialNumber(hexLine+6);
  }
  else if(0x010B==registerId){
    parseModelName(hexLine+6);
  }
  else if(0x0140==registerId){
    parseCapabilities(hexLine+6);
  }
  else if(0x1050<=registerId && 0x106E>=registerId){
    parseHistoryDayRecord(hexLine+6);
  }
}

void parseAsync(const char* hexLine){
  printf("Parse ASYNC response\n");
  parseGet(hexLine);
}

void parseDone(const char* hexLine){
  printf("Parse DONE response\n");
  //Parsing DONe depends on the last command we sent...
}

void parsePing(const char* hexLine){
  printf("Parse ping response\n");

  char hexShort[5];
  hexShort[0]=hexLine[4];
  hexShort[1]=hexLine[5];
  hexShort[2]=hexLine[2];
  hexShort[3]=hexLine[3];
//  printf("hexShort=%s\n", hexShort);
  uint8_t firmwareVersionNible = '9'-hexShort[0];
  uint8_t appType=firmwareVersionNible&0b1100;

//  printf("appType=%d\n", appType);
  if(4==appType){
    uint8_t rcVersion=firmwareVersionNible&0b0011;
    printf("\tApplication Version:%s RC=%d \n", hexShort+1, rcVersion);
  }
}

void parseHexLine(const char* hexLine){
  if(hexLine==NULL) {
    return;
  }
  printf("Parse %s", hexLine);
  //TODO Checksum...
  if('1'==hexLine[1]) {
    parseDone(hexLine+2);
  }
  else if('3'==hexLine[1]) {
    printf("Got UNKNOWN message\n");
  }
  else if('4'==hexLine[1]) {
    printf("Got ERROR message\n");
  }
  else if('5'==hexLine[1]) {
    parsePing(hexLine+2);
  }
  else if('7'==hexLine[1]) {
    parseGet(hexLine+2);
  }
  else if('8'==hexLine[1]) {
    printf("Got SET response\n");
  }
  else if('A'==hexLine[1]) {
    parseAsync(hexLine+2);
  }
}

void sendPing(){
//  char hexBuffer[]={1,5,4};
//  size_t hexBufferLen=sizeof(hexBuffer);
//  sendToVedirectSerial(hexBuffer, hexBufferLen);
  char buffer[]={':', '1', '5', '4', '\n'};
  write(serialFd, buffer, sizeof(buffer));

  while(true){
    size_t hexLineLen=readHexLine();
    parseHexLine(readBuffer);
  }
}

#if 0
void emptyReadBuffer(){
  printf("Emptying serial buffer\n");
  char readBuffer[32];
  while(true){
    if(read(serialFd, readBuffer, sizeof(readBuffer))<=0){
      break;
    }
//    printf("readBuffer=%c\n", readBuffer[0]);
  }
  printf("Done emptying serial buffer\n");
}
#endif

void testParser(){
  parseHexLine(":51641F9\n");
  parseHexLine(":11641FD\n");
//  parseHexLine(":A4F10000100000000006E5A00006E5A00008E12F9051E9604FFFFFFFFFFFFFFFFFFFFFFFFFF12\n");
  parseHexLine(":A501000007F000000FFFFFFFF9605DF040000000000A300830177007401000000012811F700AE\n");
}

int main(int argc, char **argv) {
  testParser();
  return 0;
	configureSerialPort();
//	emptyReadBuffer();
	sendPing();
	return 0;
}
