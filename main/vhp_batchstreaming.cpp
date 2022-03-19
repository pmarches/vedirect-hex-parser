#include <esp_log.h>

#include "vhp_batchstreaming.h"
#include "vhp_parser.h"

#define TAG __FILENAME__

VHPBatchStreaming::VHPBatchStreaming(VHPSerial* serial) : serial(serial) {
}


VHParsedSentence* VHPBatchStreaming::parseOneSentence(){
  std::string line=serial->readBufferFully();
  ESP_LOGD(TAG, "full buffer is '%s'", line.c_str());
  return NULL;
}

#if 0
void myHandler(const std::string& hexLine, const VHParsedSentence* sentence){
}

void testBatchStreaming(){
  std::string manyHexLines=VHPBatchCommands();
  VHPBatchStreaming streaming(&serial);
  streaming.addCommandToSend(":154\n");
  streaming.setSentenceHandler(myHandler);
}

#endif
