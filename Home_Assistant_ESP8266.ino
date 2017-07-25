#include <PubSubClient.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <RCSwitch.h>

#define Desk_ON         333107
#define Desk_OFF        333116
#define Nightstand_ON   333251
#define Nightstand_OFF  333260
#define Unassigned_ON   333571
#define Unassigned_OFF  333580
#define LivingRoom_ON   335107
#define LivingRoom_OFF  335116
#define Desk2_ON        341251
#define Desk2_OFF       341260

#define TX_PIN          D7
#define TX_PROTO        1
#define TX_PULSE        192
#define TX_BIT          24

const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "192.168.1.16";

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress ip(192, 168, 1, 70);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
long lastMsg = 0;
char msg[50];
int value = 0;

IRsend irsend(4);

RCSwitch s = RCSwitch();

void setup() {
  // RF Data
  pinMode(TX_PIN, OUTPUT);

  // Enable IR sending
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Print the IP address
  Serial.println(WiFi.localIP());
  
  // Transmitter is connected to Arduino Pin #10  
  s.enableTransmit(TX_PIN);
  s.setProtocol(TX_PROTO);
  s.setPulseLength(TX_PULSE);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if (String(topic).startsWith("esp8266/light")) {
    // Desk lamp 1 (corner)
    if (strcmp(topic, "esp8266/light/desk1")==0) {
      if  ((char)payload[0] == '1') {         // Power On
        s.send(Desk_ON, TX_BIT);
        client.publish("esp8266/light/desk/status", "Sent power on");
      } else if ((char)payload[0] == '0') {   // Power Off
        s.send(Desk_OFF, TX_BIT);
        client.publish("esp8266/light/desk/status", "Sent power on");
      } else {
        Serial.println("Invalid message");
      }
    }
    // Nightstand lamp
    if (strcmp(topic, "esp8266/light/nightstand")==0) {
      if  ((char)payload[0] == '1') {         // Power On
        s.send(Nightstand_ON, TX_BIT);
        client.publish("esp8266/light/nightstand/status", "Sent power on");
      } else if ((char)payload[0] == '0') {   // Power Off
        s.send(Nightstand_OFF, TX_BIT);
        client.publish("esp8266/light/nightstand/status", "Sent power on");
      } else {
        Serial.println("Invalid message");
      }
    }
    // Desk lamp 2 (near cat tree)
    if (strcmp(topic, "esp8266/light/desk2")==0) {
      if  ((char)payload[0] == '1') {         // Power On
        s.send(Desk2_ON, TX_BIT);
        client.publish("esp8266/light/desk2/status", "Sent power on");
      } else if ((char)payload[0] == '0') {   // Power Off
        s.send(Desk2_OFF, TX_BIT);
        client.publish("esp8266/light/desk2/status", "Sent power off");
      } else {
        Serial.println("Invalid message");
      }
    }
  }

  if (String(topic).startsWith("esp8266/ir")) {
    // TV
    if (strcmp(topic, "esp8266/ir/tv")==0) {
      char cmd[2];
      cmd[0] = (char)payload[0];
      cmd[1] = (char)payload[1];

      if  (strcmp(cmd, "00")==0) {        // 00 - Power
        irsend.sendSony(0xA90, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent power button");
      } else if (strcmp(cmd, "01")==0) {  // 01 - 1
        irsend.sendSony(0x10, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 1 button");
      } else if (strcmp(cmd, "02")==0) {  // 02 - 2
        irsend.sendSony(0x810, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 2 button");
      } else if (strcmp(cmd, "03")==0) {  // 03 - 3
        irsend.sendSony(0x410, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 3 button");
      } else if (strcmp(cmd, "04")==0) {  // 04 - 4
        irsend.sendSony(0xC10, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 4 button");
      } else if (strcmp(cmd, "05")==0) {  // 05 - 5
        irsend.sendSony(0x210, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 5 button");
      } else if (strcmp(cmd, "06")==0) {  // 06 - 6
        irsend.sendSony(0xA10, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 6 button");
      } else if (strcmp(cmd, "07")==0) {  // 07 - 7
        irsend.sendSony(0x610, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 7 button");
      } else if (strcmp(cmd, "08")==0) {  // 08 - 8
        irsend.sendSony(0xE10, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 8 button");
      } else if (strcmp(cmd, "09")==0) {  // 09 - 9
        irsend.sendSony(0x110, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 9 button");
      } else if (strcmp(cmd, "10")==0) {  // 10 - 0
        irsend.sendSony(0x910, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent 10 button");
      } else if (strcmp(cmd, "11")==0) {  // 11 - Input
        irsend.sendSony(0xA50, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent input button");
      } else if (strcmp(cmd, "12")==0) {  // 12 - Center
        irsend.sendSony(0xA70, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent center button");
      } else if (strcmp(cmd, "13")==0) {  // 13 - Left
        irsend.sendSony(0x2D0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent left button");
      } else if (strcmp(cmd, "14")==0) {  // 14 - Right
        irsend.sendSony(0xCD0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent right button");
      } else if (strcmp(cmd, "15")==0) {  // 15 - Up
        irsend.sendSony(0x2F0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent up button");
      } else if (strcmp(cmd, "16")==0) {  // 16 - Down
        irsend.sendSony(0xAF0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent down button");
      } else if (strcmp(cmd, "17")==0) {  // 17 - Home
        irsend.sendSony(0x70, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent home button");
      } else if (strcmp(cmd, "18")==0) {  // 18 - Action Menu
        irsend.sendSony(0x6923, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent action menu button");
      } else if (strcmp(cmd, "19")==0) {  // 19 - Guide
        irsend.sendSony(0x6D25, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent guide button");
      } else if (strcmp(cmd, "20")==0) {  // 20 - TV
        irsend.sendSony(0x250, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent TV button");
      } else if (strcmp(cmd, "21")==0) {  // 21 - Discover
        irsend.sendSony(0x6758, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent discover button");
      } else if (strcmp(cmd, "22")==0) {  // 22 - Back
        irsend.sendSony(0x62E9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent back button");
      } else if (strcmp(cmd, "23")==0) {  // 23 - Vol Up
        irsend.sendSony(0x490, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent vol up button");
      } else if (strcmp(cmd, "24")==0) {  // 24 - Vol Dn
        irsend.sendSony(0xC90, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent vol dn button");
      } else if (strcmp(cmd, "25")==0) {  // 25 - Ch Up
        irsend.sendSony(0x90, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent ch up button");
      } else if (strcmp(cmd, "26")==0) {  // 26 - Ch Dn
        irsend.sendSony(0x890, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent ch dn button");
      } else if (strcmp(cmd, "27")==0) {  // 27 - Mute
        irsend.sendSony(0x290, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent mute button");
      } else if (strcmp(cmd, "28")==0) {  // 28 - Subtitle
        irsend.sendSony(0xAE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent subtitle button");
      } else if (strcmp(cmd, "29")==0) {  // 29 - Audio
        irsend.sendSony(0xE90, 13, 2);
        client.publish("esp8266/ir/tv/status", "Sent audio button");
      } else if (strcmp(cmd, "30")==0) {  // 30 - Jump
        irsend.sendSony(0xDD0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent jump button");
      } else if (strcmp(cmd, "31")==0) {  // 31 - Play
        irsend.sendSony(0x2CE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent play button");
      } else if (strcmp(cmd, "32")==0) {  // 32 - Rew
        irsend.sendSony(0x6CE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent rew button");
      } else if (strcmp(cmd, "33")==0) {  // 33 - FF
        irsend.sendSony(0x1CE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent ff button");
      } else if (strcmp(cmd, "34")==0) {  // 34 - Prev
        irsend.sendSony(0x1EE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent prev button");
      } else if (strcmp(cmd, "35")==0) {  // 35 - Next
        irsend.sendSony(0x5EE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent next button");
      } else if (strcmp(cmd, "36")==0) {  // 36 - Pause
        irsend.sendSony(0x4CE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent pause button");
      } else if (strcmp(cmd, "37")==0) {  // 37 - Wide
        irsend.sendSony(0x5E25, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent wide button");
      } else if (strcmp(cmd, "38")==0) {  // 38 - Stop
        irsend.sendSony(0xCE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent stop button");
      } else if (strcmp(cmd, "39")==0) {  // 39 - Football
        irsend.sendSony(0x3758, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent football button");
      } else if (strcmp(cmd, "40")==0) {  // 40 - .
        irsend.sendSony(0x5CE9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent . button");
      } else if (strcmp(cmd, "41")==0) {  // 41 - pic off
        irsend.sendSony(0x7D0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent pic off button");
      } else if (strcmp(cmd, "42")==0) {  // 42 - Netflix
        irsend.sendSony(0x1F58, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent Netflix button");
      } else if (strcmp(cmd, "43")==0) {  // 43 - Yellow
        irsend.sendSony(0x72E9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent yellow button");
      } else if (strcmp(cmd, "44")==0) {  // 44 - Blue
        irsend.sendSony(0x12E9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent blue button");
      } else if (strcmp(cmd, "45")==0) {  // 45 - Red
        irsend.sendSony(0x52E9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent red button");
      } else if (strcmp(cmd, "46")==0) {  // 46 - Green
        irsend.sendSony(0x32E9, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent green button");
      } else if (strcmp(cmd, "47")==0) {  // 47 - Display
        irsend.sendSony(0x5D0, 12, 2);
        client.publish("esp8266/ir/tv/status", "Sent display button");
      } else if (strcmp(cmd, "48")==0) {  // 48 - Help
        irsend.sendSony(0x5923, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent help button");
      } else if (strcmp(cmd, "49")==0) {  // 49 - Sync Menu
        irsend.sendSony(0xD58, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent sync menu button");
      } else if (strcmp(cmd, "50")==0) {  // 50 - Split Screen
        irsend.sendSony(0x7725, 15, 2);
        client.publish("esp8266/ir/tv/status", "Sent split screen button");
      } else {
        Serial.println("Invalid message");
      }
    }
    // Fan
    if (strcmp(topic, "esp8266/ir/fan")==0) {
      if ((char)payload[0] == '0' ) {       // Power
        uint16_t  rawData[119] = {1350,400, 1350,400, 500,1250, 1350,350, 1350,400, 500,1250, 500,1250, 500,1250, 500,1250, 1350,400, 500,1250, 500,7750, 1350,400, 1350,400, 500,1250, 1350,350, 1350,400, 500,1250, 500,1250, 500,1250, 500,1250, 1350,400, 500,1250, 550,7700, 1350,400, 1350,350, 500,1250, 1350,350, 1350,350, 500,1250, 550,1200, 500,1250, 550,1200, 1350,400, 500,1250, 550,7750, 1350,350, 1350,350, 550,1200, 1350,350, 1350,350, 550,1200, 550,1200, 550,1200, 550,1200, 1350,350, 550,1200, 550,7800, 1350,350, 1350,350, 550,1200, 1350,350, 1350,350, 550,1200, 550,1200, 550,1200, 550,1200, 1350,350, 550,1200, 550};
        irsend.sendRaw(rawData, 119, 38);
        client.publish("esp8266/ir/fan/status", "Power toggled");
      } else if ((char)payload[0] == '1' ) { // Speed up
        uint16_t rawData[143] = {1300,450, 1300,450, 450,1300, 1300,450, 1250,450, 1300,450, 450,1300, 450,1300, 450,1250, 1250,450, 1300,450, 450,7800, 1300,450, 1300,450, 450,1250, 1250,450, 1300,450, 1250,450, 450,1300, 450,1300, 450,1250, 1300,450, 1300,450, 450,7800, 1300,450, 1250,450, 450,1250, 1300,450, 1300,450, 1300,450, 450,1300, 450,1250, 450,1250, 1300,450, 1250,450, 450,7800, 1250,450, 1300,450, 450,1250, 1250,450, 1250,450, 1300,450, 450,1300, 450,1250, 450,1250, 1250,450, 1300,450, 450,7800, 1300,450, 1300,450, 450,1250, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1250, 1250,450, 1250,450, 450,7850, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1250,450, 450,1250, 450,1250, 450,1250, 1300,450, 1250,450, 450}; 
        irsend.sendRaw(rawData, 143, 38);
        client.publish("esp8266/ir/fan/status", "Speed increased by 1");
      } else if ((char)payload[0] == '2' ) { // Toggle rotation
        uint16_t  rawData[119] = {1250,450, 1250,450, 450,1300, 1250,450, 1250,450, 1250,450, 450,1300, 500,1250, 450,1250, 500,1250, 1300,450, 1300,6950, 1300,400, 1350,400, 500,1200, 1350,400, 1300,400, 1350,400, 500,1250, 500,1250, 500,1250, 500,1250, 1350,400, 1350,6900, 1350,400, 1350,400, 500,1250, 1350,400, 1300,400, 1350,400, 500,1250, 500,1250, 500,1250, 500,1200, 1350,400, 1350,6900, 1350,400, 1300,400, 500,1200, 1350,400, 1350,400, 1350,400, 500,1250, 500,1250, 500,1250, 500,1250, 1300,400, 1300,7000, 1300,400, 1250,450, 450,1250, 1250,450, 1250,450, 1250,450, 450,1300, 450,1300, 450,1300, 450,1300, 1250,450, 1250};
        irsend.sendRaw(rawData, 119, 38);
        client.publish("esp8266/ir/fan/status", "Rotation toggled");
      } else {
        Serial.println("Invalid message");
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected!");
      client.subscribe("esp8266/light/#");
      client.subscribe("esp8266/ir/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
