
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
#include "passwords.h"
#include "output.h"



#define OutArmExtend D5
#define OutArmRetract D6
//In seconds
#define ArmMoveTime 20      



WiFiServer server(80);

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



void EEWriteStr(int dest, char *src, int max) {
	int sp = 0;
	for (int i = 0; i < max; i++) {
		EEPROM.write(dest + i, (uint8_t)src[sp]);
		if (src[sp] != 0) sp++;
	}
}

void EEReadStr(int src, char *dest, int max) {
	for (int i = 0; i < max; i++) {
		char c = EEPROM.read(src + i);
		dest[i] = c;
		if (c == 0) return;
	}
}

EvapCooler cooler(PIN_PUMP_OUT, PIN_FAN_OUT, PIN_THERMOSTATC, PIN_THERMOSTATC);
void setup() {


	netTime.setTimeSecs(1200);
	EEPROM.begin(512);

	Serial.begin(9600);
	delay(100);



	//Test and see if it's valid data.......  This is our signature.......
	Serial.println("Reading EEPROM");
	if ((EEPROM.read(0) == 'C') &&
		(EEPROM.read(1) == 'R') &&
		(EEPROM.read(2) == 'W') &&
		(EEPROM.read(3) == 0)) {
		char str[40];
		//Good EEPROM (hah)
		char myStr[40];

		EEReadStr(4, myStr, 40);
		Serial.println("EEPROM DATA:");
		Serial.println(myStr);
	}
	else {
		Serial.println("Writing EEPROM");
		EEWriteStr(0, "CRW", 4);
		EEWriteStr(4, "Testing", 40);
		EEPROM.commit();
	}




	int j[10];
	j[5] = 4;
	int p = 5[j];


	//pinMode(TRIG, OUTPUT);
	pinMode(ledPin, OUTPUT);

	Serial.print("SDA Pin:");
	Serial.println(SDA);
	Serial.print("SCL Pin:");
	Serial.println(SCL);

	if (!mcp.begin()) {
		Serial.println("Couldn't find MCP9808!");
	}
	else {
		Serial.println("Found MCP9808!");
	}

	Wire.setClock(10000);


	WiFi.mode(WIFI_STA);

	WiFi.hostname(wifiHostName);

	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");

	WiFi.config(ip, gateway, subnet);
	WiFi.begin(wifiNetwork, wifiPassword);

	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
		delay(125);
		digitalWrite(ledPin, LOW);
		delay(125);
		digitalWrite(ledPin, HIGH);
	}

	// Start the server
	server.begin();
	Serial.println("Server started");

	Serial.print("Use this URL to connect: ");
	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");


	//int sz = EEPROM.read(1);
	/*
	Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
	String subject = "ESP8285 Water monitoring system";
	if(gsender->Subject(subject)->Send("charles@loneaspen.com", "The system has been restarted after a power down.")) {
	Serial.println("Message sent.");
	} else {
	Serial.print("Error sending message: ");
	Serial.println(gsender->getError());
	}
	*/

	WindSensor::Init(PIN_WIND_SPEED_IN, 5.0f);

	cooler.reset();


}


int lastWaterLevel = 0;
float lastTemp = 0.0f;

//volatile float lastHz = 0;


WiFiClient *tempClient = NULL;
void loop() {
	// Pass INC as a parameter to get the distance in inches

	



	WiFiClient client = server.available();
	int lps = 0;
	while (client == NULL)
	{
		if (lps > 50) return;  //Skip wifi stuff.
		delay(1);
		client = server.available();
		lps++;
	}


	// Wait until the client sends some data
	//Serial.println("new client");

	while (client.connected() && !client.available())
	{
		delay(1);
		lps++;
		if (lps > 500) return;  //Skip wifi stuff.
	}

	//Bail out, something failed.
	if (!client.connected()) return;

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	//Serial.println(request);
	client.flush();

	// Match the request
	int value = LOW;
	if (request.indexOf("/ToggleLed") != -1) {
		digitalWrite(ledPin, !digitalRead(ledPin));
		client.println("<br>Updated<br>");
		return;
	}
	// Return the response
	tempClient = &client;
	outputSite(&clientPrint);
	tempClient = NULL;
	delay(5);
}


void clientPrint(const char *data) {
	tempClient->println(data);
}

