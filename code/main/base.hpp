#include <ESP8266WebServer.h>
#include <Arduino.h>
#pragma once
class base
{
	public:
		base (ESP8266WebServer* server);

		void Render (void);
		void GetAjaxValues (void);
		void SetCallback_submit (void (*callback)(void));
		void Set_temp (String value);
		String Get_temp ();
		void Set_speed (String value);
		String Get_speed ();
		void Set_headline (String value);
		String Get_headline ();
		void Set_mqtt (String value);
		String Get_mqtt ();
	private:
		void Submit_Callback(void);
		void(*submit_UserCallback)(void);
		ESP8266WebServer* server;

		String temp;
		String speed;
		String headline;
		String mqtt;
};