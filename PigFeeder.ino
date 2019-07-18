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


#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

//Local Stuff
#include "passwords.h"
#include "nettime.h"

#include "output.h"
#include "outputedit.h"
#include "Task.h"
#include "OTASetup.h"
#include "WebLog.h"
#include "astronomical.h"
#include "ScheduleConfig.h"

#include "PostMessage.h"

//For storing the arrays of stuff
#include <vector>

#include "pins.h"

WiFiServer server(80);


//These come from passwords.h
IPAddress ip(DEVICE_IP); // where .xxx is the desired IP Address
IPAddress gateway(DEVICE_GATEWAY); // set gateway to match your network
IPAddress subnet(DEVICE_SUBNET); // set subnet mask to match your

IPAddress dns1(DEVICE_DNS1);
IPAddress dns2(DEVICE_DNS2);


const char *textHtml = "text/html";

//Schedule Management
std::vector<ScheduleObject*> schedule = std::vector<ScheduleObject*>();
std::vector<ScheduleObject*>::iterator schedulePtr;

int SchCompare(ScheduleObject *a, ScheduleObject *b) {
	return (a->timeOfDay < b->timeOfDay);
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(115200);
	delay(100);
	Serial.println("Starting up");

	pinMode(D4, OUTPUT);
	pinMode(OutArmExtend, OUTPUT);
	pinMode(OutArmRetract, OUTPUT);
	pinMode(OutShaker, OUTPUT);
	pinMode(OutAux, OUTPUT);
	Serial.println("Pin Mode set");

	digitalWrite(OutArmExtend, HIGH);
	digitalWrite(OutArmRetract, HIGH);
	digitalWrite(OutShaker, HIGH);
	digitalWrite(OutAux, HIGH);

	
	
	Tasks.reserve(20);
	schedule.reserve(25);
	

	sConfig = sConfig.LoadFromEEPROM();
	Serial.println("Starting Wifi");

	//Now to get WIFI Going....... 
	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	WiFi.hostname(wifiHostName);

	WiFi.config(ip, gateway, subnet, dns1, dns2);

	WiFi.begin(wifiNetwork, wifiPassword);
	webLog.println("Wifi Begin");
	Serial.println("Wifi Begin");
	//Do a little flashy dance while we connect
	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(D4, LOW);
		delay(125);
		digitalWrite(D4, HIGH);
		delay(250);
	}
	webLog.println("Wifi Connected");
	Serial.println("Wifi Connected");
	SetupOTA(OTAName);
	
	netTime.Init(sConfig.DST?(DEVICE_TZ+1):DEVICE_TZ);
	netTime.funcTimeCalc = CalcSunriseSunset;
	netTime.funcTimeValid = ReloadSchedule;
	netTime.funcMidnight = NoteMidnight;
	// Start the server
	server.begin();
	Serial.println("Setup over");

}

//A fucked up pointer because Arduino is fucked up sometimes.
WiFiClient *tempClient = NULL;

void NoteMidnight() {
	webLog.println("Midnight cross over");
}

void CalcSunriseSunset() {
	double sunRise = 6;
	double sunSet = 6;
	SunSet sun = SunSet();
	sun.setPosition(DEVICE_LAT, DEVICE_LON, sConfig.DST ? (DEVICE_TZ + 1) : DEVICE_TZ);
	sun.setCurrentDate(netTime.year, netTime.month, netTime.day);
	sunRise = sun.calcSunrise() / 60;
	sunSet = sun.calcSunset() / 60;
	sConfig.Sunrise = sunRise;
	sConfig.Sunset = sunSet;
	char buffer[80];
	snprintf(buffer, 80, "Sunrise:%f Sunset:%f", sunRise, sunSet);
	webLog.println(buffer);
}


bool runScheduler = false;



void ReloadSchedule() {
	schedule.clear();
	SpecTask::DeleteAll();
	ScheduleObject::DeleteAll();
	Tasks.clear();
	Task::DeleteAll();
	webLog.println("Loading schedule");
	int j = 0;
	for (int i = 0; i < 20; i++) {
		ScheduleObject *o = ScheduleObject::Get(sConfig.Get(i));
		if (o) {
			o->completed = false;
			if (o->specTask != NULL) {
				schedule.push_back(o);
			}
		}
	}
	sort(schedule.begin(), schedule.end(), SchCompare);

	schedulePtr = schedule.begin();
	runScheduler = true;

	char buffer[80];
	while (schedulePtr != schedule.end()) {
		ScheduleObject &so = *(*schedulePtr);
		float ttime = netTime.getHourFloat();
		if (so.timeOfDay <= netTime.getHourFloat()) {
			so.completed = true;
			snprintf(buffer, 80, "Skipped %2.2f past task: %s", so.timeOfDay, so.specTask->name);
			webLog.println(buffer);
		}
		else {
			break;
		}
		schedulePtr++;
	}
}

