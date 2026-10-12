#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <TimerEvent.h>
#include <ArduinoJson.h>
#include "base.hpp"
#include "Logger.hpp"
#include "OneWire.h"
#include <DallasTemperature.h>
#include "Save.hpp"
#include "Mqtt.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "HeizluefterAlex";
const char* title = "HeizluefterAlex";
const unsigned int timerOnePeriod = 1000;
int pwmPin = 16; //D0 @ nodeMCU
int pwmValue = 0; // Drehzahl in %

ESP8266WebServer server(80);
base indexPage(&server);
Logger* logger = Logger::instance();
SaveEEprom storage;
MqttBridge mqtt;

OneWire oneWire(4); //D2 @ nodeMCU
DallasTemperature sensors(&oneWire);
TimerEvent timerOne;

// Drehzahl in % setzen (0-100), gemeinsamer Weg fuer Web, HTTP und MQTT
void setSpeed(int percent)
{
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  pwmValue = percent;
  analogWrite(pwmPin, pwmValue); // Range ist in setup() auf 100 gesetzt
  indexPage.Set_speed(String(pwmValue));
  mqtt.PublishSpeed(pwmValue);
}

// true bei einer ganzen Zahl 0-100
bool parsePercent(String s, int* out)
{
  s.trim();
  if (s.length() == 0 || s.length() > 3)
  {
    return false;
  }
  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (!isDigit(s[i]))
    {
      return false;
    }
  }
  int v = s.toInt();
  if (v > 100)
  {
    return false;
  }
  *out = v;
  return true;
}

// Weboberflaeche: POST /submit {"speed":"60"}
void handleSubmit()
{
  int value;
  if (parsePercent(indexPage.Get_speed(), &value))
  {
    setSpeed(value);
    server.send(200, "text/plain", String(pwmValue));
  }
  else
  {
    indexPage.Set_speed(String(pwmValue));
    server.send(400, "text/plain", "Bad value");
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void Render()
{
  indexPage.Set_headline(String(title) + " - " + myhostname);
  indexPage.Render();
}

// HTTP-Kommando:
//   /cm?speed=60            Drehzahl in %
//   /cm?cmnd=Speed 60       dito
//   /cm?cmnd=60             dito
//   /cm?cmnd=Power off      Drehzahl 0
void Release()
{
  String cmd = server.arg("cmnd");
  String val;
  int value;

  if (server.hasArg("speed"))
  {
    val = server.arg("speed");
  }
  else if (cmd.equalsIgnoreCase("Power off"))
  {
    val = "0";
  }
  else if (cmd.startsWith("Speed ") || cmd.startsWith("speed "))
  {
    val = cmd.substring(6);
  }
  else
  {
    val = cmd;
  }

  if (!parsePercent(val, &value))
  {
    server.send(400, "text/plain", String("Bad Command: ") + (cmd.length() ? cmd : val));
    return;
  }
  setSpeed(value);
  server.send(200, "text/plain", String(pwmValue));
}

void sendTemp()
{
  server.send(200, "text/plain", indexPage.Get_temp());
}

// GET /mqtt -> aktuelle MQTT-Konfiguration als JSON (ohne Passwort)
void sendMqttConfig()
{
  MqttConfig* c = storage.Mqtt();
  StaticJsonDocument < 512 > doc;
  doc["enabled"] = c->enabled;
  doc["host"] = c->host;
  doc["port"] = c->port;
  doc["user"] = c->user;
  doc["hasPass"] = (0 != c->pass[0]);
  doc["topic"] = c->topic;
  doc["state"] = mqtt.State();
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

// POST /mqtt -> Konfiguration speichern und MQTT neu verbinden
void saveMqttConfig()
{
  MqttConfig* c = storage.Mqtt();
  String host = server.arg("host");
  String user = server.arg("user");
  String pass = server.arg("pass");
  String topic = server.arg("topic");
  int port = server.arg("port").toInt();

  host.trim();
  topic.trim();
  if (host.length() >= sizeof(c->host) || user.length() >= sizeof(c->user) ||
      pass.length() >= sizeof(c->pass) || topic.length() >= sizeof(c->topic))
  {
    server.send(400, "text/plain", "Eingabe zu lang");
    return;
  }
  if (topic.length() == 0 || topic.startsWith("/") || topic.endsWith("/") ||
      topic.indexOf('#') >= 0 || topic.indexOf('+') >= 0)
  {
    server.send(400, "text/plain", "Ungueltiges Topic");
    return;
  }
  if (port < 1 || port > 65535)
  {
    port = 1883;
  }

  c->enabled = (server.arg("enabled") == "1");
  strlcpy(c->host, host.c_str(), sizeof(c->host));
  c->port = port;
  strlcpy(c->user, user.c_str(), sizeof(c->user));
  if (pass.length() > 0)
  {
    strlcpy(c->pass, pass.c_str(), sizeof(c->pass)); // leer = unveraendert
  }
  if (0 == c->user[0])
  {
    c->pass[0] = 0; // ohne Benutzer kein Passwort
  }
  strlcpy(c->topic, topic.c_str(), sizeof(c->topic));

  storage.Save();
  mqtt.Apply();
  sendMqttConfig();
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(myhostname);
  WiFi.begin(ssid, password);
  sensors.begin();
  sensors.setWaitForConversion(false); // nicht 750 ms in loop() blockieren
  sensors.requestTemperatures();

  analogWriteRange(100); // analogWrite() direkt in %
  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0);

  server.on("/", Render);
  server.on("/cm", Release);
  server.on("/temp", sendTemp);
  server.on("/mqtt", HTTP_GET, sendMqttConfig);
  server.on("/mqtt", HTTP_POST, saveMqttConfig);
  indexPage.SetCallback_submit(handleSubmit);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  timerOne.set(timerOnePeriod, timerOneFunc);
  indexPage.Set_headline(String(title) + " - " + myhostname);
  indexPage.Set_speed(String(pwmValue));
  indexPage.Set_temp("--");

  mqtt.SetSpeedCallback(setSpeed);
  mqtt.Begin(storage.Mqtt(), String(myhostname) + "-" + String(ESP.getChipId(), HEX));
  indexPage.Set_mqtt(mqtt.State());
}

void loop(void) {
  server.handleClient();
  timerOne.update();
  mqtt.Loop();
}

void timerOneFunc()
{
  // Ergebnis der vorigen Anforderung lesen, dann naechste Messung starten
  float temp = sensors.getTempCByIndex(0);
  sensors.requestTemperatures();

  indexPage.Set_temp(String(temp, 1));
  indexPage.Set_speed(String(pwmValue));
  indexPage.Set_headline(String(title) + " - " + myhostname);
  indexPage.Set_mqtt(mqtt.State());
  mqtt.SetTemp(temp);
}
