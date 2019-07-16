
#include <math.h>
#include <time.h>


//This was snagged from github, I forgot the source.......

class SunSet {
public:
	SunSet();
	SunSet(double, double, int);
	~SunSet();

	void setPosition(double lat, double lon, int tz);
	void setTZOffset(int);
	double setCurrentDate(int y, int m, int d);
	double calcSunriseUTC();
	double calcSunsetUTC();
	double calcSunrise();
	double calcSunset();
	int moonPhase(int);
	//int moonPhase();

private:
	double degToRad(double);
	double radToDeg(double);
	double calcMeanObliquityOfEcliptic(double);
	double calcGeomMeanLongSun(double);
	double calcObliquityCorrection(double);
	double calcEccentricityEarthOrbit(double);
	double calcGeomMeanAnomalySun(double);
	double calcEquationOfTime(double);
	double calcTimeJulianCent(double);
	double calcSunTrueLong(double);
	double calcSunApparentLong(double);
	double calcSunDeclination(double);
	double calcHourAngleSunrise(double, double);
	double calcHourAngleSunset(double, double);
	double calcJD(int, int, int);
	double calcJDFromJulianCent(double);
	double calcSunEqOfCenter(double);

	double latitude;
	double longitude;
	double julianDate;
	int m_year;
	int m_month;
	int m_day;
	int tzOffset;
};
#pragma once
#ifndef __CRC_H__
#define __CRC_H__ 1

#include <stdint.h>

uint8_t crcSlow(uint8_t const *message, int nBytes);

#endif#pragma once

class FeedConfig {
public:
	float TaskTimeOpen = 1.0f;
	float TaskTimeClose = 1.0f;
	float TaskTimeBuzz = 1.0f;
};

#ifndef __FixedRam_H__
#define __FixedRam_H__ 1


template <typename T>
class FixedRam {
protected:
	static int		taskSizeBytes;
	static uint8_t	*ramBuffer; //The data area 
#ifdef TESTSETUP 
	static uint8_t	*ramNameBuffer; //Just the name for debugging
#endif
	static uint8_t	*ramPtr;  //New heap location
public:
	void* allocate(size_t size) {
		uint8_t *temp = ramPtr - size;
		if (temp < ramBuffer) return NULL;
		ramPtr = temp;
		return temp;
	}
	void Reset() {
		ramPtr = (ramBuffer + taskSizeBytes);
	}
	FixedRam<T>(const char* iname, int sz) {
		FixedRam<T>::taskSizeBytes = sz;
		ramBuffer = new uint8_t[taskSizeBytes];
		Reset();
#ifdef TESTSETUP 
		//This isn't cool, but it's needed for now.  It's only once......
		FixedRam<T>::ramNameBuffer = new uint8_t[strlen(iname) + 1];
		strcpy((char *)ramNameBuffer, iname);
#endif
	}
};

/*
template<typename T>
uint8_t *FixedRam<T>::ramNameBuffer = NULL;
*/
template<typename T>
uint8_t *FixedRam<T>::ramBuffer = NULL;

template<typename T>
uint8_t *FixedRam<T>::ramPtr = NULL;

template<typename T>
int FixedRam<T>::taskSizeBytes = 1024;

#endif
/*
	LinkedList.h - V1.1 - Generic LinkedList implementation
	Works better with FIFO, because LIFO will need to
	search the entire List to find the last one;

	For instructions, go to https://github.com/ivanseidel/LinkedList

	Created by Ivan Seidel Gomes, March, 2013.
	Released into the public domain.
*/


#ifndef LinkedList_h
#define LinkedList_h

#include <stddef.h>

template<class T>
struct ListNode
{
	T data;
	ListNode<T> *next;
};

template <typename T>
class LinkedList{

protected:
	int _size;
	ListNode<T> *root;
	ListNode<T>	*last;

	// Helps "get" method, by saving last position
	ListNode<T> *lastNodeGot;
	int lastIndexGot;
	// isCached should be set to FALSE
	// everytime the list suffer changes
	bool isCached;

	ListNode<T>* getNode(int index);

public:
	LinkedList();
	~LinkedList();

