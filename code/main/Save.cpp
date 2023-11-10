#include <cstring>
#include "Save.hpp"
#include <ESP8266WiFi.h>

SaveEEprom::SaveEEprom()
{
  this->Read();
}

void SaveEEprom::Read()
{
  dataStruct storage;
  
  EEPROM.begin(sizeof(storage)); 
  EEPROM.get(0, storage);
  EEPROM.end();
  this->slow = storage.slow;
  this->fast = storage.fast;
}
void SaveEEprom::Save()
{
  dataStruct storage;
    
  storage.slow = this->slow;
  storage.fast = this->fast;

	EEPROM.begin(sizeof(storage)); 
	EEPROM.put(0, storage); 
	EEPROM.commit();
	EEPROM.end();
  this->Read();
}
void SaveEEprom::Set_slow(int slow)
{
  if (this->slow != slow)
  {
	  this->slow = slow;
    this->Save();
  }
}
void SaveEEprom::Set_fast(int fast)
{
	if (this->fast != fast)
  {
    this->fast = fast;
    this->Save();
  }
}
int SaveEEprom::Get_slow()
{
	return this->slow;
}
int SaveEEprom::Get_fast()
{
	return this->fast;
}
