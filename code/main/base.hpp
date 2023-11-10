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
		void Set_slow (String value);
		String Get_slow ();
		void Set_fast (String value);
		String Get_fast ();
		void Set_temp (String value);
		String Get_temp ();
		void Set_headline (String value);
		String Get_headline ();
		void Set_speed (String value);
		String Get_speed ();
	private:
		void Submit_Callback(void);
		void(*submit_UserCallback)(void);
		ESP8266WebServer* server;

		String slow;
		String fast;
		String temp;
		String headline;
		String speed;
};