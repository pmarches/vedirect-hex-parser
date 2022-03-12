#include "vhp_parser.h"
#include "vhp_serial.h"

/***
 * This is a replacement for the VHPDriver class. This is a better API for applications that stream  the vedirect values and do not need a request-response style algorithm.
 */
class VHPBatchStreaming {
public:
  VHPBatchStreaming(VHPSerial* serial);

};

void myHandler(VHParsedSentence* sentence){

}

void testBatchStreaming(){
  std::string manyHexLines=VHPBatchCommands();
  VHPBatchStreaming streaming(&serial);
  serial.write(manyHexLines);
  streaming.streamFromSerialToHandler(myHandler);
}
