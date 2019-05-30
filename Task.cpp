#include "Task.h"
#include <ESP8266WiFi.h>


Task::Task() {
}


TaskOpen::TaskOpen(unsigned ipin, int imillis) {
	pin = ipin;
	length = imillis;
}

void TaskOpen::Start() {
	startTime = millis();
	digitalWrite(pin, LOW);
	Serial.print("Open:Start:");
	Serial.println(pin);
}

bool TaskOpen::Process() {
	if ((millis() - startTime) > length) {
		Serial.println("Open:Process over");
		return false;
	}
	return true;
}

void TaskOpen::End() {
	digitalWrite(pin, HIGH);
	Serial.print("Open:End:");
	Serial.println(pin);
}

void TaskWait::End() {
	//Nothing
}

void TaskWait::Start() {
	startTime = millis();
}
