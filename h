
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

uint8_t crcSlow(uint8_t const message[], int nBytes);

#endif#pragma once

class FeedConfig {
public:
	float TaskTimeOpen = 1.0f;
	float TaskTimeClose = 1.0f;
	float TaskTimeBuzz = 1.0f;
};
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
	bool first = true;
	unsigned long lastTime = 0;
	long secsPastMid = 0;
	long secsSinceLastTimeUpdate = 0;
	void sendTimeRequest();
	void checkTimeRequest();
	long timeZone = 0;

public:
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
	void(*GotNewTime)() = NULL;
	std::function<void(void)> newTimeValid = NULL;

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
	byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
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

//USE COMMA, NOT . FOR THIS.
//IE:  192,168,0,1

#define DEVICE_IP 192,168,89,103

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

#define DEVICE_TZ -8  //Pacific standard time
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




class Scheduler {
private:
	LinkedList<ScheduleItem*> list = LinkedList<ScheduleItem*>();
};

#pragma once
#ifndef __PINS_H
#define __PINS_H 1

//This is the hardware layout for the 
#define D5 5
#define D6 6
#define D7 7
#define A0 0


#define PIN_DOOR_OPEN D5
#define PIN_DOOR_CLOSE D6
#define PIN_LIGHT D7
#define PIN_AUDIO A0

#endif


#pragma once
#ifndef __ScheduleConfig_H__ 
#define __ScheduleConfig_H__ 1


#include "TaskChicken.h"
#include "crc.h"
#include <stdint.h>
#include "WebLog.h"


//Basically, we're gonna support 20 different things on the schedule, and it will have to be sorted also.

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

	bool LoadFromEEPROM();

	inline void ResetDefault() {
		
		ClearSchedule();
		Add("5:1.1,0,0,Test Message");
		Add("5:0.1,0,0,Schedule Startup");
		Add("5:0.0,1,0,Sunrise");
		Add("5:16.15,0,0,Uhoh-It's Times");
		Add("5:0.0,0,1,Sunset");
		DST = false;
		strcpy(Title, "New Remote Action Unit");
		strcpy(Version, "Enter something here");
	}
};

extern ScheduleConfig sConfig;



#endif#ifndef __Task_h__
#define __Task_h__ 1

#include "pch.h"

class Task {
public:
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
	char buffer[80];
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
#include <iterator>
#include <algorithm>
#include <array>

#ifndef __TaskChicken_H_
#define __TaskChicken_H_ 1

class SpecNothing;

extern SpecNothing SpecificTaskOfNothing;

class SpecTask {
public:
	bool stately = false;
	const char *name;
	//	static const int TaskId = 0;  //No task here
	static TaskWait EmptyTask;
	float timeOfDay = 0.0f;
	virtual Task *GetTask() {
		return NULL;
	};
	//virtual const char *GetScanStr() = 0;
	static inline float rangeInF(float val, float min, float max) {
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
	
};


class ScheduleObject {
public:
	bool completed = false;
	float timeOfDay = 0;
	SpecTask* specTask = NULL;
	//It may change things, but everything is back where it was when it finishes = true
	bool stately = false;
public:  //Static stuff
	//Class factory 
	static ScheduleObject Get(const char *str);
};


class SpecNothing : public SpecTask {
public:
	SpecNothing() = default;
};

class SpecOpenDoor : public SpecTask {
private:
	TaskOpen task;
	SpecOpenDoor(unsigned long secs);
	SpecOpenDoor() = default;
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 1;
	inline Task *GetTask() override {
		return (&task);
	}
};

class SpecCloseDoor : SpecTask {
private:
	TaskOpen task;
	SpecCloseDoor(unsigned long secs);
	SpecCloseDoor() = default;
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 2;
	inline Task *GetTask() override {
		return (&task);
	}
};


class SpecLightOn : SpecTask {
private:
	TaskSetState task;
	SpecLightOn();
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 3;

	inline Task *GetTask() override {
		return &task;
	}
};

class SpecLightOff : SpecTask {
private:
	TaskSetState task;
	SpecLightOff();
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 4;

	inline Task *GetTask() override {
		return &task;
	}
};


class SpecLogText : SpecTask {
private:
	TaskLogMsg task;
	SpecLogText(const char *msg);
public:
	static ScheduleObject Create(const char *vals);
	static const int TaskId = 5;
	inline Task *GetTask() override {
		return &task;
	}
};

struct SpecTaskClass {
	std::function<ScheduleObject(const char*)> func;
	int Id;
	const char *name;
};


extern SpecTaskClass ourTasks[];


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

