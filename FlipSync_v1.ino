#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include "LCD1602A_Display.h"
#include "secrets.h"

//-------------------------------------
//             Constants
//-------------------------------------

// Input Device Constants v1
#define ENCODER_CLOCK 19 // Labeled CLK
#define ENCODER_DATA 18 // Labeled DT
#define ENCODER_SWITCH 15 // Labeled SW
#define SEND_BUTTON 23

// // Input Device Constants v2
// #define ENCODER_CLOCK 25 // Labeled CLK
// #define ENCODER_DATA 33 // Labeled DT
// #define ENCODER_SWITCH 32 // Labeled SW
// #define SEND_BUTTON 23

// AWS Constants
#define THINGNAME "ESP32-13576052"


// MQTT Constants
#define TOPIC "flipsync_01"


//-------------------------------------
//           Global Variables
//-------------------------------------

//------LCD1602A_Display Globals-------
LCD1602A_Display display;

//------------WiFi Globals-------------
WiFiManager wifiManager;

//--------AWS IOT Core Globals---------
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

//-------Rotory Encoder Globals--------
enum class EncoderDirection {
  CLOCKWISE,
  COUNTER_CLOCKWISE,
  STATIONARY
};
int currentStateCLK;
int lastStateCLK;
unsigned long lastRotorPress;
unsigned long lastBtnPress;


//-------------------------------------
//            WiFi Methods
//-------------------------------------

void connectWiFi() {
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect();
}

void configModeCallback(WiFiManager* myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
  char ssidMsg[17];
  strcpy(ssidMsg, "Connect to ");
  strncpy(ssidMsg + 11, myWiFiManager->getConfigPortalSSID().c_str(), 5);
  display.setMessage(ssidMsg, "Wifi to Setup");
}

//-------------------------------------
//         AWS IOT Core Methods
//-------------------------------------

void connectAWS(const char* topic) {
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.println("Connecting to AWS IOT");

  // Generate Board UID
  uint32_t chipId = 0;
  for(int i=0; i<17; i=i+8) {
	  chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
  char* boardUID = new char[17];
  sprintf(boardUID,"ESP32-%ld", chipId);
  
  Serial.print("BoardUID: ");
  Serial.println(boardUID);

  // Connect to MQTT Client
  while (!client.connect(boardUID)) {
    Serial.print(".");
    delay(100);
  }


  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(topic);

  Serial.println("AWS IoT Connected!");
}

void publishMessage(const char* message) {
  client.publish(TOPIC, message);
}

void messageHandler(const String &topic,const String &payload) {
  display.setMessage(payload);
  Serial.print("Recieved message on topic");
  Serial.print(topic);
  Serial.println(". Message: ");
  display.printMessage();
}

//-------------------------------------
//        Input Device Methods
//-------------------------------------

// returns true if both the "send" button is pressed 
// and it has been more than 50 milliseconds since the
// last "send" button press
bool sendBtnPressed() {
  bool sendBtnPressed = false;

  // Read the button state
  int btnState = digitalRead(SEND_BUTTON);

  //If we detect HIGH signal, button is pressed
  if (btnState == HIGH) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastBtnPress > 50) {
      sendBtnPressed = true;
    }

    // Remember last button press event
    lastBtnPress = millis();
  }

  // Return encoderBtnPressed. This should have stayed 
  // false if button state is Low or less then 50ms 
  // have passed
  return sendBtnPressed;
}

// returns CLOCKWISE or COUNTER_CLOCKWISE if the encoder
// has been spun that direction since last method call,
// or STATIONARY if there is no change
EncoderDirection getEncoderDirection() {
  EncoderDirection currentDir = EncoderDirection::STATIONARY;

  // Read the current state of CLK
  currentStateCLK = digitalRead(ENCODER_CLOCK);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW
    if (digitalRead(ENCODER_DATA) != currentStateCLK) {
      currentDir = EncoderDirection::COUNTER_CLOCKWISE;
    } else {
      // Encoder is rotating CW
      currentDir = EncoderDirection::CLOCKWISE;
    }
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;

  return currentDir;
}

// returns true if both the encoder button is pressed 
// and it has been more than 50 milliseconds since the
// last encoder button press
bool encoderBtnPressed() {
  bool encoderBtnPressed = false;

  // Read the button state
  int rotorBtnState = digitalRead(ENCODER_SWITCH);

  //If we detect LOW signal, button is pressed
  if (rotorBtnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastRotorPress > 50) {
      encoderBtnPressed = true;
    }

    // Remember last button press event
    lastRotorPress = millis();
  }

  // Return encoderBtnPressed. This should have stayed 
  // false if button state is Low or less then 50ms 
  // have passed
  return encoderBtnPressed;
}

char* getBoardUID() {

}


//-------------------------------------
//              Setup
//-------------------------------------

void setup() {

  // Setup Serial Monitor
  Serial.begin(9600);
  while (!Serial) { delay(1); }
  // delay(10000);
  Serial.println("start");


  // Setup Display
  display.init();

  // Setup Wifi
  display.setMessage("Connecting Wifi");
  connectWiFi();


  // Setup AWS
  display.setMessage("Connecting to Server");
  
  connectAWS(TOPIC);

  //LCD Setup
  display.setMessage("Hello World!");



  // Set encoder pins as inputs
  pinMode(ENCODER_CLOCK, INPUT);
  pinMode(ENCODER_DATA, INPUT);
  pinMode(ENCODER_SWITCH, INPUT_PULLUP);
  pinMode(SEND_BUTTON, INPUT);


  // Read the initial state of CLK
  lastStateCLK = digitalRead(ENCODER_CLOCK);

  lastRotorPress = 0;
  lastBtnPress = 0;
}

//-------------------------------------
//             Main Loop
//-------------------------------------

void loop() {

  // Update MQTT Client 
  client.loop();

  // idk if I need this
  // if (!client.connected()) {
  //   reconnect();
  //   client.subscribe(TOPIC);
  // }

  
  // Check Rotory Encoder
  EncoderDirection encoderDirection = getEncoderDirection();
  // Change current character depending on encoder direction
  if (encoderDirection == EncoderDirection::CLOCKWISE) {
    display.incrementChar();
  } 
  else if (encoderDirection == EncoderDirection::COUNTER_CLOCKWISE) {
    display.decrementChar();
  }

  
  if (encoderBtnPressed()) {
    display.incrementCursor();
  }
  
  if (sendBtnPressed()) {
    publishMessage(display.getMessage());
    display.flashMessage("Message Sent!", 500);
  }

  
  // Put in a slight delay to help debounce the reading
  delay(1);
}