	/*
		Returns current size of LinkedList
	*/
	virtual int size();
	/*
		Adds a T object in the specified index;
		Unlink and link the LinkedList correcly;
		Increment _size
	*/
	virtual bool add(int index, T);
	/*
		Adds a T object in the end of the LinkedList;
		Increment _size;
	*/
	virtual bool add(T);
	/*
		Adds a T object in the start of the LinkedList;
		Increment _size;
	*/
	virtual bool unshift(T);
	/*
		Set the object at index, with T;
		Increment _size;
	*/
	virtual bool set(int index, T);
	/*
		Remove object at index;
		If index is not reachable, returns false;
		else, decrement _size
	*/
	virtual T remove(int index);
	/*
		Remove last object;
	*/
	virtual T pop();
	/*
		Remove first object;
	*/
	virtual T shift();
	/*
		Get the index'th element on the list;
		Return Element if accessible,
		else, return false;
	*/
	virtual T get(int index);

	/*
		Clear the entire array
	*/
	virtual void clear();

};

// Initialize LinkedList with false values
template<typename T>
LinkedList<T>::LinkedList()
{
	root=NULL;
	last=NULL;
	_size=0;

	lastNodeGot = root;
	lastIndexGot = 0;
	isCached = false;
}

// Clear Nodes and free Memory
template<typename T>
LinkedList<T>::~LinkedList()
{
	ListNode<T>* tmp;
	while(root!=NULL)
	{
		tmp=root;
		root=root->next;
		delete tmp;
	}
	last = NULL;
	_size=0;
	isCached = false;
}

/*
	Actualy "logic" coding
*/

template<typename T>
ListNode<T>* LinkedList<T>::getNode(int index){

	int _pos = 0;
	ListNode<T>* current = root;

	// Check if the node trying to get is
	// immediatly AFTER the previous got one
	if(isCached && lastIndexGot <= index){
		_pos = lastIndexGot;
		current = lastNodeGot;
	}

	while(_pos < index && current){
		current = current->next;

		_pos++;
	}

	// Check if the object index got is the same as the required
	if(_pos == index){
		isCached = true;
		lastIndexGot = index;
		lastNodeGot = current;

		return current;
	}

	return false;
}

template<typename T>
int LinkedList<T>::size(){
	return _size;
}

template<typename T>
bool LinkedList<T>::add(int index, T _t){

	if(index >= _size)
		return add(_t);

	if(index == 0)
		return unshift(_t);

	ListNode<T> *tmp = new ListNode<T>(),
				 *_prev = getNode(index-1);
	tmp->data = _t;
	tmp->next = _prev->next;
	_prev->next = tmp;

	_size++;
	isCached = false;

	return true;
}

template<typename T>
bool LinkedList<T>::add(T _t){

	ListNode<T> *tmp = new ListNode<T>();
	tmp->data = _t;
	tmp->next = NULL;
	
	if(root){
		// Already have elements inserted
		last->next = tmp;
		last = tmp;
	}else{
		// First element being inserted
		root = tmp;
		last = tmp;
	}

	_size++;
	isCached = false;

	return true;
}

template<typename T>
bool LinkedList<T>::unshift(T _t){

	if(_size == 0)
		return add(_t);

	ListNode<T> *tmp = new ListNode<T>();
	tmp->next = root;
	tmp->data = _t;
	root = tmp;
	
	_size++;
	isCached = false;
	
	return true;
}

template<typename T>
bool LinkedList<T>::set(int index, T _t){
	// Check if index position is in bounds
	if(index < 0 || index >= _size)
		return false;

	getNode(index)->data = _t;
	return true;
}

template<typename T>
T LinkedList<T>::pop(){
	if(_size <= 0)
		return T();
	
	isCached = false;

	if(_size >= 2){
		ListNode<T> *tmp = getNode(_size - 2);
		T ret = tmp->next->data;
		delete(tmp->next);
		tmp->next = NULL;
		last = tmp;
		_size--;
		return ret;
	}else{
		// Only one element left on the list
		T ret = root->data;
		delete(root);
		root = NULL;
		last = NULL;
		_size = 0;
		return ret;
	}
}

template<typename T>
T LinkedList<T>::shift(){
	if(_size <= 0)
		return T();

	if(_size > 1){
		ListNode<T> *_next = root->next;
		T ret = root->data;
		delete(root);
		root = _next;
		_size --;
		isCached = false;

		return ret;
	}else{
		// Only one left, then pop()
		return pop();
	}

}

