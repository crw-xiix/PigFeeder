#pragma once
#ifndef __ScheduleConfig_H__ 
#define __ScheduleConfig_H__ 1


#include "SpecTask.h"
#include "crc.h"
#include <stdint.h>
#include "WebLog.h"
#include "ScheduleObject.h"

class ScheduleConfig {
private:
	char elements[21][80];
	int sz = 0;
public:
	//Title of the App
	char Title[80];
	//Special notes about the version
	char Version[80];
	int  DST = false;
	//These get updated by the sunrise code and saved when config is changed.
	//If no time server, it can be adjusted manually.
	float Sunrise = 6.0f;
	float Sunset = 18.0f;
	float extraFloats[8] = { 0,0,0,0,0,0,0,0 };
	uint8_t extraBytes[8] = { 0,0,0,0,0,0,0,0 };
	int	extraInts[8] = { 0,0,0,0,0,0,0,0 };

	inline const char *Get(int loc) {
		if (loc < 0) return "";
		if (loc >= 20) return "";
		if (loc >= sz) return "";
		return elements[loc];
	}
	inline void ClearSchedule() {
		for (int i = 0; i < 21; i++) {
			memset(elements[i], 0, 80);
			elements[i][0] = 0;
		}
		sz = 0;
	}
	void Add(const char *command);
	inline void Set(int loc, const char *command) {
		if (loc < 0) return;
		if (loc >= 20) return;
		if (loc > sz) return;
		strncpy(elements[loc], command, 80);
	}

	inline unsigned getCRC() {
		return crcSlow(((uint8_t*)this), sizeof(this));
	}

	ScheduleConfig LoadFromEEPROM();
	bool SaveToEEPROM();
	void ResetDefault();
};

extern ScheduleConfig sConfig;

#endif