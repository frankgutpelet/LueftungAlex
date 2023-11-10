#include <ESP_EEPROM.h>

class SaveEEprom
{
  typedef struct
  {
    int slow;
    int fast;
  }dataStruct;
  
	public:
    SaveEEprom();
	  void Save();
    void Read();
    void Set_slow(int tempMin);
	  void Set_fast(int tempMax);

	  int Get_slow();
	  int Get_fast();

	
	private:
	  int slow;
	  int fast;

};