template<typename T>
T LinkedList<T>::remove(int index){
	if (index < 0 || index >= _size)
	{
		return T();
	}

	if(index == 0)
		return shift();
	
	if (index == _size-1)
	{
		return pop();
	}

	ListNode<T> *tmp = getNode(index - 1);
	ListNode<T> *toDelete = tmp->next;
	T ret = toDelete->data;
	tmp->next = tmp->next->next;
	delete(toDelete);
	_size--;
	isCached = false;
	return ret;
}


template<typename T>
T LinkedList<T>::get(int index){
	ListNode<T> *tmp = getNode(index);

	return (tmp ? tmp->data : T());
}

template<typename T>
void LinkedList<T>::clear(){
	while(size() > 0)
		shift();
}

#endif
#pragma once
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <functional>

/*
	NetTime.h:

	Provides a means for keeping track of time in seconds without a RTC.  It also gets the time with a call to time servers via the NTP Pool.

	Author:  Charles Wood
	Date:  2019-6-1

	Call netTime.Init(timeZone) in setup()
	Call netTime.process() in loop() at least once a second.  (more is okay)
	
*/


struct TimeODay {
	int hour;
	int minute;
	int second;
};

class NetTime {
private:
	unsigned long runTime = 0;
	unsigned long lastTime = 0;
	long secsPastMid = 0;
	long secsSinceLastTimeUpdate = 0;
	void sendTimeRequest();
	void checkTimeRequest();
	long timeZone = 0;

public:
	bool triggerReload = false;
	bool first = true;
	bool invalidTime = true;

	NetTime();
	int month = 1, day = 1, year = 2000;
	void Init(long iTZ);
	void setTime(int h, int m, int s);
	void setTimeSecs(int s);
	long getTimeSec();
	TimeODay getTime();
	//We are just going to assume we get called once a second or so...
	void process();
	bool needNewTime();
	float getRunTimeHours();
	float getHourFloat();// 0-24.0
	//This is called all the time saying we got a time server update, use to calc sunrise and shit.
	std::function<void(void)> funcTimeCalc = NULL;
	//This is called when we go from invalid time to good time, ie: start and after midnight
	std::function<void(void)> funcTimeValid = NULL;
	//This is called to let the program be aware all time are now invalid......
	std::function<void(void)> funcMidnight = NULL;
private:
	//Time stuff
	unsigned int localPort = 2390;      // local port to listen for UDP packets

										/* Don't hardwire the IP address or we won't get the benefits of the pool.
										*  Lookup the IP address for the host name instead */
										//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

	WiFiUDP udp;
	IPAddress timeServerIP; // time.nist.gov NTP server address
	const char* ntpServerName = "time.nist.gov";

	static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
	uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
	unsigned long lastSendTime = 0;
	unsigned long sendNTPpacket(IPAddress& address);
};

extern NetTime netTime;



#pragma once


void SetupOTA(const char* boardName);

#ifndef output_h
#define output_h

//Forwards....

//extern WifiClient;

void outputSite(void(*printFunction)(const char *));


#endif

#ifndef __output_edit_h__
#define __output_edit_h__ 1

//Forwards....

//extern WifiClient;

void outputEditSite(void(*printFunction)(const char *));


#endif

#pragma once

//The name of the device
const char *wifiHostName = "Chicken";
//Name of the wifi network to connect to 
const char *wifiNetwork = "Wood"; 
//Password for the Wifi network
const char *wifiPassword = "slipperywhendusty";

const char *OTAName = "ChickenShit8235";


//USE COMMA, NOT . FOR THIS.
//IE:  192,168,0,1

#define DEVICE_IP 192,168,89,104

#define DEVICE_GATEWAY 192,168,89,1
#define DEVICE_SUBNET 255,255,255,0
#define DEVICE_DNS1 8,8,8,8
#define DEVICE_DNS2 8,8,4,4

// Charles's place
#define DEVICE_LAT 37.676
#define DEVICE_LON -118.087

//Jordan's place
/*
#define DEVICE_LAT 33.481837
#define DEVICE_LON -97.070883
*/

//Uncomment the one that is correct for you........

#define DEVICE_TZ -8   //Pacific standard time
//#define DEVICE_TZ -7  //Mountain standard time/pacific daylight
//#define DEVICE_TZ -6  //Central standard time
//#define DEVICE_TZ -5  //Eastern standard time



#pragma once

//The name of the device
const char *wifiHostName = "Chicken";
//Name of the wifi network to connect to
const char *wifiNetwork = "Linksys04983";
//Password for the Wifi network
const char *wifiPassword = "708bz4ehwt";

