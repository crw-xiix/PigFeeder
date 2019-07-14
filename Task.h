#ifndef __Task_h__
#define __Task_h__ 1

#include "pch.h"
#include "FixedRam.h"


class Task {
protected: 
	static FixedRam<Task> ramBase;
public:
	inline void* operator new(size_t size) {
		return ramBase.allocate(size);
	}
	static void DeleteAll();
	Task();
	virtual void Start() = 0;
	virtual bool Process() = 0;
	virtual void End() = 0;
};

class TaskOpen : public Task {
protected:
	unsigned pin = 0;
	unsigned long startTime = 0;
	unsigned long length = 20000;  //ms

public:
	TaskOpen() = default;
	TaskOpen(unsigned ipin, int imillis=20000);
	//start is very important, it must fire things up multiple times.
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskOpenBuzz : public TaskOpen {
protected:
	int buzzStart;
	int buzzEnd;
	unsigned buzzPin;
public:
	TaskOpenBuzz() = default;
	TaskOpenBuzz(unsigned ipin, unsigned bpin, int imillis = 20000, int bStart = 6000, int bEnd = 10000);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskSetState: public TaskOpen {
	bool On;
public:
	TaskSetState() = default;
	TaskSetState(int iPin, bool iOn);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskWait : public TaskOpen {
public:
	TaskWait() = default;
	void virtual Start();
	void virtual End();
};

class TaskLogMsg : public Task {
	//See:  https://stackoverflow.com/questions/35413821/how-to-fix-this-array-used-as-initializer-error
	char buffer[80];  //Was  buffer[80] = "";
public:
	TaskLogMsg();
	TaskLogMsg(const char *msg);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

#endif