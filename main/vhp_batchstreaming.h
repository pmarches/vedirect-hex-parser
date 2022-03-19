#ifndef COMPONENTS_VEDIRECT_HEX_PARSER_MAIN_VHP_BATCHSTREAMING_H_
#define COMPONENTS_VEDIRECT_HEX_PARSER_MAIN_VHP_BATCHSTREAMING_H_

#include <inttypes.h>
#include "vhp_driver.h"

/***
 * This is a replacement for the VHPDriver class. This is a better API for applications that stream  the vedirect values and do not need a request-response style algorithm.
 */
class VHPBatchStreaming {
  VHPSerial* serial;
public:
  VHPBatchStreaming(VHPSerial* serial);
  void addCommandToSend(std::string& hexCommandToSend);
//  void setSentenceHandler((void)(const std::string&, const VHParsedSentence*));
  VHParsedSentence* parseOneSentence();
};



#endif /* COMPONENTS_VEDIRECT_HEX_PARSER_MAIN_VHP_BATCHSTREAMING_H_ */
