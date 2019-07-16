#pragma once
#ifndef __ScheduleConfig_H__ 
#define __ScheduleConfig_H__ 1


#include "TaskChicken.h"
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
	inline const char *Get(int loc) {
		if (loc < 0) return "";
		if (loc >= 20) return "";
		if (loc >= sz) return "";
		return elements[loc];
	}
	inline void ClearSchedule() {
		for (int i = 0; i < 20; i++) {
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
		//return crcSlow(std::reinterpretcast<uint8_t*>( this), sizeof(this));
		return crcSlow(((uint8_t*)this), sizeof(this));
	}

	ScheduleConfig LoadFromEEPROM();
	bool SaveToEEPROM();


	inline void ResetDefault() {
		ClearSchedule();
		Add("5:1.1,0,0,Test Message");
		Add("5:0.1,0,0,Schedule Startup");
		Add("5:0.0,1,0,Wake up! It's sunrise; you're burning daylight!");
		Add("5:0.0,0,1,Sunset Go to bed!");
		DST = false;
		strcpy(Title, "New Remote Action Unit");
		strcpy(Version, "Enter something here");
	}
};

extern ScheduleConfig sConfig;

#endif