#pragma once

#include "LinkedList.h"

class ScheduleItem {
public:
	unsigned timeOfDay;
	unsigned completed;
	char desc[40];
};

class Scheduler {
private:
	LinkedList<ScheduleItem*> list = LinkedList<ScheduleItem*>();


};

