#include "pch.h"
#include "TaskChicken.h"
#include "pins.h"
#include "WebLog.h"
#include "ScheduleConfig.h"


SpecNothing SpecificTaskOfNothing;


ScheduleObject ScheduleObject::Get(const char *str) {
	//Format is:  function,vars, vars,vars,vars, no commas in vars
	ScheduleObject junk = ScheduleObject();
	int func = 0;
	sscanf(str, "%d:", &func);
	//Meh, returning on the stack, ick
	if (func == 0) return junk;

	const char *ptr = strchr(str, ':');
	ptr++;
	//ourTasks is not null, there is something in it.....
	for (int i = 0;; i++) {
		if (ourTasks[i].func == NULL) break;		//we hit the end of the array
		if (ourTasks[i].Id == func) return ourTasks[i].func(ptr);
	}
	return junk;
}

//Format:  14.25 (2:15pm), [0/1], [0/1],.......the rest.........
bool SpecTask::ParseTime(const char *str, float* itime, char *irest) {
	//Will need to know those things, so.........
	//Lets do it to it.
	int isunrise = 0;
	int isunset = 0;

	int result = sscanf(str, "%f,%d,%d,%60[^/n]s", itime, &isunrise, &isunset, irest);
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

SpecOpenDoor::SpecOpenDoor(unsigned long secs) {
		//Now technically, we know what our Door Pin is.......
	task = TaskOpen(PIN_DOOR_OPEN, secs);
	name = "Open Door";
}


//Format, since we are coming from the config file.....
//TimeOfDay(#-2.2f),sunrise (%d) ,sunset (%d), holdTime %d
//3.14,1,0,65
ScheduleObject SpecOpenDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	int seconds;

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals,&timeOfDay,buffer)) {
		return SpecLogText::Create("0,0,0,Error");
	}
	sscanf(buffer, "%d", &seconds);
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecOpenDoor(seconds*1000ul);
	//Door is left open, that's not stately
	scho.stately = false;
	return scho;
}

SpecCloseDoor::SpecCloseDoor(unsigned long secs) {
	task = TaskOpen(PIN_DOOR_CLOSE, secs);
	name = "Close Door";
}

ScheduleObject SpecCloseDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	int seconds;

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Close Door");
	}
	sscanf(buffer, "%d", &seconds);
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecCloseDoor(seconds * 1000ul);
	//Door is left open, that's not stately
	scho.stately = false;
	return scho;
}

SpecLightOn::SpecLightOn() {
	task = TaskSetState(PIN_LIGHT, true);
	name = "Light On";
}


ScheduleObject SpecLightOn::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light On");
	}
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecLightOn();
	//Light is on from off, that's not stately
	scho.stately = false;
	return scho;
}

SpecLightOff::SpecLightOff() {
	task = TaskSetState(PIN_LIGHT, false);
	name = "Light Off";
}


ScheduleObject SpecLightOff::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light Off");
	}
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecLightOff();
	//Light is on from off, that's not stately
	scho.stately = false;
	return scho;
}

SpecLogText::SpecLogText(const char* msg) {
	task = TaskLogMsg(msg);
	name = "Log Message";
}

ScheduleObject SpecLogText::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Log Msg");
	}
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecLogText(buffer);
	//This is harmless, so we want it to execute on restart......
	scho.stately = false;
	return scho;
}


SpecTaskClass ourTasks[] = {
	{&SpecOpenDoor::Create,SpecOpenDoor::TaskId,"Open Door"},
	{&SpecCloseDoor::Create,SpecCloseDoor::TaskId,"Close Door"},
	{&SpecLightOn::Create,SpecLightOn::TaskId,"Light On"},
	{&SpecLightOn::Create,SpecLightOff::TaskId,"Light Off"},
	{&SpecLogText::Create,SpecLogText::TaskId, "Log Message"},
	//End it with a null, better that way
	{NULL, 0}
};














