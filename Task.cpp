#include "Task.h"
#include "WebLog.h"
#include <Arduino.h>

FixedRam<Task> Task::ramBase = FixedRam<Task>("Task", 1024);
std::vector<Task *> Tasks = std::vector<Task *>();


Task::Task() {
}

void Task::Start() {
	started = true;
}

void Task::DeleteAll() {
	ramBase.Reset();
}

bool Task::Process() {
	if (!started) Start();
	return false;
}

TaskOpen::TaskOpen(unsigned ipin, float secs) {
	pin = ipin;
	length = (unsigned long) (secs * 1000);
}

void TaskOpen::Start() {
	Task::Start();
	const int sz = 40;
	char buffer[sz];
	snprintf(buffer, sz, "Open:Start:%d", pin);
	webLog.println(buffer);
	startTime = millis();
	digitalWrite(pin, LOW);

}

bool TaskOpen::Process() {
	Task::Process();
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
	Task::Start();
	startTime = millis();
}

bool TaskWait::Process() {
	Task::Process();
	if ((millis() - startTime) > length) {
		return false;
	}
	return true;
}

/************************   TaskOpenBuzz     ************************/

TaskOpenBuzz::TaskOpenBuzz(unsigned ipin, unsigned bPin, float secs, float bStart, float bEnd) :TaskOpen(ipin, secs) {
	buzzStart = bStart*1000;
	buzzEnd = bEnd*1000;
	buzzPin = bPin;
}


void TaskOpenBuzz::Start() {
	//Nothing to do really
	TaskOpen::Start();
}

bool TaskOpenBuzz::Process() {
	TaskOpen::Process();
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



TaskSetState::TaskSetState(int iPin, bool iOn) :Task() {
	pin = iPin;
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
	Task::Process();
	return false;
}

void TaskSetState::End() {
	//Nothing, state is set.
}

/* This just prints a line in the log....*/

TaskLogMsg::TaskLogMsg() {
	//Empty message.....  Compiler bitched about the array initialization......
	//See:  https://stackoverflow.com/questions/35413821/how-to-fix-this-array-used-as-initializer-error
	buffer[0] = 0;
}

TaskLogMsg::TaskLogMsg(const char *msg) : Task() {
	strncpy(buffer, msg, 80);
}

void TaskLogMsg::Start() { 	}

void TaskLogMsg::End() { }

bool TaskLogMsg::Process() {
	Task::Process();
	webLog.println(buffer);
	return false;
}


/******************** BBB *******************
*/


const char daBeet[16] = {
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
	  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0
};
