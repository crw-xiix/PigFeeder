#pragma once


class Task {
public:
	Task();
	virtual void Start() = 0;
	virtual bool Process() = 0;
	virtual void End() = 0;
};


class TaskOpen : public Task {
protected:
	unsigned pin;
	unsigned long startTime = 0;
	unsigned long length = 20000;  //ms
public:
	TaskOpen(unsigned ipin, int imillis=20000);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskWait : public TaskOpen {
public:
	void virtual Start();
	void virtual End();
};

