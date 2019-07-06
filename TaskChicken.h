#include "pch.h"
#include "task.h"
#include <functional>
#include <array>
#include <iterator>
#include <algorithm>
#include <array>

#ifndef __TaskChicken_H_
#define __TaskChicken_H_ 1

class SpecNothing;

extern SpecNothing SpecificTaskOfNothing;

class SpecTask {
public:
	bool stately = false;
	const char *name;
	//	static const int TaskId = 0;  //No task here
	static TaskWait EmptyTask;
	float timeOfDay = 0.0f;
	virtual Task *GetTask() {
		return NULL;
	};
	//virtual const char *GetScanStr() = 0;
	static inline float rangeInF(float val, float min, float max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}
	static inline int rangeIn(int val, int min, int max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}
	static bool ParseTime(const char *str, float* time, char *rest);
	
};


class ScheduleObject {
public:
	bool completed = false;
	float timeOfDay = 0;
	SpecTask* specTask = NULL;
	//It may change things, but everything is back where it was when it finishes = true
	bool stately = false;
public:  //Static stuff
	//Class factory 
	static ScheduleObject Get(const char *str);
};


class SpecNothing : public SpecTask {
public:
	SpecNothing() = default;
};

class SpecOpenDoor : public SpecTask {
private:
	TaskOpen task;
	SpecOpenDoor(unsigned long secs);
	SpecOpenDoor() = default;
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 1;
	inline Task *GetTask() override {
		return (&task);
	}
};

class SpecCloseDoor : SpecTask {
private:
	TaskOpen task;
	SpecCloseDoor(unsigned long secs);
	SpecCloseDoor() = default;
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 2;
	inline Task *GetTask() override {
		return (&task);
	}
};


class SpecLightOn : SpecTask {
private:
	TaskSetState task;
	SpecLightOn();
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 3;

	inline Task *GetTask() override {
		return &task;
	}
};

class SpecLightOff : SpecTask {
private:
	TaskSetState task;
	SpecLightOff();
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 4;

	inline Task *GetTask() override {
		return &task;
	}
};


class SpecLogText : SpecTask {
private:
	TaskLogMsg task;
	SpecLogText(const char *msg);
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 5;
	inline Task *GetTask() override {
		return &task;
	}
};

struct SpecTaskClass {
	std::function<ScheduleObject(const char*)> func;
	int Id;
	const char *name;
};


extern SpecTaskClass ourTasks[];


#endif


