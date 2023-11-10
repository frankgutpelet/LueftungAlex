#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <TimerEvent.h>
#include "base.hpp"
#include "Logger.hpp"
#include "OneWire.h"; 
#include <DallasTemperature.h>
#include "Save.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "HeizluefterAlex";
const char* title = "HeizluefterAlex";
const unsigned int timerOnePeriod = 1000;
const unsigned int relOffBreakSeconds = 10; //10 minutes
unsigned int relOffBreakCounter = 0;
int pwmPin = 16; //D0 @ nodeMCU
int pwmValue = 0;

ESP8266WebServer server(80);
base indexPage(&server);
Logger* logger = Logger::instance();
SaveEEprom storage;

OneWire oneWire(4); //D2 @ nodeMCU
DallasTemperature sensors(&oneWire);
TimerEvent timerOne;

void handleSubmit() 
{

  if (String("") != indexPage.Get_speed())
  {
    pwmValue = indexPage.Get_speed().toInt();
    analogWrite(pwmPin, pwmValue);
  }
  if (String("") != indexPage.Get_slow())
  {
    storage.Set_slow(indexPage.Get_slow().toInt());
  }
  if (String("") != indexPage.Get_fast())
  {
    storage.Set_fast(indexPage.Get_fast().toInt());
  }

  Render();
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

void Release()
{
  String cmd = server.arg("cmnd");
  Serial.println(cmd);
  if (String("Slow") == cmd)
  {
     pwmValue = storage.Get_slow();
  }
  else if (String("Fast") == cmd)
  {
    pwmValue = storage.Get_fast();

  }
  else if (String("Power off") == cmd)
  {
    pwmValue = 0;
    indexPage.Set_speed("0");
  }
  else
  {
    server.send(404, "text/plain", String("Bad Command: ") + cmd);
    return;
  }
  server.send(200, "text/plain", cmd);
}
void sendTemp()
{
  server.send(200, "text/plain", indexPage.Get_temp());
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(myhostname);
  WiFi.begin(ssid, password);
  sensors.begin();


  server.on("/", Render);
  server.on("/cm", Release);
  server.on("/temp", sendTemp);
  indexPage.SetCallback_submit(handleSubmit); 
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  timerOne.set(timerOnePeriod, timerOneFunc);
  indexPage.Set_headline(String(title) + " - " + myhostname);
  pinMode(pwmPin, OUTPUT);
  analogWriteFreq(25000);

}

void loop(void) {
  server.handleClient();
  timerOne.update();
}

void timerOneFunc()
{
  double temp = 0.0;

  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  indexPage.Set_temp(String(temp));  
  indexPage.Set_slow(String(storage.Get_slow()));
  indexPage.Set_fast(String(storage.Get_fast()));
  indexPage.Set_speed(String(pwmValue));

  analogWrite(pwmPin, (pwmValue * 255) /100);

}
