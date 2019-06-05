#include "nettime.h"
#include <ESP8266WiFi.h>


NetTime::NetTime() {
	secsPastMid = 0;
	lastTime = millis();
	secsSinceLastTimeUpdate = 3600*24;
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
	char buffer[256];
	int pos = 0;
	buffer[0] = 0;
	WiFiClient client;
	
	if (client.connect("time.nist.gov", 13)) {
	//if (client.connect("www.google.com", 80)) {
		int bytes = 0;
		Serial.println("Connected");
		do {
			int bytes = client.available();
			if (bytes > 0) {
				client.readBytes(&buffer[pos], bytes);
				pos += bytes;
			}
		} while (client.connected());
		buffer[pos] = 0;
		Serial.println(buffer);
	}
	else {
		Serial.println("failed to connect to time server");
	}

	secsSinceLastTimeUpdate = 0;

/*
	TimeODay time;
	time.second = (secsPastMid % 60);
	time.minute = (secsPastMid % 3600) / 60;
	time.hour = secsPastMid / 24;
	return time;
	*/
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
	//if (secsSinceLastTimeUpdate > (60)) return true; // On the minute for now...
	if (secsSinceLastTimeUpdate > (3600)) return true; // On the hour for now...
	return false;
}

NetTime netTime = NetTime();