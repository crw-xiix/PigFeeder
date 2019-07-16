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
}