//USE COMMA, NOT . FOR THIS.
//IE:  192,168,0,1

#define DEVICE_IP 192,168,1,101
#define DEVICE_GATEWAY 192,168,1,1
#define DEVICE_SUBNET 255,255,255,0

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

// TODO: add headers that you want to pre-compile here
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstring>


#endif //PCH_H
#pragma once

#include "LinkedList.h"

/*

class ScheduleItem {
public:
	unsigned timeOfDay;  //in minutes since midnight
	//O-Open, C-Close, S-Shake
	char task[40];  //String with room for 39 events
	float time;  //Seconds
	bool complete;
};
*/
//Examples:  in decimal seconds
/*
	//Just close
	C,9    
	//Open
	O,8.0 
	//Just buzz 4.2 seconds
	B,4.2         
*/

/*So in the scheduler:

	timeOfDay = '0600'    (6 am STANDARD TIME)
	task = "O"
	time = 8.0

	timeOfDay = '0'    
	task = "B"
	time = 1.3
	
	timeOfDay = '0'    
	task = "C"
	time = 9.0

	timeOfDay = '0'
	task = "B"
	time = 2.0

	//Repeat for 1800 (6PM)

	timeOfDay = '1800'    (6 am STANDARD TIME)
	task = "O"
	time = 8.0

	timeOfDay = '0'
	task = "B"
	time = 1.3

	timeOfDay = '0'
	task = "C"
	time = 9.0

	timeOfDay = '0'
	task = "B"
	time = 2.0
*/


/*

class Scheduler {
private:
	LinkedList<ScheduleItem*> list = LinkedList<ScheduleItem*>();
};

*/#pragma once
#ifndef __PINS_H
#define __PINS_H 1
#include <Arduino.h>

//This is the hardware layout for the 
/*
#define D5 5
#define D6 6
#define D7 7
#define A0 0
*/

#define PIN_DOOR_OPEN D5
#define PIN_DOOR_CLOSE D6
#define PIN_LIGHT D7
#define PIN_AUDIO A0

#define OutArmExtend D5
#define OutArmRetract D6
#define OutShaker D7
#define OutAux D8


#endif


#pragma once
#ifndef __PostMessage_H__
#define __PostMessage_H__ 1
#include <ESP8266WiFi.h>


void handlePost(WiFiClient& client);

#endif
#pragma once
#ifndef __ScheduleConfig_H__ 
#define __ScheduleConfig_H__ 1


#include "TaskChicken.h"
#include "crc.h"
#include <stdint.h>
#include "WebLog.h"
#include "ScheduleObject.h"

class ScheduleConfig {
private:
	char elements[21][80];
	int sz = 0;
public:
	//Title of the App
	char Title[80];
	//Special notes about the version
	char Version[80];
	int  DST = false;
	//These get updated by the sunrise code and saved when config is changed.
	//If no time server, it can be adjusted manually.
	float Sunrise = 6.0f;
	float Sunset = 18.0f;
	inline const char *Get(int loc) {
		if (loc < 0) return "";
		if (loc >= 20) return "";
		if (loc >= sz) return "";
		return elements[loc];
	}
	inline void ClearSchedule() {
		for (int i = 0; i < 20; i++) {
			elements[i][0] = 0;
		}
		sz = 0;
	}
	void Add(const char *command);
	inline void Set(int loc, const char *command) {
		if (loc < 0) return;
		if (loc >= 20) return;
		if (loc > sz) return;
		strncpy(elements[loc], command, 80);
	}

	inline unsigned getCRC() {
		//return crcSlow(std::reinterpretcast<uint8_t*>( this), sizeof(this));
		return crcSlow(((uint8_t*)this), sizeof(this));
	}

	ScheduleConfig LoadFromEEPROM();
	bool SaveToEEPROM();


	inline void ResetDefault() {
		ClearSchedule();
		Add("5:1.1,0,0,Test Message");
		Add("5:0.1,0,0,Schedule Startup");
		Add("5:0.0,1,0,Wake up! It's sunrise; you're burning daylight!");
		Add("5:0.0,0,1,Sunset Go to bed!");
		DST = false;
		strcpy(Title, "New Remote Action Unit");
		strcpy(Version, "Enter something here");
	}
};

extern ScheduleConfig sConfig;

#endif#pragma once
#include "FixedRam.h"
#include "TaskChicken.h"


