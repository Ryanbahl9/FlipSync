#ifndef MQTT_Methods_h
#define MQTT_Methods_h
#include <PubSubClient.h>
#include <WiFiManager.h>

// MQTT Globals

extern WiFiClient espClientt;
extern PubSubClient client;
extern const char* mqtt_server;



// void callback(String topic, byte* message, unsigned int length);
void reconnect();

#endif