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

uint8_t crcSlow(uint8_t const *message, int nBytes) {
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

#include "pch.h"
#include "FixedRam.h"

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
		if ((secsPastMid >= 86400ul)||(triggerReload)) {
			triggerReload = false;
			invalidTime = true;
			secsPastMid = secsPastMid % 86400ul;
			if (funcMidnight != NULL) funcMidnight();
			//Trigger a time update if it's there, otherwise continue
			//With whatever we had before.
			first = true;
		}
	}
	checkTimeRequest();
}

// On the hour for now...
bool NetTime::needNewTime() {
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

		if (funcTimeCalc != NULL) funcTimeCalc();

		
		if (invalidTime) {
			invalidTime = false;
			//Fire off a function.......
			if (funcTimeValid != NULL) funcTimeValid();
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
//Use % % (with no spaces) to define a variable string replacement

#include "output.h"

void outputSite(void (*printFunction)(const char *) ) {
	printFunction("<!doctype html>");
	printFunction("<html lang=\"en\">");
	printFunction("<head>");
	printFunction("    <meta charset=\"utf-8\">");
	printFunction("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
	printFunction("    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\">");
	printFunction("    <title>{%1}</title>");
	printFunction("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
	printFunction("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>");
	printFunction("    <script>");
	printFunction("        function openButton() {");
	printFunction("            console.log(\"Trying to Open\");");
	printFunction("            $.get(\"/Open\", function (data) {");
	printFunction("                //refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function closeButton() {");
	printFunction("            console.log(\"Trying to Close\");");
	printFunction("            $.get(\"/Close\", function (data) {");
	printFunction("                //refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function cycleButton() {");
	printFunction("            console.log(\"Trying to Cycle\");");
	printFunction("            $.get(\"/Cycle\", function (data) {");
	printFunction("                //refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("        function danceButton() {");
	printFunction("            console.log(\"Trying to Dance\");");
	printFunction("            $.get(\"/Dance\", function (data) {");
	printFunction("                //refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function auxOn() {");
	printFunction("            console.log(\"Aux On\");");
	printFunction("            $.get(\"/AuxOn\", function (data) {");
	printFunction("                //refresh();");
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function auxOff() {");
	printFunction("            console.log(\"Aux Off\");");
	printFunction("            $.get(\"/AuxOff\", function (data) {");
	printFunction("                //refresh();");
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
	printFunction("    </style>");
	printFunction("");
	printFunction("</head>");
	printFunction("<body>   ");
	printFunction("    <div class=\"container text-center\">");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            <h1>{%1}</h1>");
	printFunction("            <h2>{%2}</h2>");
	printFunction("            <div class=\"row\">");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-success\" id=\"bCycle\">Cycle Feeder</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-warning\" id=\"bOpen\">Open Feed Tube</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn  btn-warning\" id=\"bClose\">Close Feed Tube</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-danger\" id=\"bDance\">Cmon now lets Dance!</button>");
	printFunction("                </div>");
	printFunction("            </div>");
	printFunction("            <br />");
	printFunction("            <div class=\"row\">");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-success\" id=\"bAuxOn\">Aux On</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <button type=\"button\" class=\"btn btn-warning\" id=\"bAuxOff\">Aux Off</button>");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                </div>");
	printFunction("                <div class=\"col\">");
	printFunction("                    <a href=\"/Edit\" class=\"btn btn-info\">Edit Config</a>");
	printFunction("                </div></div></div><div id=\"dLog\"></div></div>");
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
	printFunction("            });");
	printFunction("        }");
	printFunction("");
	printFunction("        function timerHit() {");
	printFunction("        }");
	printFunction("");
	printFunction("        function loadIt() {");
	printFunction("            $.getJSON(\"/data.json\", function (data) {");
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
	printFunction("                $(\"#iTitle\").html(data.Title);");
	printFunction("                $(\"#iVersion\").html(data.Version);");
	printFunction("                $(\"#DName\").val(data.Title);");
	printFunction("                $(\"#DVersion\").val(data.Version);");
	printFunction("                $(\"#DST\").prop('checked', data.DST);");
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
	printFunction("            var st = \"\";");
	printFunction("            st += \"Title=\" + $(\"#DName\").val();");
	printFunction("            st += \"\\n\";");
	printFunction("            st += \"Version=\" + $(\"#DVersion\").val();");
	printFunction("            st += \"\\n\";");
	printFunction("            st += \"DST=\";");
	printFunction("            if ($(\"#DST\").prop('checked')) {");
	printFunction("                st += \"1\";");
	printFunction("            } else { st += \"0\"; }");
	printFunction("            st += \"\\n\";");
	printFunction("");
	printFunction("            for (i = 0; i < 20; i++) {");
	printFunction("                ");
	printFunction("                var t = \"Task=\";");
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
	printFunction("                st += t + \"\\n\";");
	printFunction("            }");
	printFunction("            st += \"END\\n\";");
	printFunction("            var xhr = new XMLHttpRequest();");
	printFunction("            xhr.open('POST', '/SetConfig', true);");
	printFunction("            xhr.setRequestHeader('Content-Type', 'text/html; charset=UTF-8');");
	printFunction("            // send the collected data as JSON");
	printFunction("            xhr.send(st);");
	printFunction("            console.log(st);");
	printFunction("            xhr.onloadend = function () {");
	printFunction("                window.location.href = \"/\";");
	printFunction("            };");
	printFunction("        }");
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
	printFunction("            <h1 id=\"iTitle\"></h1>");
	printFunction("            <h2 id=\"iVersion\"></h2>");
	printFunction("        </div>");
	printFunction("        <div class=\"jumbotron\">");
	printFunction("            Edit Setup");
	printFunction("            <label for=\"DName\">Device Name:</label>");
	printFunction("            <input type=\"text\" class=\"form-control\" id=\"DName\">");
	printFunction("            <label for=\"DVersion\">Device Version:</label>");
	printFunction("            <input type=\"text\" class=\"form-control\" id=\"DVersion\">");
	printFunction("            <label class=\"checkbox-inline\"><input type=\"checkbox\" id=\"DST\" value=\"\">Daylight Savings Time</label>");
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
#include "PostMessage.h"
#include "ScheduleConfig.h"

/*
Title=New%20Remote%20Action%20Unit
Version=Enter%20something%20here
Task=1:1.1,0,0,Test%20Message
Task=5:0.1,0,0,Schedule%20Startup
Task=5:0.0,1,0,Wake%20up!%20It's%20sunrise%3B%20you're%20burning%20daylight!
Task=5:0.0,0,1,Sunset
*/

unsigned char h2int(char c)
{
	c = toupper(c);
	if (c >= '0' && c <= '9') return((unsigned char)c - '0');
	if (c >= 'A' && c <= 'F') return((unsigned char)c - 'A' + 10);
	return 0;
}

void urldecode(char *dest, const char* src, int len)
{
	char c;
	char code0;
	char code1;
	int j = 0;
	int slen = strlen(src);
	for (int i = 0; i < slen; i++) {
		if (j >= (len - 1)) break;
		c = src[i];
		if (c == '+') {
			dest[j++] = ' ';
		} else
			if (c == '%') {
				code0 = src[i++];
				code1 = src[i++];
				c = (h2int(code0) << 4) | h2int(code1);
				dest[j++] = c;
			}
			else {
				dest[j++] = c;
			}
	}
	dest[j] = 0;
}


void handlePost(WiFiClient& client) {
	//read till 2 line feeds
	char buffer[120];
	char decoded[100];
	const char *ptr;
	int lastLen = 119;
	bool okay = true;
	bool firstTask = true;
	while (true) {
		if (!client.connected()) break;
		int len = client.readBytesUntil('\n', buffer, 120);
		buffer[len] = 0;
		Serial.println(buffer);
		if (strstr(buffer, "Title") == buffer) {
			ptr = strstr(buffer, "=");
			if (ptr) {
				ptr++;
				strncpy(sConfig.Title, ptr, sizeof(sConfig.Title));
				Serial.println(sizeof(sConfig.Title));
			}
		}
		if (strstr(buffer, "DST") == buffer)
		{
			int v;
			ptr = strstr(buffer, "=");
			if (ptr) {
				ptr++;
				if (sscanf(ptr, "%d", &v)>0) sConfig.DST = (v != 0);
			}
		}
		if (strstr(buffer, "Version") == buffer) {
			ptr = strstr(buffer, "=");
			if (ptr) {
				ptr++;
				strncpy(sConfig.Version, ptr, sizeof(sConfig.Version));
			}
		}
		if (strstr(buffer, "Task") == buffer) {
			if (firstTask) {
				firstTask = false;
				sConfig.ClearSchedule();
			}
			ptr = strstr(buffer, "=");
			if (ptr) {
				ptr++;
				sConfig.Add(ptr);
			}
		}
		if (strstr(buffer, "END") == buffer) {
			break;
		}
	}
	sConfig.SaveToEEPROM();
	Serial.println("Exiting set config");
	client.flush();
}#include "pch.h"
#include "ScheduleConfig.h"
#include <EEPROM.h>
#include "nettime.h"


ScheduleConfig ScheduleConfig::LoadFromEEPROM() {
	uint8_t ecrc = 0;
	ScheduleConfig sc;
	int size = sizeof(ScheduleConfig) + 4;

	EEPROM.begin(size);
	EEPROM.get(0, sc);
	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	EEPROM.end();
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	

	if (crc == ecrc) {
		webLog.println("Loaded EEPROM DATA");
		return sc;
	}
	webLog.println("Invalid EEPROM DATA");
	ResetDefault();
	return *this;
}

bool ScheduleConfig::SaveToEEPROM() {
	int size = sizeof(ScheduleConfig) + 4;
	uint8_t ecrc = 0;
	ScheduleConfig sc = *this;
	uint8_t crc = crcSlow((uint8_t*)(&sc), sizeof(ScheduleConfig));
	EEPROM.begin(size);
	EEPROM.put(0, sc);
	EEPROM.put(sizeof(ScheduleConfig), crc);
	EEPROM.commit();

	EEPROM.get(sizeof(ScheduleConfig), ecrc);
	if (crc != ecrc) {
		webLog.println("On Save: Invalid EEPROM DATA");
	}
	else {
		webLog.println("Verified EEPROM data");
	}
	EEPROM.end();
	netTime.triggerReload = true;
}

void ScheduleConfig::Add(const char *command) {
	if (sz >= 20) return;
	strncpy(elements[sz], command, 80);
	sz++;
}

ScheduleConfig sConfig = ScheduleConfig();#include "pch.h"
#include "ScheduleObject.h"

ScheduleObject* ScheduleObject::Get(const char *str) {
	//Format is:  function,vars, vars,vars,vars, no commas in vars
	int func = 0;
	int args = sscanf(str, "%d:", &func);
	//Meh, returning on the stack, ick
	if (func == 0) return NULL;

	const char *ptr = strchr(str, ':');
	if (!ptr) return NULL;
	ptr++;
	//ourTasks is not null, there is something in it.....
	for (int i = 0;; i++) {
		if (ourTasks[i].func == NULL) break;		//we hit the end of the array
		if (ourTasks[i].Id == func) return ourTasks[i].func(ptr);
	}
	return NULL;
}

void *ScheduleObject::operator new(size_t size) {
	return ramBase.allocate(size);
}

FixedRam<ScheduleObject> ScheduleObject::ramBase = FixedRam<ScheduleObject>("Schedule", 256);
#include "Task.h"
#include "WebLog.h"
#include <Arduino.h>

FixedRam<Task> Task::ramBase = FixedRam<Task>("Task", 1024);


Task::Task() {
}

void Task::DeleteAll() {
	ramBase.Reset();
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
	//Empty message.....  Compiler bitched about the array initialization......
	//See:  https://stackoverflow.com/questions/35413821/how-to-fix-this-array-used-as-initializer-error
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
#include "FixedRam.h"
#include "ScheduleObject.h"
#include "ScheduleConfig.h"

void* SpecTask::operator new(size_t size) {
	return ramBase.allocate(size);
}

//Format:  14.25 (2:15pm), [0/1], [0/1],.......the rest.........
bool SpecTask::ParseTime(const char *str, float* itime, char *irest) {
	//Will need to know those things, so.........
	//Lets do it to it.
	int isunrise = 0;
	int isunset = 0;
	int result = sscanf(str, "%f,%d,%d,%80[^\n]", itime, &isunrise, &isunset, irest);
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

ScheduleObject *SpecOpenDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	int seconds;

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;
	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Open Door");
	}
	sscanf(buffer, "%d", &seconds);
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecOpenDoor(seconds * 1000ul);

	//Door is left open, that's not stately
	scho->stately = false;
	return scho;
}



SpecOpenDoor::SpecOpenDoor(unsigned long secs) {
	//Now technically, we know what our Door Pin is.......
	task = new TaskOpen(PIN_DOOR_OPEN, secs);
	name = "Open Door";
}




SpecCloseDoor::SpecCloseDoor(unsigned long secs) {
	task = new TaskOpen(PIN_DOOR_CLOSE, secs);
	name = "Close Door";
}

ScheduleObject *SpecCloseDoor::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	int seconds;

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Close Door");
	}
	sscanf(buffer, "%d", &seconds);
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecCloseDoor(seconds * 1000ul);
	//Door is left open, that's not stately
	scho->stately = false;
	return scho;
}

SpecLightOn::SpecLightOn() {
	task = new TaskSetState(PIN_LIGHT, true);
	name = "Light On";
}


ScheduleObject *SpecLightOn::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light On");
	}
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecLightOn();
	//Light is on from off, that's not stately
	scho->stately = false;
	return scho;
}

SpecLightOff::SpecLightOff() {
	task = new TaskSetState(PIN_LIGHT, false);
	name = "Light Off";
}


ScheduleObject *SpecLightOff::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];

	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;

	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Light Off");
	}
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecLightOff();
	//Light is on from off, that's not stately
	scho->stately = false;
	return scho;
}

