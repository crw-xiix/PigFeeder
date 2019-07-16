#include "pch.h"
#include "ScheduleConfig.h"
#include <EEPROM.h>
#include "nettime.h"


ScheduleConfig ScheduleConfig::LoadFromEEPROM() {
	uint8_t ecrc = 0;
	ScheduleConfig sc;
	int size = sizeof(ScheduleConfig) + 4;

	EEPROM.begin(size);
	EEPROM.get(0, sc);
	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	EEPROM.end();
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	

	if (crc == ecrc) {
		webLog.println("Loaded EEPROM DATA");
		return sc;
	}
	webLog.println("Invalid EEPROM DATA");
	ResetDefault();
	return *this;
}

bool ScheduleConfig::SaveToEEPROM() {
	int size = sizeof(ScheduleConfig) + 4;
	uint8_t ecrc = 0;
	ScheduleConfig sc = *this;
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	EEPROM.begin(size);
	EEPROM.put(0, sc);
	EEPROM.put(sizeof(ScheduleConfig), crc);
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

ScheduleConfig sConfig = ScheduleConfig();