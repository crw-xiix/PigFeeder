#include "pch.h"
#include "SpecTask.h"
#include "pins.h"
#include "WebLog.h"
#include "FixedRam.h"
#include "ScheduleObject.h"
#include "ScheduleConfig.h"

void* SpecTask::operator new(size_t size) {
	return ramBase.allocate(size);
}

//Format:  14.25 (2:15pm), [0/1], [0/1] |.......the rest.........
bool SpecTask::ParseTime(const char *str, float* itime, char *irest) {
	//Will need to know those things, so.........
	//Lets do it to it.
	int isunrise = 0;
	int isunset = 0;
	int result = sscanf(str, "%f,%d,%d|%80[^\n]", itime, &isunrise, &isunset, irest);
	if (result != 4) {
		webLog.It(0, "Invalid info on Parse Command:");
		webLog.It(0, str);
		return false;
	}
	isunset = rangeIn(isunset, 0, 1);
	isunrise = rangeIn(isunrise, 0, 1);

	if (isunrise) (*itime) += sConfig.Sunrise;
	if (isunset) (*itime) += sConfig.Sunset;
	*itime = rangeInF(*itime, 0.0f, 24.0f);
	return true;
}

ScheduleObject *SpecOpenDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	float seconds;

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return NULL;
	}

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;
	sscanf(buffer, "%f", &seconds);
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecOpenDoor(seconds);
	sprintf(buffer, "OpenDoor:%f secs", seconds);
	webLog.println(buffer);

	return scho;
}



SpecOpenDoor::SpecOpenDoor(float secs) {
	//Now technically, we know what our Door Pin is.......
	task = new TaskOpen(PIN_DOOR_OPEN, secs);
	name = "Open Door";
}




SpecCloseDoor::SpecCloseDoor(float secs) {
	task = new TaskOpen(PIN_DOOR_CLOSE, secs);
	name = "Close Door";
}

ScheduleObject *SpecCloseDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	float seconds;
	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return NULL;
	}

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;
	sscanf(buffer, "%f", &seconds);
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecCloseDoor(seconds);
	return scho;
}

SpecLightOn::SpecLightOn() {
	task = new TaskSetState(PIN_LIGHT, true);
	name = "Light On";
}


ScheduleObject *SpecLightOn::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light On");
	}
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecLightOn();
	return scho;
}

SpecLightOff::SpecLightOff() {
	task = new TaskSetState(PIN_LIGHT, false);
	name = "Light Off";
}


ScheduleObject *SpecLightOff::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return NULL;
	}

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecLightOff();
	return scho;
}

SpecLogText::SpecLogText(const char* msg)
{
	task = new TaskLogMsg(msg);
	name = "Log Message";
}

ScheduleObject *SpecLogText::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	//Will need to know those things, so.........
	//Lets do it to it.
	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return NULL;
	}

	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecLogText(buffer);

	return scho;
}

/******************   SpecOpenBuzz ********************/
SpecOpenBuzz::SpecOpenBuzz(float secs, float bstart, float bend) {
	task = new TaskOpenBuzz(OutArmExtend, OutShaker, secs, bstart, bend);
//	Tasks.push_back(new TaskOpenBuzz(OutArmRetract, OutShaker, 21.0, 9.0, 13.0));
	name = "OpenBuzz";
}

ScheduleObject* SpecOpenBuzz::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	float seconds, bstart, bend;
	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return NULL;
	}
	if (sscanf(buffer, "%f,%f,%f", &seconds, &bstart, &bend) != 3) return NULL;

	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;

	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecOpenBuzz(seconds,bstart,bend);
	return scho;
}

/******************   SpecCloseBuzz ********************/
SpecCloseBuzz::SpecCloseBuzz(float secs, float bstart, float bend) {
	task = new TaskOpenBuzz(OutArmRetract, OutShaker, secs, bstart, bend);
	name = "CloseBuzz";
}

ScheduleObject* SpecCloseBuzz::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	float seconds, bstart, bend;
	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return NULL;
	}
	if (sscanf(buffer, "%f,%f,%f", &seconds, &bstart, &bend) != 3) return NULL;

	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;

	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecCloseBuzz(seconds, bstart, bend);

	return scho;
}


SpecTaskClass ourTasks[] = {
	{&SpecOpenDoor::Create,SpecOpenDoor::TaskId,"Open Door"},
	{&SpecCloseDoor::Create,SpecCloseDoor::TaskId,"Close Door"},
	{&SpecLightOn::Create,SpecLightOn::TaskId,"Light On"},
	{&SpecLightOn::Create,SpecLightOff::TaskId,"Light Off"},
	{&SpecLogText::Create,SpecLogText::TaskId, "Log Message"},
	{&SpecOpenBuzz::Create,SpecOpenBuzz::TaskId, "FeedTubeOpenBuzz"},
	{&SpecCloseBuzz::Create,SpecCloseBuzz::TaskId, "FeedTubeCloseBuzz"},
	//End it with a null, better that way
	{NULL, 0}
};


FixedRam<SpecTask> SpecTask::ramBase = FixedRam<SpecTask>("SpecTask", 1024);