SpecLogText::SpecLogText(const char* msg)
{
	task = new TaskLogMsg(msg);
	name = "Log Message";
}

ScheduleObject *SpecLogText::Create(const char *vals) {
	float timeOfDay;
	char buffer[80];
	//Will need to know those things, so.........
	//Lets do it to it.
	ScheduleObject *scho = new ScheduleObject();
	if (!scho) return NULL;



	if (!ParseTime(vals, &timeOfDay, buffer)) {
		return SpecLogText::Create("0,0,0,Error: Log Msg");
	}
	scho->timeOfDay = timeOfDay;
	scho->specTask = new SpecLogText(buffer);
	//This is harmless, so we want it to execute on restart......
	scho->stately = false;
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

FixedRam<SpecTask> SpecTask::ramBase = FixedRam<SpecTask>("SpecTask", 1024);









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

void WebLog::floatToHourMin(char *dest, float t) {
	int mins = (t * 60);
	mins = mins % 60;
	sprintf(dest, "%02d:%02d", (int)t, mins);
}



void WebLog::Print(void(*pf)(const char *)) {
	char buffer[20];
	pf("<div class=\"well\">\n");
	sprintf(buffer, "C++ Version: %ld", __cplusplus);
	pf(buffer);
	sprintf(buffer, "Free ram (bytes):%ld", system_get_free_heap_size());
	pf(buffer);
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
	pf("System Run Time: ");
	sprintf(buffer, "%6.2f", netTime.getRunTimeHours());
	pf(buffer);
	pf("</div>\n");

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
	pf("<div class=\"well\">\n");
	sprintf(buffer, "C++ Version: %ld", __cplusplus);
	pf(buffer);
	sprintf(buffer, "Free ram (bytes):%ld", system_get_free_heap_size());
	pf(buffer);
	pf("<p>Compile date/time");
	pf(__DATE__);
	pf(__TIME__);
	pf("</p>");
	pf("System Run Time: ");
	sprintf(buffer, "%6.2f", netTime.getRunTimeHours());
	pf(buffer);
	pf("</div>\n");
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
		floatToHourMin(buffer, entries[i].timeOfDay);
//		sprintf(buffer, "%2.2f", entries[i].timeOfDay);
		pf(buffer);
		pf("</td><td>");
		pf(entries[i].message);  //Gotta watch the //\\ special chars here
		pf("</td></tr>\n");
	}
	pf("</tbody>");
	pf("</table>");
}



WebLog webLog = WebLog(40);