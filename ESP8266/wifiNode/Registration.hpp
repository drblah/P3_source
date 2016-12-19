#ifndef REGISTRATION_H
#define REGISTRATION_H

#define MESSAGESIZE 255
#define SAVE_REG_SUCCESS 1
#define WRONG_REPLY_TYPE -2
#define PARSE_FAILURE -1

#define LENGTH_OF_IP 12

#include <ArduinoJson.h>

class Registration
{
public:
	Registration();
	unsigned int getDeviceId();
	const char* getServerIp();
	unsigned int getServerPort();
	void genRequest(char request[], unsigned int requestSize, unsigned int profile_id, const char ip[], const char MAC[]);
	int saveRegistration(char reply[]);
private:
	struct Reg {
		unsigned int device_id = 0;
		char server_ip[LENGTH_OF_IP+1] = ""; // IP address can be up to 12 characters + 1 for \0 termination.
		unsigned int server_port = 0;
	} registration;

	
};



#endif