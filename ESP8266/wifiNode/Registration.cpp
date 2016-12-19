

#include "Registration.hpp"

Registration::Registration() 
{
}

void Registration::genRequest(char request[], unsigned int requestSize, unsigned int profile_id, const char ip[], const char MAC[])
{
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(4);
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject &root = jsonBuffer.createObject();

	root["type"] = "register";
	root["profile_id"] = profile_id;
	root["device_ip"] = ip;
	root["device_MAC"] = MAC;

	root.printTo(request, requestSize);
}


/*	Expected response layout:
* 		{"type":"regok","device_id":123,"server_ip":"10.0.0.100","server_port":1337}
*	Returns 1 on succes, -1 on JSON parse failure and -2 on wrong reply type.
*/ 
int Registration::saveRegistration(char reply[])
{
	
	const int BUFFER_SIZE = JSON_OBJECT_SIZE(4);
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject &root = jsonBuffer.parseObject(reply);

	if (root.success())
	{
		if (strcmp(root["type"], "regok") == 0)
		{
			if (root.containsKey("device_id") && root.containsKey("server_ip") && root.containsKey("server_port"))
			{
				registration.device_id = root["device_id"];
				strncpy(registration.server_ip, root["server_ip"], LENGTH_OF_IP);
				registration.server_port = root["server_port"];
			}
			else
			{
				return WRONG_REPLY_TYPE;
			}
			return SAVE_REG_SUCCESS;
		}
		else
		{
			return WRONG_REPLY_TYPE;
		}
	}
	else
	{
		return PARSE_FAILURE;
	}
}

unsigned int Registration::getDeviceId()
{
	return registration.device_id;
}

const char* Registration::getServerIp()
{
	return registration.server_ip;
}

unsigned int Registration::getServerPort()
{
	return registration.server_port;
}