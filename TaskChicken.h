#include "pch.h"
#include "task.h"
#include <functional>
#include <array>
#include <iostream>

#ifndef __TaskChicken_H_
#define __TaskChicken_H_ 1
#include "FixedRam.h"

class ScheduleObject;

class SpecTask {
private:
	static FixedRam<SpecTask> ramBase;
public:
	void* operator new(size_t size);
	bool stately = false;
	const char *name;
	//	static const int TaskId = 0;  //No task here
	float timeOfDay = 0.0f;
	Task *task = NULL;
	static inline float rangeInF(float val, float min,  float max) {
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
	static inline void DeleteAll() {
		ramBase.Reset();
	}
};

class SpecOpenDoor : public SpecTask {
private:
	SpecOpenDoor(unsigned long secs);
	SpecOpenDoor() = default;
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 1;
};

class SpecCloseDoor : SpecTask {
private:
	SpecCloseDoor(unsigned long secs);
	SpecCloseDoor() = default;
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 2;
};


class SpecLightOn : SpecTask {
private:
	SpecLightOn();
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 3;
};

class SpecLightOff : SpecTask {
private:
	SpecLightOff();
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 4;
};

class SpecLogText : SpecTask {
private:
	char asdf[80];
	SpecLogText(const char *msg);
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 5;
};



#endif


