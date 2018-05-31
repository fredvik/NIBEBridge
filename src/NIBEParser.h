#include <stdint.h>

class NIBEParser {
public:
  NIBEParser();
  ~NIBEParser(); // Is the destructor needed or useful?
  void begin();

private:
  typedef void *(*StateFunc)();
  StateFunc statefunc;
  StateFunc nextstate;

  uint8_t newchar;
  uint8_t messagelength;
  uint8_t checksum;

  // State functions
  void *idle();
  void *addressbegun();
  void *addressed();
  void *getsender();
  void *getlength();
  void *getregisterhigh();
  void *getregisterlow();
  void *getvaluehigh();
  void *getvaluelow();
  void *checktelegram();
  void *endoftelegram();
  void *error();
};