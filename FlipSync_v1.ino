#include "LCD1602A_Manager.h"
#include "MQTT_Methods.h"

#define BTN 23

// // Rotary Encoder Inputs v1
// #define CLK 19
// #define DT 18
// #define SW 15

// Rotary Encoder Inputs v2
#define CLK 25
#define DT 33
#define SW 32

#define MQTT_PORT 30004

#define MAX_ROW 16
#define MAX_COL 2
#define TOPIC "testTopic"

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastRotorPress;
unsigned long lastBtnPress;
// char curMsg[33] = { ' ', ' ', 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0' };
// int curChar = 0;

WiFiManager wifiManager;
LCD1602A_Manager display;


void callback(String topic, byte* message, unsigned int length) {
  display.setMessage((char*)message, (int)length);
  Serial.print("Recieved message on topic");
  Serial.print(TOPIC);
  Serial.println(". Message: ");
  display.printMessage();
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

void setup() {

  // Setup Serial Monitor
  Serial.begin(9600);
  while (!Serial)
    ;
  // delay(10000);
  Serial.println("start");


  //LCD Setup
  display.init();

  // Setup Wifi
  display.setMessage("Connecting Wifi");
  // delay(3000);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect();


  // Setup MQTT
  display.setMessage("Connecting MQTT");
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(callback);

  // Connect to our Broker
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    /*
     * If you are using username and password authentication, the statement will be
     * if (client.connect("test_client", mqttUser, mqttPassword ))
     * NOT
     * if (client.connect("test_client"))
     * 
     * NOTE : The Client Name should be unique.
     */

    if (client.connect("test_client_1")) {
      display.setMessage("MQTT Connected!");
      Serial.println("MQTT Connected!");
    } else {
      display.setMessage("MQTT Connection", "     Failed");
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe(TOPIC);

  //LCD Setup
  display.setMessage("Hello World!");



  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BTN, INPUT);


  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);

  lastRotorPress = 0;
  lastBtnPress = 0;
}


void loop() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK);

  if (!client.connected()) {
    reconnect();
    client.subscribe(TOPIC);
  }

  client.loop();

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(DT) != currentStateCLK) {
      counter--;
      display.incrementChar();
      currentDir = "CCW";
    } else {
      // Encoder is rotating CW so increment
      counter++;
      display.decrementChar();
      currentDir = "CW";
    }

    // Serial.print("Direction: ");
    // Serial.print(currentDir);
    // Serial.print(" | Counter: ");
    // Serial.println(counter);
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;

  // Read the button state
  int rotorBtnState = digitalRead(SW);

  //If we detect LOW signal, button is pressed
  if (rotorBtnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastRotorPress > 50) {
      display.incrementCursor();
    }

    // Remember last button press event
    lastRotorPress = millis();
  }

  // Read the button state
  int btnState = digitalRead(BTN);

  //If we detect HIGH signal, button is pressed
  if (btnState == HIGH) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
    if (millis() - lastBtnPress > 50) {
      if (!client.connected()) {
        Serial.println("not connected");
      }
      Serial.print("Publishing Message on ");
      Serial.print(TOPIC);
      Serial.println(". Message");

      client.publish(TOPIC, display.getMessage());
    }

    // Remember last button press event
    lastBtnPress = millis();
  }
  // Put in a slight delay to help debounce the reading
  delay(1);
}