/*
 * NibeBridge - link your Nibe Fighter to your favourite home automation system
 */

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <Syslog.h>         // https://github.com/arcao/Syslog
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h> // ESP8266-specific version by Peter Lerup, modified
#include <NIBEConnect.h>

// Syslog server connection info
#define SYSLOG_SERVER "192.168.1.5"
#define SYSLOG_PORT 514
#define DEVICE_HOSTNAME "NIBE-ESP"
#define APP_NAME "NibeBridge"

WiFiUDP udpClient; // A UDP instance to send and receive packets over UDP

// Create a new syslog instance with LOG_KERN facility
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
int iteration = 1;

// WiFi configuration
const char *wifissid = "Eirreann";
const char *wifipasswd = "nightcap";

// Softserial configuration
SoftwareSerial softSerial(12, 14); // RX, Dx = D6, D5

// NIBEConnect configuration
NIBEConnect nibe(softSerial);

// MQTT configuration
const char* mqtt_server = "192.168.1.5";
WiFiClient espClient;
PubSubClient mqtt(espClient);
long lastMsg = 0;
char mqttMsg[75];
char mqttTopic[75];
int value = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    syslog.logf(LOG_INFO, "Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "NibeBridge-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt.connect(clientId.c_str())) {
      // Once connected, publish an announcement...
      // mqtt.publish("outTopic", "hello world");
      // ... and resubscribe
      Serial.println("Connected to MQTT broker");
      syslog.logf(LOG_INFO, "Connected to MQTT broker as client %s", clientId.c_str());
      syslog.logf(LOG_INFO, "Read from heatpump on topic NibeBridge/out");  // TODO 

      mqtt.subscribe("NibeBridge/in");
      syslog.logf(LOG_INFO, "Send to heatpump on topic NibeBridge/in");  // TODO 
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      syslog.logf(LOG_ERR, "MQTT connection failed from IP %s in state %d", WiFi.localIP().toString().c_str(), mqtt.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}





// Other variables and constants of less importance
int counter = 0;
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

  // Initialize SoftSerial for comms and Serial for debug
  softSerial.begin(19200, 8, NONE, 1);
  Serial.begin(19200);

  // TODO - move WiFi and OTA setup to function WiFiOTASetup

  // Initialize WiFi - required for operation
  // DHCP IP adress assigned: 192.168.1.250 - hostname: ESP-NIBE
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifissid, wifipasswd);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("ERROR - WiFi connection failed! Rebooting...");
    delay(10000);
    ESP.restart();
  }

  // Over The Air (OTA) setup
  // ArduinoOTA.setPort(uint16_t 8266);                   // default 8266
  // ArduinoOTA.setHostname(const char* "espnibe");       // TODO - set name and password for OTA updates at top of file
  // ArduinoOTA.setPassword(const char* "nibepass");

  ArduinoOTA.onStart([]() { syslog.logf(LOG_INFO, "Starting OTA..."); });
  ArduinoOTA.onEnd([]() { syslog.logf(LOG_INFO, "Completed OTA. Restarting."); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    syslog.logf(LOG_ERR, "Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      syslog.logf(LOG_ERR, "Auth failed.");
    else if (error == OTA_BEGIN_ERROR)
      syslog.log(LOG_ERR, "Begin failed.");
    else if (error == OTA_CONNECT_ERROR)
      syslog.log(LOG_ERR, "Connect failed.");
    else if (error == OTA_RECEIVE_ERROR)
      syslog.log(LOG_ERR, "Receive failed.");
    else if (error == OTA_END_ERROR)
      syslog.log(LOG_ERR, "End failed.");
  });
  ArduinoOTA.begin();
  syslog.logf(LOG_INFO, "NibeBridge starting on IP address %s", WiFi.localIP().toString().c_str());
  syslog.logf(LOG_DEBUG, "Free sketch space: %u", ESP.getFreeSketchSpace());

  // Setup NIBE protocol parsing state machine

  // Setup MQTT client
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(mqttCallback);
  syslog.logf(LOG_INFO, "MQTT broker set to %s", mqtt_server);
}

void loop() {

  unsigned long millisNow = millis();
  ArduinoOTA.handle();

  // TODO - make blinks useful info, e.g. connected or not.
  if (millisNow - lastblinktime >= blinkinterval) {
    lastblinktime = millis();
    ledState = HIGH - ledState; // toggle the LED
    digitalWrite(LED_BUILTIN, ledState);
    if (counter < 1) {
      // softSerial.printf("Softserial port iteration %d\n", counter);
      // Serial.printf("Serial port iteration %d\n", counter);
      Serial.print("Serial connection at your service\n");
      counter++;
    }

  }
  nibe.action();

  if (!mqtt.connected()) {
    mqttReconnect();
  }
  mqtt.loop();

  // syslog.logf(LOG_ERR,  "This is error message no. %d", counter);
  // syslog.logf(LOG_INFO, "This is info message no. %d", counter);

  // Test MQTT connection by sending pings every 5 seconds
  /* long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    snprintf (mqttMsg, 75, "Hello world #%ld");
    Serial.print("Publishing message: ");
    Serial.println(mqttMsg);
    mqtt.publish("outTopic", mqttMsg);
  }*/

}
