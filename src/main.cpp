 /*
 * NibeBridge - link your Nibe Fighter to your favourite home automation system
*/

#include <ESP8266WiFi.h>
//#include <Arduino.h>                  // Not needed at all(?)
//#include <ESP8266mDNS.h>              // Not needed at this point
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SoftwareSerial.h>

#include <RemoteDebug.h>                // Logging/debug via telnet

// Consider using for simple logging:
// https://github.com/mrRobot62/Arduino-logging-library

// Configuration
const char* wifissid = "Eirreann";
const char* wifipasswd = "nightcap";

SoftwareSerial mySerial(10, 11);        // RX, TX
RemoteDebug Logger;

// Other variables and constants of less importance
int counter=0;


// ****************************************************
// Setup code here, to run once: 
// ****************************************************
void setup() {

    // Initialize mySerial and digital pin for LED output.
    mySerial.begin(19200);
    Serial.begin(19200);

    pinMode(LED_BUILTIN, OUTPUT);
    counter = 0;

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
    // ArduinoOTA.setHostname(const char* "espnibe");       // default none
    // ArduinoOTA.setPassword(const char* "nibepass");      // default none

    ArduinoOTA.onStart([]() {
        Serial.println("Starting OTA...");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnded OTA...");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) mySerial.println("Auth failed.");
        else if (error == OTA_BEGIN_ERROR) mySerial.println("Begin failed.");
        else if (error == OTA_CONNECT_ERROR) mySerial.println("Connect failed.");
        else if (error == OTA_RECEIVE_ERROR) mySerial.println("Receive failed.");
        else if (error == OTA_END_ERROR) mySerial.println("End failed.");
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Free sketch space is: [%i]", ESP.getFreeSketchSpace());

    // Initialize the telnet server of RemoteDebug
    // Logger.begin("Telnet_HostName"); // Initiaze the telnet server - this name is used in MDNS.begin
    // or
    // Logger.begin(HOST_NAME); // Initiaze the telnet server - HOST_NAME is the used in MDNS.begin
    // or 
    Logger.begin("ESP-NIBE"); // Initiaze the telnet server - HOST_NAME is the used in MDNS.begin and set the initial debug level

    Logger.setResetCmdEnabled(true); // Enable the reset command
    Logger.showTime(true); // To show time
    // Logger.showProfiler(true); // To show profiler - time between messages of Debug

}

void loop() {
    // put your main code here, to run repeatedly:
    ArduinoOTA.handle();

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED off (HIGH is the voltage level)
    delay(200);                        // wait for halfa second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED on by making the voltage LOW
    delay(1000);                       // wait for two seconds
    counter++;
    Serial.print("Debug ");
    Serial.println(counter, DEC);

    Logger.print("Debug ");
    Logger.println(counter, DEC);
    Logger.handle();
    // DEBUG_V("* Time: %u seconds (VERBOSE)\n", mTimeSeconds);
}
