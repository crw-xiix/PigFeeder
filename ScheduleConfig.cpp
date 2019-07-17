#include "pch.h"
#include "ScheduleConfig.h"
#include <EEPROM.h>
#include "nettime.h"


ScheduleConfig ScheduleConfig::LoadFromEEPROM() {
	bool fail = false;
	uint8_t ecrc = 0;
	int esize;
	ScheduleConfig sc;
	int size = sizeof(ScheduleConfig) + 8;
	
	EEPROM.begin(size);
	EEPROM.get(0, sc);
	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	EEPROM.get(sizeof(ScheduleConfig) + 1, esize);
	EEPROM.end();
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	

	if (crc != ecrc) {
		webLog.println("CRC Error, Invalid EEPROM DATA");
		sc.ResetDefault();
	}
	if (esize != sizeof(ScheduleConfig)) {
		webLog.println("Size mismatch, Invalid EEPROM DATA");
		sc.ResetDefault();
	}
	webLog.println("Loaded EEPROM DATA");
	return sc;
}

bool ScheduleConfig::SaveToEEPROM() {
	int size = sizeof(ScheduleConfig) + 8;
	int esize = sizeof(ScheduleConfig);

	uint8_t ecrc = 0;
	ScheduleConfig sc = *this;
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	EEPROM.begin(size);
	EEPROM.put(0, sc);
	EEPROM.put(sizeof(ScheduleConfig), crc);
	EEPROM.put(sizeof(ScheduleConfig) + 1, esize);
	EEPROM.commit();

	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	if (crc != ecrc) {
		webLog.println("On Save: Invalid EEPROM DATA");
	}
	else {
		webLog.println("Verified EEPROM data");
	}
	EEPROM.end();
	netTime.triggerReload = true;
}

void ScheduleConfig::Add(const char *command) {
	if (sz >= 20) return;
	strncpy(elements[sz], command, 80);
	sz++;
}

void ScheduleConfig::ResetDefault() {
	ClearSchedule();
	Add("5:1.1,0,0|Test Message");
	Add("5:0.1,0,0|Schedule Startup");
	Add("5:0.0,1,0|Wake up! It's sunrise; you're burning daylight!");
	Add("5:0.0,0,1|Sunset Go to bed!");
	DST = false;
	strcpy(Title, "New Remote Action Unit");
	strcpy(Version, "Enter something here");
}




ScheduleConfig sConfig = ScheduleConfig();