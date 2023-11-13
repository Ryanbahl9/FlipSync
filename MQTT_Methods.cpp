#include "MQTT_Methods.h"

// MQTT Globals

// //v1
WiFiClient espClientt;
PubSubClient client(espClientt);
const char* mqtt_server = "mqtt.thatlazyslacker.com";

// void callback(String topic, byte* message, unsigned int length) {
//   Serial.print("Message arrived on topic: ");
//   Serial.print(topic);
//   Serial.print(". Message: ");
//   String messageTemp;

//   for (int i = 0; i < length; i++) {
//     Serial.print((char)message[i]);
//     messageTemp += (char)message[i];
//   }
//   Serial.println();
// }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect("test_client_2")) {
      Serial.println("connected");
      client.publish("testTopic", "Reconnected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}