class ScheduleObject {
	static FixedRam<ScheduleObject> ramBase;
public:
	bool completed = false;
	float timeOfDay = 0;
	SpecTask* specTask = NULL;
	//It may change things, but everything is back where it was when it finishes = true
	bool stately = false;
public:  //Static stuff
	void* operator new(size_t size);
	static inline void DeleteAll() {
		ramBase.Reset();
	}
	//Class factory 
	static ScheduleObject* Get(const char *str);
};

//This is just storing a list of stuff......
struct SpecTaskClass {
	std::function<ScheduleObject*(const char*)> func;
	int Id;
	const char *name;
};

//This is to be filled in after classes are created
extern SpecTaskClass ourTasks[];



#ifndef __Task_h__
#define __Task_h__ 1

#include "pch.h"
#include "FixedRam.h"


class Task {
protected: 
	static FixedRam<Task> ramBase;
public:
	inline void* operator new(size_t size) {
		return ramBase.allocate(size);
	}
	static void DeleteAll();
	Task();
	virtual void Start() = 0;
	virtual bool Process() = 0;
	virtual void End() = 0;
};

class TaskOpen : public Task {
protected:
	unsigned pin = 0;
	unsigned long startTime = 0;
	unsigned long length = 20000;  //ms

public:
	TaskOpen() = default;
	TaskOpen(unsigned ipin, int imillis=20000);
	//start is very important, it must fire things up multiple times.
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskOpenBuzz : public TaskOpen {
protected:
	int buzzStart;
	int buzzEnd;
	unsigned buzzPin;
public:
	TaskOpenBuzz() = default;
	TaskOpenBuzz(unsigned ipin, unsigned bpin, int imillis = 20000, int bStart = 6000, int bEnd = 10000);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskSetState: public TaskOpen {
	bool On;
public:
	TaskSetState() = default;
	TaskSetState(int iPin, bool iOn);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

class TaskWait : public TaskOpen {
public:
	TaskWait() = default;
	void virtual Start();
	void virtual End();
};

class TaskLogMsg : public Task {
	//See:  https://stackoverflow.com/questions/35413821/how-to-fix-this-array-used-as-initializer-error
	char buffer[80];  //Was  buffer[80] = "";
public:
	TaskLogMsg();
	TaskLogMsg(const char *msg);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};

#endif#include "pch.h"
#include "task.h"
#include <functional>
#include <array>
#include <iostream>

#ifndef __TaskChicken_H_
#define __TaskChicken_H_ 1
#include "FixedRam.h"

class ScheduleObject;

class SpecTask {
private:
	static FixedRam<SpecTask> ramBase;
public:
	void* operator new(size_t size);
	bool stately = false;
	const char *name;
	//	static const int TaskId = 0;  //No task here
	float timeOfDay = 0.0f;
	Task *task = NULL;
	static inline float rangeInF(float val, float min,  float max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}
	static inline int rangeIn(int val, int min, int max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}
	static bool ParseTime(const char *str, float* time, char *rest);
	static inline void DeleteAll() {
		ramBase.Reset();
	}
};

class SpecOpenDoor : public SpecTask {
private:
	SpecOpenDoor(unsigned long secs);
	SpecOpenDoor() = default;
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 1;
};

class SpecCloseDoor : SpecTask {
private:
	SpecCloseDoor(unsigned long secs);
	SpecCloseDoor() = default;
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 2;
};


class SpecLightOn : SpecTask {
private:
	SpecLightOn();
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 3;
};

class SpecLightOff : SpecTask {
private:
	SpecLightOff();
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 4;
};

class SpecLogText : SpecTask {
private:
	char asdf[80];
	SpecLogText(const char *msg);
public:
	static ScheduleObject* Create(const char *vals);
	static const int TaskId = 5;
};


#endif


#pragma once

class WebEntry {
public:
	float timeOfDay;
	char message[80];
	void Clear() {
		message[0] = 0;
		timeOfDay = 0;
	}
};

class WebLog {
private:
	int size = 20;
	int headPtr = 0;
	int tailPtr = 0;
	WebEntry *entries;
public:
	void floatToHourMin(char *dest, float t);
	WebLog(int isize);
	void It(float time, const char *message);
	void println(const char *msg);
	void Print(void(*printFunction)(const char *));
	void PrintReverse(void(*printFunction)(const char *));
	void Clear();

};

extern WebLog webLog;
