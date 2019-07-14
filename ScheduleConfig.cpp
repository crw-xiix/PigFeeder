#include "pch.h"
#include "ScheduleConfig.h"

bool ScheduleConfig::LoadFromEEPROM() {
	ResetDefault();
	return true;
}

void ScheduleConfig::Add(const char *command) {
	if (sz >= 20) return;
	strncpy(elements[sz], command, 80);
	sz++;
}

ScheduleConfig sConfig = ScheduleConfig();