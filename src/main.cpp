 /*
 * NibeBridge - link your Nibe Fighter to your favourite home automation system
*/

#include <ESP8266WiFi.h>
//#include <Arduino.h>                  // Not needed at all(?)
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>             // https://github.com/addibble/SoftwareSerial9
#include <Syslog.h>                     // https://github.com/arcao/Syslog

// Syslog server connection info
#define SYSLOG_SERVER "192.168.1.5"
#define SYSLOG_PORT 514
#define DEVICE_HOSTNAME "NIBE-ESP"
#define APP_NAME "NibeBridge"
WiFiUDP udpClient;                      // A UDP instance to send and receive packets over UDP

// Create a new syslog instance with LOG_KERN facility
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
int iteration = 1;

// WiFi Configuration
const char* wifissid = "Eirreann";
const char* wifipasswd = "nightcap";

// Serial comm configuration
SoftwareSerial mySerial(10, 11);        // RX, TX

// Other variables and constants of less importance
int counter=0;
unsigned long blinkinterval;
unsigned long lastblinktime;
int ledState = LOW;

// ****************************************************
// Setup code here, to run once: 
// ****************************************************
void setup() {

    // Initialize digital pin for LED output.
    pinMode(LED_BUILTIN, OUTPUT);
    blinkinterval = 2000;
    lastblinktime = millis();
    counter = 0;

    // Initialize mySerial
    mySerial.begin(19200);
    Serial.begin(19200);

    // Initialize WiFi - required for operation
    // DHCP IP adress assigned: 192.168.1.250 - hostname: ESP-NIBE
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifissid, wifipasswd);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("ERROR - WiFi connection failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    // Over The Air (OTA) setup
    // ArduinoOTA.setPort(uint16_t 8266);                   // default 8266
    // ArduinoOTA.setHostname(const char* "espnibe");       // default none - TODO
    // ArduinoOTA.setPassword(const char* "nibepass");      

    ArduinoOTA.onStart([]() {
        syslog.logf(LOG_INFO, "Starting OTA...");
    });
    ArduinoOTA.onEnd([]() {
        syslog.logf(LOG_INFO, "\nEnded OTA...");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        syslog.logf(LOG_INFO, "OTA progressing...");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        syslog.logf(LOG_ERR, "Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) syslog.logf(LOG_ERR, "Auth failed.");
        else if (error == OTA_BEGIN_ERROR) syslog.log(LOG_ERR, "Begin failed."); 
        else if (error == OTA_CONNECT_ERROR) syslog.log(LOG_ERR, "Connect failed.");
        else if (error == OTA_RECEIVE_ERROR) syslog.log(LOG_ERR, "Receive failed.");
        else if (error == OTA_END_ERROR) syslog.log(LOG_ERR, "End failed.");
    });
    ArduinoOTA.begin();
    syslog.logf(LOG_INFO, "IP address: %s", WiFi.localIP().toString().c_str());
    syslog.logf(LOG_DEBUG, "Free sketch space is %u", ESP.getFreeSketchSpace());
}

void loop() {
    // put your main code here, to run repeatedly:
    ArduinoOTA.handle();

    unsigned long millisNow = millis();
    if (millisNow-lastblinktime >= blinkinterval) {
        lastblinktime = millis();
        ledState = HIGH - ledState;             // toggle the LED
        digitalWrite(LED_BUILTIN, ledState);   
    }

    // syslog.logf(LOG_ERR,  "This is error message no. %d", counter);
    // syslog.logf(LOG_INFO, "This is info message no. %d", counter);

}