	WebLog(int isize);
	void It(float time, const char *message);
	void println(const char *msg);
	void Print(void(*printFunction)(const char *));
	void PrintReverse(void(*printFunction)(const char *));
	void Clear();

};

extern WebLog webLog;
#include "astronomical.h"

//This was snagged from github, I forgot the source.......


SunSet::SunSet()
{
	latitude = 0.0;
	longitude = 0.0;
	julianDate = 0.0;
	tzOffset = 0;
}

SunSet::SunSet(double lat, double lon, int tz)
{
	setPosition(lat, lon, tz);
	julianDate = 0.0;
}

SunSet::~SunSet()
{
}

void SunSet::setPosition(double lat, double lon, int tz)
{
	latitude = lat;
	longitude = lon;
	tzOffset = tz;
}

double SunSet::degToRad(double angleDeg)
{
	return (M_PI * angleDeg / 180.0);
}

double SunSet::radToDeg(double angleRad)
{
	return (180.0 * angleRad / M_PI);
}

double SunSet::calcMeanObliquityOfEcliptic(double t)
{
	double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
	double e0 = 23.0 + (26.0 + (seconds / 60.0)) / 60.0;

	return e0;              // in degrees
}

double SunSet::calcGeomMeanLongSun(double t)
{
	double L = 280.46646 + t * (36000.76983 + 0.0003032 * t);

	while ((int)L > 360) {
		L -= 360.0;
	}

	while (L <  0) {
		L += 360.0;
	}

	return L;              // in degrees
}

double SunSet::calcObliquityCorrection(double t)
{
	double e0 = calcMeanObliquityOfEcliptic(t);
	double omega = 125.04 - 1934.136 * t;
	double e = e0 + 0.00256 * cos(degToRad(omega));

	return e;               // in degrees
}

double SunSet::calcEccentricityEarthOrbit(double t)
{
	double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
	return e;               // unitless
}

double SunSet::calcGeomMeanAnomalySun(double t)
{
	double M = 357.52911 + t * (35999.05029 - 0.0001537 * t);
	return M;               // in degrees
}

double SunSet::calcEquationOfTime(double t)
{
	double epsilon = calcObliquityCorrection(t);
	double l0 = calcGeomMeanLongSun(t);
	double e = calcEccentricityEarthOrbit(t);
	double m = calcGeomMeanAnomalySun(t);
	double y = tan(degToRad(epsilon) / 2.0);

	y *= y;

	double sin2l0 = sin(2.0 * degToRad(l0));
	double sinm = sin(degToRad(m));
	double cos2l0 = cos(2.0 * degToRad(l0));
	double sin4l0 = sin(4.0 * degToRad(l0));
	double sin2m = sin(2.0 * degToRad(m));
	double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0 - 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;
	return radToDeg(Etime)*4.0;	// in minutes of time
}

double SunSet::calcTimeJulianCent(double jd)
{
	double T = (jd - 2451545.0) / 36525.0;
	return T;
}

double SunSet::calcSunTrueLong(double t)
{
	double l0 = calcGeomMeanLongSun(t);
	double c = calcSunEqOfCenter(t);

	double O = l0 + c;
	return O;               // in degrees
}

double SunSet::calcSunApparentLong(double t)
{
	double o = calcSunTrueLong(t);

	double  omega = 125.04 - 1934.136 * t;
	double  lambda = o - 0.00569 - 0.00478 * sin(degToRad(omega));
	return lambda;          // in degrees
}

double SunSet::calcSunDeclination(double t)
{
	double e = calcObliquityCorrection(t);
	double lambda = calcSunApparentLong(t);

	double sint = sin(degToRad(e)) * sin(degToRad(lambda));
	double theta = radToDeg(asin(sint));
	return theta;           // in degrees
}

double SunSet::calcHourAngleSunrise(double lat, double solarDec)
{
	double latRad = degToRad(lat);
	double sdRad = degToRad(solarDec);
	double HA = (acos(cos(degToRad(90.833)) / (cos(latRad)*cos(sdRad)) - tan(latRad) * tan(sdRad)));

	return HA;              // in radians
}

double SunSet::calcHourAngleSunset(double lat, double solarDec)
{
	double latRad = degToRad(lat);
	double sdRad = degToRad(solarDec);
	double HA = (acos(cos(degToRad(90.833)) / (cos(latRad)*cos(sdRad)) - tan(latRad) * tan(sdRad)));

	return -HA;              // in radians
}

double SunSet::calcJD(int y, int m, int d)
{
	if (m <= 2) {
		y -= 1;
		m += 12;
	}
	int A = floor(y / 100);
	int B = 2 - A + floor(A / 4);

	double JD = floor(365.25*(y + 4716)) + floor(30.6001*(m + 1)) + d + B - 1524.5;
	return JD;
}

double SunSet::calcJDFromJulianCent(double t)
{
	double JD = t * 36525.0 + 2451545.0;
	return JD;
}

double SunSet::calcSunEqOfCenter(double t)
{
	double m = calcGeomMeanAnomalySun(t);
	double mrad = degToRad(m);
	double sinm = sin(mrad);
	double sin2m = sin(mrad + mrad);
	double sin3m = sin(mrad + mrad + mrad);
	double C = sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;

	return C;		// in degrees
}

double SunSet::calcSunriseUTC()
{
	double t = calcTimeJulianCent(julianDate);
	// *** First pass to approximate sunrise
	double  eqTime = calcEquationOfTime(t);
	double  solarDec = calcSunDeclination(t);
	double  hourAngle = calcHourAngleSunrise(latitude, solarDec);
	double  delta = longitude + radToDeg(hourAngle);
	double  timeDiff = 4 * delta;	// in minutes of time
	double  timeUTC = 720 - timeDiff - eqTime;	// in minutes
	double  newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC / 1440.0);

	eqTime = calcEquationOfTime(newt);
	solarDec = calcSunDeclination(newt);

	hourAngle = calcHourAngleSunrise(latitude, solarDec);
	delta = longitude + radToDeg(hourAngle);
	timeDiff = 4 * delta;
	timeUTC = 720 - timeDiff - eqTime; // in minutes

	return timeUTC;
}

double SunSet::calcSunrise()
{
	/*
	double t = calcTimeJulianCent(julianDate);
	// *** First pass to approximate sunrise
	double  eqTime = calcEquationOfTime(t);
	double  solarDec = calcSunDeclination(t);
	double  hourAngle = calcHourAngleSunrise(latitude, solarDec);
	double  delta = longitude + radToDeg(hourAngle);
	double  timeDiff = 4 * delta;	// in minutes of time
	double  timeUTC = 720 - timeDiff - eqTime;	// in minutes
	double  newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC / 1440.0);

	eqTime = calcEquationOfTime(newt);
	solarDec = calcSunDeclination(newt);

	hourAngle = calcHourAngleSunrise(latitude, solarDec);
	delta = longitude + radToDeg(hourAngle);
	timeDiff = 4 * delta;
	timeUTC = 720 - timeDiff - eqTime; // in minutes
	*/
	return calcSunriseUTC() + (60 * tzOffset);
}