//Reads until timeout or terminator or buffer full.  Returns true if all succeed, false if timeout or overrun. It always terminates with /0
//terminator charaacter IS INCLUDED in the string
bool readBytesUntil(WiFiClient& client, char* dest, char terminator, int len, unsigned long timeout_ms) {
	unsigned long start = millis();
	int pos = 0;
	while ((millis() - start) < timeout_ms) {
		if (!client.connected()) { 
			dest[0] = 0;
			break; 
		}
		if (client.available()) {
			char c = client.read();
			dest[pos++] = c;
			if (c == terminator) {
				dest[pos] = 0;
				return true;
			}
			if (pos >= len) {
				dest[len - 1] = 0;
				return false;
			}
		}
		yield();
	}
	return false;
}

//forward
void printHeader(WiFiClient& client, const char *ctype);

int requestNo = 0;
// Add the main program code into the continuous loop() function
unsigned long togMillis = 0;
void loop()
{
	char buffer[120];
	//for OTA stuff.........
	ArduinoOTA.handle();
	netTime.process();
	
	if (runScheduler && (schedulePtr != schedule.end())) {
		ScheduleObject &so = *(*schedulePtr);
		float timeDiff = netTime.getHourFloat() - so.timeOfDay;
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
	if ((millis() % 1024) < 500) {
		analogWrite(D4,950);
	}
	else {
		//Dim it some for Jordan's apricot and peach sensitive eyes
		digitalWrite(D4, 1);
	}

	//Now handle our tasks
	if (Tasks.size() > 0) {
		auto thisTask= *(Tasks.begin());
		bool val = thisTask->Process();
		if (!val) {
			thisTask->End();
			Tasks.erase(Tasks.begin());
			if (Tasks.size() == 0) {
				//Last one, reset the memory....
				Task::DeleteAll();
				webLog.It(netTime.getHourFloat(), "Freeing Task Memory");
			}
		}
	}
	
	//WIFI Stuff last.........
	WiFiClient client = server.available();
	int lps = 0;
	if (!client) {
		delay(2);
		return;
	}
	digitalWrite(D4, 0);
	Serial.printf("Req: %d Incoming\r\n", requestNo++);

	// Wait until the client sends some data
	while (client.connected() && !client.available())
	{
		delay(1);
		lps++;
		if (lps > 250) {
			Serial.println("Too slow, recycling");
			client.stop();
			return;  //Skip wifi stuff.
		}
	}

	//Bail out, something failed.
	if (!client.connected()) {
		Serial.println("!Connected");
		return;
	}

	// Read the first line of the request
	//client.setDefaultSync(true);

	Serial.println("Reading.......");
	Serial.flush();
	if (!readBytesUntil(client, buffer, '\r', 110, 250)) {
		Serial.printf("Failed read, recycling\r\n");
	};

	Serial.printf("Got %s\r\n", buffer);

	if (!client.connected()) {
		Serial.println("!Connected 2");
		return;
	}
	do {
		if (strstr(buffer, "POST /SetConfig")) {
			handlePost(client);
			printHeader(client, textHtml);
			client.println("Config Set");
			break;
		}
		if (strstr(buffer, "/Open")) {
			webLog.It(netTime.getHourFloat(), "Opening");
			Tasks.push_back(new TaskOpen(OutArmExtend, 65.0));
			printHeader(client, textHtml);
			client.println("Open");
			break;
		}
		if (strstr(buffer, "/Close")) {
			webLog.It(netTime.getHourFloat(), "Closing");
			Tasks.push_back(new TaskOpen(OutArmRetract, 65.0));
			printHeader(client, textHtml);
			client.println("Close");
			break;
		}
		if (strstr(buffer, "/Cycle")) {
			webLog.It(netTime.getHourFloat(), "Cycling");
			Tasks.push_back(new TaskOpenBuzz(OutArmExtend, OutShaker, 20.0, 6.0, 10.0));
			Tasks.push_back(new TaskOpenBuzz(OutArmRetract, OutShaker, 21.0, 9.0, 13.0));
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
			webLog.It(netTime.getHourFloat(), "Triggering Schedule Reload");
			//OutShaker ---------- 2000 = 2 seconds    
			
			netTime.triggerReload = true;
			printHeader(client, textHtml);
			client.println("Reload");

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
		break;
		
	} while (0);
	Serial.println("Request Finished");
	
}

///ctype:  application/json : text/html 
void printHeader(WiFiClient& client, const char *ctype) {
	//application / json
	if (!client.connected()) return;
	client.println(F("HTTP/1.1 200 OK"));
	client.print(F("Content-Type: "));
	client.print(ctype);
	client.println(F("; charset = UTF-8"));
	client.println("");
}


const char *getVarData(int val) {
	switch (val) {
	case 1: return sConfig.Title;
	case 2: return sConfig.Version;
	default: return "Undefined";
	}
}


void clientPrint(const char *data) {
	int iid = 0;
	//Simple replacement of title, version
	const char *ptr = NULL;
	ptr = strstr(data, "{%");
	if (!tempClient->connected()) return;
	if (ptr) {
		tempClient->write(data, ptr - data);
		ptr += 2;
		if (sscanf(ptr, "%d", &iid) == 1) {
			tempClient->print(getVarData(iid));
		}
		ptr += 2;
		tempClient->println(ptr);
		return;
	}

	tempClient->println(data);
}
