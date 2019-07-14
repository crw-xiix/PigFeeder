#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <EEPROM.h>

/*
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
#include <BearSSLHelpers.h> */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//Local Crap
#include "LinkedList.h"
#include "passwords.h"
#include "nettime.h"

#include "output.h"
#include "outputedit.h"
#include "Task.h"
#include "OTASetup.h"
#include "WebLog.h"
#include "astronomical.h"
#include "ScheduleConfig.h"


#include <list>



#define OutArmExtend D5
#define OutArmRetract D6
#define OutShaker D7
#define OutAux D8


WiFiServer server(80);


//These come from passwords.h
IPAddress ip(DEVICE_IP); // where .xxx is the desired IP Address
IPAddress gateway(DEVICE_GATEWAY); // set gateway to match your network
IPAddress subnet(DEVICE_SUBNET); // set subnet mask to match your

IPAddress dns1(DEVICE_DNS1);
IPAddress dns2(DEVICE_DNS2);


const char *textHtml = "text/html";


// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(D4, OUTPUT);
	pinMode(OutArmExtend, OUTPUT);
	pinMode(OutArmRetract, OUTPUT);
	pinMode(OutShaker, OUTPUT);
	pinMode(OutAux, OUTPUT);


	digitalWrite(OutArmExtend, HIGH);
	digitalWrite(OutArmRetract, HIGH);
	digitalWrite(OutShaker, HIGH);
	pinMode(OutAux, HIGH);

	sConfig.LoadFromEEPROM();


	//Now to get WIFI Going.......
	WiFi.mode(WIFI_STA);
	WiFi.hostname(wifiHostName);


	WiFi.persistent(false);

	WiFi.config(ip, gateway, subnet, dns1, dns2);

	WiFi.begin(wifiNetwork, wifiPassword);
	webLog.println("Wifi Begin");
	//Do a little flashy dance while we connect
	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(D4, LOW);
		delay(125);
		digitalWrite(D4, HIGH);
		delay(250);
	}
	webLog.println("Wifi Connected");

	SetupOTA("PigFeeder-ESP8235");




	netTime.Init(DEVICE_TZ);
	netTime.GotNewTime = dummy;
	netTime.newTimeValid = WeGotTime;
	// Start the server
	server.begin();

}





//Task management
std::list<Task *> Tasks = std::list<Task *>();
Task *curTask = NULL;

//A fucked up pointer because Arduino is fucked up sometimes.
WiFiClient *tempClient = NULL;



double sunRise = 6;
double sunSet = 6;

void dummy() {
	SunSet sun = SunSet();
	sun.setPosition(DEVICE_LAT, DEVICE_LON, DEVICE_TZ);
	sun.setCurrentDate(netTime.year, netTime.month, netTime.day);
	sunRise = sun.calcSunrise() / 60;
	sunSet = sun.calcSunset() / 60;
	sConfig.Sunrise = sunRise;
	sConfig.Sunset = sunSet;
	char buffer[80];
	snprintf(buffer, 80, "Sunrise:%f Sunset:%f", sunRise, sunSet);
	webLog.println(buffer);

}
//Git me out of here
//ScheduleObject schedule[20];

std::list<ScheduleObject> schedule = std::list<ScheduleObject>();
std::list<ScheduleObject>::iterator schedulePtr;

int SchCompare(ScheduleObject a, ScheduleObject b) {
	return (a.timeOfDay < b.timeOfDay);
}
bool runScheduler = false;



void WeGotTime() {
	//Just clear it as we only want this once......
	netTime.newTimeValid = NULL;
	webLog.println("Loading schedule");
	int j = 0;
	for (int i = 0; i < 20; i++) {
		ScheduleObject *o = ScheduleObject::Get(sConfig.Get(i));
		if (o) {
			o->completed = false;
			if (o->specTask != NULL) {
				schedule.push_back(*o);
			}
		}
	}
	schedule.sort(SchCompare);

	webLog.println("Schedule Load Complete");
	schedulePtr = schedule.begin();
	runScheduler = true;


	char buffer[80];
	while (schedulePtr != schedule.end()) {
		ScheduleObject so = (*schedulePtr);
		float ttime = netTime.getHourFloat();
		if (so.timeOfDay <= netTime.getHourFloat()) {
			//This event has passed....
			if (!so.stately) {
				so.completed = true;
				Tasks.push_back(so.specTask->task);
				snprintf(buffer, 80, "Added %2.2f past task: %s", so.timeOfDay, so.specTask->name);
				webLog.println(buffer);
			}
		}
		else {
			break;
		}
		schedulePtr++;
	}
}

//forward
void printHeader(WiFiClient client, const char *ctype);

