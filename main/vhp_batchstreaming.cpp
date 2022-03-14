#include "vhp_parser.h"
#include "vhp_serial.h"

/***
 * This is a replacement for the VHPDriver class. This is a better API for applications that stream  the vedirect values and do not need a request-response style algorithm.
 */
class VHPBatchStreaming {
public:
  VHPBatchStreaming(VHPSerial* serial);
  void addCommandToSend(std::string& hexCommandToSend);
  void setSentenceHandler((void)(const std::string&, const VHParsedSentence*));
};

void myHandler(const std::string& hexLine, const VHParsedSentence* sentence){
}

void testBatchStreaming(){
  std::string manyHexLines=VHPBatchCommands();
  VHPBatchStreaming streaming(&serial);
  streaming.addCommandToSend(":154\n");
  streaming.setSentenceHandler(myHandler);
}
