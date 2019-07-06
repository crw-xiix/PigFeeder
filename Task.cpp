#include "pch.h"
#include "Task.h"
#include "WebLog.h"
#include <Arduino.h>

Task::Task() {
}


TaskOpen::TaskOpen(unsigned ipin, int imillis) {
	pin = ipin;
	length = imillis;
}

void TaskOpen::Start() {
	const int sz = 40;
	char buffer[sz];
	snprintf(buffer, sz, "Open:Start:%d", pin);
	webLog.println(buffer);

	startTime = millis();
	digitalWrite(pin, LOW);

}

bool TaskOpen::Process() {
	if ((millis() - startTime) > length) {
		return false;
	}
	return true;
}

void TaskOpen::End() {
	const int sz = 40;
	char buffer[sz];
	snprintf(buffer, sz, "Open:End:%d", pin);
	webLog.println(buffer);

	digitalWrite(pin, HIGH);
}

/*************************  TaskWait *********************************/

void TaskWait::End() {
	//Nothing
}

void TaskWait::Start() {
	startTime = millis();
}

/************************   TaskOpenBuzz     ************************/

TaskOpenBuzz::TaskOpenBuzz(unsigned ipin, unsigned bPin, int imillis, int bStart, int bEnd) :TaskOpen(ipin, imillis) {
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

		return false;
	}
	return true;
}

void TaskOpenBuzz::End() {
	TaskOpen::End();
	//Clean up our pin
	digitalWrite(buzzPin, HIGH);
}


///  Task set state
/*
class TaskSetState : public TaskOpen {
	bool On;
public:
	TaskSetState(int iPin, bool iOn);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};*/


TaskSetState::TaskSetState(int iPin, bool iOn) :TaskOpen(iPin) {
	On = iOn;
}

//It's still reverse logic for the board.
void TaskSetState::Start() {
	const int sz = 40;
	char buffer[sz];
	digitalWrite(pin, !On);
	snprintf(buffer, sz, "Set:Output: %d->%s", pin, On ? "On" : "Off");
	webLog.println(buffer);
}

bool TaskSetState::Process() {
	return false;
}

void TaskSetState::End() {
	//Nothing, state is set.
}

/* This just prints a line in the log....*/

TaskLogMsg::TaskLogMsg() {
	buffer[0] = 0;
}

TaskLogMsg::TaskLogMsg(const char *msg) {
	strncpy(buffer, msg, 80);
}

void TaskLogMsg::Start() { 	}

void TaskLogMsg::End() { }

bool TaskLogMsg::Process() {
	webLog.println(buffer);
	return false;
}

/******************** BBB *******************

*/

const char daBeet[16] = {
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
	  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0
};