// Add the main program code into the continuous loop() function
unsigned long togMillis = 0;
void loop()
{
	char buffer[120];

	//for OTA stuff.........
	ArduinoOTA.handle();
	netTime.process();
	
	if (runScheduler && (schedulePtr != schedule.end())) {
		float timeDiff = netTime.getHourFloat() - ((*schedulePtr).timeOfDay);
		ScheduleObject &so = (*schedulePtr);
		if (timeDiff > 0.0f) {
			so.completed = true;
			Tasks.push_back(so.specTask->task);
			{
				char buffer[80];
				snprintf(buffer, 80, "Added Task: %s", so.specTask->name);
				webLog.println(buffer);
			}
			schedulePtr++;
		}
	}


	//Toggle the LED light for heartbeat
	if ((millis() % 1000) < 500) {
		digitalWrite(D4, 1);
	}
	else {
		//digitalWrite(D4,1);
		analogWrite(D4, 867);
	}

	//Now handle our tasks
	if (Tasks.size() > 0) {
		auto ttt = Tasks.begin();
		Task *thisTask = (*ttt);
		if (thisTask != curTask) {
			curTask = thisTask;
			thisTask->Start();
		}
		bool val = thisTask->Process();
		if (!val) {
			thisTask->End();
			Tasks.pop_front();
			if (Tasks.size() == 0) {
				//Last one, reset the memory....
				Task::DeleteAll();
				webLog.It(netTime.getHourFloat(), "Freeing Task Memory");
			}
			curTask = NULL;
		}
	}
	//WIFI Stuff last.........
	WiFiClient client = server.available();
	int lps = 0;
	if (client == NULL) {
		delay(1);
		lps++;
		if (lps > 5) {
			return;
		}
	}

	// Wait until the client sends some data
	while (client.connected() && !client.available())
	{
		delay(10);
		lps++;
		if (lps > 40) {
			return;  //Skip wifi stuff.
		}
	}

	//Bail out, something failed.
	if (!client.connected()) return;

	// Read the first line of the request


	int len = client.readBytesUntil('\r', buffer, 80);
	buffer[len] = 0;

	client.flush();
	if (!client.connected()) return;
	do {
		if (strstr(buffer, "/Open")) {
			webLog.It(netTime.getHourFloat(), "Opening");
			Tasks.push_back(new TaskOpen(OutArmExtend, 65000));
			printHeader(client, textHtml);
			client.println("Open");
			break;
		}
		if (strstr(buffer, "/Close")) {
			webLog.It(netTime.getHourFloat(), "Closing");
			Tasks.push_back(new TaskOpen(OutArmRetract, 65000));
			printHeader(client, textHtml);
			client.println("Close");
			break;
		}
		if (strstr(buffer, "/Cycle")) {
			webLog.It(netTime.getHourFloat(), "Cycling");
			Tasks.push_back(new TaskOpenBuzz(OutArmExtend, OutShaker, 20000, 6000, 10000));
			Tasks.push_back(new TaskOpenBuzz(OutArmRetract, OutShaker, 21000, 9000, 13000));
			printHeader(client, textHtml);
			client.println("Cycled");
			break;
		}
		if (strstr(buffer, "/AuxOn")) {
			webLog.It(netTime.getHourFloat(), "Aux ON");
			Tasks.push_back(new TaskSetState(OutAux, true));
			printHeader(client, textHtml);
			client.println("AuxOn");
			break;

		}
		if (strstr(buffer, "/AuxOff")) {
			webLog.It(netTime.getHourFloat(), "Aux OFF");
			Tasks.push_back(new TaskSetState(OutAux, false));
			printHeader(client, textHtml);
			client.println("AuxOff");
			break;

		}
		if (strstr(buffer, "/log")) {
			printHeader(client, textHtml);
			tempClient = &client;
			webLog.PrintReverse(clientPrint);
			tempClient = NULL;
			break;
		}
		if (strstr(buffer, "/Dance")) {
			webLog.It(netTime.getHourFloat(), "Dancing");
			//OutShaker ---------- 2000 = 2 seconds    
			Tasks.push_back(new TaskOpen(OutShaker, 2000));
			printHeader(client, textHtml);
			client.println("Dance");

			break;
		}
		if (strstr(buffer, "/Edit")) {
			printHeader(client, textHtml);
			tempClient = &client;
			outputEditSite(&clientPrint);
			tempClient = NULL;
			break;
		}
		if (strstr(buffer, "/schedule.json")) {
			printHeader(client, "application/json");
			client.println("{");
			client.printf("\"Title\": \"%s\",\n", sConfig.Title);
			client.printf("\"Version\": \"%s\",\n", sConfig.Version);
			client.printf("\"DST\": %s,\n", sConfig.DST ? "true" : "false");
			client.println("\"TasksTotal\": 20,");

			client.println(" \"TaskItems\" : [ ");
			for (int i = 0; ; i++) {
				if (ourTasks[i].func == NULL) break;
				if (i != 0) client.println(",");
				client.println("{");
				client.printf("\"value\": \"%d\",\n", ourTasks[i].Id);
				client.printf("\"text\": \"%s\"\n", ourTasks[i].name);
				client.println("}");
			}
			client.println("],");

			//Now the tasks

			client.println(" \"Tasks\" : [ ");
			for (int i = 0;; i++) {
				if (i == 20) break;
				const char *temp = sConfig.Get(i);
				if (strlen(temp) == 0) continue;
				if (i != 0) client.println(",");
				if (strlen(temp) == 0) continue;
				client.printf(" { \"task\": \"%s\"}\n", temp);
			}
			client.println(" ]");
			client.println("}");
			break;
		}

		//Dump out the website......... Invalid commands fall or root fall here
		if (!client.connected()) return;
		printHeader(client, textHtml);
		tempClient = &client;
		outputSite(&clientPrint);
		tempClient = NULL;
		
	} while (0);
	client.flush();
	delay(1);
}

///ctype:  application/json : text/html 
void printHeader(WiFiClient client, const char *ctype) {
	//application / json
	client.println(F("HTTP/1.1 200 OK"));
	client.print(F("Content-Type: "));
	client.print(ctype);
	client.println(F("; charset = UTF-8"));
	client.println("");
}

void clientPrint(const char *data) {
	tempClient->println(data);
}
