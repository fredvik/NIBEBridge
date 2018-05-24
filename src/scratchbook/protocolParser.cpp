/*
 * State machine protocol to read data from Nibe 360P
 * v0.1 - January 2018
**/

/*
 * telegram = hela paketet från eller till RCU
 * message  = listan av C0, length, parametrar/värden - som beräknar checksum
**/
 
// Refactor till en klass?
/*
typedef void *(*StateFunc)();
StateFunc statefunc, nextstate;

char newchar;
char messagelength;  

// States must be prototype declared
void *idle();
void *adressbegun();
void *adressed();
void *getsender();
void *getlength();
void *getregisterhigh();
void *getregisterlow();
void *getvaluehigh();
void *getvaluelow();
void *checktelegram();
void *endoftelegram();

// Functions/states in the statemachine
void *idle() {
    if (newchar == 0x00) {
        nextstate = addressbegun;
    }
    else {
        // nextstate = idle;    // There may be stray bytes with MARK parity that are not addresses
    }

    return (void*) nextstate;   // Need to cast into correct function pointer type
    // https://stackoverflow.com/questions/42951696/error-invalid-conversion-from-void-to-void-in-case-of-dlsysm
}

void *addressbegun() {
    if (newchar == 0x14) {
        nextstate = addressed;
    }
    else {
        nextstate = idle;       // Message is not for us
    }
    return (void*) nextstate;
}

void *adressed() {
    if (newchar == 0xC0) {      // Command byte(?)
        checksum = newchar;
        nextstate = getsender;
    }
    else {
        nextstate = idle;       // Error - consider logging
    }
    return (void*) nextstate;
}

void *getsender() {
    if (newchar == 0x24) {      // Sender is the NIBE controller
        checksum ^= newchar;
        nextstate = getlength;
    }
    else {
        nextstate = idle;       // Error - unkonwn sender
    }
    return (void*) nextstate;
}

void *getlength() {
    if (newchar <> 0x00) {      // Command byte(?)
        messagelength = newchar;
        checksum ^= newchar;
        nextstate = getregisterhigh;
    }
    else {
        nextstate = error;       // Error - consider logging
    }
    return (void*) nextstate;
}

void *getregisterhigh() {
    if (newchar == 0x00) {      // First byte of register is always 0x00
        checksum ^= newchar;
        nextstate = getregisterlow;
    }
    else {
        nextstate = error;       // Error - consider logging
    }
    return (void*) nextstate;
}

void *getregisterlow() {
    paramno = newchar;
    checksum ^= newchar;
    if (getparamlength(paramno) == 1) {
        nextstate = getvaluelow;
    }
    else {
        nextstate = getvaluehigh;    
    }
    return (void*) nextstate;
}

void *getvaluehigh() {
    // Byte manipulation from https://stackoverflow.com/questions/13900302/set-upper-and-lower-bytes-of-an-short-int-in-c
    paramval = (paramval & 0x00FF) | (newchar << 8);    // Set the high byte of the parameter value
    checksum ^= newchar;
    nextstate = getvaluelow;
}

void *getvaluelow() {
    paramval = (paramval & 0xFF00) | newchar;           // Set the low byte of the parameter value
    checksum ^= newchar;
    nextstate = getvaluelow;
}

void *checktelegram() {
    if (newchar == checksum) {
        sendack();               // Confirm to NIBE that message was not corrupt
        nextstate = error;    
    }
    else {
        sendack();               // Always send ACK to NIBE to avoid alarms
        nextstate = error;    
    }
}

void *endoftelegram() {
    if (newchar == 0x03) {       // ETX received
        nextstate = idle;
    }
    else {
        nextstate = error;
    }
    
}

void *error() {
}



void setup() {
    // put your setup code here, to run once: 
}

void loop() {
    // put your main code here, to run repeatedly:

    // while input is available from RS485 input, read newchar
    // Call statemachine and use the char
    statefunc = (StateFunc)(*statefunc)();

    // if (msgreceived != '') {
        

}*/