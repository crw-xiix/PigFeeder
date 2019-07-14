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

*/