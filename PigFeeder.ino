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

//Local Crap
#include "LinkedList.h"
#include "passwordsMom.h"
//#include "passwords.h"
#include "output.h"
#include "Task.h"



#define OutArmExtend D5
#define OutArmRetract D6
#define OutBBC D7

WiFiServer server(80);


//These come from passwords.h
IPAddress ip(DEVICE_IP); // where .xxx is the desired IP Address
IPAddress gateway(DEVICE_GATEWAY); // set gateway to match your network
IPAddress subnet(DEVICE_SUBNET); // set subnet mask to match your

// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(OutArmExtend, OUTPUT);
	pinMode(OutArmRetract, OUTPUT);
	pinMode(OutBBC, OUTPUT);
	Serial.begin(9600);
	digitalWrite(OutArmExtend, HIGH);
	digitalWrite(OutArmRetract, HIGH);
	digitalWrite(OutBBC, HIGH);
	delay(100);

	//Now to get WIFI Going.......
	WiFi.mode(WIFI_STA);
	WiFi.hostname(wifiHostName);

	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");

	WiFi.config(ip, gateway, subnet);
	WiFi.begin(wifiNetwork, wifiPassword);

	//Do a little flashy dance while we connect
	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(D4, LOW);
		delay(125); 
		digitalWrite(D4, HIGH);
		delay(125);
	}
	digitalWrite(D4, LOW);
	delay(250);
	digitalWrite(D4, HIGH);
	delay(125);
	digitalWrite(D4, LOW);
	delay(255);
	digitalWrite(D4, HIGH);
	delay(125);
	digitalWrite(D4, LOW);
	delay(255);
	digitalWrite(D4, HIGH);
	delay(125);
	digitalWrite(D4, HIGH);
	delay(255);
	digitalWrite(D4, LOW);
	delay(125);
	digitalWrite(D4, HIGH);
	delay(255);
	digitalWrite(D4, LOW);
	delay(125);
	digitalWrite(D4, HIGH);
	// Start the server
	server.begin();
	Serial.println("Server started");
	Serial.print("Use this URL to connect: ");
	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");
}

//Task management
LinkedList<Task*> Tasks = LinkedList<Task*>();
Task *curTask = NULL;

//A fucked up pointer because Arduino is fucked up sometimes.
WiFiClient *tempClient = NULL;

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

	//WIFI Stuff last.........
	WiFiClient client = server.available();
	int lps = 0;
	if (client == NULL) {
		delay(1);
		lps++;
		if (lps > 50) return;
	}

	// Wait until the client sends some data
	while (client.connected() && !client.available())
	{
		delay(1);
		lps++;
		if (lps > 100) return;  //Skip wifi stuff.
	}

	//Bail out, something failed.
	if (!client.connected()) return;

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.print("New Client: Request: ");
	Serial.println(request);
	client.flush();

	if (request.indexOf("/Open") != -1) {
		client.println("<br>Opening<br>");
		Tasks.add(new TaskOpen(OutArmExtend));
		return;
	}
	if (request.indexOf("/Close") != -1) {
		client.println("<br>Closing<br>");
		Tasks.add(new TaskOpen(OutArmRetract));
		return;
	}
	if (request.indexOf("/Cycle") != -1) {
		client.println("<br>Cycling<br>");
		Tasks.add(new TaskOpen(OutArmExtend));
		Tasks.add(new TaskOpen(OutArmRetract));
		return;
	}
	if (request.indexOf("/Dance") != -1) {
		client.println("<br>Dancing<br>");
		//OutBBC ---------- 2000 = 2 seconds 
		Tasks.add(new TaskOpen(OutBBC,2000));
		return;
	}

	if (request.indexOf("/data.json") != -1) {
		client.println("{");
		client.println("\"test1\" : 14.2,");
		client.print("\"moving\" : ");
		client.print((Tasks.size() > 0) ? "1" : "0");
		client.println(",");
		client.println("\"test1\" : 14.2");
		client.println("}");
		return;
	}
	//Dump out the website.........
	tempClient = &client;
	outputSite(&clientPrint);
	tempClient = NULL;
	delay(5);
}


void clientPrint(const char *data) {
	tempClient->println(data);
}

