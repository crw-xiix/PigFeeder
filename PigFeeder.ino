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
#include "Task.h"
#include "OTASetup.h"
#include "WebLog.h"
#include "astronomical.h"




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


// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(D4, OUTPUT);
	pinMode(OutArmExtend, OUTPUT);
	pinMode(OutArmRetract, OUTPUT);
	pinMode(OutShaker, OUTPUT);
	Serial.begin(115200);
	digitalWrite(OutArmExtend, HIGH);
	digitalWrite(OutArmRetract, HIGH);
	digitalWrite(OutShaker, HIGH);
	delay(100);

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
 
  //How do we prevent me from fucking this up? 
  //Really?
  //Charles Likes BBC, just FYI. Don't let him touch you.
	SetupOTA("ChickenBus-ESP8235");
  

	netTime.Init(DEVICE_TZ);
	netTime.GotNewTime = dummy;
	// Start the server
	server.begin();

}

//Task management
LinkedList<Task*> Tasks = LinkedList<Task*>();
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
	char buffer[80];
	snprintf(buffer, 80, "Sunrise:%f Sunset:%f", sunRise, sunSet);
	webLog.println(buffer);
}
//Git me out of here



// Add the main program code into the continuous loop() function
unsigned long togMillis = 0;
void loop()
{

	//for OTA stuff.........
	ArduinoOTA.handle();
	netTime.process();

	//Toggle the LED light for heartbeat
	if ((millis() - togMillis) > 100) {
		digitalWrite(D4, !digitalRead(D4));
		togMillis = millis();
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

	//WIFI Stuff last.........
	WiFiClient client = server.available();
	int lps = 0;
	if (client == NULL) {
		delay(1);
		lps++;
		if (lps > 50) {
			
			return;
		}
	}

	// Wait until the client sends some data
	while (client.connected() && !client.available())
	{
		delay(1);
		lps++;
		if (lps > 100) {
			
			return;  //Skip wifi stuff.
		}
	}

	//Bail out, something failed.
	if (!client.connected()) return;

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.print("New Client: Request: ");
	Serial.println(request);
	client.flush();
	if (!client.connected()) return;

	if (request.indexOf("/Open") != -1) {
		webLog.It(netTime.getHourFloat(),"Opening");
		//Tasks.add(new TaskOpen(D4, 500));
		Tasks.add(new TaskOpen(OutArmExtend,65000));
		return;
	}
	if (request.indexOf("/Close") != -1) {
		webLog.It(netTime.getHourFloat(), "Closing");
		//Tasks.add(new TaskOpen(D4, 500));
		Tasks.add(new TaskOpen(OutArmRetract,65000));
		return;
	}
	if (request.indexOf("/Cycle") != -1) {
		webLog.It(netTime.getHourFloat(), "Cycling");
		Tasks.add(new TaskOpenBuzz(OutArmExtend,OutShaker,20000,6000,10000));
		Tasks.add(new TaskOpenBuzz(OutArmRetract,OutShaker,21000,9000,13000));
		return;
	}
	if (request.indexOf("/AuxOn") != -1) {
		Tasks.add(new TaskSetState(OutAux, true));
	}
	if (request.indexOf("/AuxOff") != -1) {
		Tasks.add(new TaskSetState(OutAux, false));
	}
	if (request.indexOf("/log") != -1) {
		tempClient = &client;
		webLog.PrintReverse(clientPrint);
		tempClient = NULL;
		return;
	}
	if (request.indexOf("/Dance") != -1) {
		webLog.It(netTime.getHourFloat(), "Dancing");
		//OutShaker ---------- 2000 = 2 seconds    
		Tasks.add(new TaskOpen(OutShaker,2000));
		return;
	}

	//Dump out the website.........
	if (!client.connected()) return;
	tempClient = &client;
	outputSite(&clientPrint);
	tempClient = NULL;
	delay(50);
}


void clientPrint(const char *data) {
	tempClient->println(data);
}