double SunSet::calcSunsetUTC()
{
	double t = calcTimeJulianCent(julianDate);
	// *** First pass to approximate sunset
	double  eqTime = calcEquationOfTime(t);
	double  solarDec = calcSunDeclination(t);
	double  hourAngle = calcHourAngleSunset(latitude, solarDec);
	double  delta = longitude + radToDeg(hourAngle);
	double  timeDiff = 4 * delta;	// in minutes of time
	double  timeUTC = 720 - timeDiff - eqTime;	// in minutes
	double  newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC / 1440.0);

	eqTime = calcEquationOfTime(newt);
	solarDec = calcSunDeclination(newt);

	hourAngle = calcHourAngleSunset(latitude, solarDec);
	delta = longitude + radToDeg(hourAngle);
	timeDiff = 4 * delta;
	timeUTC = 720 - timeDiff - eqTime; // in minutes

	return timeUTC;	// return time in minutes from midnight
}

double SunSet::calcSunset()
{
	return calcSunsetUTC() + (60 * tzOffset);
}

double SunSet::setCurrentDate(int y, int m, int d)
{
	m_year = y;
	m_month = m;
	m_day = d;
	julianDate = calcJD(y, m, d);
	return julianDate;
}

void SunSet::setTZOffset(int tz)
{
	tzOffset = tz;
}

int SunSet::moonPhase(int fromepoch)
{
	int moonepoch = 614100;

	int phase = (fromepoch - moonepoch) % 2551443;
	int res = floor(phase / (24 * 3600)) + 1;
	if (res == 30)
		res = 0;

	return res;
}
#pragma once
#include "pch.h"
#include "crc.h"


#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8
#define crc uint8_t

uint8_t crcSlow(uint8_t const message[], int nBytes) {
	crc  remainder = 0;
	/*
	 * Perform modulo-2 division, a byte at a time.
	 */
	for (int byte = 0; byte < nBytes; ++byte)
	{
		/*
		 * Bring the next byte into the remainder.
		 */
		remainder ^= (message[byte] << (WIDTH - 8));

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (uint8_t bit = 8; bit > 0; --bit)
		{
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}

	/*
	 * The final remainder is the CRC result.
	 */
	return (remainder);

}   /* crcSlow() */

#include "nettime.h"
#include <Arduino.h>
#include <WiFiUdp.h>

#include "WebLog.h"
#include <time.h>

/*


*/

NetTime::NetTime() {
	//Don't put anything in ctor that regs another class......
	secsPastMid = 0;
	lastTime = millis();
	secsSinceLastTimeUpdate = 0;  //Trigger it to look
	runTime = 0;
}

void NetTime::Init(long iTZ) {
	this->timeZone = iTZ;
	if (!udp.begin(localPort)) {
		webLog.println("Can't open port!");
	}
	webLog.It(-1,"NetTime::Initializing");
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

float NetTime::getHourFloat() {
	return secsPastMid / 3600.0f;
}

float NetTime::getRunTimeHours() {
	return runTime / 3600.0f;
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
		runTime++;
		if (secsPastMid >= 86400ul) {
			invalidTime = true;
			secsPastMid = secsPastMid % 86400ul;
			//Trigger a time update if it's there, otherwise continue
			//With whatever we had before.
			first = true;
		}
	}
	checkTimeRequest();
}

// On the hour for now...
bool NetTime::needNewTime() {
	//if (secsSinceLastTimeUpdate > (60)) return true; // On the minute for now...
	if (secsSinceLastTimeUpdate > (3600)) return true; // On the hour for now...
	return false;
}

void NetTime::sendTimeRequest() {
	//get a random server from the pool
	WiFi.hostByName(ntpServerName, timeServerIP);
	lastSendTime = millis();
	sendNTPpacket(timeServerIP); // send an NTP packet to a time server
	webLog.It(getHourFloat(), "NetTime::Sending time request.");
								 // wait to see if a reply is available
//	Serial.println(timeServerIP);
}
void NetTime::checkTimeRequest() {
	char buffer[80];
	if (needNewTime() || first) {
		if (((millis() - lastSendTime) > 20000) || (first)) {
			first = false;
			sendTimeRequest();
			lastSendTime = millis();
		}
	}
	int cb = udp.parsePacket();
	if (!cb) {
		return;
	}
	else {
		secsSinceLastTimeUpdate = 0;
		//Serial.print("Packet received, length=");
//		Serial.println(cb);
		// We've received a packet, read the data from it
		udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

												 //the timestamp starts at byte 40 of the received packet and is four bytes,
												 // or two words, long. First, esxtract the two words:

		unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
		unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
		// combine the four bytes (two words) into a long integer
		// this is NTP time (seconds since Jan 1 1900):
		unsigned long secsSince1900 = highWord << 16 | lowWord;
		// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
		const unsigned long seventyYears = 2208988800UL;
		// subtract seventy years:

		unsigned long epoch = secsSince1900 - seventyYears;
		
		//Adjust for Time Zone

		
		epoch += (timeZone * (3600l));

		setTimeSecs(epoch % 86400ul);
		
		int hour, min, sec;
		hour = (epoch % 86400ul) / 3600; //
		min = (epoch % 3600ul) / 60;       
		sec = (epoch % 60ul);

		//Figure out a better way to do this, this feels so 1970
		time_t t = epoch;
		tm *timeResult;
		timeResult = gmtime(&t);

		month = timeResult->tm_mon+1;
		day = timeResult->tm_mday;
		year = timeResult->tm_year+1900;

		//This way, if we want to show the time, we can......
		setTimeSecs(epoch % 86400ul);
		if (GotNewTime != NULL) GotNewTime();
		if (invalidTime) {
			invalidTime = false;
			//Fire off a function.......
			if (newTimeValid != NULL) newTimeValid();
		}
	}
	// wait ten seconds before asking for the time again
}


// send an NTP request to the time server at the given address
unsigned long NetTime::sendNTPpacket(IPAddress& address)
{
//	Serial.println("sending NTP packet...");
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
							 // 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	udp.beginPacket(address, 123); //NTP requests are to port 123
	udp.write(packetBuffer, NTP_PACKET_SIZE);
	udp.endPacket();

}

NetTime netTime = NetTime();#include "OTASetup.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>



void SetupOTA(const char *boardName) {
	ArduinoOTA.setHostname(boardName);

	// No authentication by default
	// ArduinoOTA.setPassword("admin");

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");


	//Setup the OTA things
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		}
		else { // U_SPIFFS
			type = "filesystem";
		}

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) {
			Serial.println("Auth Failed");
		}
		else if (error == OTA_BEGIN_ERROR) {
			Serial.println("Begin Failed");
		}
		else if (error == OTA_CONNECT_ERROR) {
			Serial.println("Connect Failed");
		}
		else if (error == OTA_RECEIVE_ERROR) {
			Serial.println("Receive Failed");
		}
		else if (error == OTA_END_ERROR) {
			Serial.println("End Failed");
		}
	});
	ArduinoOTA.begin();
}//Must output the website with no external refs, just plain html...
//Use % % % (with no spaces) to represent a line converted, it will be repeated
#include "output.h"

