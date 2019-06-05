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

/*************************  TaskWait *********************************/

void TaskWait::End() {
	//Nothing
}

void TaskWait::Start() {
	startTime = millis();
}

/************************   TaskOpenBuzz     ************************/

TaskOpenBuzz::TaskOpenBuzz(unsigned ipin, unsigned bPin, int imillis, int bStart,int bEnd):TaskOpen(ipin,imillis) {
	buzzStart = bStart;
	buzzEnd = bEnd;
	buzzPin = bPin;
}


void TaskOpenBuzz::Start() {
	//Nothing to do really
	TaskOpen::Start();
}

bool TaskOpenBuzz::Process() {
	unsigned long diff = (millis() - startTime);

	if ((diff >= buzzStart) && (diff <= buzzEnd)) {
		digitalWrite(buzzPin, LOW);
	}
	else {
		digitalWrite(buzzPin, HIGH);
	}

	if (diff > length) {
		Serial.println("Open:Process over");
		return false;
	}
	return true;
}

void TaskOpenBuzz::End() {
	digitalWrite(pin, HIGH);
	digitalWrite(buzzPin, HIGH);
	Serial.print("Open:End:");
	Serial.println(pin);
}


/******************** BBB *******************

*/
				
const char daBeet[16] = {
					//0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
				      1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0
};
