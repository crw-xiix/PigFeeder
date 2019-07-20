#pragma once
#include "FixedRam.h"
#include "SpecTask.h"


class ScheduleObject {
	static FixedRam<ScheduleObject> ramBase;
public:
	bool completed = false;
	float timeOfDay = 0;
	SpecTask* specTask = NULL;
	//It may change things, but everything is back where it was when it finishes = true
	bool stately = false;
public:  //Static stuff
	void* operator new(size_t size);
	static inline void DeleteAll() {
		ramBase.Reset();
	}
	//Class factory 
	static ScheduleObject* Get(const char *str);
};

//This is just storing a list of stuff......
struct SpecTaskClass {
	std::function<ScheduleObject*(const char*)> func;
	int Id;
	const char *name;
	const char *help;
};

//This is to be filled in after classes are created
extern SpecTaskClass ourTasks[];



