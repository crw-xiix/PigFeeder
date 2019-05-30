#pragma once


struct TimeODay {
	int hour;
	int minute;
	int second;
};

class NetTime {
private:
	unsigned long lastTime = 0;
	long secsPastMid = 0;
	long secsSinceLastTimeUpdate = 0;
public:
	NetTime();
	void setTime(int h, int m, int s);
	void setTimeSecs(int s);
	long getTimeSec();
	TimeODay getTime();
	//We are just going to assume we get called once a second or so...
	void process();
	bool needNewTime();
};

extern NetTime netTime;



