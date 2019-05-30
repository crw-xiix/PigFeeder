
#include <WiFiUdp.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecure.h>
#include <WiFiServer.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <CertStoreBearSSL.h>
#include <BearSSLHelpers.h>
#include <ESP8266WiFi.h>


#include "LinkedList.h"


// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       PigFeeder.ino
    Created:	5/30/2019 12:23:57 PM
    Author:     charles-THINK\charles
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

#define OutArmExtend D5
#define OutArmRetract D6
//In seconds
#define ArmMoveTime 20      




// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(OutArmExtend, OUTPUT);
	pinMode(OutArmRetract, OUTPUT);
	Serial.begin(9600);
	digitalWrite(OutArmExtend, HIGH);
	digitalWrite(OutArmRetract, HIGH);

	delay(100);
}




class Task {
public:
	Task();
	virtual void Start() = 0;
	virtual bool Process() = 0;
	virtual void End() = 0;
};

Task::Task() {
}


class TaskOpen : public Task {
protected:
	unsigned pin;
	unsigned long startTime = 0;
	unsigned long length = 1000;  //ms
public:
	TaskOpen(unsigned ipin);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

TaskOpen::TaskOpen(unsigned ipin) {
	pin = ipin;
}

void TaskOpen::Start() {
	startTime = millis();
	digitalWrite(pin, LOW);
	Serial.print("Open:Start:");
	Serial.println(pin);
}

bool TaskOpen::Process() {
	if ((millis() - startTime) > length) {
		Serial.println("Open:Process over");
		return false;
	} 
	return true;
}

void TaskOpen::End() {
	digitalWrite(pin, HIGH);
	Serial.print("Open:End:");
	Serial.println(pin);
}

LinkedList<Task*> Tasks = LinkedList<Task*>();


Task *curTask = NULL;

// Add the main program code into the continuous loop() function
void loop()
{
	
	//Serial.println("loop");
	if (Serial.available()) {
		char ca[10];

		Serial.readBytes(ca, 1);
		Serial.println("Char");
		char c = ca[0];
		if ((c == 'o') || (c == 'O')) {
			Serial.println("Add Task:Open");
			Tasks.add(new TaskOpen(OutArmExtend));
		}

		if ((c == 'c') || (c == 'C')) {
			Serial.println("Add Task:Close");
			Tasks.add(new TaskOpen(OutArmRetract));
		}
	}

	//Now handle our tasks
	if (Tasks.size() > 0) {
		Task *thisTask = Tasks.get(0);
		if (thisTask != curTask) {
			curTask = thisTask;
			thisTask->Start();
		}
		bool val = thisTask->Process();
		if (!val) {
			thisTask->End();
			Tasks.remove(0);
			curTask = NULL;
		}
	}
	delay(50);
}