void outputSite(void (*printFunction)(const char *) ) {
	printFunction("<!doctype html>");
	printFunction("<!-- test -->");
	printFunction("<html lang=\"en\">");
	printFunction("<head>");
	printFunction("    <meta charset=\"utf-8\">");
	printFunction("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
	printFunction("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
	printFunction("    <title>Pig Feeding Beast</title>");
	printFunction("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
	printFunction("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");
	printFunction("<!--    <script src=\"//cdn.rawgit.com/Mikhus/canvas-gauges/gh-pages/download/2.1.5/radial/gauge.min.js\"></script> -->");
	printFunction("");
	printFunction("    <script>");
	printFunction("        function openButton() {");
	printFunction("            console.log(\"Trying to Open\");");
	printFunction("            $.get(\"/Open\", function (data) {");
	printFunction("                refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function closeButton() {");
	printFunction("            console.log(\"Trying to Close\");");
	printFunction("            $.get(\"/Close\", function (data) {");
	printFunction("                refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function cycleButton() {");
	printFunction("            console.log(\"Trying to Cycle\");");
	printFunction("            $.get(\"/Cycle\", function (data) {");
	printFunction("                refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function danceButton() {");
	printFunction("            console.log(\"Trying to Dance\");");
	printFunction("            $.get(\"/Dance\", function (data) {");
	printFunction("                refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function auxOn() {");
	printFunction("            console.log(\"Aux On\");");
	printFunction("            $.get(\"/AuxOn\", function (data) {");
	printFunction("                refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function auxOff() {");
	printFunction("            console.log(\"Aux Off\");");
	printFunction("            $.get(\"/AuxOff\", function (data) {");
	printFunction("                refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function refresh() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                console.log(data);");
	printFunction("            });");
	printFunction("            $.get(\"/log.json\", function (data) {");
	printFunction("                $(\"#dLog\").html(data);");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        $(document).ready(function () {");
	printFunction("            $(\"#bCycle\").click(cycleButton);");
	printFunction("            $(\"#bOpen\").click(openButton);");
	printFunction("            $(\"#bClose\").click(closeButton);");
	printFunction("            $(\"#bDance\").click(danceButton);");
	printFunction("            $(\"#bAuxOn\").click(auxOn);");
	printFunction("            $(\"#bAuxOff\").click(auxOff);");
	printFunction("            refresh();");
	printFunction("            setInterval(refresh, 1500);");
	printFunction("        });");
	printFunction("");
	printFunction("    </script>");
	printFunction("    <style>");
	printFunction("        .paused {");
	printFunction("            animation-play-state: paused !important;");
	printFunction("        }");
	printFunction("        ");
	printFunction("        .animated {");
	printFunction("            -webkit-animation-duration: 1s;");
	printFunction("            animation-duration: 1s;");
	printFunction("            animation-name: flash;");
	printFunction("            animation-iteration-count: infinite;");
	printFunction("            animation-play-state: running;");
	printFunction("        }");
	printFunction("");
	printFunction("        @-webkit-keyframes flash {");
	printFunction("            from {");
	printFunction("                -webkit-transform: scale3d(1, 1, 1);");
	printFunction("                transform: scale3d(1, 1, 1);");
	printFunction("                rotation: 0deg;");
	printFunction("            }");
	printFunction("");
	printFunction("            50% {");
	printFunction("                -webkit-transform: scale3d(1.05, 1.05, 1.05);");
	printFunction("                transform: scale3d(1.55, 1.55, 1.55);");
	printFunction("                rotation: 90deg;");
	printFunction("            }");
	printFunction("");
	printFunction("            to {");
	printFunction("                -webkit-transform: scale3d(1, 1, 1);");
	printFunction("                transform: scale3d(1, 1, 1);");
	printFunction("                rotation: 0deg;");
	printFunction("            }");
	printFunction("        }");
	printFunction("");
	printFunction("        @keyframes flash {");
	printFunction("            from, 50%, to {");
	printFunction("                top: 0px;");
	printFunction("                transform: scale3d(1.00, 1.15, 1.15);");
	printFunction("                transform-origin: 50%;");
	printFunction("            }");
	printFunction("");
	printFunction("            25%, 75% {");
	printFunction("                top: 10px;");
	printFunction("                transform: scale3d(1, 1, 1);");
	printFunction("            }");
	printFunction("        }");
	printFunction("    </style>");
	printFunction("");
	printFunction("</head>");
	printFunction("");
	printFunction("<!-- test -->");
	printFunction("<body>   ");
	printFunction("    <div class=\"container text-center\">");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            <h1>Jordan - Pig Feeding Beast</h1>");
	printFunction("            <h2>OTA Code Upload</h2>");
	printFunction("            <div class=\"row\">");
	printFunction("                <div class=\"col\" id=\"dTemp\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-success\" id=\"bCycle\">Cycle Feeder</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\" id=\"dWind\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-warning\" id=\"bOpen\">Open Feed Tube</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\" id=\"dWater\">");
	printFunction("                    <button type=\"button\" class=\"btn  btn-warning\" id=\"bClose\">Close Feed Tube</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\" id=\"dfWater\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-danger\" id=\"bDance\">Cmon now lets Dance!</button>");
	printFunction("                </div>");
	printFunction("            </div>");
	printFunction("            <br />");
	printFunction("            <div class=\"row\">");
	printFunction("                <div class=\"col\" id=\"dAuxOn\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-success\" id=\"bAuxOn\">Aux On</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\" id=\"dAuxOff\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-warning\" id=\"bAuxOff\">Aux Off</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\" id=\"\">");
	printFunction("                </div>");
	printFunction("                <div class=\"col\" id=\"\">");
	printFunction("                    <a href=\"/Edit\" class=\"btn btn-info\">Edit Config</a>");
	printFunction("");
	printFunction("");
	printFunction("                </div>");
	printFunction("            </div>");
	printFunction("        </div>");
	printFunction("        <br>");
	printFunction("        <div id=\"dLog\">");
	printFunction("            Enable Javascript to see the log......");
	printFunction("        </div>");
	printFunction("    </div>");
	printFunction("</body>");
	printFunction("</html>");
}
//Must output the website with no external refs, just plain html...
//Use % % % (with no spaces) to represent a line converted, it will be repeated
#include "outputedit.h"

void outputEditSite(void (*printFunction)(const char *) ) {
	printFunction("<!doctype html>");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("<html lang=\"en\">");
	printFunction("<head>");
	printFunction("    <meta charset=\"utf-8\">");
	printFunction("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
	printFunction("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
	printFunction("    <title>CRW System Monitor</title>");
	printFunction("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
	printFunction("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");
	printFunction("");
	printFunction("");
	printFunction("    <script>");
	printFunction("        String.prototype.formatUnicorn = String.prototype.formatUnicorn ||");
	printFunction("            function () {");
	printFunction("                \"use strict\";");
	printFunction("                var str = this.toString();");
	printFunction("                if (arguments.length) {");
	printFunction("                    var t = typeof arguments[0];");
	printFunction("                    var key;");
	printFunction("                    var args = (\"string\" === t || \"number\" === t) ?");
	printFunction("                        Array.prototype.slice.call(arguments)");
	printFunction("                        : arguments[0];");
	printFunction("");
	printFunction("                    for (key in args) {");
	printFunction("                        str = str.replace(new RegExp(\"\\\\{\" + key + \"\\\\}\", \"gi\"), args[key]);");
	printFunction("                        ");
	printFunction("");
	printFunction("                    }");
	printFunction("                }");
	printFunction("");
	printFunction("                return str;");
	printFunction("            };");
	printFunction("");
	printFunction("        function refresh() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                console.log(data);");
	printFunction("                if (data.moving > 0) {");
	printFunction("                    animateOn();");
	printFunction("                } else {");
	printFunction("                    animateOff();");
	printFunction("                }");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function timerHit() {");
	printFunction("//            loadIt();");
	printFunction("  //          logButton();");
	printFunction("        }");
	printFunction("");
	printFunction("        function loadIt() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
	printFunction("                /*");
	printFunction("                gauges[0].value = data.Temp;");
	printFunction("                gauges[1].value = (data.Water);");
	printFunction("                gauges[2].value = (data.Time);");
	printFunction("                gauges[3].value = (data.LastOutTemp);");
	printFunction("                gauges[4].value = (data.LastRemoteTemp);");
	printFunction("                gauges[5].value = (data.ThermostatSetting);");
	printFunction("                $(\"#dPump\").html(\"Cooler Pump:\" + data.Pump);");
	printFunction("                $(\"#dFan\").html(\"Cooler Fan:\" + data.Fan);");
	printFunction("                $(\"#dThermo\").html(\"Thermo:\" + data.ThermostatCool); ");
	printFunction("                */");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function logButton() {");
	printFunction("            $.get(\"/log.json\", function (data) {");
	printFunction("                $(\"#dLog\").html(data);");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function FillPage() {");
	printFunction("            var i;");
	printFunction("            for (i = 0; i < 20; i++) {");
	printFunction("                var st = '<tr><td><label for=\"task{0}\">Task:</label><select class=\"form-control taskList\" id=\"task{0}\"><option value=\"0\">Nothing</option></select></td><td><label for=\"usr\">Time:</label><input type=\"text\" class=\"form-control\" id=\"time{0}\"></td > <td><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunrise{0}\" value=\"\">Sunrise</label><br /><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunset{0}\" value=\"\">Sunset</label></td><td><label for=\"time{0}\">Option:</label><input type=\"text\" class=\"form-control\" id=\"option{0}\"></td></tr>'.formatUnicorn(i);");
	printFunction("                $(\".DataArea\").append(st);");
	printFunction("            }");
	printFunction("        }");
	printFunction("        function huh() {");
	printFunction("            $.getJSON(\"/schedule.json\", function (data) {");
	printFunction("                console.log(\"asfasdfa\");");
	printFunction("                $(\"#iTitle\").html(data.Title);");
	printFunction("                $(\"#iVersion\").html(data.Version);");
	printFunction("                loadTaskItems(data.TaskItems);");
	printFunction("                loadTaskForm(data.Tasks);");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function loadTaskItems(items) {");
	printFunction("            for (i = 0; i < items.length; i++) {");
	printFunction("                $('.taskList').append($('<option>', items[i]));");
	printFunction("            }");
	printFunction("        }");
	printFunction("        function loadTaskForm(items) {");
	printFunction("            for (i = 0; i < items.length; i++) {");
	printFunction("                var st = items[i].task;");
	printFunction("                var objs = st.split(\":\")");
	printFunction("                var ttask = objs[0];");
	printFunction("                var others = objs[1].split(\",\",4);");
	printFunction("                //Now we can set the values on screen");
	printFunction("                $(\"#task\"+i).val(ttask);");
	printFunction("                $(\"#time\" + i).val(others[0]);");
	printFunction("                $(\"#sunrise\" + i).prop('checked', (others[1] != 0));");
	printFunction("                $(\"#sunset\" + i).prop('checked', (others[2] != 0));");
	printFunction("                $(\"#option\" + i).val(others[3]);");
	printFunction("            }");
	printFunction("        }");
	printFunction("");
	printFunction("        function sendData() {");
	printFunction("            var st = \"?\";");
	printFunction("            st += \"Title=\" + $(\"#iTitle\").html();");
	printFunction("            st += \"&\";");
	printFunction("            st += \"Version=\" + $(\"#iVersion\").html();");
	printFunction("            st += \"&\";");
	printFunction("            for (i = 0; i < 20; i++) {");
	printFunction("                ");
	printFunction("                var t = \"\";");
	printFunction("                var taskSelected = $(\"#task\" + i + \" option:selected\").val();");
	printFunction("                if (taskSelected == 0) continue;");
	printFunction("");
	printFunction("                t = t + taskSelected;");
	printFunction("");
	printFunction("                t = t + \":\" + $(\"#time\" + i).val() + \",\";");
	printFunction("                if ($(\"#sunrise\" + i).prop('checked')) {");
	printFunction("                    t += \"1,\";");
	printFunction("                } else {");
	printFunction("                    t += \"0,\";");
	printFunction("                }");
	printFunction("                if ($(\"#sunset\" + i).prop('checked')) {");
	printFunction("                    t += \"1,\";");
	printFunction("                } else {");
	printFunction("                    t += \"0,\";");
	printFunction("                }");
	printFunction("                t += $(\"#option\" + i).val();");
	printFunction("                st += t + \"&\";");
	printFunction("            }");
	printFunction("");
	printFunction("            $.get(\"/setConfig\" + t, function (data) {");
	printFunction("                //We sent the data.........  wait for reboot");
	printFunction("                window.location.href = \"/\";");
	printFunction("            });");
	printFunction("        }");
	printFunction("        ");
	printFunction("");
	printFunction("        $(document).ready(function () {");
	printFunction("            huh();");
	printFunction("            $(\"#bSave\").click(sendData);");
	printFunction("            FillPage();");
	printFunction("        });");
	printFunction("");
	printFunction("    </script>");
	printFunction("");
	printFunction("</head>");
	printFunction("<!-- test -->");
	printFunction("<body>");
	printFunction("    <div class=\"container\">");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            <h1 id=\"iTitle\">%%Title%%</h1>");
	printFunction("            <h2 id=\"iVersion\">%%Version%%</h2>");
	printFunction("        </div>");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            Edit Setup");
	printFunction("        </div>");
	printFunction("        <div class=\"well\">");
	printFunction("            <h3>Events</h3>");
	printFunction("            <table class=\"table\">");
	printFunction("                <thead>");
	printFunction("                    <tr>");
	printFunction("                        <th>Task</th>");
	printFunction("                        <th>Time</th>");
	printFunction("                        <th>Offsets</th>");
	printFunction("                        <th>Options</th>");
	printFunction("                        <th>Actions</th>");
	printFunction("                    </tr>");
	printFunction("                </thead>");
	printFunction("                <tbody id=\"tableBody\" class=\"DataArea\"></tbody>");
	printFunction("            </table>");
	printFunction("            <button type=\"button\" class=\"btn btn-success\" id=\"bSave\">Save</button>");
	printFunction("        </div>");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("");
	printFunction("        <div>Log File</div>");
	printFunction("        <div id=\"dLog\"></div>");
	printFunction("        <br />");
	printFunction("        <div id=\"dRaw\"></div>");
	printFunction("    </div>");
	printFunction("</body>");
	printFunction("</html>");
	printFunction("");
	printFunction("<!--");
	printFunction("                                <option value=\"0\">Nothing</option>");
	printFunction("                                <option value=\"1\">Open Door</option>");
	printFunction("                                <option value=\"2\">Close Door</option>");
	printFunction("                                <option value=\"3\">Light On</option>");
	printFunction("                                <option value=\"4\">Light Off</option>");
	printFunction("                                <option value=\"5\">Log Message</option>");
	printFunction("<tr><td><label for=\"task%%\">Task:</label><select class=\"form-control\" id=\"task%%\"></select></td><td><label for=\"usr\">Time:</label><input type=\"text\" class=\"form-control\" id=\"time%%\">");
	printFunction("</td><td><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunrise%%\" value=\"\">Sunrise</label><br /><label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"sunset%%\" value=\"\">Sunset</label>");
	printFunction("</td><td><label for=\"time0\">Option:</label><input type=\"text\" class=\"form-control\" id=\"option%%\"></td></tr>");
	printFunction("");
	printFunction("    -->");
}
// pch.cpp: source file corresponding to pre-compiled header; necessary for compilation to succeed

#include "pch.h"

// In general, ignore this file, but keep it around if you are using pre-compiled headers.
#include "ScheduleConfig.h"

ScheduleConfig sConfig = ScheduleConfig();


void ScheduleConfig::Add(const char *command) {
	if (sz >= 20) return;
	strncpy(elements[sz], command, 80);
	sz++;
}


bool ScheduleConfig::LoadFromEEPROM()
{
	uint8_t crc = 0;
	/*  //Windows method for testing, we'll keep it for now
	FILE *filp = fopen("filename.bin", "rb");
	do {
		if (filp == NULL) break;
		fread(this->elements, sizeof(char), sizeof(this), filp);
		fread(&crc, 1, 1, filp);
	} while (0);
	if (filp) fclose(filp);
	*/

	//if (getCRC() == crc) return true;
	//Something is wrong......
	ResetDefault();
}#include "pch.h"
#include "Task.h"
#include "WebLog.h"
#include <Arduino.h>

Task::Task() {
}


TaskOpen::TaskOpen(unsigned ipin, int imillis) {
	pin = ipin;
	length = imillis;
}

void TaskOpen::Start() {
	const int sz = 40;
	char buffer[sz];
	snprintf(buffer, sz, "Open:Start:%d", pin);
	webLog.println(buffer);

	startTime = millis();
	digitalWrite(pin, LOW);

}

bool TaskOpen::Process() {
	if ((millis() - startTime) > length) {
		return false;
	}
	return true;
}

void TaskOpen::End() {
	const int sz = 40;
	char buffer[sz];
	snprintf(buffer, sz, "Open:End:%d", pin);
	webLog.println(buffer);

	digitalWrite(pin, HIGH);
}

/*************************  TaskWait *********************************/

void TaskWait::End() {
	//Nothing
}

void TaskWait::Start() {
	startTime = millis();
}

/************************   TaskOpenBuzz     ************************/

TaskOpenBuzz::TaskOpenBuzz(unsigned ipin, unsigned bPin, int imillis, int bStart, int bEnd) :TaskOpen(ipin, imillis) {
	buzzStart = bStart;
	buzzEnd = bEnd;
	buzzPin = bPin;
}


void TaskOpenBuzz::Start() {
	//Nothing to do really
	TaskOpen::Start();
}

bool TaskOpenBuzz::Process() {
	unsigned long diff = (millis() - startTime);

	if ((diff >= buzzStart) && (diff <= buzzEnd)) {
		digitalWrite(buzzPin, LOW);
	}
	else {
		digitalWrite(buzzPin, HIGH);
	}

	if (diff > length) {

		return false;
	}
	return true;
}

void TaskOpenBuzz::End() {
	TaskOpen::End();
	//Clean up our pin
	digitalWrite(buzzPin, HIGH);
}


///  Task set state
/*
class TaskSetState : public TaskOpen {
	bool On;
public:
	TaskSetState(int iPin, bool iOn);
	void virtual Start();
	void virtual End();
	bool virtual Process();
};*/


TaskSetState::TaskSetState(int iPin, bool iOn) :TaskOpen(iPin) {
	On = iOn;
}

//It's still reverse logic for the board.
void TaskSetState::Start() {
	const int sz = 40;
	char buffer[sz];
	digitalWrite(pin, !On);
	snprintf(buffer, sz, "Set:Output: %d->%s", pin, On ? "On" : "Off");
	webLog.println(buffer);
}

bool TaskSetState::Process() {
	return false;
}

void TaskSetState::End() {
	//Nothing, state is set.
}

/* This just prints a line in the log....*/

TaskLogMsg::TaskLogMsg() {
	buffer[0] = 0;
}

TaskLogMsg::TaskLogMsg(const char *msg) {
	strncpy(buffer, msg, 80);
}

void TaskLogMsg::Start() { 	}

void TaskLogMsg::End() { }

bool TaskLogMsg::Process() {
	webLog.println(buffer);
	return false;
}

/******************** BBB *******************

*/

const char daBeet[16] = {
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
	  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0
};
#include "pch.h"
#include "TaskChicken.h"
#include "pins.h"
#include "WebLog.h"
#include "ScheduleConfig.h"


SpecNothing SpecificTaskOfNothing;


ScheduleObject ScheduleObject::Get(const char *str) {
	//Format is:  function,vars, vars,vars,vars, no commas in vars
	ScheduleObject junk = ScheduleObject();
	int func = 0;
	sscanf(str, "%d:", &func);
	//Meh, returning on the stack, ick
	if (func == 0) return junk;

	const char *ptr = strchr(str, ':');
	ptr++;
	//ourTasks is not null, there is something in it.....
	for (int i = 0;; i++) {
		if (ourTasks[i].func == NULL) break;		//we hit the end of the array
		if (ourTasks[i].Id == func) return ourTasks[i].func(ptr);
	}
	return junk;
}

//Format:  14.25 (2:15pm), [0/1], [0/1],.......the rest.........
bool SpecTask::ParseTime(const char *str, float* itime, char *irest) {
	//Will need to know those things, so.........
	//Lets do it to it.
	int isunrise = 0;
	int isunset = 0;

	int result = sscanf(str, "%f,%d,%d,%60[^/n]s", itime, &isunrise, &isunset, irest);
	if (result != 4) {
		webLog.It(0, "Invalid info on Parse Command:");
		webLog.It(0, str);
		return false;
	}
	isunset = rangeIn(isunset, 0, 1);
	isunrise = rangeIn(isunrise, 0, 1);

	if (isunrise) (*itime) += sConfig.Sunrise;
	if (isunset) (*itime) += sConfig.Sunset;
	*itime = rangeInF(*itime, 0.0f, 24.0f);
	return true;
}

SpecOpenDoor::SpecOpenDoor(unsigned long secs) {
		//Now technically, we know what our Door Pin is.......
	task = TaskOpen(PIN_DOOR_OPEN, secs);
	name = "Open Door";
}


//Format, since we are coming from the config file.....
//TimeOfDay(#-2.2f),sunrise (%d) ,sunset (%d), holdTime %d
//3.14,1,0,65
ScheduleObject SpecOpenDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	int seconds;

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals,&timeOfDay,buffer)) {
		return SpecLogText::Create("0,0,0,Error");
	}
	sscanf(buffer, "%d", &seconds);
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecOpenDoor(seconds*1000ul);
	//Door is left open, that's not stately
	scho.stately = false;
	return scho;
}

SpecCloseDoor::SpecCloseDoor(unsigned long secs) {
	task = TaskOpen(PIN_DOOR_CLOSE, secs);
	name = "Close Door";
}

ScheduleObject SpecCloseDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	int seconds;

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Close Door");
	}
	sscanf(buffer, "%d", &seconds);
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecCloseDoor(seconds * 1000ul);
	//Door is left open, that's not stately
	scho.stately = false;
	return scho;
}

SpecLightOn::SpecLightOn() {
	task = TaskSetState(PIN_LIGHT, true);
	name = "Light On";
}


ScheduleObject SpecLightOn::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light On");
	}
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecLightOn();
	//Light is on from off, that's not stately
	scho.stately = false;
	return scho;
}

SpecLightOff::SpecLightOff() {
	task = TaskSetState(PIN_LIGHT, false);
	name = "Light Off";
}


ScheduleObject SpecLightOff::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light Off");
	}
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecLightOff();
	//Light is on from off, that's not stately
	scho.stately = false;
	return scho;
}

SpecLogText::SpecLogText(const char* msg) {
	task = TaskLogMsg(msg);
	name = "Log Message";
}

ScheduleObject SpecLogText::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject scho = ScheduleObject();

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Log Msg");
	}
	scho.timeOfDay = timeOfDay;
	scho.specTask = new SpecLogText(buffer);
	//This is harmless, so we want it to execute on restart......
	scho.stately = false;
	return scho;
}


SpecTaskClass ourTasks[] = {
	{&SpecOpenDoor::Create,SpecOpenDoor::TaskId,"Open Door"},
	{&SpecCloseDoor::Create,SpecCloseDoor::TaskId,"Close Door"},
	{&SpecLightOn::Create,SpecLightOn::TaskId,"Light On"},
	{&SpecLightOn::Create,SpecLightOff::TaskId,"Light Off"},
	{&SpecLogText::Create,SpecLogText::TaskId, "Log Message"},
	//End it with a null, better that way
	{NULL, 0}
};















#include "WebLog.h"
#include "nettime.h"

WebLog::WebLog(int isize) {
	size = isize;
	entries = new WebEntry[size];
}

void WebLog::Clear() {
	headPtr = 0;
	tailPtr = 0;
	for (int i = 0; i < size; i++) {
		entries[i].Clear();
	}
}

void WebLog::It(float time, const char *message) {
	entries[headPtr].timeOfDay = time;
	strncpy(entries[headPtr].message, message, 80);
	headPtr++;
	headPtr = headPtr%size;
	if (headPtr == tailPtr) {
		tailPtr++;
		tailPtr = tailPtr % size;
	}
}

void WebLog::println(const char *msg) {
	It(netTime.getHourFloat(), msg);
}

void WebLog::Print(void(*pf)(const char *)) {
	char buffer[20];
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
	pf("<p>Run time:");
	sprintf(buffer, "%6.2f", netTime.getRunTimeHours());
	pf(buffer);
	pf("</p>");

	pf("<table class=\"table table-hover table-bordered\">");
	pf("<thead><tr><th>Time</th><th>Message</th></tr></thead>");
	pf("<tbody>");
	int i = tailPtr;
	while (i != headPtr) {
		pf("<tr><td>");
		sprintf(buffer, "%2.2f", entries[i].timeOfDay);
		pf(buffer);
		pf("</td><td>");
		pf(entries[i].message);  //Gotta watch the //\\ special chars here
		pf("</td></tr>");
		i++;
		i = i%size;
	}
	pf("</tbody>");
	pf("</table>");
}

void WebLog::PrintReverse(void(*pf)(const char *)) {
	char buffer[20];
	sprintf(buffer, "C++ Version: %ld", __cplusplus);

	pf(buffer);
	
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
	pf("<p>Run time:");
	sprintf(buffer, "%6.2f", netTime.getRunTimeHours());

	pf(buffer);
	pf("</p>");
	pf("<table class=\"table table-hover table-bordered\">");
	pf("<thead><tr><th>Time</th><th>Message</th></tr></thead>");
	pf("<tbody>");
	int i = headPtr;
	for (;;) {
		if (i == tailPtr) break;
		i--;
		if (i<0) i += size;
		i = i%size;

		pf("<tr><td>");
		sprintf(buffer, "%2.2f", entries[i].timeOfDay);
		pf(buffer);
		pf("</td><td>");
		pf(entries[i].message);  //Gotta watch the //\\ special chars here
		pf("</td></tr>\n");
	}
	pf("</tbody>");
	pf("</table>");
}



WebLog webLog = WebLog(40);