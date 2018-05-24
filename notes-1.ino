/*
Here are notes for the project

MEMORY
======
Use the F() macro when using literal strings in printouts etc. Then they are stored in flash and not in SRAM.
E.g. use:   Serial.println(F(“Hello World”));
https://www.baldengineer.com/arduino-f-macro.html
Even more important for Pro Mini!

Use PROGMEM() macro when declaring arrays and larger data that should be in the program memory and not in the SRAM.





int paramno;        // Nibe modbus parameter number
int ourparamno;     // Our internal number for the parameter
int paramtype[70]   // Data type for each parameter
int paramfactor[70] // Conversion factor for each parameter
int parammin[70]    // Min values for some parameters
int parammax[70]    // Max values for some parameters


if (paramtype(paramno) == char) {
    convert char value to u16int
}

switch (paramno) {
    case 20:  // Kompressor, cirkpump1, cirkpump 2
        

}





*/
