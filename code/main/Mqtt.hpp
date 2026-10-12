#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Save.hpp"

// Topics (<topic> = Basis-Topic aus der Weboberflaeche):
//   <topic>/speed/set   subscribe, Drehzahl in % (0-100)
//   <topic>/speed       publish (retained), aktuelle Drehzahl in %
//   <topic>/temp        publish, Temperatur in Grad C
//   <topic>/status      publish (retained), online / offline (Last Will)
class MqttBridge
{
	public:
		MqttBridge();

		void Begin(MqttConfig* config, String clientId);
		void SetSpeedCallback(void (*callback)(int));
		void Apply();                 // nach geaenderter Konfiguration aufrufen
		void Loop();                  // regelmaessig aus loop()
		void PublishSpeed(int value);
		void SetTemp(float value);
		const char* State();          // "aus", "getrennt", "verbunden"

	private:
		void Connect();
		void OnMessage(char* topic, byte* payload, unsigned int length);

		WiFiClient wifi;
		PubSubClient client;
		MqttConfig* cfg;
		String clientId;
		void (*speedCallback)(int);

		unsigned long lastAttempt;
		unsigned long lastTemp;
		bool speedPending;
		int speedValue;
		float tempValue;
};
