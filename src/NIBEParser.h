#include <stdint.h>

//typedef pStateFunc *(*StateFunc)();


class NIBEParser {
typedef void (*pStateFunc)();
public:
  NIBEParser();
  // virtual ~NIBEParser(); // Is the destructor needed or useful?
  void begin();
  void performState();
private:
  void setStateFunction(pStateFunc fptr);
  // Variables
  pStateFunc stateFunction;
  uint8_t newchar;
  uint8_t messagelength;
  uint8_t checksum;

  // State functions
   void idle();
   void addressbegun();
   void addressed();
   void getsender();
   void getlength();
   void getregisterhigh();
   void getregisterlow();
   void getvaluehigh();
   void getvaluelow();
   void checktelegram();
   void endoftelegram();
   void error();
};