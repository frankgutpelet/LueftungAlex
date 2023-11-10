#include "base.hpp"
#include <ArduinoJson.h>

StaticJsonDocument < 600 > base_doc;
const char base_text[] = "﻿<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
\n\
<head>\n\
	<link rel=\"stylesheet\" style=\"text/css\">\n\
<meta content=\"de\" http-equiv=\"Content-Language\" />\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\" />\n\
<title>Embedded Template</title>\n\
<style type=\"text/css\">\n\
\n\
	html {\n\
	  background: linear-gradient(to bottom right, navy, black);\n\
		background-repeat: no-repeat;\n\
		min-height: 100%;\n\
	}\n\
	.button {\n\
	  display: inline-block;\n\
	  padding: 15px 20px;\n\
	  font-size: 20px;\n\
	  cursor: pointer;\n\
	  text-align: center;\n\
	  text-decoration: none;\n\
	  outline: none;\n\
	  color: #fff;\n\
	  background-color: silver;\n\
	  border: none;\n\
	  border-radius: 10px;\n\
	  box-shadow: 0 9px black;\n\
	}\n\
	.auto-style-headline {\n\
    text-transform: uppercase;\n\
    font-family: verdana;\n\
    font-size: 3em;\n\
    font-weight: 700;\n\
    color: #f5f5f5;\n\
    text-shadow: 1px 1px 1px #919191,\n\
        1px 1px 1px #919191,\n\
        1px 2px 1px #919191,\n\
        1px 3px 1px #919191,\n\
        1px 4px 1px #919191,\n\
        1px 5px 1px #919191,\n\
\n\
    1px 18px 6px rgba(16,16,16,0.4),\n\
    1px 22px 10px rgba(16,16,16,0.2),\n\
    1px 25px 35px rgba(16,16,16,0.2);\n\
}\n\
.auto-style3 {\n\
	color: #EEEEEE;\n\
}\n\
.auto-style4 {\n\
	color: #008000;\n\
}\n\
.auto-style5 {\n\
	color: #A70D2A;\n\
}\n\
.slider {\n\
  -webkit-appearance: none;\n\
  width: 100%;\n\
  height: 35px;\n\
  background: #silver;\n\
  outline: none;\n\
  opacity: 0.7;\n\
  -webkit-transition: .2s;\n\
  transition: opacity .2s;\n\
}\n\
</style>\n\
\n\
<body>\n\
	<p class=\"auto-style-headline\" id=\"headline\" >Embedded Template</p>\n\
	<table class=\"auto-style3\">\n\
		<tr>\n\
			<td><strong>Temperatur: </strong></td>\n\
			<td><strong id=\"temp\"></strong></td>\n\
		</tr>\n\
		<tr>\n\
			<td><strong>Status: </strong></td>\n\
			<td><strong id=\"status\"></strong></td>\n\
		</tr>\n\
\n\
		<tr>\n\
			<td>aktuell</td>\n\
			<td>\n\
				<div class=\"slidecontainer\">\n\
				<input type=\"range\" min=\"1\" max=\"100\" value={{ flap }} class=\"slider\" id=\"speed\" onchange=\"sliderChange()\">\n\
			</div>\n\
			</td>\n\
\n\
		</tr>\n\
		<tr>\n\
\n\
		</tr>\n\
		<tr>\n\
			<td>langsam</td>\n\
			<td>\n\
				<div class=\"slidecontainer\">\n\
				<input type=\"range\" min=\"1\" max=\"100\" value={{ flap }} class=\"slider\" id=\"slow\" onchange=\"sliderSlowChange()\">\n\
			</div>\n\
			</td>\n\
\n\
		</tr>\n\
\n\
		<tr>\n\
			<td>schnell</td>\n\
			<td>\n\
				<div class=\"slidecontainer\">\n\
				<input type=\"range\" min=\"1\" max=\"100\" value={{ flap }} class=\"slider\" id=\"fast\" onchange=\"sliderFastChange()\">\n\
			</div>\n\
			</td>\n\
\n\
		</tr>\n\
\n\
		<td>\n\
			<form action=\"submit\" method=\"post\" >\n\
				<button class=\"button2\" id=\"submitBtn\" type=\"submit\" name=\"action\" value=\"\" >übernehmen</button>\n\
			</form>\n\
		</td>\n\
\n\
\n\
	</table>\n\
</body>\n\
</html>\n\
\n\
<script>\n\
	var values =\n\
	{\n\
		\"temp\" : \"dynamic\",         \n\
		\"speed\" : \"dynamic\",        \n\
		\"slow\" : \"dynamic\",         \n\
		\"fast\" : \"dynamic\",         \n\
		\"headline\" : \"dynamic\",     \n\
	}\n\
	var editMode = false\n\
\n\
	document.title = values.headline\n\
	interval = setInterval(updateValues, 1000)\n\
\n\
	function sliderChange()\n\
	{\n\
		editMode = true\n\
		values.speed = document.getElementById(\"speed\").value\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function sliderSlowChange()\n\
	{\n\
		editMode = true\n\
		values.slow = document.getElementById(\"slow\").value\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
	function sliderFastChange()\n\
	{\n\
		editMode = true\n\
		values.fast = document.getElementById(\"fast\").value\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
\n\
	function updateValues()\n\
	{\n\
		var xhttp = new XMLHttpRequest();\n\
		xhttp.onreadystatechange = function(){\n\
			pairs = this.responseText.split(\";\");\n\
			for(let i = 0; i < pairs.length; i++)\n\
			{\n\
				pair = pairs[i].split(':');\n\
				values[pair[0]] = pair[1];\n\
				toChange = document.getElementById(pair[0])\n\
				if (null == toChange)\n\
				{\n\
					console.warn(\"Element \" + pair[0] + \"not found\");\n\
					continue\n\
				}\n\
				if (document.getElementById(pair[0]).type == \"range\")\n\
				{\n\
					if (!editMode)\n\
					{\n\
						document.getElementById(pair[0]).value = pair[1]\n\
					}\n\
				}\n\
				else\n\
				{\n\
					document.getElementById(pair[0]).innerHTML = pair[1]\n\
				}\n\
			}\n\
		}\n\
		xhttp.open(\"GET\", \"getValues\", true);\n\
		xhttp.send();\n\
	}\n\
\n\
</script>\n\
";

base::base(ESP8266WebServer* server)
{
	this->server = server;
	this->submit_UserCallback = (void(*)()) NULL;
	this->server->on("/submit", std::bind( & base::Submit_Callback, this));
	this->server->on("/getValues", std::bind( & base::GetAjaxValues, this));
}
void base::Submit_Callback(void)
{
	String jsonstring = this->server->arg("action");
	DeserializationError error = deserializeJson(base_doc, jsonstring);
	Serial.println(jsonstring);
	JsonObject obj = base_doc.as < JsonObject > ();

	if (error)
	{
	Serial.print(F("deserializeJson() failed: "));
	Serial.println(error.f_str());
	}
	else
	{
		this->slow = obj["slow"].as < String > ();
		this->fast = obj["fast"].as < String > ();
		this->temp = obj["temp"].as < String > ();
		this->headline = obj["headline"].as < String > ();
		this->speed = obj["speed"].as < String > ();

	}
	if (NULL != this->submit_UserCallback)
	{
		this->submit_UserCallback();
	}
}

void base::SetCallback_submit (void (*callback)(void))
{
	this->submit_UserCallback = callback;
}

void base::Set_slow (String value)
{
	this->slow = value;
}

String base::Get_slow ( void )
{
	return this->slow;
}
void base::Set_fast (String value)
{
	this->fast = value;
}

String base::Get_fast ( void )
{
	return this->fast;
}
void base::Set_temp (String value)
{
	this->temp = value;
}

String base::Get_temp ( void )
{
	return this->temp;
}
void base::Set_headline (String value)
{
	this->headline = value;
}

String base::Get_headline ( void )
{
	return this->headline;
}
void base::Set_speed (String value)
{
	this->speed = value;
}

String base::Get_speed ( void )
{
	return this->speed;
}
void base::Render( void )
{
	this->server->send( 200, base_text);
}
void base::GetAjaxValues( void )
{
	String message = "slow:" + this->slow + ";fast:" + this->fast + ";temp:" + this->temp + ";headline:" + this->headline + ";speed:" + this->speed;
	this->server->send(200, "text/plain", message);
}
