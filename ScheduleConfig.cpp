#include "ScheduleConfig.h"

ScheduleConfig sConfig = ScheduleConfig();


void ScheduleConfig::Add(const char *command) {
	if (sz >= 20) return;
	strncpy(elements[sz], command, 80);
	sz++;
}


bool ScheduleConfig::LoadFromEEPROM()
{
	uint8_t crc = 0;
	/*  //Windows method for testing, we'll keep it for now
	FILE *filp = fopen("filename.bin", "rb");
	do {
		if (filp == NULL) break;
		fread(this->elements, sizeof(char), sizeof(this), filp);
		fread(&crc, 1, 1, filp);
	} while (0);
	if (filp) fclose(filp);
	*/

	//if (getCRC() == crc) return true;
	//Something is wrong......
	ResetDefault();
}