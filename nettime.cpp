#include "nettime.h"
#include <Arduino.h>


NetTime::NetTime() {
	secsPastMid = 0;
	lastTime = millis();
	secsSinceLastTimeUpdate = 0;
	Serial.println("NetTime::NetTime()");


}
	
void NetTime::setTime(int h, int m, int s) {
	secsSinceLastTimeUpdate = 0;
	//For safety
	secsPastMid = ((unsigned long)h) * ((unsigned long )3600);
	secsPastMid += m * 60;
	secsPastMid += s;
}

void NetTime::setTimeSecs(int s) {
	secsSinceLastTimeUpdate = 0;
	secsPastMid = s;
}

long NetTime::getTimeSec() {
	return secsPastMid;
}

TimeODay NetTime::getTime() {
	TimeODay time;
	time.second = (secsPastMid % 60);
	time.minute = (secsPastMid % 3600) / 60;
	time.hour = secsPastMid / 24;
	return time;
}
	//We are just going to assume we get called once a second...
void NetTime::process() {
	if ((millis() - lastTime) > 1000) {
		//for rollover
		lastTime += ((unsigned long)1000);
		secsPastMid++;
		secsSinceLastTimeUpdate++;
	}
}

// On the hour for now...
bool NetTime::needNewTime() {
	if (secsSinceLastTimeUpdate > (60)) return true; // On the minute for now...
	if (secsSinceLastTimeUpdate > (3600)) return true; // On the hour for now...
	return false;
}

NetTime netTime;