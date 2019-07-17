#ifndef __Task_h__
#define __Task_h__ 1

#include "pch.h"
#include "FixedRam.h"
#include <vector>

class Task {
protected: 
	static FixedRam<Task> ramBase;
	bool started = false;
public:
	inline void* operator new(size_t size) {
		return ramBase.allocate(size);
	}
	static void DeleteAll();
	Task();
	virtual bool Process();
	virtual void End() = 0;
protected:
	virtual void Start();

};

class TaskOpen : public Task {
protected:
	unsigned pin = 0;
	unsigned long startTime = 0;
	unsigned long length = 20000;  //ms
	void virtual Start();
public:
	TaskOpen() = default;
	TaskOpen(unsigned ipin, float seconds);
	//start is very important, it must fire things up multiple times.
	
	void virtual End();
	bool virtual Process();
};

class TaskOpenBuzz : public TaskOpen {
protected:
	int buzzStart;
	int buzzEnd;
	unsigned buzzPin;
	void virtual Start();
public:
	TaskOpenBuzz() = default;
	TaskOpenBuzz(unsigned ipin, unsigned bpin, float secs = 20.0f, float bStart = 6.0f, float bEnd = 10.f);
	
	void virtual End();
	bool virtual Process();
};

class TaskSetState: public Task {
	bool On;
	unsigned pin = 0;
	void virtual Start();
public:
	TaskSetState() = default;
	TaskSetState(int iPin, bool iOn);
	
	void virtual End();
	bool virtual Process();
};

class TaskWait : public TaskOpen {
	void virtual Start();
public:
	TaskWait() = default;

	bool virtual Process() override;
	void virtual End();
};

class TaskLogMsg : public Task {
	//See:  https://stackoverflow.com/questions/35413821/how-to-fix-this-array-used-as-initializer-error
	char buffer[80];  //Was  buffer[80] = "";
	void virtual Start();
public:
	TaskLogMsg();
	TaskLogMsg(const char *msg);
	void virtual End();
	bool virtual Process();
};


extern std::vector<Task *> Tasks;



#endif