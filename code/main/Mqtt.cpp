#include "Mqtt.hpp"

static const unsigned long RETRY_MS = 10000;
static const unsigned long TEMP_MS = 10000;

MqttBridge::MqttBridge() : client(wifi)
{
	this->cfg = NULL;
	this->speedCallback = NULL;
	this->lastAttempt = 0;
	this->lastTemp = 0;
	this->speedPending = false;
	this->speedValue = 0;
	this->tempValue = -127.0;
}

void MqttBridge::Begin(MqttConfig* config, String clientId)
{
	this->cfg = config;
	this->clientId = clientId;
	// kurze Timeouts, damit ein nicht erreichbarer Broker die Weboberflaeche nicht blockiert
	this->wifi.setTimeout(2000);
	this->client.setSocketTimeout(2);
	this->client.setCallback([this](char* topic, byte* payload, unsigned int length)
	{
		this->OnMessage(topic, payload, length);
	});
	this->Apply();
}

void MqttBridge::SetSpeedCallback(void (*callback)(int))
{
	this->speedCallback = callback;
}

void MqttBridge::Apply()
{
	if (this->client.connected())
	{
		this->client.disconnect();
	}
	this->client.setServer(this->cfg->host, this->cfg->port);
	this->lastAttempt = 0;
	this->speedPending = true; // Zustand nach dem Verbinden einmal senden
}

const char* MqttBridge::State()
{
	if (!this->cfg->enabled || 0 == this->cfg->host[0])
	{
		return "aus";
	}
	return this->client.connected() ? "verbunden" : "getrennt";
}

void MqttBridge::PublishSpeed(int value)
{
	this->speedValue = value;
	this->speedPending = true;
}

void MqttBridge::SetTemp(float value)
{
	this->tempValue = value;
}

void MqttBridge::Connect()
{
	String base = String(this->cfg->topic);
	String lwt = base + "/status";
	bool ok;

	if (0 != this->cfg->user[0])
	{
		ok = this->client.connect(this->clientId.c_str(), this->cfg->user, this->cfg->pass, lwt.c_str(), 0, true, "offline");
	}
	else
	{
		ok = this->client.connect(this->clientId.c_str(), NULL, NULL, lwt.c_str(), 0, true, "offline");
	}

	if (ok)
	{
		this->client.publish(lwt.c_str(), "online", true);
		this->client.subscribe((base + "/speed/set").c_str());
		this->speedPending = true;
		this->lastTemp = 0;
	}
}

void MqttBridge::OnMessage(char* topic, byte* payload, unsigned int length)
{
	if (NULL == this->speedCallback || length == 0 || length > 4)
	{
		return;
	}
	char buf[6];
	memcpy(buf, payload, length);
	buf[length] = 0;

	// nur ganze Zahlen 0-100 akzeptieren
	char* end = NULL;
	long value = strtol(buf, &end, 10);
	if (end == buf || *end != 0 || value < 0 || value > 100)
	{
		return;
	}
	this->speedCallback((int) value);
}

void MqttBridge::Loop()
{
	if (!this->cfg->enabled || 0 == this->cfg->host[0] || WiFi.status() != WL_CONNECTED)
	{
		if (this->client.connected())
		{
			this->client.disconnect();
		}
		return;
	}

	if (!this->client.connected())
	{
		unsigned long now = millis();
		if (0 != this->lastAttempt && (now - this->lastAttempt) < RETRY_MS)
		{
			return;
		}
		this->lastAttempt = now;
		this->Connect();
		if (!this->client.connected())
		{
			return;
		}
	}

	this->client.loop();

	String base = String(this->cfg->topic);
	if (this->speedPending)
	{
		this->speedPending = false;
		this->client.publish((base + "/speed").c_str(), String(this->speedValue).c_str(), true);
	}
	unsigned long now = millis();
	if (this->tempValue > -100.0 && (0 == this->lastTemp || (now - this->lastTemp) >= TEMP_MS))
	{
		this->lastTemp = now;
		this->client.publish((base + "/temp").c_str(), String(this->tempValue, 1).c_str());
	}
}
